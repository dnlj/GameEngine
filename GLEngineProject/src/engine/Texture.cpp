#include <engine/Texture.hpp>

namespace engine {
	Texture::Texture(engine::index idx) : index{idx} {
	}

	Texture::~Texture() {
	}

	GLuint Texture::getTexture() {
		// NOTE: Optimization: Could speed this up by caching the actual texture id (the GLuint) on the texture object
		return textures[index].texture;
	}

	bool Texture::operator==(const Texture &tex) const {
		return index == tex.index;
	};

	////////////////////////////////////////////////////////////////
	// Static Stuff
	////////////////////////////////////////////////////////////////
	std::vector<TextureData> Texture::textures;
	std::unordered_map<std::string, engine::index> Texture::pathLookup;


	Texture Texture::loadTexture(const std::string &path, const TextureFormat &format) {
		// TODO: Add an option to the TextureFormat for the internal format (GL_RGBA, GL_SRGB8_ALPHA8, GL_RGB, etc)
		//			So that you can make rgba, rgb, and grayscale textures if you need them

		// TODO: Need to add path/TextureData check ot see if we already loeadded the textue

		{ // TODO: Untested
			auto it = pathLookup.find(path);
			if (it != pathLookup.end()) {
				if (textures[it->second].format == format) {
					return Texture{it->second};
				}
			}
		}
		
		if (!format.useMipmaps && format.filterModeMin != TextureFormat::FilterMin::LINEAR
			&& format.filterModeMin != TextureFormat::FilterMin::NEAREST) {
			// TODO: Error
		}


		TextureData textureData;
		int width, height, channels;
		
		glGenTextures(1, &textureData.texture);
		glBindTexture(GL_TEXTURE_2D, textureData.texture);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, TextureFormat::enumToOpenGL(format.wrapModeU));
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, TextureFormat::enumToOpenGL(format.wrapModeV));

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, TextureFormat::enumToOpenGL(format.filterModeMin));
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, TextureFormat::enumToOpenGL(format.filterModeMag));

		unsigned char* image;
		image = SOIL_load_image(path.c_str(), &width, &height, &channels, SOIL_LOAD_RGBA);

		if (format.useGammaCorrection) {
			glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB8_ALPHA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
		} else {
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
		}


		if (format.useMipmaps) {
			glGenerateMipmap(GL_TEXTURE_2D);
		}

		glBindTexture(GL_TEXTURE_2D, 0);
		SOIL_free_image_data(image);

		engine::index index = static_cast<engine::index>(textures.size());
		textureData.format = format;
		textures.push_back(textureData);
		pathLookup[path] = index; // TODO: Untested

		return Texture{index};
	}

	void Texture::cleanup() {
		for (const TextureData& data : textures) { // TODO: since this is a reference to a vector element could this cause problems if the vector re-allocates?
			glDeleteTextures(1, &data.texture);
		}

		textures = {};
		textures.shrink_to_fit();

		pathLookup = {};
		pathLookup.rehash(11);
	}
}
