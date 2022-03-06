// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "Game.h"
#include <iostream>

int* windowWidth, * windowHeight;
Uint32 startTiming, endTiming;
float secondsElapsed = 0.0f;


Game::Game()
	:window(nullptr)
	, renderer(nullptr)
	, ticksCount(0)
	, isRunning(true)
	, playerBoard(0)
	, gameState(GameState::StartScreen)
	, menuBitmapName("menu.bmp")
	, fieldBitmapName("campo.bmp")
	, gameMode(GameMode::SinglePlayer)
{}

bool Game::Initialize()
{
	SDL_Log("Initializing video...");
	int sdlResult = SDL_Init(SDL_INIT_VIDEO);

	if (sdlResult != 0)
	{
		SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
		return false;
	}

 	SDL_Log("Getting display mode...");
	SDL_DisplayMode displayMode;
	SDL_GetDesktopDisplayMode(0, &displayMode);

	if (SDL_GetDesktopDisplayMode(0, &displayMode) != 0)
	{
		SDL_Log("SDL_GetDesktopDisplayMode failed: %s", SDL_GetError());
		return false;
	}

	windowWidth = &displayMode.w;
	windowHeight = &displayMode.h;

	SDL_Log("Window size: %d x %d", *windowWidth, *windowHeight);

	SDL_Log("Creating window...");
	window = SDL_CreateWindow(
		"P.O.N.G",
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		*windowWidth,	
		*windowHeight,
		SDL_WINDOW_MAXIMIZED
	);

	if (!window)
	{
		SDL_Log("Failed to create window: %s", SDL_GetError());
		return false;
	}

	SDL_Log("Creating renderer...");
	renderer = SDL_CreateRenderer(
		window, // Window to create renderer for
		-1,		 // Usually -1
		SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
	);

	if (!renderer)
	{
		SDL_Log("Failed to create renderer: %s", SDL_GetError());
		return false;
	}

	screenSurface = SDL_GetWindowSurface(window);

	SDL_Log("Loading background...");
	Game::LoadBackground();

	Vector2 paddlePos = Vector2(10.0f, *windowHeight / 2.0f);
	paddle = Paddle(paddlePos);

	Vector2 ballPos = Vector2(1000.f, 540.f);
	Vector2 ballVel = Vector2(-200.f, 500.f);
	balls.push_back(Ball());
	balls[0].position = ballPos;
	balls[0].velocity = ballVel;

	startTiming = SDL_GetTicks();
	return true;
}

bool Game::LoadBitmap(
	std::string bitmapFileName,
	SDL_Surface* surface,
	SDL_Texture* texture,
	SDL_Rect* screenDest,
	SDL_Renderer* renderer
){
	surface = SDL_LoadBMP(bitmapFileName.c_str());

	if (surface == NULL) {
		SDL_Log("Unable to load image %s! SDL Error: %s\n", bitmapFileName.c_str(), SDL_GetError());
		return false;
	}

	int blitResult = SDL_BlitScaled(surface, NULL, screenSurface, screenDest);

	if (blitResult < 0) {
		SDL_Log("Unable to blit image %s onto surface! SDL Error: %s\n", bitmapFileName.c_str(), SDL_GetError());
		return false;
	}

	texture = SDL_CreateTextureFromSurface(renderer, surface);

	if (texture == 0) {
		SDL_Log("Unable to create texture from surface of image %s! SDL Error: %s\n", bitmapFileName.c_str(), SDL_GetError());
		return false;
	}

	return true;
}

void Game::LoadBackground()
{
	SDL_Rect screenDest;
	screenDest.x = 0;
	screenDest.y = 0;
	screenDest.w = *windowWidth;
	screenDest.h = *windowHeight;

	SDL_Log("Loading %s...", menuBitmapName.c_str());
	LoadBitmap(menuBitmapName, menuSurface, menuTexture, &screenDest, renderer);

	SDL_Log("Loading %s...", fieldBitmapName.c_str());
	LoadBitmap(fieldBitmapName, fieldSurface, fieldTexture, &screenDest, renderer);

	SDL_RenderCopy(renderer, menuTexture, NULL, NULL);
	SDL_RenderPresent(renderer);

	SDL_UpdateWindowSurface(window);
}

void Game::RunLoop()
{
	while (isRunning)
	{
		ProcessInput();
		if (gameState != GameState::StartScreen)
		{
			UpdateGame();
			GenerateOutput();
		}
	}
}

void Game::ProcessInput()
{
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
			case SDL_QUIT:
				isRunning = false;
				break;
		}
	}

	// Get state of keyboard - podemos buscar por alguma tecla específica pressionada no teclado, nesse caso, Escape
	const Uint8* state = SDL_GetKeyboardState(NULL);
	// If escape is pressed, also end loop
	if (state[SDL_SCANCODE_ESCAPE])
	{
		isRunning = false;
	}


	// Game screen
	if (gameState == GameState::StartScreen) 
	{
		if (state[SDL_SCANCODE_1] || state[SDL_SCANCODE_KP_1])
		{
			gameMode = GameMode::SinglePlayer;
			gameState = GameState::Playing;
		}
		if (state[SDL_SCANCODE_2] || state[SDL_SCANCODE_KP_2])
		{
			gameMode = GameMode::MultiPlayer;
			gameState = GameState::Playing;
		}
		if (state[SDL_SCANCODE_2] || state[SDL_SCANCODE_KP_3])
		{
			gameMode = GameMode::IA;
			gameState = GameState::Playing;
		}
	}
	else
	{
		// Update paddle direction based on W/S keys - atualize a direção da raquete com base na entrada do jogador
		// W -> move a raquete para cima, S -> move a raquete para baixo
		paddle.direction.y = 0;
		if (state[SDL_SCANCODE_W])
		{
			paddle.direction.y -= 1;
		}
		if (state[SDL_SCANCODE_S])
		{
			paddle.direction.y += 1;
		}
	}
}

void Game::UpdateGame()
{
	// Espere que 16ms tenham passado desde o último frame - limitando os frames
	while (!SDL_TICKS_PASSED(SDL_GetTicks(), ticksCount + 16))
		;

	// Delta time é a diferença em ticks do último frame
	// (convertido pra segundos) - calcula o delta time para atualização do jogo
	float deltaTime = (SDL_GetTicks() - ticksCount) / 1000.0f;

	// "Clamp" (lima/limita) valor máximo de delta time
	if (deltaTime > 0.05f)
	{
		deltaTime = 0.05f;
	}

	// atualize a contagem de ticks par ao próximo frame
	ticksCount = SDL_GetTicks();

	// atualiza a posição da raquete
	if (paddle.direction.y != 0)
	{
		paddle.position.y += paddle.direction.y * paddle.speed * paddle.speedFactor * deltaTime;
		// verifique que a raquete não se move para fora da tela - usamos "paddle.width", que definimos como a altura dos elementos
		if (paddle.position.y < (paddle.height / 2.0f + paddle.width))
		{
			paddle.position.y = paddle.height / 2.0f + paddle.width;
		}
		else if (paddle.position.y > (*windowHeight - paddle.height / 2.0f - paddle.width))
		{
			paddle.position.y = *windowHeight - paddle.height / 2.0f - paddle.width;
		}
	}

	endTiming = SDL_GetTicks();
	secondsElapsed = (endTiming - startTiming) / 1000.f;

	for (auto& ball : balls) {
		// atualiza a posição da bola com base na sua velocidade
		ball.position.x += ball.velocity.x * ball.speed * deltaTime;
		ball.position.y += ball.velocity.y * ball.speed * deltaTime;

		// atualiza a posição da bola se ela colidiu com a raquete
		float diff = paddle.position.y - ball.position.y;
		//pegue o valor absoluto de diferença entre o eixo y da bolinha e da raquete
		//isso é necessário para os casos em que no próximo frame a bolinha ainda não esteja tão distante da raquete
		//verifica se a bola está na area da raquete e na mesma posição no eixo x
		diff = (diff > 0.0f) ? diff : -diff;
		if (
			// A diferença no eixo y y-difference is small enough
			diff <= paddle.height / 2.0f &&
			// Estamos na posicao x correta
			ball.position.x <= 25.0f && ball.position.x >= 20.0f &&
			// A bolinha está se movendo para a esquerda
			ball.velocity.x < 0.0f)
		{
			ball.velocity.x *= -1.0f;

			#pragma region score_board
			string newTitle = "Placar: " + to_string(++playerBoard);
			SDL_SetWindowTitle(window, newTitle.c_str());
			#pragma endregion controls score board

			#pragma region add_new_ball

			if (playerBoard % 1 == 0)
			{
				const float distanceFromCollidedBall = 10.0f;
				const float velocityIncreaseFactor = 15.0f;

				Ball newBall = Ball();

				newBall.position = Vector2(
					paddle.position.x + paddle.width + distanceFromCollidedBall,
					ball.position.y + paddle.width + distanceFromCollidedBall
				);

				newBall.velocity = Vector2(
					ball.velocity.x,
					-(ball.velocity.y + velocityIncreaseFactor)
				);

				balls.push_back(newBall);
			}

			#pragma endregion add new ball on increase 3 points
		}

		//Verifica se a bola saiu da tela (no lado esquerdo, onde é permitido)
		//Se sim, encerra o jogo
		// 
		else if (ball.position.x <= 0.0f)
		{
			isRunning = false;
			ball.velocity.x *= -1.0f;
		}

		// Atualize (negative) a velocidade da bola se ela colidir com a parede do lado direito
		// 
		else if (ball.position.x >= (*windowWidth - paddle.width) && ball.velocity.x > 0.0f)
		{
			ball.velocity.x *= -1.0f;
		}

		// Atualize (negative) a posição da bola se ela colidir com a parede de cima
		// 
		if (ball.position.y <= paddle.width && ball.velocity.y < 0.0f)
		{
			ball.velocity.y *= -1;
		}

		// Atualize (negative) a posição da bola se ela colidiu com a parede de baixo
		// Did the ball collide with the bottom wall?
		else if (
			ball.position.y >= (*windowHeight - paddle.width) 
			&& ball.velocity.y > 0.0f
		){
			ball.velocity.y *= -1;
		}

		// verifica se houve colisão com outras bolas e faz as bolas irem para direções opostas
		/*for (auto& mBallAux : balls) {
			if (ball.position.x < mBallAux.position.x + paddle.width &&
				ball.position.x + paddle.width > mBallAux.position.x &&
				ball.position.y < mBallAux.position.y + paddle.width &&
				ball.position.y + paddle.width > mBallAux.position.y
				)
			{
				ball.velocity.x *= -1;
				mBallAux.velocity.x *= -1;
			}
		}*/

		if ((int)(secondsElapsed) % 10 == 0) {
			ball.speed += 0.005f;
		}
	}

}

//Desenhando a tela do jogo
void Game::GenerateOutput()
{
	SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0x00);
	SDL_RenderClear(renderer);
	SDL_RenderCopy(renderer, fieldTexture, NULL, NULL);

	// Desenha as paredes - mudamos a cor de renderer para o desenho dos retangulos que formaram as paredes
	SDL_SetRenderDrawColor(renderer, 255, 0, 255, 255);

	// Desenha a parede de cima - desenhando um retangulo no topo da tela, coordenada x e y = 0, 0 representa o topo esquerdo
	//primeiro definimos as coordenadas e tamanhos do triangulo
	SDL_Rect wall{
		0,			// Top left x
		0,			// Top left y
		*windowWidth,		// Width
		paddle.width	// Height
	};
	SDL_RenderFillRect(renderer, &wall);//finalmente, desenhamos um retangulo no topo

	SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);

	//desenhamos as outras paredes apenas mudando as coordenadas de wall

	// parede de baixo
	wall.y = *windowHeight - paddle.width;
	SDL_RenderFillRect(renderer, &wall);

	SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);

	// parede da direita
	wall.x = *windowWidth - paddle.width;
	wall.y = 0;
	wall.w = *windowWidth;
	wall.h = paddle.width;
	SDL_RenderFillRect(renderer, &wall);

	paddle.Draw(renderer);

	for (auto& ball : balls) {
		ball.Draw(renderer);
	}

	SDL_RenderPresent(renderer);
}


void Game::Shutdown()
{
	SDL_FreeSurface(menuSurface);
	SDL_FreeSurface(screenSurface);
	SDL_FreeSurface(fieldSurface);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}
