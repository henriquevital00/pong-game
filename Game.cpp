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
	, firstPlayerScore(0)
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

	*windowWidth = *windowWidth / 2.f;
	*windowHeight = *windowHeight / 2.f;

	SDL_Log("Window size: %d x %d", *windowWidth, *windowHeight);

	SDL_Log("Creating window...");
	window = SDL_CreateWindow(
		"P.O.N.G",
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		*windowWidth,	
		*windowHeight,
		NULL
	);

	if (!window)
	{
		SDL_Log("Failed to create window: %s", SDL_GetError());
		return false;
	}

	SDL_Log("Creating renderer...");
	renderer = SDL_CreateRenderer(
		window, 
		-1,		
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

	Game::InitializeVariables();

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

	SDL_RenderCopy(renderer, menuTexture, NULL, NULL);
	SDL_RenderPresent(renderer);

	SDL_UpdateWindowSurface(window);
}

void Game::RunLoop()
{
	fieldSurface = SDL_LoadBMP("campo.bmp");
	fieldTexture = SDL_CreateTextureFromSurface(renderer, fieldSurface);

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

void Game::ProcessMenuInput(const Uint8* keyboardState)
{
	if (keyboardState[SDL_SCANCODE_1] || keyboardState[SDL_SCANCODE_KP_1])
	{
		gameMode = GameMode::SinglePlayer;
		gameState = GameState::Playing;
	}
	else if (keyboardState[SDL_SCANCODE_2] || keyboardState[SDL_SCANCODE_KP_2])
	{
		gameMode = GameMode::MultiPlayer;
		gameState = GameState::Playing;
	}
	else if (keyboardState[SDL_SCANCODE_3] || keyboardState[SDL_SCANCODE_KP_3])
	{
		gameMode = GameMode::IA;
		gameState = GameState::Playing;
	}
}

void Game::ProcessSingleplayerInput(const Uint8* keyboardState)
{
	firstPaddle.direction.y = 0;

	if (keyboardState[SDL_SCANCODE_W])
	{
		firstPaddle.direction.y -= 1;
	}
	if (keyboardState[SDL_SCANCODE_S])
	{
		firstPaddle.direction.y += 1;
	}
}

void Game::ProcessBotInput(const Uint8* keyboardState)
{
	firstPaddle.direction.y = 0;

	if (keyboardState[SDL_SCANCODE_W])
	{
		firstPaddle.direction.y -= 1;
	}
	if (keyboardState[SDL_SCANCODE_S])
	{
		firstPaddle.direction.y += 1;
	}
}

void Game::ProcessMultiplayerInput(const Uint8* keyboardState)
{
	firstPaddle.direction.y = 0;
	secondPaddle.direction.y = 0;

	if (keyboardState[SDL_SCANCODE_W])
	{
		firstPaddle.direction.y -= 1;
	}
	if (keyboardState[SDL_SCANCODE_S])
	{
		firstPaddle.direction.y += 1;
	}

	if (keyboardState[SDL_SCANCODE_UP])
	{
		secondPaddle.direction.y -= 1;
	}
	if (keyboardState[SDL_SCANCODE_DOWN])
	{
		secondPaddle.direction.y += 1;
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

	const Uint8* state = SDL_GetKeyboardState(NULL);
	
	if (state[SDL_SCANCODE_ESCAPE])
	{
		isRunning = false;
	}

	if (gameState == GameState::StartScreen) 
	{
		ProcessMenuInput(state);
	}
	else if (gameState == GameState::Playing)
	{
		if (gameMode == GameMode::SinglePlayer)
		{
			ProcessSingleplayerInput(state);
		}
		else if (gameMode == GameMode::MultiPlayer)
		{
			ProcessMultiplayerInput(state);
		}
		else if (gameMode == GameMode::IA)
		{
			ProcessBotInput(state);
		}
	}
}


void Game::UpdatePaddle(Paddle* paddle, float deltaTime)
{
	if (paddle->direction.y != 0)
	{
		paddle->position.y += paddle->direction.y * paddle->speed * paddle->speedFactor * deltaTime;
		
		if (paddle->position.y < 0)
		{
			paddle->position.y = 0;
		}
		else if (paddle->position.y + paddle->height > *windowHeight)
		{
			paddle->position.y = *windowHeight - paddle->height;
		}
	}
}

void Game::UpdateScoreBoard(int firstPlayerScore, int secondPlayerScore)
{
	string newTitle = "Jogador 01: " + to_string(firstPlayerScore);

	if (secondPlayerScore != -1) {
		newTitle += " | Jogador 02: " + to_string(secondPlayerScore);
	}

	SDL_SetWindowTitle(window, newTitle.c_str());
}

void Game::AddNewBall(Vector2 velocity)
{
	Ball newBall;

	int randomNumber = Utils::RandNumber(2, 7);

	newBall.position = Vector2(
		*windowWidth/2 + *windowWidth/randomNumber,
		*windowHeight/2 + *windowHeight/randomNumber
	);

	newBall.velocity = velocity;

	balls.push_back(newBall);
}

void Game::InitializeVariables() {
	float paddleWidth = 15.0f;
	float paddleMargin = 20.0f;
	firstPaddle = Paddle(Vector2(paddleMargin, *windowHeight / 2.0f));
	secondPaddle = Paddle(Vector2(*windowWidth - (paddleMargin + paddleWidth), *windowHeight / 2.0f));

	Vector2 ballPos = Vector2(*windowWidth / 2.f, *windowHeight / 2.f);
	Vector2 ballVel = Vector2(-200.f, 500.f);
	balls = vector<Ball>();
	balls.push_back(Ball());
	balls[0].position = ballPos;
	balls[0].velocity = ballVel;

	startTiming = SDL_GetTicks();

	firstPlayerScore = 0;
	secondPlayerScore = 0;

	SDL_SetWindowTitle(window, "P.O.N.G");
}

void Game::ResetGame() 
{
	gameState = GameState::StartScreen;
	gameMode = GameMode::None;
	InitializeVariables();
	Game::LoadBackground();
}

void Game::CheckBallCollisionWithWalls(Ball* ball)
{
	if (ball->position.x <= 0.0f)
	{
		SDL_Log("ball position less than 0.0f: (%.2f, %.2f)", ball->position.x, ball->position.y);
		//isRunning = false;
		secondPlayerScore += 1;
		ball->velocity.x *= -1.0f;
	}

	if (gameMode == GameMode::MultiPlayer) {
		if (ball->position.x >= *windowWidth)
		{
			//isRunning = false;
			firstPlayerScore += 1;
			ball->velocity.x *= -1.0f;
		}
	}
	else if (gameMode == GameMode::SinglePlayer) {
		if (ball->position.x >= (*windowWidth - firstPaddle.width) && ball->velocity.x > 0.0f)
		{
			ball->velocity.x *= -1.0f;
		}
	}

	if (ball->position.y <= firstPaddle.width && ball->velocity.y < 0.0f)
	{
		ball->velocity.y *= -1;
	}

	else if (
		ball->position.y >= (*windowHeight - firstPaddle.width)
		&& ball->velocity.y > 0.0f
	) {
		ball->velocity.y *= -1;
	}
}

void Game::UpdateGame()
{
	while (!SDL_TICKS_PASSED(SDL_GetTicks(), ticksCount + 16));

	float deltaTime = (SDL_GetTicks() - ticksCount) / 1000.0f;

	if (deltaTime > 0.05f) {
		deltaTime = 0.05f;
	}

	ticksCount = SDL_GetTicks();

	endTiming = ticksCount;
	secondsElapsed = (endTiming - startTiming) / 1000.f;

	if (gameMode == GameMode::SinglePlayer) {
		UpdatePaddle(&firstPaddle, deltaTime);

		for (auto& ball : balls) {
			ball.position.x += ball.velocity.x * ball.speed * deltaTime;
			ball.position.y += ball.velocity.y * ball.speed * deltaTime;

			if (ball.DidCollideWithPaddle(&firstPaddle)) {
				ball.InvertVelocityOnPaddleCollide(&firstPaddle, false);

				UpdateScoreBoard(++firstPlayerScore);

				if (firstPlayerScore % 3 == 0)
				{
					Vector2 newBallVelocity = Vector2(-200.0f, -ball.velocity.y);
					AddNewBall(newBallVelocity);
				}
			}

			CheckBallCollisionWithWalls(&ball);

			for (auto& collidedBall : balls) {
				ball.CheckCollisionWithAnotherBall(&collidedBall);
			}
		}

	}
	else if (gameMode == GameMode::MultiPlayer) {
		Paddle* paddles[] = { &firstPaddle, &secondPaddle };

		for (auto& ball : balls) 
		{
			ball.position.x += ball.velocity.x * ball.speed * deltaTime;
			ball.position.y += ball.velocity.y * ball.speed * deltaTime;

			for (auto& paddle : paddles)
			{
				UpdatePaddle(paddle, deltaTime);

				if (ball.DidCollideWithPaddle(paddle))
				{
					ball.InvertVelocityOnPaddleCollide(paddle, true);
				}

				UpdateScoreBoard(firstPlayerScore, secondPlayerScore);

				bool firstPlayerWon = firstPlayerScore > 0 && firstPlayerScore % 3 == 0;
				bool secondPlayerWon = secondPlayerScore > 0 && secondPlayerScore % 3 == 0;

				if (firstPlayerWon || secondPlayerWon)
				{
					ResetGame();
				}
			}

			CheckBallCollisionWithWalls(&ball);
		}

	}
	else if (gameMode == GameMode::IA) {
		UpdatePaddle(&firstPaddle, deltaTime);

		for (auto& ball : balls) {
			ball.position.x += ball.velocity.x * ball.speed * deltaTime;
			ball.position.y += ball.velocity.y * ball.speed * deltaTime;

			secondPaddle.position.y = ball.position.y;

			Paddle* collidedPaddle =
				ball.DidCollideWithPaddle(&firstPaddle) ? &firstPaddle :
				ball.DidCollideWithPaddle(&secondPaddle) ? &secondPaddle : nullptr;

			if (collidedPaddle != nullptr) 
			{
				ball.InvertVelocityOnPaddleCollide(collidedPaddle, true);
			}

			CheckBallCollisionWithWalls(&ball);
		}
	}
}

void Game::GenerateOutput()
{
	if (gameMode == GameMode::None) {
		return;
	}

	SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0x00);
	SDL_RenderClear(renderer);
	SDL_RenderCopy(renderer, fieldTexture, NULL, NULL);

	if (gameMode == GameMode::SinglePlayer) {
		firstPaddle.Draw(renderer);
	}
	else if (gameMode == GameMode::MultiPlayer) {
		firstPaddle.Draw(renderer);
		secondPaddle.Draw(renderer);
	}
	else if (gameMode == GameMode::IA) {
		firstPaddle.Draw(renderer);
		secondPaddle.Draw(renderer);
	}

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
