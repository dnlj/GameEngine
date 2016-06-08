#include "Program.h"


Program::Program() : program(glCreateProgram()) {}

Program::~Program() {
	glDeleteProgram(program);
}

void Program::attachShader(Shader &shdr) {
	glAttachShader(program, shdr.getShader());
}

void Program::link() {
	glLinkProgram(program);
}

void Program::detachShaders() {
	GLint count;
	glGetProgramiv(program, GL_ATTACHED_SHADERS, &count);
	GLuint *shaders = new GLuint[count];
	glGetAttachedShaders(program, count, nullptr, shaders);

	for (int i = 0; i < count; i++) {
		glDetachShader(program, shaders[i]);
	}

	delete [] shaders;
	glGetProgramiv(program, GL_ATTACHED_SHADERS, &count);
}

void Program::checkLinkStatus() {
	GLint status;
	
	glGetProgramiv(program, GL_LINK_STATUS, &status);

	if (status == GL_FALSE) {
		std::string errorMsg = "Program linking failed with error:\n";
		
		GLint leng;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &leng);
			
		GLchar *buffer = new GLchar[leng]();
		glGetProgramInfoLog(program, leng, nullptr, buffer);

		errorMsg.append(buffer);
		delete [] buffer;
		deleteProgram();

		engine_error(errorMsg);
	}
}

GLuint Program::get() {
	return program;
}

void Program::deleteProgram() {
	glDeleteProgram(program);
	program = 0;
}

void Program::use() {
	glUseProgram(program);
}

GLint Program::getAttribLocation(const char *name) const { // TODO: Convert to use strings
	if (name == nullptr) {
		engine_error("name was not specified in Program::getAttrib");
	}

	GLint attrib = glGetAttribLocation(program, name);
	if (attrib == -1) {
		engine_error(std::string("Program attribute not found: ").append(name)); // TODO: Convert to use new error handling
	}

	return attrib;
};

GLint Program::getUniformLocation(const char *name) const { // TODO: Convert to use strings
	if (name == nullptr) {
		engine_error("name was not specified in Program::getUniform"); // TODO: Convert to use new error handling
	}

	GLint uniform = glGetUniformLocation(program, name);
	if (uniform == -1) {
		engine_error(std::string("Program uniform not found: ").append(name)); // TODO: Convert to use new error handling
	}

	return uniform;
};