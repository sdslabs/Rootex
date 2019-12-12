#pragma once

#include <GLEW/include/GL/glew.h>

void GLClearError()
{
	while (glGetError() != GL_NO_ERROR)
		;
}

// Pass in OpenGL calls for debugging errors while executing OpenGL code
#define GLCall(x)                                                                                    \
	GLClearError();                                                                                  \
	x;                                                                                               \
	while (GLenum error = glGetError())                                                              \
	{                                                                                                \
		int z = toHex(error);                                                                        \
		ERR("OpenGL Error: 0x" + ((z < 0x1000) ? "0" + std::to_string(z) : std::to_string(z))); \
		std::cin.get();                                                                              \
	}
