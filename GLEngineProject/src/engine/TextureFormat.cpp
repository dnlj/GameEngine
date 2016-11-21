// Engine
#include <engine/util/util.hpp>
#include <engine/TextureFormat.hpp>


namespace engine {
	bool TextureFormat::operator==(const TextureFormat &format) const {
		return (useGammaCorrection == format.useGammaCorrection)
			&& (useMipmaps == format.useMipmaps)
			&& (wrapU == format.wrapU)
			&& (wrapV == format.wrapV)
			&& (filter == format.filter);
	}

	void TextureFormat::checkValid() const {
		if (!useMipmaps) {
			if (filter != Filter::NEAREST && filter != Filter::LINEAR) {
				throw InvalidTextureFormat{"When TextureFormat::filter is not Filter::NEAREST or Filter::LINEAR mipmaps must be enabled"};
			}
		}
	}

	TextureFormat::FilterMag TextureFormat::getMagFilter() const {
		switch (filter) {
			case Filter::NEAREST:
				return FilterMag::NEAREST;
			case Filter::LINEAR:
			case Filter::BILINEAR:
			case Filter::TRILINEAR:
				return FilterMag::LINEAR;
			case Filter::ANISOTROPIC:
				engine_error("Not yet implemented");
		}

		return FilterMag::NEAREST;
	}

	TextureFormat::FilterMin TextureFormat::getMinFilter() const {
		switch (filter) {
			case Filter::NEAREST:
				return FilterMin::NEAREST;
			case Filter::LINEAR:
				return FilterMin::LINEAR;
			case Filter::BILINEAR:
				return FilterMin::LINEAR_MIPMAP_NEAREST;
			case Filter::TRILINEAR:
				return FilterMin::LINEAR_MIPMAP_LINEAR;
			case Filter::ANISOTROPIC:
				engine_error("Not yet implemented");
		}

		return FilterMin::NEAREST;
	}

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
				return GL_INVALID_ENUM;
		}
	}
	
	GLenum TextureFormat::enumToOpenGL(FilterMag e) {
		switch (e) {
			case FilterMag::NEAREST:
				return GL_NEAREST;
			case FilterMag::LINEAR:
				return GL_LINEAR;
			default:
				return GL_INVALID_ENUM;
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
				return GL_INVALID_ENUM;
		}
	}

	GLenum TextureFormat::enumToOpenGL(Type e) {
		switch (e) {
			case Type::TEXTURE_1D:
				return GL_TEXTURE_1D;
			case Type::TEXTURE_2D:
				return GL_TEXTURE_2D;
			case Type::TEXTURE_3D:
				return GL_TEXTURE_3D;
			case Type::TEXTURE_CUBE:
				return GL_TEXTURE_CUBE_MAP;
			default:
				return GL_INVALID_ENUM;
		}
	}

	TextureFormat::Type TextureFormat::stringToType(const std::string& type) {
		if (type == "TEXTURE_1D") {
			return Type::TEXTURE_1D;
		} else if (type == "TEXTURE_2D") {
			return Type::TEXTURE_2D;
		} else if (type == "TEXTURE_3D") {
			return Type::TEXTURE_3D;
		} else if (type == "TEXTURE_CUBE") {
			return Type::TEXTURE_CUBE;
		}

		engine_error("Unable to convert " + type + " to TextureFormat::Type");
		return Type::TEXTURE_1D;
	}

	TextureFormat::Filter TextureFormat::stringToFilter(const std::string& filter) {
		if (filter == "NEAREST") {
			return Filter::NEAREST;
		} else if (filter == "LINEAR") {
			return Filter::LINEAR;
		} else if (filter == "BILINEAR") {
			return Filter::BILINEAR;
		} else if (filter == "TRILINEAR") {
			return Filter::TRILINEAR;
		} else if (filter == "ANISOTROPIC") {
			return Filter::ANISOTROPIC;
		}

		engine_error("Unable to convert " + filter + " to TextureFormat::Filter");
		return Filter::NEAREST;
	}

	TextureFormat::Wrap TextureFormat::stringToWrap(const std::string& wrap) {
		if (wrap == "REPEAT") {
			return Wrap::REPEAT;
		} else if (wrap == "CLAMP_TO_EDGE") {
			return Wrap::CLAMP_TO_EDGE;
		} else if (wrap == "CLAMP_TO_BORDER") {
			return Wrap::CLAMP_TO_BORDER;
		} else if (wrap == "MIRRORED_REPEAT") {
			return Wrap::MIRRORED_REPEAT;
		} else if (wrap == "MIRROR_CLAMP_TO_EDGE") {
			return Wrap::MIRROR_CLAMP_TO_EDGE;
		}

		engine_error("Unable to convert " + wrap + " to TextureFormat::Wrap");
		return Wrap::CLAMP_TO_EDGE;
	}
}