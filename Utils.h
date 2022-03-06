#pragma once

#include <stdio.h>
#include <string>
#include <vector>

using namespace std;

class Vector2 {
public:
	Vector2(float x = .0f, float y = .0f);
	static Vector2 Zero();

	float x;
	float y;
};

class Vector3 {
public:
	Vector3(float x = .0f, float y = .0f, float z = .0f);
	static Vector3 Zero();

	float x;
	float y;
	float z;
};

class Vector4 {
public:
	Vector4(float x = .0f, float y = .0f, float w = .0f, float z = .0f);
	static Vector4 Zero();

	float x;
	float y;
	float w;
	float z;
};