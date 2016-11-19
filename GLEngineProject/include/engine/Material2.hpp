#pragma once

// Engine
#include <engine/Resource.hpp>
#include <engine/MaterialData2.hpp>
#include <engine/json/json.hpp>

namespace engine {
	class Material2 : public Resource<Material2, MaterialData2> {
		public:
			using Resource<Material2, MaterialData2>::Resource;
			~Material2() = default;

			ShaderProgram2 getProgram() const;
			void use() const;

		////////////////////////////////////////////////////////////////
		// Static
		////////////////////////////////////////////////////////////////
		public:
			static Material2 load(const ResourcePath& path);
	};
}