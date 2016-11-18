// Engine
#include <engine/ShaderProgramData2.hpp>

namespace engine {
	void ShaderProgramData2::loadProgramUniforms() {
		GLint numActiveUniforms = 0;
		glGetProgramInterfaceiv(program, GL_UNIFORM, GL_ACTIVE_RESOURCES, &numActiveUniforms);
	
		std::vector<GLchar> nameData;
		nameData.reserve(128);
	
		std::vector<GLenum> props;
		props.push_back(GL_NAME_LENGTH);
		props.push_back(GL_TYPE);
		props.push_back(GL_LOCATION);
	
		std::vector<GLint> values(props.size());
	
		for (int uniform = 0; uniform < numActiveUniforms; uniform++) {
			glGetProgramResourceiv(program, GL_UNIFORM, uniform, static_cast<GLsizei>(props.size()), &props[0], static_cast<GLsizei>(values.size()), nullptr, &values[0]);
	
			nameData.resize(values[0]);
			glGetProgramResourceName(program, GL_UNIFORM, uniform, static_cast<GLsizei>(nameData.size()), nullptr, &nameData[0]);
			std::string name(&nameData[0], nameData.size() - 1);
			nameData.resize(values[0] - 1);
	
			properties.emplace_back(name, static_cast<GLenum>(values[1]), values[2]);
		}
	}

	void ShaderProgramData2::deleteData() {
		std::cout << "Delete program: " << program << "\n";
		glDeleteProgram(program);
	}
}