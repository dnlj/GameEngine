// SOIL
#include <SOIL.h>

// Engine
#include <engine/Texture.hpp>


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
	Texture Texture::loadTexture(const ResourcePath& path, const TextureFormat& format) {
		const auto resolvedPath = path.getResolvedPath();
		auto& loadInfo = loadResource(resolvedPath);
		
		// Load the object if it has not already been loaded
		if (!loadInfo.alreadyLoaded) {
			auto& textureData = loadInfo.data;
			textureData.format = format;
			
			// TODO: Add an option to the TextureFormat for the internal format (GL_RGBA, GL_SRGB8_ALPHA8, GL_RGB, etc)
			//			So that you can make rgba, rgb, and grayscale textures if you need them


			if (!format.useMipmaps
				&& format.filterModeMin != TextureFormat::FilterMin::LINEAR
				&& format.filterModeMin != TextureFormat::FilterMin::NEAREST) {
				// TODO: Error
			}

			int width, height, channels;

			// Create and bind a texture
			glGenTextures(1, &textureData.texture);
			glBindTexture(GL_TEXTURE_2D, textureData.texture);

			// Set parameteres
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, TextureFormat::enumToOpenGL(format.wrapModeU));
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, TextureFormat::enumToOpenGL(format.wrapModeV));

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, TextureFormat::enumToOpenGL(format.filterModeMin));
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, TextureFormat::enumToOpenGL(format.filterModeMag));

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
				image = new unsigned char[4 * 4] {
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
				delete [] image;
			}
		}

		return loadInfo.object;
	}
}