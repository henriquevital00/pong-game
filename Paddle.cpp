#include "Paddle.h"

Paddle::Paddle(
	Vector2 position,
	float height,
	float width,
	float speed,
	Vector2 direction,
	Vector4 color
)
	:position(position),
	height(height),
	width(width),
	speed(speed),
	direction(direction),
	color(color)
{
}

void Paddle::Draw(SDL_Renderer* renderer) {
	SDL_SetRenderDrawColor(
		renderer,
		this->color.x,
		this->color.y,
		this->color.w,
		this->color.z
	);

	SDL_Rect paddle{
		static_cast<int>(this->position.x),//static_cast converte de float para inteiros, pois SDL_Rect trabalha com inteiros
		static_cast<int>(this->position.y - this->height / 2),
		this->width,
		static_cast<int>(this->height)
	};

	SDL_RenderFillRect(renderer, &paddle);
}