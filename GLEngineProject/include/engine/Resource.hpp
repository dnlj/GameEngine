#pragma once

// STD
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <fstream>

// Engine
#include <engine/ResourcePath.hpp>
#include <engine/ReferenceCounter.hpp>
#include <engine/IndexData.hpp>

namespace engine {
	template<class Type, class TypeData>
	class Resource {
		public:
			Resource(engine::index index);
			Resource(const IndexData& data);
			virtual ~Resource() = 0;

			ReferenceCounter::CounterType getRefCount() const;
			engine::index getIndex() const;

		protected:
			ReferenceCounter refCounter;
			engine::index index;


		////////////////////////////////////////////////////////////////
		// Static
		////////////////////////////////////////////////////////////////
		public:
			static void cleanup();

		protected:
			class ResourceLoadInfo {
				public:
					Type object;
					TypeData& data;
					bool alreadyLoaded;

			};

			static ResourceLoadInfo loadResource(const std::string& path);
			static const TypeData& getDataAt(engine::index index);

		private:
			static engine::index getNextIndex();
			static std::unordered_map<std::string, IndexData> lookupMap;
			static std::vector<TypeData> container;
			static std::vector<engine::index> deletedIndices; // TODO: may want to use some kind of queue or something here
	};
}

// Include the template implementation
#include <engine/Resource.tpp>