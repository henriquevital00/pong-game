// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#pragma once
#include "SDL/SDL.h"
#include <stdio.h>
#include <vector>
#include <string>
#include <math.h>
#include "Paddle.h"
#include "Ball.h"
#include "Utils.h"

using namespace std;


enum class GameState { StartScreen, Playing };

enum class GameMode { SinglePlayer, MultiPlayer, IA };


class Game
{
public:
	Game();
	bool Initialize();
	void LoadBackground();
	void RunLoop();
	void Shutdown();
private:
	void ProcessInput();
	void UpdateGame();
	void GenerateOutput();
	bool LoadBitmap(
		std::string bitmapFileName,
		SDL_Surface* surface,
		SDL_Texture* texture,
		SDL_Rect* screenDest,
		SDL_Renderer* renderer
	);

	SDL_Window* window;

	SDL_Surface* screenSurface = NULL;
	SDL_Surface* menuSurface = NULL;
	SDL_Texture* menuTexture = NULL;

	SDL_Surface* fieldSurface = NULL;
	SDL_Texture* fieldTexture = NULL;

	SDL_Renderer* renderer = NULL;
	
	Uint32 ticksCount;
	
	bool isRunning;

	GameState gameState;
	GameMode gameMode;


	Paddle paddle;
	vector<Ball> balls;
	int playerBoard;

	std::string menuBitmapName;
	std::string fieldBitmapName;
};
