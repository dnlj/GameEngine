#pragma once

// Includes
#include <string>
#include <iostream>

// glLoadGen
#include <glloadgen\gl_core_4_5.h>

// Engine
#include <engine/engine.hpp>

namespace engine {
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
}