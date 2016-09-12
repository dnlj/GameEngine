#pragma once

#include <engine/engine.hpp>
#include <engine/ReferenceCounter.hpp>

namespace engine {
	class IndexData {
		public:
			engine::index index;
			ReferenceCounter refCounter;
	};
}