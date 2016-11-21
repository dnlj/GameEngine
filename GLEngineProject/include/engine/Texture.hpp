#pragma once

// C++ STD
#include <string>

// glLoadGen
#include <glloadgen/gl_core_4_5.hpp>

// RapidJSON
#include <rapidjson/document.h>

// Engine
#include <engine/Resource.hpp>
#include <engine/TextureData.hpp>


// TODO: More testing, to make sure the Resrouce class is working with cleanup and duplicate load files
namespace engine {
	class Texture : public Resource<Texture, TextureData> {
		public:
			using Resource<Texture, TextureData>::Resource;

			/**
			 * Deconstructor.
			 */
			~Texture();

			/**
			 * Equality operator.
			 */
			bool operator==(const Texture& tex) const;

			/**
			 * Gets the OpenGL texture type.
			 *
			 * @return The OpenGL texture type.
			 */
			GLuint getType() const;

			/**
			 * Gets the OpenGL texture id.
			 * 
			 * @return The OpenGL texture id.
			 */
			GLuint getTexture() const;

		////////////////////////////////////////////////////////////////
		// Static
		////////////////////////////////////////////////////////////////
		public:
			/**
			 * Loads a Texture from a file.
			 * 
			 * @param[in] The file to load the Texture from.
			 * @return The Texture that was loaded.
			 */
			static Texture load(const ResourcePath& path);

		private:
			using LoadInfo = Resource<Texture, TextureData>::Resource::ResourceLoadInfo;

			// TODO: static void load1DTexture

			/**
			 * A loads a 2D Texture.
			 * 
			 * @param[in] document The documet containing the Texture information.
			 * @param[in] format The TextureFormat to use for the Texture.
			 * @param[in] loadInfo The LoadInfo for the Texture.
			 * @return void
			 */
			static void load2DTexture(rapidjson::Document& document, const TextureFormat& format, LoadInfo& loadInfo);

			// TODO: static void load3DTexture

			/**
			 * A loads a cubemap Texture.
			 *
			 * @param[in] document The documet containing the Texture information.
			 * @param[in] format The TextureFormat to use for the Texture.
			 * @param[in] loadInfo The LoadInfo for the Texture.
			 * @return void
			 */
			static void loadCubeTexture(rapidjson::Document& document, const TextureFormat& format, LoadInfo& loadInfo, const std::string& resolvedPath);

			/**
			 * Sets the cube faces from a horizontal cross cubemap.
			 * 
			 * @param[in] image The source cubemap image.
			 * @param[in] width The width of @p image
			 * @param[in] height The height of @p image
			 * @param[in] channels The number of channels in @p image
			 * @retunr void
			 */
			static void setupHorizontalCross(unsigned char* image, int width, int height, int channels);

			/**
			 * Sets the cube faces from a vertical cross cubemap.
			 *
			 * @param[in] image The source cubemap image.
			 * @param[in] width The width of @p image
			 * @param[in] height The height of @p image
			 * @param[in] channels The number of channels in @p image
			 * @retunr void
			 */
			static void setupVerticalCross(unsigned char* image, int width, int height, int channels);

			/**
			 * Sets the cube faces from a horizontal line cubemap.
			 *
			 * @param[in] image The source cubemap image.
			 * @param[in] width The width of @p image
			 * @param[in] height The height of @p image
			 * @param[in] channels The number of channels in @p image
			 * @retunr void
			 */
			static void setupHorizontalLine(unsigned char* image, int width, int height, int channels);

			/**
			 * Sets the cube faces from a vertical line cubemap.
			 *
			 * @param[in] image The source cubemap image.
			 * @param[in] width The width of @p image
			 * @param[in] height The height of @p image
			 * @param[in] channels The number of channels in @p image
			 * @retunr void
			 */
			static void setupVerticalLine(unsigned char* image, int width, int height, int channels);

			/**
			 * Sets the cube faces from an array of files.
			 *
			 * @param[in] images The file paths to the images.
			 * @retunr void
			 */
			static void setupSplitSides(const std::vector<std::string>& images);

	};
}