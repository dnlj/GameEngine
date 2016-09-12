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
#include <engine/Uniform.hpp>

// TODO: Maybe make this take an vector of Shaders or something
// TODO: should i make ShaderPrograms like shader pointers? so that i dont have to wory abotu passing references around? need to implement copy/move/whatever 
// TODO: Need to simplify this class so that there isnt so many functions to call when setting one up
namespace engine {
	class ShaderProgram {
		public:
			ShaderProgram(const std::vector<Shader> &shaders, bool deleteShaders=false); // Pretty sure this should be using rvalue/move/forwqard/whatever
			~ShaderProgram();
			// TODO: In the future convert this to be like the texture/mesh class so we dont have to worry about this
			ShaderProgram(const ShaderProgram &program) = delete; // NOTE: This must be here since the deconstructor callse glDeleteProgram


			void detachShaders();
			void checkLinkStatus();
			GLint getAttribLocation(const char *name) const; // TODO: Conver to use const std::string &
			GLint getUniformLocation(const char *name) const;// TODO: Conver to use const std::string &
			GLuint getProgram() const;
			void use() const;
			void loadProgramUniforms();
			const std::vector<Uniform>& getProperties() const;

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
			std::vector<Uniform> properties;
	};
}