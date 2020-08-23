#pragma once

// 2D Random
static float random(glm::vec2 st) {

	return (float)glm::fract(
		sin(glm::dot(st,
			glm::vec2(12.9898, 78.233)))
		* 43758.5453123)

		;
}

// 2D Noise based on Morgan McGuire @morgan3d
// https://www.shadertoy.com/view/4dS3Wd
static float noise(glm::vec2 st) {
	glm::vec2 i = floor(st);
	glm::vec2 f = fract(st);

	// Four corners in 2D of a tile
	float a = random(i);
	float b = random(i + glm::vec2(1.0, 0.0));
	float c = random(i + glm::vec2(0.0, 1.0));
	float d = random(i + glm::vec2(1.0, 1.0));

	// Smooth Interpolation

	// Cubic Hermine Curve.  Same as SmoothStep()
	glm::vec2 f2 = f;
	f2.x *= 2.0;
	f2.x = 3.0f - f2.x;
	f2.y *= 2.0;
	f2.y = 3.0f - f2.y;
	glm::vec2 u = f * f* f2;
	// u = smoothstep(0.,1.,f);

	// Mix 4 coorners percentages
	return (float) glm::mix(a, b, u.x) +
		(c - a)* u.y * (1.0f - u.x) +
		(d - b) * u.x * u.y;
}


/*
// Based on Morgan McGuire @morgan3d
// https://www.shadertoy.com/view/4dS3Wd
static float noise(glm::vec2 st) {
	glm::vec2 i = floor(st);
	glm::vec2 f = fract(st);

	// Four corners in 2D of a tile
	float a = random(i);
	float b = random(i + glm::vec2(1.0, 0.0));
	float c = random(i + glm::vec2(0.0, 1.0));
	float d = random(i + glm::vec2(1.0, 1.0));

	glm::vec2 f2 = f;
	f2.x *= 2.0;
	f2.x = 3.0 - f2.x;
	f2.y *= 2.0;
	f2.y = 3.0 - f2.y;
	glm::vec2 u = f * f* f2;

	return glm::mix(a, b, u.x) +
		(c - a)* u.y * (1.0 - u.x) +
		(d - b) * u.x * u.y;
}*/

#define OCTAVES 6
static float fbm(glm::vec2 st) {
	// Initial values
	float value = 0.0;
	float amplitude = .5;
	float frequency = 0.0;
	//
	// Loop of octaves
	for (int i = 0; i < OCTAVES; i++) {
		value += amplitude * noise(st);
		st *= 2.;
		amplitude *= .5;
	}
	return value;
}



static float Perlin3D(float x, float y, float z) {
	float ab = fbm(glm::vec2(x, y));
	float bc = fbm(glm::vec2(y, z));
	float ac = fbm(glm::vec2(x, z));

	float ba = fbm(glm::vec2(y, x));
	float cb = fbm(glm::vec2(z, y));
	float ca = fbm(glm::vec2(z, x));

	float abc = ab + bc + ac + ba + cb + ca;
	return abc / 6.0f;
}
