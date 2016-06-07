#pragma once

#include <iostream>
#include <string>

#include <glloadgen/gl_core_4_4.h>

namespace glhelper {
	void checkShaderErrors(GLuint shader);
	void checkLinkingErrors(GLuint program);
}