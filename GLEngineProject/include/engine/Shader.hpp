#pragma once

#include <engine/ShaderData.hpp>
#include <engine/Resource.hpp>


// TODO: More testing, to make sure the Resrouce class is working with cleanup and duplicate load files
namespace engine {
	class Shader : public Resource<Shader, ShaderData> {
		public:
			using Resource<Shader, ShaderData>::Resource;
			virtual ~Shader();

			GLenum getType() const;
			GLuint getShader() const;


		////////////////////////////////////////////////////////////////
		// Static
		////////////////////////////////////////////////////////////////
		public:
			static Shader loadShader(const ResourcePath& path, const GLenum type);
	};
}