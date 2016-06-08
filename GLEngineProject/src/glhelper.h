#pragma once

#include <iostream>
#include <string>

#include <glloadgen/gl_core_4_4.h>

// Engine
#include <engine/engine.hpp>

namespace glhelper {
	void checkShaderErrors(GLuint shader);
	void checkLinkingErrors(GLuint program);
}