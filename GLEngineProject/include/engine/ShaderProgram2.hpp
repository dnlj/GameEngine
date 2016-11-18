#pragma once

// Engine
#include <engine/Resource.hpp>
#include <engine/ShaderProgramData2.hpp>
#include <engine/Texture.hpp>

namespace engine {
	// TODO: move to own file
	class InvalidUniformName : public std::runtime_error {
		public:
			using std::runtime_error::runtime_error;
	};

	class ShaderProgram2 : public Resource<ShaderProgram2, ShaderProgramData2> {
		public:
			using Resource<ShaderProgram2, ShaderProgramData2>::Resource;
			
			~ShaderProgram2() = default;

			GLuint getProgram() const;

			// TODO: split into tpp file
			template<class T>
			void setUniform(const std::string& name, const T& data) {
				setUniform(getUniformIndex(name), data);
			}

			void setUniform(const GLint location, const GLint data);
			void setUniform(const GLint location, const GLuint data);
			void setUniform(const GLint location, const GLfloat data);
			void setUniform(const GLint location, const Texture& data);

			GLint getUniformIndex(const std::string& name);

		private:

		////////////////////////////////////////////////////////////////
		// Static
		////////////////////////////////////////////////////////////////
		public:
			static ShaderProgram2 load(const ResourcePath& path);
	};
}