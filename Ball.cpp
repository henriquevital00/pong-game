#include "Ball.h"

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
		position.x,
		position.y
	);

	bottomRight = Vector2(
		position.x + radius,
		position.y + radius
	);
}

void Ball::Draw(SDL_Renderer* renderer) {
	SDL_SetRenderDrawColor(
		renderer,
		color.x,
		color.y,
		color.w,
		color.z
	);

	SDL_Rect ball{
		static_cast<int>(position.x),
		static_cast<int>(position.y),
		radius,
		radius
	};


	SDL_RenderFillRect(renderer, &ball);
}

bool Ball::DidCollideWithPaddle(Paddle* paddle)
{
	const bool hasCollided = position.x <= paddle->position.x + paddle->width &&
		position.x + radius >= paddle->position.x &&
		position.y <= paddle->position.y + paddle->height &&
		position.y + radius >= paddle->position.y;

	if (hasCollided)
		SDL_Log("Colidiu com raquete");

	return hasCollided;
}

void Ball::InvertVelocityOnPaddleCollide(Paddle* paddle, bool hasToUpdateSpeed)
{
	if (hasToUpdateSpeed)
	{
		speed += 0.01f;
	}

	velocity.x *= -1;
	
	#pragma region vertical collision 
	float topPositionBeforeCollide = (position.y + radius) - velocity.y;
	float bottomPositionBeforeCollide = position.y - velocity.y;

	bool isTopCollision = topPositionBeforeCollide < paddle->position.y;
	bool isBottomCollision = bottomPositionBeforeCollide > paddle->position.y + paddle->height;

	if (isTopCollision || isBottomCollision)
	{
		velocity.y *= -1;
	}
	# pragma endregion 
}

void Ball::CheckCollisionWithAnotherBall(Ball* ball)
{
	if (ball == this) return; // prevents compare a ball with itself

	if (position.x <= ball->position.x + ball->radius &&
		position.x + radius >= ball->position.x &&
		position.y <= ball->position.y + ball->radius &&
		position.y + ball->radius >= ball->position.y
	)
	{
		velocity.x *= -1;
		ball->velocity.x *= -1;

		#pragma region vertical collision 
		float topPositionBeforeCollide = (position.y + radius) - velocity.y;
		float bottomPositionBeforeCollide = position.y - velocity.y;

		bool isTopCollision = topPositionBeforeCollide < ball->position.y;
		bool isBottomCollision = bottomPositionBeforeCollide > ball->position.y + ball->radius;

		if (isTopCollision || isBottomCollision)
		{
			velocity.y *= -1;
			ball->velocity.y *= -1;
		}
		# pragma endregion 
	}
}

