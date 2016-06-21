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

void Program::getProgramUniforms() {
	if (false) {
		GLint i;
		GLint count;

		GLint size; // size of the variable
		GLenum type; // type of the variable (float, vec3 or mat4, etc)

		const GLsizei bufSize = 128; // maximum name length
		GLchar name[bufSize]; // variable name in GLSL
		GLsizei length; // name length

		glGetProgramiv(program, GL_ACTIVE_UNIFORMS, &count);
		printf("Active Uniforms: %d\n", count);

		for (i = 0; i < count; i++) {
			glGetActiveUniform(program, (GLuint)i, bufSize, &length, &size, &type, name);

			printf("Uniform #%d Type: %u Name: %s\n", i, type, name);
		}
	} else {
		//GLint numActiveAttribs = 0;
		//GLint numActiveUniforms = 0;
		//glGetProgramInterfaceiv(program, GL_PROGRAM_INPUT, GL_ACTIVE_RESOURCES, &numActiveAttribs);
		//glGetProgramInterfaceiv(program, GL_UNIFORM, GL_ACTIVE_RESOURCES, &numActiveUniforms);
		//std::vector<GLchar> nameData(256);
		//std::vector<GLenum> properties;
		//properties.push_back(GL_NAME_LENGTH​);
		//properties.push_back(GL_TYPE​);
		//properties.push_back(GL_ARRAY_SIZE​);
		//std::vector<GLint> values(properties.size());
		//
		//for (int attrib = 0; attrib < numActiveAttribs; ++attrib) {
		//	glGetProgramResourceiv(program, GL_PROGRAM_INPUT, attrib, properties.size(),
		//		&properties[0], values.size(), NULL, &values[0]);
		//
		//	nameData.resize(properties[0]); //The length of the name.
		//	glGetProgramResourceName(program, GL_PROGRAM_INPUT, attrib, nameData.size(), NULL, &nameData[0]);
		//	std::string name((char*)&nameData[0], nameData.size() - 1);
		//}
	}
}