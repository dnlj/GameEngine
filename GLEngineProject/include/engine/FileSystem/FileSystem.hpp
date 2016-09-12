#pragma once

#include <filesystem>

// TODO: After C++17 convert over to std::filesystem and get rid of this class

namespace engine {
	namespace FileSystem {
		using namespace ::std::experimental::filesystem;
	}
}