#include <engine/CubeMapData.hpp>

namespace engine {
	void CubeMapData::deleteData() {
		std::cout << "Delete cubemap: " << texture << "\n";
		glDeleteTextures(1, &texture);
	}
}