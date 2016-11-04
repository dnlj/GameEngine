// SOIL
#include <SOIL.h>

// GLM
#include <glm/glm.hpp>

// Engine
#include <engine/Texture.hpp>
#include <engine/json/json.hpp>
#include <engine/util/util.hpp>


namespace engine {
	Texture::~Texture() {
	}

	bool Texture::operator==(const Texture& tex) const {
		return index == tex.index;
	}

	GLuint Texture::getTexture() const {
		// TODO: Should i cache this?
		return getDataAt(index).texture;
	}

	////////////////////////////////////////////////////////////////
	// Static
	////////////////////////////////////////////////////////////////
	
	//Texture Texture::loadTexture(const ResourcePath& path, const TextureFormat& format) {
	//	const auto resolvedPath = path.getResolvedPath();
	//	auto& loadInfo = loadResource(resolvedPath);
	//	
	//	// Load the object if it has not already been loaded
	//	if (!loadInfo.alreadyLoaded) {
	//		auto& textureData = loadInfo.data;
	//		textureData.format = format;
	//		
	//		// TODO: Add an option to the TextureFormat for the internal format (GL_RGBA, GL_SRGB8_ALPHA8, GL_RGB, etc)
	//		//			So that you can make rgba, rgb, and grayscale textures if you need them
	//
	//
	//		if (!format.useMipmaps
	//			&& format.filterModeMin != TextureFormat::FilterMin::LINEAR
	//			&& format.filterModeMin != TextureFormat::FilterMin::NEAREST) {
	//			// TODO: Error
	//		}
	//
	//		int width, height, channels;
	//
	//		// Create and bind a texture
	//		glGenTextures(1, &textureData.texture);
	//		glBindTexture(GL_TEXTURE_2D, textureData.texture);
	//
	//		// Set parameteres
	//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, TextureFormat::enumToOpenGL(format.wrapModeU));
	//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, TextureFormat::enumToOpenGL(format.wrapModeV));
	//
	//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, TextureFormat::enumToOpenGL(format.filterModeMin));
	//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, TextureFormat::enumToOpenGL(format.filterModeMag));
	//
	//		// Load the image from a file
	//		unsigned char* image;
	//		image = SOIL_load_image(resolvedPath.c_str(), &width, &height, &channels, SOIL_LOAD_RGBA);
	//		
	//		bool failedToLoad = false;
	//		if (image == nullptr) {
	//			std::string err = "Failed to load image \"" + resolvedPath + "\" with error: ";
	//			err += SOIL_last_result();
	//			engine_warning(err);
	//
	//			failedToLoad = true;
	//			
	//			width = 2;
	//			height = 2;
	//			channels = 4;
	//			image = new unsigned char[4 * 4] {
	//				255, 000, 000, 255,    000, 255, 000, 255,
	//				000, 255, 000, 255,    255, 000, 000, 255,
	//			};
	//		}
	//
	//		// TODO: Would like to make this part of TextureFormat
	//		// Determine the format 
	//		if (format.useGammaCorrection) {
	//			glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB8_ALPHA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	//		} else {
	//			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	//		}
	//
	//		// Generate mipmaps if needed
	//		if (format.useMipmaps) {
	//			glGenerateMipmap(GL_TEXTURE_2D);
	//		}
	//
	//		// Unbind the texture and free the image data
	//		glBindTexture(GL_TEXTURE_2D, 0);
	//		if (!failedToLoad) {
	//			SOIL_free_image_data(image);
	//		} else {
	//			delete [] image;
	//		}
	//	}
	//
	//	return loadInfo.object;
	//}

	Texture Texture::load(const ResourcePath& path) {
		const auto resolvedPath = path.getResolvedPath();
		auto& loadInfo = loadResource(resolvedPath);
		
		// Load the object if it has not already been loaded
		if (!loadInfo.alreadyLoaded) {
			rapidjson::Document doc;
			json::loadDocument(resolvedPath, doc);
			auto type = TextureFormat::stringToType(json::getString(doc, "type")->value.GetString());
			auto formatObj = json::getObject(doc, "format");
			std::string filter = json::getString(formatObj->value, "filter")->value.GetString();
			TextureFormat format;

			if (filter == "DEFAULT") {
				// TODO: This should be loaded from a settings file or something
				format.filter = TextureFormat::Filter::BILINEAR;
			} else {
				format.filter = TextureFormat::stringToFilter(filter);
			}

			format.useMipmaps = json::getBool(formatObj->value, "use_mipmaps")->value.GetBool();
			format.useGammaCorrection = json::getBool(formatObj->value, "use_gamma_correction")->value.GetBool();
			format.wrapU = TextureFormat::stringToWrap(json::getString(formatObj->value, "wrap_u")->value.GetString());
			format.wrapV = TextureFormat::stringToWrap(json::getString(formatObj->value, "wrap_v")->value.GetString());

			if (type == TextureFormat::Type::TEXTURE_CUBE) {
				loadCubeTexture(doc, format, loadInfo);
			} else {
				ResourcePath texPath = json::getString(doc, "path")->value.GetString();
				// TODO: change this to pass in doc like cubemap does, it gives the loadign funcitons more freedom
				if (type == TextureFormat::Type::TEXTURE_1D) {
					// TODO: implement
				} else if (type == TextureFormat::Type::TEXTURE_2D) {
					load2DTexture(texPath.getResolvedPath(), format, loadInfo);
				} else if (type == TextureFormat::Type::TEXTURE_3D) {
					// TODO: implement
				}
			}
		}
	
		return loadInfo.object;
	}

	void Texture::load2DTexture(const std::string& resolvedPath, const TextureFormat& format, LoadInfo& loadInfo) {
		auto& textureData = loadInfo.data;

		// TODO: Add an option to the TextureFormat for the internal format (GL_RGBA, GL_SRGB8_ALPHA8, GL_RGB, etc)
		//			So that you can make rgba, rgb, and grayscale textures if you need them

		auto filterMin = format.getMinFilter();
		auto filterMag = format.getMagFilter();

		if (!format.useMipmaps && filterMin != TextureFormat::FilterMin::NEAREST) {
			engine_error("Invalid filter setting in texture \"" + resolvedPath + "\"\n"\
				"When not using min filter NEAREST mipmaps must be enabled."
			);
		}

		int width;
		int height;
		int channels;

		// Create and bind a texture
		glGenTextures(1, &textureData.texture);
		glBindTexture(GL_TEXTURE_2D, textureData.texture);

		// Set parameteres
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, TextureFormat::enumToOpenGL(format.wrapU));
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, TextureFormat::enumToOpenGL(format.wrapV));

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, TextureFormat::enumToOpenGL(filterMin));
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, TextureFormat::enumToOpenGL(filterMag));

		// Load the image from a file
		unsigned char* image;
		image = SOIL_load_image(resolvedPath.c_str(), &width, &height, &channels, SOIL_LOAD_RGBA);

		bool failedToLoad = false;
		if (image == nullptr) {
			std::string err = "Failed to load image \"" + resolvedPath + "\" with error: ";
			err += SOIL_last_result();
			engine_warning(err);

			failedToLoad = true;

			width = 2;
			height = 2;
			channels = 4;
			image = new unsigned char[4 * 4]{
				255, 000, 000, 255,    000, 255, 000, 255,
				000, 255, 000, 255,    255, 000, 000, 255,
			};
		}

		// TODO: Would like to make this part of TextureFormat
		// Determine the format 
		if (format.useGammaCorrection) {
			glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB8_ALPHA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
		} else {
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
		}

		// Generate mipmaps if needed
		if (format.useMipmaps) {
			glGenerateMipmap(GL_TEXTURE_2D);
		}

		// Unbind the texture and free the image data
		glBindTexture(GL_TEXTURE_2D, 0);

		if (!failedToLoad) {
			SOIL_free_image_data(image);
		} else {
			delete[] image;
		}
	}

	void Texture::loadCubeTexture(rapidjson::Document& document, const TextureFormat& format, LoadInfo& loadInfo) {
		// TODO: add support for generate mip-maps and gamma correction, p mush everythign format supports
		auto& cubeMapData = loadInfo.data;

		glGenTextures(1, &cubeMapData.texture);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapData.texture);

		// will also need to add filter checks, maybe add a  isValid() to TextureFromat or something
		// TODO: make it get this from format
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		// TODO: p sure wrap_r isnt needed
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);


		if (document.HasMember("cube_sides")) {
			auto cubeSides = json::getObject(document, "cube_sides");

			setupSplitSides({
				engine::ResourcePath{json::getString(cubeSides->value, "pos_x")->value.GetString()}.getResolvedPath(),
				engine::ResourcePath{json::getString(cubeSides->value, "neg_x")->value.GetString()}.getResolvedPath(),
				engine::ResourcePath{json::getString(cubeSides->value, "pos_y")->value.GetString()}.getResolvedPath(),
				engine::ResourcePath{json::getString(cubeSides->value, "neg_y")->value.GetString()}.getResolvedPath(),
				engine::ResourcePath{json::getString(cubeSides->value, "pos_z")->value.GetString()}.getResolvedPath(),
				engine::ResourcePath{json::getString(cubeSides->value, "neg_z")->value.GetString()}.getResolvedPath()
			});

			util::checkGLErrors();
		} else {
			ResourcePath texPath = json::getString(document, "path")->value.GetString();

			int width;
			int height;
			int channels;

			unsigned char* image = SOIL_load_image(texPath.getResolvedPath().c_str(), &width, &height, &channels, SOIL_LOAD_AUTO);

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
		}

		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	}

	void Texture::setupHorizontalCross(unsigned char* image, int width, int height, int channels) {
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

	void Texture::setupVerticalCross(unsigned char* image, int width, int height, int channels) {
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

	void Texture::setupHorizontalLine(unsigned char* image, int width, int height, int channels) {
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

	void Texture::setupVerticalLine(unsigned char* image, int width, int height, int channels) {
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

	void Texture::setupSplitSides(const std::vector<std::string>& images) {
		std::cout << "Split Sides: \n";

		int width;
		int height;
		int channels;
		unsigned char* image;

		for (int i = 0; i < images.size(); ++i) {
			image = SOIL_load_image(images[i].c_str(), &width, &height, &channels, SOIL_LOAD_RGB);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_SRGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
			SOIL_free_image_data(image);
		}
	}
}