// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "./Game.h"
#include <iostream>


const int thickness = 15;//sera usado para setar a altura de alguns objetos
const float paddleH = 100.0f;//tamanho da raquete

Game::Game()
	:mWindow(nullptr)//para criar uma janela
	, mRenderer(nullptr)//para fins de renderização na tela
	, mTicksCount(0)//para guardar o tempo decorrido no jogo
	, mIsRunning(true)//verificar se o jogo ainda deve continuar sendo executado
	, mPaddleDir(0)//direcao da bolinha
	, playerBoard(0)//player score
	, mGameState(GameState::StartScreen) // game state
{

}

bool Game::Initialize()
{
	// Initialize SDL
	int sdlResult = SDL_Init(SDL_INIT_VIDEO);
	if (sdlResult != 0)
	{
		SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
		return false;
	}

	// Create an SDL Window
	mWindow = SDL_CreateWindow(
		"Game Programming in C++ (Chapter 1)", // Window title
		100,	// Top left x-coordinate of window
		100,	// Top left y-coordinate of window
		1024,	// Width of window
		768,	// Height of window
		0		// Flags (0 for no flags set)
	);

	if (!mWindow)
	{
		SDL_Log("Failed to create window: %s", SDL_GetError());
		return false;
	}

	//// Create SDL renderer
	mRenderer = SDL_CreateRenderer(
		mWindow, // Window to create renderer for
		-1,		 // Usually -1
		SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
	);

	if (!mRenderer)
	{
		SDL_Log("Failed to create renderer: %s", SDL_GetError());
		return false;
	}

	mPaddlePos.x = 10.0f; //posição inicial da raquete eixo x
	mPaddlePos.y = 768.0f / 2.0f; //posição inicial da raquee eixo y

	mBalls.push_back({
		{
			1000.0f, // Ball X-axis
			540.0f // Ball Y-axis
		},
		{
			-200.0f, // Ball velocity X-axis
			500.0f // Ball velocity Y-axis
		}
		});


	return true;
}

bool Game::loadBackground()
{
	bool success = true;

	gHelloWorld = SDL_LoadBMP("menu.bmp");
	if (gHelloWorld == NULL)
	{
		SDL_Log("Unable to load image %s! SDL Error: %s\n", "hello_world.bmp", SDL_GetError());
		success = false;
	}
	gScreenSurface = SDL_GetWindowSurface(mWindow);
	SDL_BlitSurface(gHelloWorld, NULL, gScreenSurface, NULL);
	texture1 = SDL_CreateTextureFromSurface(mRenderer, gHelloWorld);

	SDL_RenderCopy(mRenderer, texture1, NULL, NULL);
	SDL_RenderPresent(mRenderer);

	SDL_UpdateWindowSurface(mWindow);
	//SDL_Delay(2000);
	return success;
}

void Game::RunLoop()
{
	while (mIsRunning)
	{
		ProcessInput();
		if (mGameState != GameState::StartScreen)
		{
			UpdateGame();
			GenerateOutput();
		}
	}
}

void Game::ProcessInput()
{
	SDL_Event event;//evento, inputs do jogador são armazenados aqui
	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
			// If we get an SDL_QUIT event, end loop
		case SDL_QUIT:
			mIsRunning = false;
			break;
		}
	}

	// Get state of keyboard - podemos buscar por alguma tecla específica pressionada no teclado, nesse caso, Escape
	const Uint8* state = SDL_GetKeyboardState(NULL);
	// If escape is pressed, also end loop
	if (state[SDL_SCANCODE_ESCAPE])
	{
		mIsRunning = false;
	}


	// Game screen
	if (mGameState == GameState::StartScreen) 
	{
		if (state[SDL_SCANCODE_1] || state[SDL_SCANCODE_KP_1])
		{
			mGameMode = GameMode::SinglePlayer;
			mGameState = GameState::Playing;
		}
		if (state[SDL_SCANCODE_2] || state[SDL_SCANCODE_KP_2])
		{
			mGameMode = GameMode::MultiPlayer;
			mGameState = GameState::Playing;
		}
		if (state[SDL_SCANCODE_2] || state[SDL_SCANCODE_KP_3])
		{
			mGameMode = GameMode::IA;
			mGameState = GameState::Playing;
		}
	}
	else
	{
		// Update paddle direction based on W/S keys - atualize a direção da raquete com base na entrada do jogador
		// W -> move a raquete para cima, S -> move a raquete para baixo
		mPaddleDir = 0;
		if (state[SDL_SCANCODE_W])
		{
			mPaddleDir -= 1;
		}
		if (state[SDL_SCANCODE_S])
		{
			mPaddleDir += 1;
		}
	}
}

void Game::UpdateGame()
{
	// Espere que 16ms tenham passado desde o último frame - limitando os frames
	while (!SDL_TICKS_PASSED(SDL_GetTicks(), mTicksCount + 16))
		;

	// Delta time é a diferença em ticks do último frame
	// (convertido pra segundos) - calcula o delta time para atualização do jogo
	float deltaTime = (SDL_GetTicks() - mTicksCount) / 1000.0f;

	// "Clamp" (lima/limita) valor máximo de delta time
	if (deltaTime > 0.05f)
	{
		deltaTime = 0.05f;
	}

	// atualize a contagem de ticks par ao próximo frame
	mTicksCount = SDL_GetTicks();

	// atualiza a posição da raquete
	if (mPaddleDir != 0)
	{
		mPaddlePos.y += mPaddleDir * 300.0f * deltaTime;
		// verifique que a raquete não se move para fora da tela - usamos "thickness", que definimos como a altura dos elementos
		if (mPaddlePos.y < (paddleH / 2.0f + thickness))
		{
			mPaddlePos.y = paddleH / 2.0f + thickness;
		}
		else if (mPaddlePos.y > (768.0f - paddleH / 2.0f - thickness))
		{
			mPaddlePos.y = 768.0f - paddleH / 2.0f - thickness;
		}
	}

	for (auto& mBall : mBalls) {
		// atualiza a posição da bola com base na sua velocidade
		mBall.position.x += mBall.velocity.x * deltaTime;
		mBall.position.y += mBall.velocity.y * deltaTime;

		// atualiza a posição da bola se ela colidiu com a raquete
		float diff = mPaddlePos.y - mBall.position.y;
		//pegue o valor absoluto de diferença entre o eixo y da bolinha e da raquete
		//isso é necessário para os casos em que no próximo frame a bolinha ainda não esteja tão distante da raquete
		//verifica se a bola está na area da raquete e na mesma posição no eixo x
		diff = (diff > 0.0f) ? diff : -diff;
		if (
			// A diferença no eixo y y-difference is small enough
			diff <= paddleH / 2.0f &&
			// Estamos na posicao x correta
			mBall.position.x <= 25.0f && mBall.position.x >= 20.0f &&
			// A bolinha está se movendo para a esquerda
			mBall.velocity.x < 0.0f)
		{
			mBall.velocity.x *= -1.0f;

			#pragma region score board
			string newTitle = "Placar: " + to_string(++playerBoard);
			SDL_SetWindowTitle(mWindow, newTitle.c_str());
			#pragma endregion

			#pragma region add new ball on increase 3 points
			if (playerBoard % 1 == 0)
			{
				const float distanceFromCollidedBall = 10.0f;
				const float velocityIncreaseFactor = 15.0f;

				mBalls.push_back({
					{ mPaddlePos.x + thickness + distanceFromCollidedBall, mBall.position.y + thickness + distanceFromCollidedBall },
					{ mBall.velocity.x, -(mBall.velocity.y + velocityIncreaseFactor) }
					});
			}
			#pragma endregion
		}

		//Verifica se a bola saiu da tela (no lado esquerdo, onde é permitido)
		//Se sim, encerra o jogo
		// 
		else if (mBall.position.x <= 0.0f)
		{
			mIsRunning = false;
			mBall.velocity.x *= -1.0f;
		}

		// Atualize (negative) a velocidade da bola se ela colidir com a parede do lado direito
		// 
		else if (mBall.position.x >= (1024.0f - thickness) && mBall.velocity.x > 0.0f)
		{
			mBall.velocity.x *= -1.0f;
		}

		// Atualize (negative) a posição da bola se ela colidir com a parede de cima
		// 
		if (mBall.position.y <= thickness && mBall.velocity.y < 0.0f)
		{
			mBall.velocity.y *= -1;
		}

		// Atualize (negative) a posição da bola se ela colidiu com a parede de baixo
		// Did the ball collide with the bottom wall?
		else if (mBall.position.y >= (768 - thickness) &&
			mBall.velocity.y > 0.0f)
		{
			mBall.velocity.y *= -1;
		}

		// verifica se houve colisão com outras bolas e faz as bolas irem para direções opostas
		for (auto& mBallAux : mBalls) {
			if (mBall.position.x < mBallAux.position.x + thickness &&
				mBall.position.x + thickness > mBallAux.position.x &&
				mBall.position.y < mBallAux.position.y + thickness &&
				mBall.position.y + thickness > mBallAux.position.y
				)
			{
				mBall.velocity.x *= -1;
				mBallAux.velocity.x *= -1;
			}
		}
	}

}

//Desenhando a tela do jogo
void Game::GenerateOutput()
{
	SDL_Surface* surface = SDL_LoadBMP("campo.bmp");
	SDL_Texture* texture = SDL_CreateTextureFromSurface(mRenderer, surface);

	SDL_FreeSurface(surface);

	// limpa o back buffer
	//SDL_SetRenderDrawColor(renderer, 0xff000000, 0x00ff0000, 0x0000ff00, 0x000000ff);
	SDL_SetRenderDrawColor(mRenderer, 0x00, 0x00, 0x00, 0x00);
	SDL_RenderClear(mRenderer);
	SDL_RenderCopy(mRenderer, texture, NULL, NULL);

	// Desenha as paredes - mudamos a cor de mRenderer para o desenho dos retangulos que formaram as paredes
	SDL_SetRenderDrawColor(mRenderer, 255, 0, 255, 255);

	// Desenha a parede de cima - desenhando um retangulo no topo da tela, coordenada x e y = 0, 0 representa o topo esquerdo
	//primeiro definimos as coordenadas e tamanhos do triangulo
	SDL_Rect wall{
		0,			// Top left x
		0,			// Top left y
		1024,		// Width
		thickness	// Height
	};
	SDL_RenderFillRect(mRenderer, &wall);//finalmente, desenhamos um retangulo no topo

	SDL_SetRenderDrawColor(mRenderer, 0, 0, 255, 255);

	//desenhamos as outras paredes apenas mudando as coordenadas de wall

	// parede de baixo
	wall.y = 768 - thickness;
	SDL_RenderFillRect(mRenderer, &wall);

	SDL_SetRenderDrawColor(mRenderer, 255, 0, 0, 255);

	// parede da direita
	wall.x = 1024 - thickness;
	wall.y = 0;
	wall.w = 1024;
	wall.h = thickness;
	SDL_RenderFillRect(mRenderer, &wall);

	//como as posições da raquete e da bola serão atualizadas a cada iteração do game loop, criamos "membros" na classe
	//Game.h para tal

	//mudar a cor da raquete
	SDL_SetRenderDrawColor(mRenderer, 0, 255, 0, 255);

	//desenhando a raquete - usando mPaddlePos que é uma struc de coordenada que foi definida em Game.h
	// 
	SDL_Rect paddle{
		static_cast<int>(mPaddlePos.x),//static_cast converte de float para inteiros, pois SDL_Rect trabalha com inteiros
		static_cast<int>(mPaddlePos.y - paddleH / 2),
		thickness,
		static_cast<int>(paddleH)
	};
	SDL_RenderFillRect(mRenderer, &paddle);

	for (auto& mBall : mBalls) {
		//desenhando a bola - usando mBallPos que é uma struc de coordenadas definida como membro em Game.h

		//mudar a cor do renderizador para a bola
		SDL_SetRenderDrawColor(mRenderer, 255, 255, 0, 255);
		// Draw ball
		SDL_Rect ball{
			static_cast<int>(mBall.position.x - thickness / 2),
			static_cast<int>(mBall.position.y - thickness / 2),
			thickness,
			thickness
		};
		SDL_RenderFillRect(mRenderer, &ball);

		SDL_SetRenderDrawColor(mRenderer, 255, 255, 0, 255);
	}


	// Swap front buffer and back buffer
	SDL_RenderPresent(mRenderer);
}

//Para encerrar o jogo
void Game::Shutdown()
{
	SDL_DestroyRenderer(mRenderer);//encerra o renderizador
	SDL_DestroyWindow(mWindow);//encerra a janela aberta
	SDL_Quit();//encerra o jogo
}
