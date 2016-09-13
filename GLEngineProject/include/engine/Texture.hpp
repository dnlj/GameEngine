#pragma once

// glLoadGen
#include <glloadgen/gl_core_4_5.h>

// SOIL
#include <SOIL.h>

// Engine
#include <engine/Resource.hpp>
#include <engine/TextureData.hpp>

// TODO: More testing, to make sure the Resrouce class is working with cleanup and duplicate load files
namespace engine {
	class Texture : public Resource<Texture, TextureData> {
		public:
			using Resource<Texture, TextureData>::Resource;
			~Texture();

			bool operator==(const Texture& tex) const;

			GLuint getTexture();

		////////////////////////////////////////////////////////////////
		// Static
		////////////////////////////////////////////////////////////////
		public:
			static Texture loadTexture(const ResourcePath& path, const TextureFormat& format);
	};
}