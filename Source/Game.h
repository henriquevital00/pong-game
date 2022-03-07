// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#pragma once
#include "SDL/SDL.h"
#include <vector>
#include <string>

using namespace std;

// Vector2 struct just stores x/y coordinates
// (for now)
struct Vector2
{
	float x;
	float y;
};

typedef struct {
	// Position of ball
	Vector2 position;
	// Velocity of ball
	Vector2 velocity;
} Ball;


enum class GameState { StartScreen, Playing };

enum class GameMode { SinglePlayer, MultiPlayer, IA };


// Game class
class Game
{
public:
	Game();
	// Initialize the game
	bool Initialize();
	// Coloca um papel de parede bonito
	bool loadBackground();
	// Runs the game loop until the game is over
	void RunLoop();
	// Shutdown the game
	void Shutdown();
private:
	// Helper functions for the game loop
	void ProcessInput();
	void UpdateGame();
	void GenerateOutput();

	// Window created by SDL
	SDL_Window* mWindow;


	//The surface contained by the window
	SDL_Surface* gScreenSurface = NULL;

	//The image we will load and show on the screen
	SDL_Surface* gHelloWorld = NULL;

	SDL_Texture* texture1;

	// Renderer for 2D drawing
	SDL_Renderer* mRenderer;
	// Number of ticks since start of game
	Uint32 mTicksCount;
	// Game should continue to run
	bool mIsRunning;
	// Game state
	GameState mGameState;
	// Game mode
	GameMode mGameMode;


	// Pong specific
	// Direction of paddle
	int mPaddleDir;
	// Position of paddle
	Vector2 mPaddlePos;

	// Balls list
	vector<Ball> mBalls;

	int playerBoard;


};
