#pragma once

// glLoadGen
#include <glloadgen/gl_core_4_4.h>


namespace engine {
	class TextureFormat {
		public:
			enum class FilterMin { // TODO: Should i make this Nearest, Bilinear, Trilinear, Anisotropic 2x, Anisotropic 4x, ..., Anisotropic 32x, Anisotropic 64x, etc, probably just pass a int for the power of two
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

			enum class Wrap {
				CLAMP_TO_EDGE,
				CLAMP_TO_BORDER,
				MIRRORED_REPEAT,
				REPEAT,
				MIRROR_CLAMP_TO_EDGE,
			};

			bool operator==(const TextureFormat &format) const;

			bool useGammaCorrection = true;
			bool useMipmaps = true;
			Wrap wrapModeU = Wrap::REPEAT;
			Wrap wrapModeV = Wrap::REPEAT;
			FilterMin filterModeMin = FilterMin::LINEAR_MIPMAP_LINEAR;
			FilterMag filterModeMag = FilterMag::LINEAR;
		

		////////////////////////////////////////////////////////////////
		// Static Stuff
		////////////////////////////////////////////////////////////////
		public:
			static GLenum enumToOpenGL(FilterMin e);
			static GLenum enumToOpenGL(FilterMag e);
			static GLenum enumToOpenGL(Wrap e);
	};
}