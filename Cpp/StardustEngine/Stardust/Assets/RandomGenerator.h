#pragma once

// 2D Random
static float random(glm::vec2 st) {

	return (float)glm::fract(
		sin(glm::dot(st,
			glm::vec2(12.9898f, 78.233f)))
		* 43758.5453123f)

		;
}




// 2D Noise based on Morgan McGuire @morgan3d
// https://www.shadertoy.com/view/4dS3Wd
static float noise(glm::vec2 st) {
	glm::vec2 i = floor(st);
	glm::vec2 f = fract(st);

	// Four corners in 2D of a tile
	float a = random(i);
	float b = random(i + glm::vec2(1.0f, 0.0f));
	float c = random(i + glm::vec2(0.0f, 1.0f));
	float d = random(i + glm::vec2(1.0f, 1.0f));

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

#define OCTAVES 1



static float fbm(float x, float z) {
	glm::vec2 st = glm::vec2(x, z);
	// Initial values
	float value = 0.0f;
	float amplitude = 0.5f;
	float frequency = 0.4f;
	//
	// Loop of octaves
	for (int i = 0; i < 1; i++) {
		value += amplitude * noise(st);
		st *= 2.0f;
		amplitude *= 0.5f;
	}
	return value;
}



static float Perlin3D(float x, float y, float z) {
	float ab = fbm(x, y);
	float bc = fbm(y, z);
	float ac = fbm(x, z);

	float ba = fbm(y, x);
	float cb = fbm(z, y);
	float ca = fbm(z, x);

	float abc = ab + bc + ac + ba + cb + ca;
	return abc / 6.0f;
}


static float Perlin2D(float x, float z) {
	return fbm(x, z);
}