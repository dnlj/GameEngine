#include <engine/TextureFormat.hpp>

namespace engine {
	GLenum TextureFormat::enumToOpenGL(FilterMin e) {
		switch (e) {
			case FilterMin::NEAREST:
				return GL_NEAREST;
			case FilterMin::LINEAR:
				return GL_LINEAR;
			case FilterMin::NEAREST_MIPMAP_NEAREST:
				return GL_NEAREST_MIPMAP_NEAREST;
			case FilterMin::LINEAR_MIPMAP_NEAREST:
				return GL_LINEAR_MIPMAP_NEAREST;
			case FilterMin::NEAREST_MIPMAP_LINEAR:
				return GL_NEAREST_MIPMAP_LINEAR;
			case FilterMin::LINEAR_MIPMAP_LINEAR:
				return GL_LINEAR_MIPMAP_LINEAR;
			default:
				return GL_INVALID_ENUM; // TODO: Not sure if this is the right thing to return
		}
	}
	
	GLenum TextureFormat::enumToOpenGL(FilterMag e) {
		switch (e) {
			case FilterMag::NEAREST:
				return GL_NEAREST;
			case FilterMag::LINEAR:
				return GL_LINEAR;
			default:
				return GL_INVALID_ENUM; // TODO: Not sure if this is the right thing to return
		}
	}
	
	GLenum TextureFormat::enumToOpenGL(Wrap e) {
		switch (e) {
			case Wrap::CLAMP_TO_EDGE:
				return GL_CLAMP_TO_EDGE;
			case Wrap::CLAMP_TO_BORDER:
				return GL_CLAMP_TO_BORDER;
			case Wrap::MIRRORED_REPEAT:
				return GL_MIRRORED_REPEAT;
			case Wrap::REPEAT:
				return GL_REPEAT;
			case Wrap::MIRROR_CLAMP_TO_EDGE:
				return GL_MIRROR_CLAMP_TO_EDGE;
			default:
				return GL_INVALID_ENUM; // TODO: Not sure if this is the right thing to return
		}
	}

	bool TextureFormat::operator==(const TextureFormat &format) const {
		return (useGammaCorrection == format.useGammaCorrection)
			&& (useMipmaps == format.useMipmaps)
			&& (wrapModeU == format.wrapModeU)
			&& (wrapModeV == format.wrapModeV)
			&& (filterModeMin == format.filterModeMin)
			&& (filterModeMag == format.filterModeMag);
	}
}