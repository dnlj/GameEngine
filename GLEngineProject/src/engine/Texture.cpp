// SOIL
#include <SOIL.h>

// RapidJSON
#include <rapidjson/document.h>
#include <rapidjson/error/error.h>
#include <rapidjson/error/en.h>

// Engine
#include <engine/Texture.hpp>
#include <engine/json/json.hpp>


namespace engine {
	Texture::~Texture() {
	}

	bool Texture::operator==(const Texture& tex) const {
		return index == tex.index;
	}

	GLuint Texture::getTexture() const {
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
			ResourcePath texPath = json::getString(doc, "path")->value.GetString();
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


			if (type == TextureFormat::Type::TEXTURE_1D) {
				// TODO: implement
			} else if (type == TextureFormat::Type::TEXTURE_2D) {
				load2DTexture(texPath.getResolvedPath(), format, loadInfo);
			} else if (type == TextureFormat::Type::TEXTURE_3D) {
				// TODO: implement
			} else if (type == TextureFormat::Type::TEXTURE_CUBE) {
				// TODO: implement
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

		int width, height, channels;

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
}