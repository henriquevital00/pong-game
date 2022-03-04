#include "Utils.h"


Vector2::Vector2(float x, float y):x(x),y(y){}

Vector2 Vector2::Zero() {
	return Vector2();
}


Vector3::Vector3(float x, float y, float z) :x(x), y(y), z(z) {}

Vector3 Vector3::Zero() {
	return Vector3();
}


Vector4::Vector4(float x, float y, float w, float z) : x(x), y(y), w(0), z(z) {}

Vector4 Vector4::Zero() {
	return Vector4();
}