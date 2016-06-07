#include "glhelper.h"

void glhelper::checkShaderErrors(GLuint shader) {
	GLint status;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE) {
		std::string errorMsg = "[glhelper] Shader compile failed with error:\n";
		
		GLint leng;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &leng);
			
		GLchar *buffer = new GLchar[leng]();
		glGetShaderInfoLog(shader, leng, nullptr, buffer);

		errorMsg.append(buffer);
		delete [] buffer;
		glDeleteShader(shader);

		throw std::runtime_error(errorMsg); // TODO: Convert to use new error handling
	}
}

// TODO: Not sure if you should be deleteing the program in this function, since its purpose is to check for errors
void glhelper::checkLinkingErrors(GLuint program) {
	GLint status;
	
	glGetProgramiv(program, GL_LINK_STATUS, &status);

	if (status == GL_FALSE) {
		std::string errorMsg = "[glhelper] Program linking failed with error:\n";
		
		GLint leng;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &leng);
			
		GLchar *buffer = new GLchar[leng]();
		glGetProgramInfoLog(program, leng, nullptr, buffer);

		errorMsg.append(buffer);
		delete [] buffer;
		glDeleteProgram(program);

		throw std::runtime_error(errorMsg); // TODO: Convert to use new error handling
	}
}