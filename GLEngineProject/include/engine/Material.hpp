#pragma once

// Engine
#include <engine/ShaderProgram.hpp>
#include <engine/Texture.hpp>
#include <engine/MaterialParameter.hpp>


namespace engine {
	// TODO: Should look into move semantics/perfect forwarding for more complex types
	// TODO: look into Uniform Buffer Objects
	// TODO: Should also look into standardizing names used in the shader for convenience
	// TODO: Need to add textures
	class Material {
		public:
			Material(ShaderProgram &program);
			~Material();

			void loadParameters();

			// TODO: Add matrix/vector/array types
			// TODO: All these are more/less untested
			///////////////////////////////////////////////////
			// GLint
			///////////////////////////////////////////////////
			// TODO: Add proper doxygen
			void setParameter(const unsigned int indx, const GLint& data);
			void setParameter(const std::string& name, const GLint& data);

			///////////////////////////////////////////////////
			// GLuint
			///////////////////////////////////////////////////
			// TODO: Add proper doxygen
			void setParameter(const unsigned int indx, const GLuint& data);
			void setParameter(const std::string& name, const GLuint& data);

			///////////////////////////////////////////////////
			// GLfloat
			///////////////////////////////////////////////////
			// TODO: Add proper doxygen
			void setParameter(const unsigned int indx, const GLfloat& data);
			void setParameter(const std::string& name, const GLfloat& data);

			///////////////////////////////////////////////////
			// Texture
			///////////////////////////////////////////////////
			// TODO: Add proper doxygen
			void setParameter(const unsigned int indx, const Texture& data);
			void setParameter(const std::string& name, const Texture& data);

		private:
			using ShaderProgramFuncPtr = void (ShaderProgram::*)(const unsigned int &index, std::vector<GLubyte> data);
	
			ShaderProgram& program; // TODO: These programs should be shared between materials, with just the uniforms updating.
									// Probably need to make something like i have for meshes, where it cashes and loads by index
			std::vector<MaterialParameter> values;
			std::vector<Texture> textures;


			unsigned int findProperty(const std::string& name) const;

			template<typename T>
			void setParameterInternal(const unsigned int index, const T& data, ShaderProgramFuncPtr func);

			template<typename T>
			void setParameterInternal(const std::string& name, const T& data, ShaderProgramFuncPtr func);
	};

	template<typename T>
	void Material::setParameterInternal(const unsigned int index, const T& data, ShaderProgramFuncPtr func) {
		const std::size_t size = sizeof(T);
	
		MaterialParameter param;
		param.func = func; // TODO: Shoudl we be using a && and a move for func?
		param.data.resize(size);
		param.use = true;
		
		memcpy(&param.data[0], &data, size);
		
		values[index] = param;
	}
	
	template<typename T>
	void Material::setParameterInternal(const std::string& name, const T& data, ShaderProgramFuncPtr func) {
		setParameterInternal(findProperty(name), data, func);
	}
} // namespace: engine