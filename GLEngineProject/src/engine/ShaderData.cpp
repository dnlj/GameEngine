// STD
#include <iostream>

// Engine
#include <engine/ShaderData.hpp>


namespace engine {
	void ShaderData::deleteData() {
		std::cout << "Delete shader: " << shader << "\n";
		glDeleteShader(shader);
	}
}