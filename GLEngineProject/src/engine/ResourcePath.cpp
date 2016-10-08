// STD
#include <iostream>
#include <string>

// Engine
#include <engine/ResourcePath.hpp>
#include <engine/engine.hpp>
#include <engine/FileSystem/FileSystem.hpp>


namespace engine {
	ResourcePath::ResourcePath(const std::string& path)
		: type{}
		, path{} {

		if (path.size() == 0) { return; }

		bool found = false;

		for (auto& it = path.begin(); it != path.end() - 1; ++it) {
			if (*it == ':') {
				type = std::string{path.begin(), it};
				this->path = std::string{it+1, path.end()};
				found = true;

				break;
			}
		}

		if (!found) {
			this->path = path;
		}
	
		checkPath();
	}

	ResourcePath::ResourcePath(const char* path) : ResourcePath{std::string{path}} {
	}

	ResourcePath::ResourcePath(const std::string& type, const std::string& path)
		: type{type}
		, path{path} {

		checkPath();
	}

	bool ResourcePath::operator==(const ResourcePath& other) {
		return (type == other.type) && (path == other.path);
	}

	bool ResourcePath::operator!=(const ResourcePath& other) {
		return !(*this == other);
	}

	std::string ResourcePath::getResolvedPath() const {
		const auto& vec = typeLookup[type];

		// Get the first path of the file
		for (auto it = vec.begin(); it != vec.end(); ++it) {
			auto p = *it + path;
			if (FileSystem::exists(p)) {
				return p;
			}
		}

		// The path doesnt exist
		return path;
	}

	void ResourcePath::checkPath() {
		// We require this so we dont have to do more complicated path resolution.
		if (path.find("..") != std::string::npos) {
			engine_error("\"..\" is not allowed in resource paths.");
		}
	}

	////////////////////////////////////////////////////////////////
	// Static
	////////////////////////////////////////////////////////////////
	decltype(ResourcePath::typeLookup) ResourcePath::typeLookup;

	void ResourcePath::AddResourceDir(const std::string& type, const std::string& dir) {
		auto& vec = typeLookup[type];

		if (!FileSystem::exists(dir)) {
			engine_warning("Path " + dir + " does not exist.");
		} else {
			for (const auto& v : vec) {
				if (FileSystem::equivalent(v, dir)) {
					engine_warning("attempted to add a path that has already been added.");
					return;
				}
			}
		}

		vec.emplace_back(dir);

		std::sort(vec.begin(), vec.end());
	}

	void ResourcePath::RemoveResourceDir(const std::string& type, const std::string& dir) {
		auto& vec = typeLookup[type];

		// Remove any paths equivalent to dir
		for (auto it = vec.begin(); it != vec.end();) {
			if (FileSystem::equivalent(*it, dir)) {
				it = vec.erase(it);
			} else {
				++it;
			}
		}
	}
}