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
// TODO: Need to simplify this class so that there isnt so many functions to call when setting one up
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
			GLint getAttribLocation(const char *name) const; // TODO: Conver to use const std::string &
			GLint getUniformLocation(const char *name) const;// TODO: Conver to use const std::string &
			GLuint get() const;
			void use();
			void loadProgramUniforms();
			const std::vector<ShaderProgram::UniformPair>& getProperties() const;

			// TODO: Look into performance of const refernece vs by value for these functions since they are simple types
			// TODO: Should probably pass a pointer or reference to data instead so we dont have to copy it
			// TODO: Untested
			void setUniformInt(const unsigned int &index, std::vector<GLubyte> data);
			// TODO: Untested
			void setUniformUInt(const unsigned int &index, std::vector<GLubyte> data);
			// TODO: Untested
			void setUniformFloat(const unsigned int &index, std::vector<GLubyte> data);

			

		private:
			GLuint program;
			std::vector<ShaderProgram::UniformPair> properties;
	};
}