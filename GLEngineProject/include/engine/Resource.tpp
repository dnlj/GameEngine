#pragma once

namespace engine {
	template<class Type, class TypeData>
	decltype(Resource<Type, TypeData>::container) Resource<Type, TypeData>::container;

	template<class Type, class TypeData>
	decltype(Resource<Type, TypeData>::lookupMap) Resource<Type, TypeData>::lookupMap;

	template<class Type, class TypeData>
	decltype(Resource<Type, TypeData>::deletedIndices) Resource<Type, TypeData>::deletedIndices;


	template<class Type, class TypeData>
	Resource<Type, TypeData>::Resource(engine::index index) : index{index} {
	}


	template<class Type, class TypeData>
	Resource<Type, TypeData>::Resource(const IndexData& data)
		: index{data.index}
		, refCounter{data.refCounter} {
	}

	template<class Type, class TypeData>
	Resource<Type, TypeData>::~Resource() {
	}


	template<class Type, class TypeData>
	ReferenceCounter::CounterType Resource<Type, TypeData>::getRefCount() const {
		return refCounter.getCount();
	}

	template<class Type, class TypeData>
	engine::index Resource<Type, TypeData>::getIndex() const {
		return index;
	}


	////////////////////////////////////////////////////////////////
	// Static
	////////////////////////////////////////////////////////////////
	template<class Type, class TypeData>
	void Resource<Type, TypeData>::cleanup() {
		for (auto it = lookupMap.begin(); it != lookupMap.end();) {

			// Remove anything that is no longer referenced externally
			if (it->second.refCounter.getCount() == 1) {
				container[it->second.index].deleteData();
				deletedIndices.emplace_back(it->second.index);
				it = lookupMap.erase(it);
			} else {
				++it;
			}
		}
	}

	template<class Type, class TypeData>
	typename Resource<Type, TypeData>::ResourceLoadInfo Resource<Type, TypeData>::loadResource(const std::string& path) {
		auto& found = lookupMap.find(path);

		// Has not been loaded yet or cleanup has already been called
		if (found == lookupMap.end()) {
			Type resrc{getNextIndex()};

			lookupMap.emplace(path, IndexData{resrc.index, resrc.refCounter});

			return {resrc, false};
		}

		// Has already been loaded and is either currently still in use or has not been cleaned up yet
		return {Type{found->second}, true};
	}

	template<class Type, class TypeData>
	engine::index Resource<Type, TypeData>::getNextIndex() {
		engine::index index;

		// Check if there are any positions that can be re-used. If not create a new one.
		if (deletedIndices.size() > 0) {
			index = deletedIndices.back();
			deletedIndices.pop_back();
		} else {
			index = static_cast<engine::index>(container.size());
			container.emplace_back();
		}

		return index;
	}
}