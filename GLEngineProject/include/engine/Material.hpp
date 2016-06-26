#pragma once

#include <engine/ShaderProgram.hpp>
#include <engine/MaterialParameter.hpp>

namespace engine {

	// TODO: look into Uniform Buffer Objects
	// TODO: Should also look into standardizing names used in the shader for convenience
	class Material {
		public:
			Material(ShaderProgram &program);
			~Material();

			void loadParameters();

			// TODO: Add matrix/vector/array types
			// TODO: All these are untested
			///////////////////////////////////////////////////
			// GLint
			///////////////////////////////////////////////////
			// TODO: Add proper doxygen
			void setParameter(const std::size_t &indx, const GLint &data);
			void setParameter(const std::string &name, const GLint &data);

			///////////////////////////////////////////////////
			// GLuint
			///////////////////////////////////////////////////
			// TODO: Add proper doxygen
			void setParameter(const std::size_t &indx, const GLuint &data);
			void setParameter(const std::string &name, const GLuint &data);

			///////////////////////////////////////////////////
			// GLfloat
			///////////////////////////////////////////////////
			// TODO: Add proper doxygen
			void setParameter(const std::size_t &indx, const GLfloat &data);
			void setParameter(const std::string &name, const GLfloat &data);

		private:
			using ShaderProgramFuncPtr = void (ShaderProgram::*)(const size_t &index, std::vector<GLubyte> data);
	
			ShaderProgram &program; // TODO: These programs should be shared between materials, with just the uniforms updating. Probably need to make something like i have for meshes, where it cashes and loads by index
			std::vector<MaterialParameter> values;

			template<typename T>
			void setParameterInternal(const std::size_t &index, const T &data, ShaderProgramFuncPtr func);

			template<typename T>
			void setParameterInternal(const std::string &name, const T &data, ShaderProgramFuncPtr func);
	};

	template<typename T>
	void Material::setParameterInternal(const std::size_t &index, const T &data, ShaderProgramFuncPtr func) {
		const std::size_t size = sizeof(T);
	
		MaterialParameter param;
		param.func = func;
		param.data.resize(size);
		param.use = true;
		
		memcpy(&param.data[0], &data, size);
		
		values[index] = param;
	}
	
	template<typename T>
	void Material::setParameterInternal(const std::string &name, const T &data, ShaderProgramFuncPtr func) {
		auto &properties = program.getProperties();
		
		for (std::size_t i=0; i < properties.size(); i++) {
			if (properties[i].name == name) {
				return setParameterInternal(i, data, func);
			}
		}
		
		engine_error("No uniform with the name \"" + name + "\" found in the current program");
	}
} // namespace: engine