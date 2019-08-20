#pragma once

#include <GL/glew.h>
#include <string>
#include <iostream>
#include <fstream>
#include <unordered_map>

class ComputeShader
{
	GLuint csProgram = -1;

public:
	ComputeShader(const std::string shaderFileName);
	ComputeShader() {}
	~ComputeShader();

	void runShader(GLuint num_groups_x, GLuint num_groups_y, GLuint num_groups_z, bool block = false);
};

