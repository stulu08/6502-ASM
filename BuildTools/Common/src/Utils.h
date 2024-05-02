#pragma once
#include <string>
#include <iostream>
#include <algorithm>

namespace Common {
	inline bool StartsWith(const std::string& line, const std::string& match) {
		return line._Starts_with(match);
	}
	inline void replaceAll(std::string& str, const std::string& from, const std::string& to) {
		if (from.empty())
			return;
		size_t start_pos = 0;
		while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
			str.replace(start_pos, from.length(), to);
			start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
		}
	}
	inline void toLower(std::string& lower) {
		std::transform(lower.begin(), lower.end(), lower.begin(), [](char c) { return (char)std::tolower((int)c); });
	}
	inline std::string MakeSymboleName(const std::string& name, const std::string& fName) {
		return fName + "@" + name;
	}
	inline uint32_t MakeSymboleID(const std::string& name) {
		uint32_t hash = 0x811c9dc5;
		uint32_t prime = 0x1000193;

		for (int i = 0; i < name.size(); ++i) {
			uint8_t value = name[i];
			hash = hash ^ value;
			hash *= prime;
		}
		return hash;
	}
}