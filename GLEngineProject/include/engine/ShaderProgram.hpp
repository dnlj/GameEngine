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

// TODO: should i make ShaderPrograms like shader pointers? so that i dont have to wory abotu passing references around? need to implement copy/move/whatever 
namespace engine {
	class ShaderProgram {
		public:
			class UniformPair { // TODO: Rename? idk
				public:
					UniformPair(const std::string &name, const GLenum &type, const GLint &location) : name(name), type(type), location(location) {};
					std::string name;
					GLenum type;
					GLint location;
			};

			ShaderProgram();
			~ShaderProgram();
			void attachShader(Shader &shdr);
			void deleteProgram();
			void link();
			void detachShaders();
			void checkLinkStatus();
			GLint getAttribLocation(const char *name) const;
			GLint getUniformLocation(const char *name) const;
			GLuint get() const;
			void use();
			void loadProgramUniforms();
			const std::vector<ShaderProgram::UniformPair>& getProperties() const;

			//TODO: Should probably pass a pointer or reference to data instead so we dont have to copy it
			// TODO: Untested
			void setUniformInt(const size_t &index, std::vector<GLubyte> data);
			// TODO: Untested
			void setUniformUInt(const size_t &index, std::vector<GLubyte> data);
			// TODO: Untested
			void setUniformFloat(const size_t &index, std::vector<GLubyte> data);

			

		private:
			GLuint program;
			std::vector<ShaderProgram::UniformPair> properties;
	};
}