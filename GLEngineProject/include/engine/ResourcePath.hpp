#pragma once

#include <iostream>
#include <iomanip>
#include <string>
#include <algorithm>
#include <unordered_map>
#include <regex>

#include <engine/FileSystem/FileSystem.hpp>


namespace engine {
	class ResourcePath {
		public:
			ResourcePath(const std::string& path);
			ResourcePath(const char* path);
			ResourcePath(const std::string& type, const std::string& path);

			bool operator==(const ResourcePath& other);
			bool operator!=(const ResourcePath& other);

			std::string getResolvedPath() const;

		private:
			std::string path;
			std::string type;

			void checkPath();

		////////////////////////////////////////////////////////////////
		// Static
		////////////////////////////////////////////////////////////////
		public:
			static void AddResourceDir(const std::string& type, const std::string& dir);
			static void RemoveResourceDir(const std::string& type, const std::string& dir);

		private:
			static std::unordered_map<std::string, std::vector<std::string>> typeLookup;
	};
}