#pragma once
#include <string>
#include <vector>

namespace Linker {
	int MainLink(const std::vector<std::string>& inFiles, const std::string& outFile, const std::string& gData);
}