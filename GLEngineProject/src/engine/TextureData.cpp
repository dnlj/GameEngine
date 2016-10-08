// STD
#include <iostream>

// Engine
#include <engine/TextureData.hpp>


namespace engine {
	void TextureData::deleteData() {
		std::cout << "Delete texture: " << texture << "\n";
		glDeleteTextures(1, &texture);
	}
}