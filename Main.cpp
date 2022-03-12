// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

/*
Antonio Gostoso Me Mama		RA: 22.119.001-0
Gabriel Bueno Vila Real		RA: 22.119.077-0
Henrique Vital de Carvalho	RA: 22.119.078-8
João Vitor Dias dos Santos	RA: 22.119.006-9
Weverson Pereira da Silva	RA: 22.119.004-4
*/

#include "Game.h"

int main(int argc, char** argv)
{
	Game game;
	bool success = game.Initialize();

	if (success)
	{
		game.RunLoop();
	}

	
	game.Shutdown();
	return 0;
}
