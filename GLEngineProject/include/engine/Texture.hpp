#pragma once

// C++ STD
#include <string>

// glLoadGen
#include <glloadgen/gl_core_4_5.hpp>

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

			GLuint getTexture() const;

		////////////////////////////////////////////////////////////////
		// Static
		////////////////////////////////////////////////////////////////
		public:
			static Texture load(const ResourcePath& path);
			//static Texture load(const ResourcePath& path);

		private:
			using LoadInfo = Resource<Texture, TextureData>::Resource::ResourceLoadInfo;

			//static void load1DTexture
			static void load2DTexture(const std::string& resolvedPath, const TextureFormat& format, LoadInfo& loadInfo);
			//static void load3DTexture
			//static void loadCubeTexture
	};
}