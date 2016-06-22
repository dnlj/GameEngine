#pragma once

// Includes
#include <string>
#include <iostream>
#include <vector>

// glLoadGen
#include <glloadgen/gl_core_4_5.h>

// Engine
#include <engine/engine.hpp>
#include <engine/Shader.hpp>
#include <engine/util.hpp>

namespace engine {
	class ShaderProgram {
		public:
			ShaderProgram();
			~ShaderProgram();
			void attachShader(Shader &shdr);
			void deleteProgram();
			void link();
			void detachShaders();
			void checkLinkStatus();
			GLint getAttribLocation(const char *name) const;
			GLint getUniformLocation(const char *name) const;
			GLuint get();
			void use();

			void loadProgramUniforms();

			class UniformPair { // TODO: Rename? idk
				public:
					UniformPair(const std::string &n, const GLenum &t) : name(n), type(t) {};
					std::string name;
					GLenum type;
			};

		private:
			GLuint program;
			std::vector<UniformPair> properties;
	};
}