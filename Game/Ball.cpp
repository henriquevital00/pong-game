#include "Ball.h"

Ball::Ball(
	Vector2 position,
	float radius,
	Vector2 velocity,
	Vector2 acceleration,
	Vector4 color
)
	:position(position),
	radius(radius),
	velocity(velocity),
	acceleration(acceleration),
	color(color)
{
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