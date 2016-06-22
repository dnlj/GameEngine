#include <engine/ShaderProgram.hpp>

namespace engine {
	ShaderProgram::ShaderProgram() : program(glCreateProgram()) {}

	ShaderProgram::~ShaderProgram() {
		glDeleteProgram(program);
	}

	void ShaderProgram::attachShader(Shader &shdr) {
		glAttachShader(program, shdr.getShader());
	}

	void ShaderProgram::link() {
		glLinkProgram(program);
	}

	void ShaderProgram::detachShaders() {
		GLint count;
		glGetProgramiv(program, GL_ATTACHED_SHADERS, &count);
		GLuint *shaders = new GLuint[count];
		glGetAttachedShaders(program, count, nullptr, shaders);

		for (int i = 0; i < count; i++) {
			glDetachShader(program, shaders[i]);
		}

		delete[] shaders;
		glGetProgramiv(program, GL_ATTACHED_SHADERS, &count);
	}

	void ShaderProgram::checkLinkStatus() {
		GLint status;

		glGetProgramiv(program, GL_LINK_STATUS, &status);

		if (status == GL_FALSE) {
			std::string errorMsg = "Program linking failed with error:\n";

			GLint leng;
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &leng);

			GLchar *buffer = new GLchar[leng]();
			glGetProgramInfoLog(program, leng, nullptr, buffer);

			errorMsg.append(buffer);
			delete[] buffer;
			deleteProgram();

			engine_error(errorMsg);
		}
	}

	GLuint ShaderProgram::get() {
		return program;
	}

	void ShaderProgram::deleteProgram() {
		glDeleteProgram(program);
		program = 0;
	}

	void ShaderProgram::use() {
		glUseProgram(program);
	}

	GLint ShaderProgram::getAttribLocation(const char *name) const {
		if (name == nullptr) {
			engine_error("name was not specified in Program::getAttrib");
		}

		GLint attrib = glGetAttribLocation(program, name);
		if (attrib == -1) {
			engine_error(std::string("Program attribute not found: ").append(name));
		}

		return attrib;
	};

	GLint ShaderProgram::getUniformLocation(const char *name) const { // TODO: Convert to use strings
		if (name == nullptr) {
			engine_error("name was not specified in Program::getUniform");
		}

		GLint uniform = glGetUniformLocation(program, name);
		if (uniform == -1) {
			engine_error(std::string("Program uniform not found: ").append(name));
		}

		return uniform;
	};

	void ShaderProgram::loadProgramUniforms() {
		GLint numActiveUniforms = 0;
		glGetProgramInterfaceiv(program, GL_UNIFORM, GL_ACTIVE_RESOURCES, &numActiveUniforms);

		std::vector<GLchar> nameData;
		nameData.reserve(128);

		std::vector<GLenum> props;
		props.push_back(GL_NAME_LENGTH);
		props.push_back(GL_TYPE);
		//props.push_back(GL_ARRAY_SIZE);

		std::vector<GLint> values(props.size());

		for (int uniform = 0; uniform < numActiveUniforms; uniform++) {
			glGetProgramResourceiv(program, GL_UNIFORM, uniform, static_cast<GLsizei>(props.size()), &props[0], static_cast<GLsizei>(values.size()), nullptr, &values[0]);

			nameData.resize(values[0]);
			glGetProgramResourceName(program, GL_UNIFORM, uniform, static_cast<GLsizei>(nameData.size()), nullptr, &nameData[0]);
			std::string name(&nameData[0], nameData.size() - 1);
			nameData.resize(values[0]-1);

			properties.emplace_back(name, static_cast<GLenum>(values[1]));
		}

		// TODO: Remove, temp for debugging
		std::cout << "\n\n\n";
		for (auto v : properties) {
			std::cout << "\nname: " << v.name;
			std::cout << "\ntype: " << util::typeEnumToString(v.type);
			std::cout << "\n";
		}
		std::cout << "\n\n\n";
	}
}