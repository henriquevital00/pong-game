#pragma once

#include "SDL/SDL.h"
#include "Utils.h"


class Ball
{
public:
	Ball(
		Vector2 position = Vector2::Zero(),
		float radius = 15.f,
		float speed = 1.f,
		Vector2 velocity = Vector2::Zero(),
		Vector2 acceleration = Vector2::Zero(),
		Vector4 color = Vector4(255, 255, 0, 255)
	);
	void Draw(SDL_Renderer* renderer);


	Vector2 position;
	float radius;
	float speed;
	Vector2 velocity;
	Vector2 acceleration;

	Vector4 color;
};
