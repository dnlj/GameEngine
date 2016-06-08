#pragma once

// Includes
#include <string>
#include <iostream>

// glLoadGen
#include "glloadgen\gl_core_4_4.h"

// Engine
#include <engine/engine.hpp>

// My includes
#include "Shader.h"


class Program {
	public:
		Program();
		~Program();
		void attachShader(Shader &shdr);
		void deleteProgram();
		void link();
		void detachShaders();
		void checkLinkStatus();
		GLint getAttribLocation(const char *name) const;
		GLint getUniformLocation(const char *name) const;
		GLuint get();
		void use();
	private:
		GLuint program;
};