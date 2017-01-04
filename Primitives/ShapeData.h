#pragma once
#include <GL\glew.h>

struct ShapeData
{
	ShapeData() :
		vertices(0), numVertices(0),
		indices(0), numIndices(0),
		uvs(0), numUVs(0){}
	glm::vec3* vertices;
	GLuint numVertices;
	glm::vec2* uvs;
	GLuint numUVs;
	GLushort* indices;
	GLuint numIndices;

	GLsizeiptr vertexBufferSize() const
	{
		return numVertices * sizeof(glm::vec3);
	}
	GLsizeiptr indexBufferSize() const
	{
		return numIndices * sizeof(GLushort);
	}
	GLsizeiptr uvBufferSize() const
	{
		return numUVs * sizeof(glm::vec2);
	}
	void cleanup()
	{
		delete[] indices;
		delete[] vertices;
		delete[] uvs;
	}
};