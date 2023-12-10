#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "glm/glm.hpp"

#include <iostream>
#include <vector>
#include <sstream>
#include <chrono>
#include <thread>
#include <ctime>
#include <unordered_map>

#include "Camera.h"
#include "Chunk.h"
#include "Mesh.h"

#include "ShaderClass.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "VertexArray.h"


class Renderer
{
public:
	Renderer(unsigned int wWidth, unsigned int wHeight);
	~Renderer();

	void Loop();
private:
	void InitializeGLFW();
	void InitializeGLAD();

	void processInput();

	void ClearScreen();

	std::vector<float> GenVertices();

	// -------------------

	unsigned int width;
	unsigned height;

	GLFWwindow* window;
	Camera* camera;
	Shader* shader;

	Mesh* mesh;
	std::vector<Chunk> chunks;
};
