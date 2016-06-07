#ifndef SHADER_H
#define SHADER_H

// Includes
#include <string>
#include <iostream>

// glLoadGen
#include "glloadgen\gl_core_4_4.h"


class Shader {
	public:
		Shader(GLenum shaderType, std::string source);
		~Shader();
		GLenum getType();
		GLuint getShader();
		void deleteShader();
	private:
		GLenum type;
		GLuint shader;
};

#endif SHADER_H