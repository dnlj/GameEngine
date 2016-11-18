#pragma once

// C++ STD
#include <string>

// glLoadGen
#include <glloadgen/gl_core_4_5.hpp>


namespace engine {
	// TODO: move to own file
	class InvalidTextureFormat : public std::runtime_error {
		public:
		// TODO: since this does nothing change it to  "using std::runtime_error::runtime_error;"
			InvalidTextureFormat(const std::string& message) : runtime_error{message} {
			}
	};

	class TextureFormat {
		public:
			enum class FilterMin {
				NEAREST,
				LINEAR,
				NEAREST_MIPMAP_NEAREST,
				LINEAR_MIPMAP_NEAREST,
				NEAREST_MIPMAP_LINEAR,
				LINEAR_MIPMAP_LINEAR,
			};

			enum class FilterMag {
				NEAREST,
				LINEAR,
			};

			enum class Filter {
				NEAREST,
				LINEAR,
				BILINEAR,
				TRILINEAR,
				ANISOTROPIC, // TODO: implement anisotropic
			};

			enum class Wrap {
				REPEAT,
				CLAMP_TO_EDGE,
				CLAMP_TO_BORDER,
				MIRRORED_REPEAT,
				MIRROR_CLAMP_TO_EDGE,
			};

			enum class Type {
				TEXTURE_ERROR,
				TEXTURE_1D,
				TEXTURE_2D,
				TEXTURE_3D,
				TEXTURE_CUBE,
			};

			bool operator==(const TextureFormat &format) const;

			void checkValid() const;
			
			FilterMin getMinFilter() const;
			FilterMag getMagFilter() const;

			bool useGammaCorrection = true;
			bool useMipmaps = true;
			Wrap wrapU = Wrap::REPEAT;
			Wrap wrapV = Wrap::REPEAT;
			Filter filter = Filter::BILINEAR;
			unsigned char filterLevel = 0; // 2, 4, 8, 16


		////////////////////////////////////////////////////////////////
		// Static Stuff
		////////////////////////////////////////////////////////////////
		public:
			static GLenum enumToOpenGL(FilterMin e);
			static GLenum enumToOpenGL(FilterMag e);
			static GLenum enumToOpenGL(Wrap e);

			static Type stringToType(const std::string& type);
			static Filter stringToFilter(const std::string& type);
			static Wrap stringToWrap(const std::string& type);
	};
}