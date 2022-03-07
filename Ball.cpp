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
		static_cast<int>(this->position.x - this->radius / 2),
		static_cast<int>(this->position.y - this->radius / 2),
		this->radius,
		this->radius
	};

	SDL_RenderFillRect(renderer, &ball);
}

bool Ball::DidColideWithFirstPaddle(Paddle* paddle)
{
	float diffY = abs(paddle->position.y - position.y);

	float distance = Utils::EuclidianDistance(paddle->position, position);

	if (
		diffY <= paddle->height / 2.0f
		&& distance <= radius + paddle->width
		&& velocity.x < 0.0f
	) {

		SDL_Log("Ball collided with paddle");
		return true;
	}


	return false;
}

bool Ball::DidColideWithSecondPaddle(Paddle* paddle)
{
	float diffY = abs(paddle->position.y - position.y);

	float distance = Utils::EuclidianDistance(paddle->position, position);

	if (
		diffY <= paddle->height / 2.0f
		&& distance <= radius/2.f + paddle->width/2.f
		&& velocity.x > 0.0f
		) {

		SDL_Log("Ball collided with paddle");
		return true;
	}


	return false;
}