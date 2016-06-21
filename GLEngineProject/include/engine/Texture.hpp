#pragma once

#include <iostream>
#include <string>
#include <unordered_map>

// glLoadGen
#include <glloadgen/gl_core_4_5.h>

// Engine
#include <engine/util.hpp>
#include <engine/TextureData.hpp>

// SOIL
#include <SOIL.h>

namespace engine {
	class Texture {
		public:
			Texture::Texture(size_t idx);
			~Texture();

			GLuint getTexture();

			bool operator==(const Texture &tex) const;

		private:
			size_t index;

		////////////////////////////////////////////////////////////////
		// Static Stuff
		////////////////////////////////////////////////////////////////
		public:
			static Texture loadTexture(const std::string &path, const TextureFormat &format);
			static void cleanup();

		private:
			static std::vector<TextureData> textures;
			static std::unordered_map<std::string, size_t> pathLookup;

	};
}