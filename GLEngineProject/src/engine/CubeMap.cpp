#include <engine/CubeMap.hpp>

namespace engine {
	CubeMap::~CubeMap() {
	}

	GLuint CubeMap::getCubeMapTexture() {
		return container[index].texture;
	}

	////////////////////////////////////////////////////////////////
	// Static
	////////////////////////////////////////////////////////////////
	CubeMap CubeMap::loadCubeMap(const ResourcePath& path) {
		const auto resolvedPath = path.getResolvedPath();
		auto& loadInfo = loadResource(resolvedPath);

		// Load the object if it has not already been loaded
		if (!loadInfo.alreadyLoaded) {
			auto& cubeMapData = container[loadInfo.object.index];

			int width;
			int height;
			int channels;
			unsigned int faceSize = 0;
			int posXIndex = 0;
			int negXIndex = 1;
			int posYIndex = 2;
			int negYIndex = 3;
			int posZIndex = 4;
			int negZIndex = 5;

			glGenTextures(1, &cubeMapData.texture);
			glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapData.texture);

			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

			unsigned char* image = SOIL_load_image(path.getResolvedPath().c_str(), &width, &height, &channels, SOIL_LOAD_RGB);
			std::cout << "width: " << width << "\theight: " << height << "\tchannels: " << channels << "\n";

			std::vector<std::vector<glm::u8vec3>> images;

			for (int i = 0; i < 6; ++i) {
				images.emplace_back();

				// Reserve some memory. Probably not going to have cubemaps smaller than 128.
				images.back().reserve(128*128);
			}


			// TODO: could probably integrate the gl texture calls into the look to get rid of the multiple vectors and just use a single faceSize^2 array
			if (3 * width == 4 * height) { // Horizontal cross
				faceSize = width / 4;
				std::cout << "Horizontal Cross: " << faceSize << "\n";

			} else if (3 * height == 4 * width) { // Vertical cross
				faceSize = height / 4;
				std::cout << "Vertical Cross: " << faceSize << "\n";

			} else if (width == 6 * height) { // Horizontal line
				faceSize = width / 6;
				std::cout << "Horizontal Line: " << faceSize << "\n";

				for (unsigned int i = 0; i < 6; ++i) {
					for (size_t y = 0; y < faceSize; ++y) {
						for (size_t x = 0; x < faceSize; ++x) {
							size_t xComp = x * channels + i * faceSize * 3;
							size_t yComp = y * width * channels;
							size_t index = xComp + yComp;
							images[i].emplace_back(image[index + 0], image[index + 1], image[index + 2]);
						}
					}
				}
			} else if (height == 6 * width) { // Vertical line
				faceSize = height / 6;
				std::cout << "Vertical Line: " << faceSize << "\n";

				for (unsigned int i = 0; i < 6; ++i) {
					for (size_t y = 0; y < faceSize; ++y) {
						for (size_t x = 0; x < faceSize; ++x) {
							size_t xComp = x * channels;
							size_t yComp = y * width * channels + i * width * faceSize * channels;
							size_t index = xComp + yComp;
							images[i].emplace_back(image[index + 0], image[index + 1], image[index + 2]);
						}
					}
				}
			}

			SOIL_free_image_data(image);

			// TODO: could probably move this part into the loops
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_SRGB8, faceSize, faceSize, 0, GL_RGB, GL_UNSIGNED_BYTE, &images[posXIndex][0]);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_SRGB8, faceSize, faceSize, 0, GL_RGB, GL_UNSIGNED_BYTE, &images[negXIndex][0]);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_SRGB8, faceSize, faceSize, 0, GL_RGB, GL_UNSIGNED_BYTE, &images[posYIndex][0]);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_SRGB8, faceSize, faceSize, 0, GL_RGB, GL_UNSIGNED_BYTE, &images[negYIndex][0]);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_SRGB8, faceSize, faceSize, 0, GL_RGB, GL_UNSIGNED_BYTE, &images[posZIndex][0]);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_SRGB8, faceSize, faceSize, 0, GL_RGB, GL_UNSIGNED_BYTE, &images[negZIndex][0]);

			glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
		}

		return loadInfo.object;
	}
}