#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#if defined(WIN32) || defined(_WIN32)
#include <Windows.h>
#else
#include <unistd.h>
#endif

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/vec3.hpp> 
#include "shaders/ComputeShader.h"

void printWorkGroupsCapabilities() {
	GLint64  val_array[3];
	GLint64  value;

	glGetInteger64i_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, &val_array[0]);
	glGetInteger64i_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 1, &val_array[1]);
	glGetInteger64i_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 2, &val_array[2]);

	printf("GL_MAX_COMPUTE_WORK_GROUP_COUNT:\n\tx:%u\n\ty:%u\n\tz:%u\n",
		val_array[0], val_array[1], val_array[2]);

	glGetInteger64i_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 0, &val_array[0]);
	glGetInteger64i_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 1, &val_array[1]);
	glGetInteger64i_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 2, &val_array[2]);

	printf("GL_MAX_COMPUTE_WORK_GROUP_SIZE:\n\tx:%u\n\ty:%u\n\tz:%u\n",
		val_array[0], val_array[1], val_array[2]);

	glGetInteger64v(GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS, &value);
	printf("GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS :\n\t%u\n", value);

	glGetInteger64v(GL_MAX_SHADER_STORAGE_BLOCK_SIZE, &value);
	printf("GL_MAX_SHADER_STORAGE_BLOCK_SIZE :\n\t%u\n", value);

	glGetInteger64v(GL_SHADER_STORAGE_BUFFER_SIZE, &value);
	printf("GL_SHADER_STORAGE_BUFFER_SIZE :\n\t%u\n", value);

	glGetInteger64v(GL_MAX_TEXTURE_SIZE, &value);
	printf("GL_MAX_TEXTURE_SIZE :\n\t%u\n", value);

	glGetInteger64v(GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS, &value);
	printf("GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS  :\n\t%u\n", value);

	glGetInteger64v(GL_MAX_COMBINED_SHADER_STORAGE_BLOCKS, &value);
	printf("GL_MAX_COMBINED_SHADER_STORAGE_BLOCKS   :\n\t%u\n", value);

	glGetInteger64v(GL_MAX_UNIFORM_BUFFER_BINDINGS, &value);
	printf("GL_MAX_UNIFORM_BUFFER_BINDINGS    :\n\t%u\n", value);

	glGetInteger64v(GL_UNIFORM_BUFFER_SIZE, &value);
	printf("GL_UNIFORM_BUFFER_SIZE   :\n\t%u\n", value);

	glGetInteger64v(GL_MAX_ATOMIC_COUNTER_BUFFER_BINDINGS, &value);
	printf("GL_MAX_ATOMIC_COUNTER_BUFFER_BINDINGS     :\n\t%u\n", value);

	glGetInteger64v(GL_ATOMIC_COUNTER_BUFFER_SIZE, &value);
	printf("GL_ATOMIC_COUNTER_BUFFER_SIZE   :\n\t%u\n", value);

	glGetInteger64v(GL_MAX_COMPUTE_SHARED_MEMORY_SIZE, &value);
	printf("GL_MAX_COMPUTE_SHARED_MEMORY_SIZE   :\n\t%u\n", value);
}

// for auto error prints, doesnt work
void GLAPIENTRY
MessageCallback(GLenum source,
	GLenum type,
	GLuint id,
	GLenum severity,
	GLsizei length,
	const GLchar* message,
	const void* userParam)
{
	printf("GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
		(type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
		type, severity, message);
}


long getTime() {
	SYSTEMTIME timeEnd;
	GetSystemTime(&timeEnd);
	return (timeEnd.wSecond * 1000) + timeEnd.wMilliseconds;
}

void checkErrors() {
	GLenum err;
	while ((err = glGetError()) != GL_NO_ERROR)
	{
		std::cout << gluErrorString(err) << std::endl;
	}
}

int main(int argc, char ** argv) {
	/* ----- Init window ----- */
	GLFWwindow * window;
	if (!glfwInit()) {
		exit(-1);
	}

	window = glfwCreateWindow(640, 480, "window name", NULL, NULL);

	if (!window) {
		exit(-2);
	}

	glfwMakeContextCurrent(window);


	if (glewInit() != 0) {
		exit(-3);
	}

	glDebugMessageCallback(MessageCallback, 0);				// doesnt work, idk why
	glEnable(GL_DEBUG_OUTPUT);


/* ----- Compute Shader ----- */

	printWorkGroupsCapabilities();

	ComputeShader cs("./Source/shaders/testCompute.shader");

	// SSBO

	float ssboValue[1024];
	for (int i = 0; i < 1024; i++) {
		ssboValue[i] = i;
	}
	float ssboValue2[1024];
	for (int i = 0; i < 1024; i++) {
		ssboValue2[i] = 2*1024-i;
	}

	cs.bindSSBO("info", sizeof(ssboValue), ssboValue, 2);
	cs.bindSSBO("info2", sizeof(ssboValue2), ssboValue2, 60);

	LONG tStart = getTime();
	cs.runShader(2, 1, 1, true);
	LONG tEnd = getTime();

	float* p = (float*)cs.getSSBO("info");
	for (int i = 0; i < 10; i++) {
		std::cout << "ssbo " << p[1024 - 1 - i] << std::endl;

	}

	std::cout << "Time: " << tEnd - tStart << std::endl;
	return 0;
}
