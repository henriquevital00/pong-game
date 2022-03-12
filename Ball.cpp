#include "Ball.h"
#include <math.h>

Ball::Ball(
	Vector2 position,
	float radius,
	float speed,
	Vector2 velocity,
	Vector2 acceleration,
	Vector4 color
)
	:position(position),
	radius(radius),
	speed(speed),
	velocity(velocity),
	acceleration(acceleration),
	color(color)
{
	topLeft = Vector2(
		position.x - radius / 2.f,
		position.y - radius / 2.f
	);

	bottomRight = Vector2(
		position.x + radius / 2.f,
		position.y + radius / 2.f
	);
}

void Ball::Draw(SDL_Renderer* renderer) {
	SDL_SetRenderDrawColor(
		renderer,
		this->color.x,
		this->color.y,
		this->color.w,
		this->color.z
	);

	SDL_Rect ball{
		static_cast<int>(this->position.x),
		static_cast<int>(this->position.y),
		this->radius,
		this->radius
	};

	SDL_RenderFillRect(renderer, &ball);
}

bool Ball::DidColideWithFirstPaddle(Paddle* paddle)
{
	bool hasCollided = position.x < paddle->position.x + paddle->width &&
		position.x + radius > paddle->position.x &&
		position.y < paddle->position.y + paddle->height &&
		position.y + radius > paddle->position.y;

	if (hasCollided)
		SDL_Log("Ball collided with paddle 1");

	return hasCollided;
}

bool Ball::DidColideWithSecondPaddle(Paddle* paddle)
{
	bool hasCollided = position.x <= paddle->position.x + paddle->width &&
		position.x + radius >= paddle->position.x &&
		position.y <= paddle->position.y + paddle->height &&
		position.y + radius >= paddle->position.y;

	if (hasCollided) 
		SDL_Log("Ball collided with paddle 2");

	return hasCollided;
}