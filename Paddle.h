#pragma once
#include "SDL/SDL.h"
#include "Utils.h"

class Paddle
{
public:
	Paddle(
		Vector2 position = Vector2::Zero(),
		float height = 100.0f,
		float width = 15.0f,
		float speed = 300.0f,
		Vector2 direction = Vector2::Zero(),
		Vector4 color = Vector4(0, 255, 0, 255)
	);
	void Draw(SDL_Renderer* renderer);


	Vector2 position;
	float height;
	float width;
	float speed;
	Vector2 direction;

	Vector4 color;

	float speedFactor = 2;

};
