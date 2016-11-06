#pragma once

#include <engine/ShaderData.hpp>
#include <engine/Resource.hpp>


// TODO: More testing, to make sure the Resrouce class is working with cleanup and duplicate load files
namespace engine {
	class Shader : public Resource<Shader, ShaderData> {
		public:
			using Resource<Shader, ShaderData>::Resource;

			/**
			 * Deconstructor.
			 */
			~Shader();

			/**
			 * Gets the OpenGl type of the Shader.
			 * 
			 * @return The OpenGL type of the Shader.
			 */
			GLenum getType() const;

			/**
			 * Get the OpenGl Shader id.
			 * 
			 * @return The OpenGL Shader id.
			 */
			GLuint getShader() const;


		////////////////////////////////////////////////////////////////
		// Static
		////////////////////////////////////////////////////////////////
		public:
			/**
			 * Loads a Shader from a file.
			 * 
			 * @param[in] path The file to load the Shader from.
			 * @param[in] type The OpenGL type of the Shader.
			 * @return The Shader that was loaded.
			 */
			static Shader load(const ResourcePath& path, const GLenum type);
	};
}