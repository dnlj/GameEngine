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
		for (size_t i=0; i < values.size(); i++) {
			if (values[i].use) {
				(program.*values[i].func)(i, values[i].data);
			}
		}
	}

	void Material::setParameter(const std::size_t &indx, const GLint &data) { setParameterInternal(indx, data, &ShaderProgram::setUniformInt); }
	void Material::setParameter(const std::string &name, const GLint &data) { setParameterInternal(name, data, &ShaderProgram::setUniformInt); }

	void Material::setParameter(const std::size_t &indx, const GLuint &data) { setParameterInternal(indx, data, &ShaderProgram::setUniformUInt); }
	void Material::setParameter(const std::string &name, const GLuint &data) { setParameterInternal(name, data, &ShaderProgram::setUniformUInt); }

	void Material::setParameter(const std::size_t &indx, const GLfloat &data) { setParameterInternal(indx, data, &ShaderProgram::setUniformFloat); }
	void Material::setParameter(const std::string &name, const GLfloat &data) { setParameterInternal(name, data, &ShaderProgram::setUniformFloat); }
}