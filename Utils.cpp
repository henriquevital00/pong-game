#include "Utils.h"


Vector2::Vector2(float x, float y) :x(x), y(y) {}

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

float Utils::EuclidianDistance(Vector2 p1, Vector2 p2)
{
	return sqrt(pow(p1.x - p2.x, 2.0f) + pow(p1.y - p2.y, 2.0f));
}

bool Utils::RectangleIntersection(Vector2 tl1, Vector2 br1, Vector2 tl2, Vector2 br2)
{
    if (
        tl1.x == br1.x 
        || tl1.y == br1.y 
        || tl2.x == br2.x
        || tl2.y == br2.y
    ) {
        return false;
    }

    if (tl1.x >= br2.x || tl2.x >= br1.x)
        return false;

    if (br1.y >= tl2.y || br2.y >= tl1.y)
        return false;
        
    return true;
}

int Utils::RandNumber(int min, int max)
{
    srand(time(NULL));
    return rand() % (max - min + 1) + min;
}
