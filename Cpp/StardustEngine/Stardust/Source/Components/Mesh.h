#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <string>

struct Mesh
{
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> normals;
};



static std::vector<glm::vec3> cube_vertices =
{
	glm::vec3(-1.0f, -1.0f, -1.0f),
	glm::vec3(1.0f, -1.0f, -1.0f),
	glm::vec3(1.0f, 1.0f, -1.0f),
	glm::vec3(1.0f, 1.0f, -1.0f),
	glm::vec3(-1.0f, 1.0f, -1.0f),
	glm::vec3(-1.0f, -1.0f, -1.0f),

	glm::vec3(-1.0f, -1.0f, 1.0f),
	glm::vec3(1.0f, -1.0f, 1.0f),
	glm::vec3(1.0f, 1.0f, 1.0f),
	glm::vec3(1.0f, 1.0f, 1.0f),
	glm::vec3(-1.0f, 1.0f, 1.0f),
	glm::vec3(-1.0f, -1.0f, 1.0f),

	glm::vec3(-1.0f, 1.0f, 1.0f),
	glm::vec3(-1.0f, 1.0f, -1.0f),
	glm::vec3(-1.0f, -1.0f, -1.0f),
	glm::vec3(-1.0f, -1.0f, -1.0f),
	glm::vec3(-1.0f, -1.0f, 1.0f),
	glm::vec3(-1.0f, 1.0f, 1.0f),

	glm::vec3(1.0f, 1.0f, 1.0f),
	glm::vec3(1.0f, 1.0f, -1.0f),
	glm::vec3(1.0f, -1.0f, -1.0f),
	glm::vec3(1.0f, -1.0f, -1.0f),
	glm::vec3(1.0f, -1.0f, 1.0f),
	glm::vec3(1.0f, 1.0f, 1.0f),

	glm::vec3(-1.0f, -1.0f, -1.0f),
	glm::vec3(1.0f, -1.0f, -1.0f),
	glm::vec3(1.0f, -1.0f, 1.0f),
	glm::vec3(1.0f, -1.0f, 1.0f),
	glm::vec3(-1.0f, -1.0f, 1.0f),
	glm::vec3(-1.0f, -1.0f, -1.0f),

	glm::vec3(-1.0f, 1.0f, -1.0f),
	glm::vec3(1.0f, 1.0f, -1.0f),
	glm::vec3(1.0f, 1.0f, 1.0f),
	glm::vec3(1.0f, 1.0f, 1.0f),
	glm::vec3(-1.0f, 1.0f, 1.0f),
	glm::vec3(-1.0f, 1.0f, -1.0f)
};

static std::vector<glm::vec3> cube_normals =
{
	glm::vec3(0.0f, 0.0f, -1.0f),
	glm::vec3(0.0f, 0.0f, -1.0f),
	glm::vec3(0.0f, 0.0f, -1.0f),
	glm::vec3(0.0f, 0.0f, -1.0f),
	glm::vec3(0.0f, 0.0f, -1.0f),
	glm::vec3(0.0f, 0.0f, -1.0f),

	glm::vec3(0.0f, 0.0f, 1.0f),
	glm::vec3(0.0f, 0.0f, 1.0f),
	glm::vec3(0.0f, 0.0f, 1.0f),
	glm::vec3(0.0f, 0.0f, 1.0f),
	glm::vec3(0.0f, 0.0f, 1.0f),
	glm::vec3(0.0f, 0.0f, 1.0f),

	glm::vec3(-1.0f, 0.0f, 0.0f),
	glm::vec3(-1.0f, 0.0f, 0.0f),
	glm::vec3(-1.0f, 0.0f, 0.0f),
	glm::vec3(-1.0f, 0.0f, 0.0f),
	glm::vec3(-1.0f, 0.0f, 0.0f),
	glm::vec3(-1.0f, 0.0f, 0.0f),

	glm::vec3(1.0f, 0.0f, 0.0f),
	glm::vec3(1.0f, 0.0f, 0.0f),
	glm::vec3(1.0f, 0.0f, 0.0f),
	glm::vec3(1.0f, 0.0f, 0.0f),
	glm::vec3(1.0f, 0.0f, 0.0f),
	glm::vec3(1.0f, 0.0f, 0.0f),

	glm::vec3(0.0f, -1.0f, 0.0f),
	glm::vec3(0.0f, -1.0f, 0.0f),
	glm::vec3(0.0f, -1.0f, 0.0f),
	glm::vec3(0.0f, -1.0f, 0.0f),
	glm::vec3(0.0f, -1.0f, 0.0f),
	glm::vec3(0.0f, -1.0f, 0.0f),

	glm::vec3(0.0f, 1.0f, 0.0f),
	glm::vec3(0.0f, 1.0f, 0.0f),
	glm::vec3(0.0f, 1.0f, 0.0f),
	glm::vec3(0.0f, 1.0f, 0.0f),
	glm::vec3(0.0f, 1.0f, 0.0f),
	glm::vec3(0.0f, 1.0f, 0.0f)
};

static std::vector<glm::vec3> quad_vertices =
{
	glm::vec3(-0.5f, -0.5f, -0.0f),
	glm::vec3(0.5f, -0.5f, -0.0f),
	glm::vec3(0.5f, 0.5f, -0.0f),
	glm::vec3(0.5f, 0.5f, -0.0f),
	glm::vec3(-0.5f, 0.5f, -0.0f),
	glm::vec3(-0.5f, -0.5f, -0.0f)
};

static std::vector<glm::vec3> quad_normals =
{
	glm::vec3(0.0f, 0.0f, -1.0f),
	glm::vec3(0.0f, 0.0f, -1.0f),
	glm::vec3(0.0f, 0.0f, -1.0f),
	glm::vec3(0.0f, 0.0f, -1.0f),
	glm::vec3(0.0f, 0.0f, -1.0f),
	glm::vec3(0.0f, 0.0f, -1.0f)
};





static std::vector<glm::vec3> pyramid_vertices =
{
	glm::vec3(-0.5f, -0.0f, -0.5f),
	glm::vec3(-0.5f, -0.0f, 0.5f),
	glm::vec3(0.5f, 0.0f, 0.5f),
	glm::vec3(-0.5f, 0.0f, -0.5f),
	glm::vec3(0.5f, 0.0f, 0.5f),
	glm::vec3(0.5f, -0.0f, -0.5f),

	glm::vec3(-0.5f, -0.0f, 0.5),
	glm::vec3(0.5f, -0.0f, 0.5),
	glm::vec3(0.0f, 1.0f, 0.0),

	glm::vec3(-0.5f, 0.0f, -0.5),
	glm::vec3(-0.5f, 0.0f, 0.5),
	glm::vec3(-0.0f, 1.0f, 0.0),

	glm::vec3(-0.5f, 0.0f, -0.5),
	glm::vec3(0.5f, 0.0f, -0.5),
	glm::vec3(-0.0f, 1.0f, 0.0),

	glm::vec3(0.5f, 0.0f, -0.5),
	glm::vec3(0.5f, 0.0f, 0.5),
	glm::vec3(-0.0f, 1.0f, 0.0)
};

static std::vector<glm::vec3> pyramid_normals;


static std::vector<glm::vec3> sphere_vertices =
{
};

static std::vector<glm::vec3> sphere_normals =
{
};

struct CubeMesh
{
};


struct QuadMesh
{
};

struct PyramidMesh
{
};

struct SphereMesh
{
};

struct Model3D
{
	std::string name;
	std::string path;
};