#include <engine/Material.hpp>

namespace engine {
	Material::Material(ShaderProgram &program) : program(program) { // TODO: Need to change how this works to use a global manager like mesh or to pass a index or something. Could make a manager like mesh but in Material annd let Material manage the programs instead of some external thing
		program.loadProgramUniforms(); // TODO: Should move this into the program construcotr or something, i shouldnt be calling this here
		auto &props = program.getProperties();
		values.resize(props.size());
	}

	Material::~Material() {
	}

	void Material::loadParameters() {
		// TODO: Need to also make this load textures

		for (int i=0; i < textures.size(); i++) {
			glActiveTexture(GL_TEXTURE0 + static_cast<GLenum>(i));
			glBindTexture(GL_TEXTURE_2D, textures[i].getTexture());
		}

		for (int i=0; i < values.size(); i++) {
			if (values[i].use) {
				(program.*values[i].func)(i, values[i].data);
			}
		}
	}

	unsigned int Material::findProperty(const std::string &name) const {
		auto &properties = program.getProperties();

		for (int i = 0; i < properties.size(); i++) {
			if (properties[i].name == name) {
				return i;
			}
		}

		engine_error("No uniform with the name \"" + name + "\" found in the current program");
	}

	void Material::setParameter(const unsigned int &indx, const GLint &data) { setParameterInternal(indx, data, &ShaderProgram::setUniformInt); }
	void Material::setParameter(const std::string &name, const GLint &data) { setParameterInternal(name, data, &ShaderProgram::setUniformInt); }

	void Material::setParameter(const unsigned int &indx, const GLuint &data) { setParameterInternal(indx, data, &ShaderProgram::setUniformUInt); }
	void Material::setParameter(const std::string &name, const GLuint &data) { setParameterInternal(name, data, &ShaderProgram::setUniformUInt); }

	void Material::setParameter(const unsigned int &indx, const GLfloat &data) { setParameterInternal(indx, data, &ShaderProgram::setUniformFloat); }
	void Material::setParameter(const std::string &name, const GLfloat &data) { setParameterInternal(name, data, &ShaderProgram::setUniformFloat); }


	void Material::setParameter(const unsigned int &indx, const engine::Texture &data) {
		setParameter(indx, static_cast<GLint>(textures.size()));
		textures.push_back(data);// TODO: Is const ref the best type to use for data since we are making a copy here? Look into move and forwarding
	}

	void Material::setParameter(const std::string &name, const engine::Texture &data) { // TODO: May want to look into rvalue reference/move/forwarding for this function
		setParameter(findProperty(name), data);
	}

}