#pragma once
#include <string>
#include <unordered_map>

#include "Utils.h"

#include <S-Bin/File.h>

namespace Common {
	enum class StepType {
		OP,
		Symbole,
		ByteValue,
		WordValue,
		SymboleValue,
	};

	struct CompileData {
		std::string origFile;
		std::vector<std::pair<StepType, uint32_t>> steps;
	};

	struct BuildData {
		// shared
		std::unordered_map<uint32_t, std::string> symboles;
		
		// not shared
		uint32_t errorCount = 0;
		static BuildData* instance;
	};

	inline BuildData& GetGData() {
		return *BuildData::instance;
	}
	inline void CreateGData() {
		BuildData::instance = new BuildData();
	}
	inline void DestroyGData() {
		delete BuildData::instance;
	}

#define ERROR(msg, ...) { fprintf(stderr, msg, __VA_ARGS__); return nullptr; }
#define ERROR_L(msg, ...) { fprintf(stderr, msg, __VA_ARGS__); ::Common::GetGData().errorCount++;continue; }
#define ERROR_C(msg, ...) { fprintf(stderr, msg, __VA_ARGS__); ::Common::GetGData().errorCount++;continue; }
#define ERROR_C_R(msg, ...) { fprintf(stderr, msg, __VA_ARGS__); ::Common::GetGData().errorCount++; }

	enum class Status {
		Error,
		Accumulator,
		Immediate,
		ZeroPage,
		ZeroPageX,
		ZeroPageY,
		Relative,
		Absolut,
		AbsolutX,
		AbsolutY,
		Indirect,
		IndexdInd,
		IndIndex,
		Label,
	};

	static Status GetValue(const std::string& line, uint64_t offset, std::string& outValue) {
		size_t start = line.find_first_not_of("\t ", offset);
		size_t end = line.find_first_of(" \t", start + 1);

		std::string value = line.substr(start, end - start);

		if (value.length() == 0) {
			return Status::Error;
		}

		// #0000 -> immediate
		if (value[0] == '#' && value.length() > 1) {
			outValue = value.substr(1);
			return Status::Immediate;
		}
		if (value[0] == '$') {
			//$00 -> Zero Page
			if (value.length() == 3) {
				outValue = value.substr(1);
				return Status::ZeroPage;
			}
			if (value[value.length() - 2] == ',') {
				// $3000,X/Y -> Absolute X/Y
				if (value.length() == 7) {
					outValue = value.substr(1, 4);
				}
				// $30,X/Y -> Zero Page X/Y
				else if (value.length() == 5) {
					outValue = value.substr(1, 2);
				}
				else {
					return Status::Error;
				}
				if (value[value.length() - 1] == 'x') {
					if (value.length() == 7) {
						return Status::AbsolutX;
					}
					else {
						return Status::ZeroPageX;
					}
				}
				if (value[value.length() - 1] == 'y') {
					if (value.length() == 7) {
						return Status::AbsolutY;
					}
					else {
						return Status::ZeroPageY;
					}
				}
				return Status::Error;
			}
			//$0000 -> Absolut
			if (value.length() == 5) {
				outValue = value.substr(1);
				return Status::Absolut;
			}
		}
		else if (std::isalpha(value[0])) {
			outValue = value;
			return Status::Label;
		}
		return Status::Error;
	}

	inline std::string SymName(const std::string& symbole, const std::string& fName, const std::vector<std::string>& globals) {
		if (std::find(globals.begin(), globals.end(), symbole) != globals.end()) {
			return MakeSymboleName(symbole, "_global");
		}
		else {
			return MakeSymboleName(symbole, fName);
		}
	}
	inline uint32_t CompilerSymID(const std::string& symbole, const std::string& fName, const std::vector<std::string>& globals) {
		auto name = SymName(symbole, fName, globals);
		auto id = MakeSymboleID(name);
		Common::GetGData().symboles[id] = name;
		return id;
	}
}

namespace SBin {
	inline Ref<Stream>& operator<<(Ref<Stream>& stream, const Common::CompileData& data) {
		stream << data.origFile << data.steps;
		return stream;
	}
	inline Ref<Stream>& operator>>(Ref<Stream>& stream, Common::CompileData& data) {
		stream >> data.origFile >> data.steps;
		return stream;
	}

	inline Ref<Stream>& operator<<(Ref<Stream>& stream, const Common::BuildData& data) {
		stream << data.symboles;
		return stream;
	}
	inline Ref<Stream>& operator>>(Ref<Stream>& stream, Common::BuildData& data) {
		stream >> data.symboles;
		return stream;
	}

	inline Ref<Stream>& operator<<(Ref<Stream>& stream, const Common::StepType& type) {
		stream << (uint32_t)type;
		return stream;
	}
	inline Ref<Stream>& operator>>(Ref<Stream>& stream, Common::StepType& type) {
		uint32_t data;
		stream >> data;
		type = (Common::StepType)data;
		return stream;
	}
}
