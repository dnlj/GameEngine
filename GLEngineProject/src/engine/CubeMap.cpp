// GLM
#include <glm/glm.hpp>

// SOIL
#include <SOIL.h>

// Engine
#include <engine/CubeMap.hpp>


namespace engine {
	CubeMap::~CubeMap() {
	}

	GLuint CubeMap::getCubeMapTexture() const {
		return getDataAt(index).texture;
	}

	////////////////////////////////////////////////////////////////
	// Static
	////////////////////////////////////////////////////////////////
	CubeMap CubeMap::loadCubeMap(const ResourcePath& path) {
		const auto resolvedPath = path.getResolvedPath();
		auto& loadInfo = loadResource(resolvedPath);

		// Load the object if it has not already been loaded
		if (!loadInfo.alreadyLoaded) {
			auto& cubeMapData = loadInfo.data;

			int width;
			int height;
			int channels;
			int faceSize = 0;
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

			unsigned char* image = SOIL_load_image(path.getResolvedPath().c_str(), &width, &height, &channels, SOIL_LOAD_AUTO);
			std::cout << "width: " << width << "\theight: " << height << "\tchannels: " << channels << "\n";

			if (3 * width == 4 * height) { // Horizontal cross
				setupHorizontalCross(image, width, height, channels);
			} else if (3 * height == 4 * width) { // Vertical cross
				setupVerticalCross(image, width, height, channels);
			} else if (width == 6 * height) { // Horizontal line
				setupHorizontalLine(image, width, height, channels);
			} else if (height == 6 * width) { // Vertical line
				setupVerticalLine(image, width, height, channels);
			}

			SOIL_free_image_data(image);

			glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
		}

		return loadInfo.object;
	}

	void CubeMap::setupHorizontalCross(unsigned char* image, int width, int height, int channels) {
		int faceSize = width / 4;
		std::cout << "Horizontal Cross: " << faceSize << "\n";

		//  0   1   2   3  | --  +Y  --  --
		//  4   5   6   7  | -X  +Z  +X  -Z
		//  8   9  10  11  | --  -Y  --  --

		std::vector<glm::u8vec3> subImage;
		subImage.reserve(faceSize * faceSize);

		const size_t rowWidth = width * channels;
		GLenum face;

		for (int i = 1; i < 10; ++i) {
			switch (i) {
				case 6:
					face = GL_TEXTURE_CUBE_MAP_POSITIVE_X;
					break;
				case 4:
					face = GL_TEXTURE_CUBE_MAP_NEGATIVE_X;
					break;
				case 1:
					face = GL_TEXTURE_CUBE_MAP_POSITIVE_Y;
					break;
				case 9:
					face = GL_TEXTURE_CUBE_MAP_NEGATIVE_Y;
					break;
				case 5:
					face = GL_TEXTURE_CUBE_MAP_POSITIVE_Z;
					break;
				case 7:
					face = GL_TEXTURE_CUBE_MAP_NEGATIVE_Z;
					break;
				default:
					continue;
			}

			for (int y = 0; y < faceSize; ++y) {
				for (int x = 0; x < faceSize; ++x) {
					size_t xComp = x * channels + (i % 4) * faceSize * channels;
					size_t yComp = y * rowWidth + (i / 4) * rowWidth * faceSize;
					size_t index = xComp + yComp;

					subImage.emplace_back(image[index + 0], image[index + 1], image[index + 2]);
				}
			}

			glTexImage2D(face, 0, GL_SRGB8, faceSize, faceSize, 0, GL_RGB, GL_UNSIGNED_BYTE, &subImage[0]);
			subImage.resize(0); // Only changes size. Does not affect capacity?
		}
	}

	void CubeMap::setupVerticalCross(unsigned char* image, int width, int height, int channels) {
		int faceSize = height / 4;
		std::cout << "Vertical Cross: " << faceSize << "\n";

		//  0   1   2  | --  +Y  --
		//  3   4   5  | +Z  +X  -Z
		//  6   7   8  | --  -Y  --
		//  9  10  11  | --  -X  --

		std::vector<glm::u8vec3> subImage;
		subImage.reserve(faceSize * faceSize);

		const size_t rowWidth = width * channels;
		GLenum face;

		for (int i = 1; i < 11; ++i) {
			switch (i) {
				case 4:
					face = GL_TEXTURE_CUBE_MAP_POSITIVE_X;
					break;
				case 10:
					face = GL_TEXTURE_CUBE_MAP_NEGATIVE_X;
					break;
				case 1:
					face = GL_TEXTURE_CUBE_MAP_POSITIVE_Y;
					break;
				case 7:
					face = GL_TEXTURE_CUBE_MAP_NEGATIVE_Y;
					break;
				case 3:
					face = GL_TEXTURE_CUBE_MAP_POSITIVE_Z;
					break;
				case 5:
					face = GL_TEXTURE_CUBE_MAP_NEGATIVE_Z;
					break;
				default:
					continue;
			}

			for (int y = 0; y < faceSize; ++y) {
				for (int x = 0; x < faceSize; ++x) {
					size_t xComp = x * channels + (i % 3) * faceSize * channels;
					size_t yComp = y * rowWidth + (i / 3) * rowWidth * faceSize;
					size_t index = xComp + yComp;

					subImage.emplace_back(image[index + 0], image[index + 1], image[index + 2]);
				}
			}

			glTexImage2D(face, 0, GL_SRGB8, faceSize, faceSize, 0, GL_RGB, GL_UNSIGNED_BYTE, &subImage[0]);
			subImage.resize(0); // Only changes size. Does not affect capacity?
		}
	}

	void CubeMap::setupHorizontalLine(unsigned char* image, int width, int height, int channels) {
		int faceSize = width / 6;
		std::cout << "Horizontal Line: " << faceSize << "\n";

		std::vector<glm::u8vec3> subImage;
		subImage.reserve(faceSize * faceSize);

		//  0  1  2  3  4  5 | +X  -X  +Y  -Y  +Z  -Z

		for (int i = 0; i < 6; ++i) {
			for (int y = 0; y < faceSize; ++y) {
				for (int x = 0; x < faceSize; ++x) {
					size_t xComp = x * channels + i * faceSize * channels;
					size_t yComp = y * width * channels;
					size_t index = xComp + yComp;

					subImage.emplace_back(image[index + 0], image[index + 1], image[index + 2]);
				}
			}

			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_SRGB8, faceSize, faceSize, 0, GL_RGB, GL_UNSIGNED_BYTE, &subImage[0]);
			subImage.resize(0); // Only changes size. Does not affect capacity?
		}
	}

	void CubeMap::setupVerticalLine(unsigned char* image, int width, int height, int channels) {
		int faceSize = height / 6;
		std::cout << "Vertical Line: " << faceSize << "\n";

		// 0 | +X
		// 1 | -X
		// 2 | +Y
		// 3 | -Y
		// 4 | +Z
		// 5 | -Z

		std::vector<glm::u8vec3> subImage;
		subImage.reserve(faceSize * faceSize);

		for (int i = 0; i < 6; ++i) {
			for (int y = 0; y < faceSize; ++y) {
				for (int x = 0; x < faceSize; ++x) {
					size_t xComp = x * channels;
					size_t yComp = y * width * channels + i * width * faceSize * channels;
					size_t index = xComp + yComp;

					subImage.emplace_back(image[index + 0], image[index + 1], image[index + 2]);
				}
			}

			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_SRGB8, faceSize, faceSize, 0, GL_RGB, GL_UNSIGNED_BYTE, &subImage[0]);
			subImage.resize(0); // Only changes size. Does not affect capacity?
		}
	}
}