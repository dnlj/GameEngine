#include <engine/ResourcePath.hpp>

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

	// TODO: unstested
	std::string ResourcePath::getResolvedPath() const {
		const auto& vec = typeLookup[type];

		// Remove any paths equivalent to dir
		for (auto it = vec.begin(); it != vec.end(); ++it) {
			auto p = *it + path;
			if (FileSystem::exists(p)) {
				return p;
			}
		}

		return path;
	}

	void ResourcePath::checkPath() {
		if (path.find("..") != std::string::npos) {
			// TODO: Add actual error handling
			std::cerr << "ERROR: \"..\" not allowed in resource paths.\n";
		}
	}

	////////////////////////////////////////////////////////////////
	// Static
	////////////////////////////////////////////////////////////////
	decltype(ResourcePath::typeLookup) ResourcePath::typeLookup;

	void ResourcePath::AddResourceDir(const std::string& type, const std::string& dir) {
		auto& vec = typeLookup[type];
	
		//FileSystem::create_directories(dir); // TODO: Remove this is just for testing

		if (!FileSystem::exists(dir)) {
			// TODO :add proper error handling
			// not even sure if this is really an error
			std::cerr << "ERROR: Path does not exist.\n";
			return;
		} else {
			for (const auto& v : vec) {
				std::error_code code; // TODO: Handel this error code
				if (FileSystem::equivalent(v, dir)) {
					// TODO :add proper error handling
					std::cerr << "ERROR: attempted to add a path that has already been added.\n";
					return;
				}

				//std::cout << code.message() << std::endl; // TODO: Remove
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