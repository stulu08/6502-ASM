#include "Compiler.h"

#include <sstream>
#include <fstream>
#include <functional>

using namespace E6502;
using namespace Common;

namespace Compiler {

	bool Instruction_STA(const std::string& line, size_t charEnd, const std::string& fileName, uint32_t lineCount, std::vector<std::string>& globalSymboles, Common::CompileData& data) {
		std::string value;
		switch (GetValue(line, charEnd + 1, value)) {
		case Status::Absolut:
			data.steps.push_back({ StepType::OP, (uint32_t)Instruction::STA_ABS });
			data.steps.push_back({ StepType::WordValue, std::stoul(value, nullptr, 16) });
			return true;
		case Status::ZeroPage:
			data.steps.push_back({ StepType::OP, (uint32_t)Instruction::STA_ABS });
			data.steps.push_back({ StepType::ByteValue, std::stoul(value, nullptr, 16) });
			return true;
		case Status::Error:
			ERROR_C_R("Cannot parse value in line: %u\n\t> %s\n", lineCount, line.c_str());
			return true;
		default:
			ERROR_C_R("Unexpected value for sta instruction in line: %u\n\t> %s\n", lineCount, line.c_str());
			return true;
		}
	}
	bool Instruction_LDA(const std::string& line, size_t charEnd, const std::string& fileName, uint32_t lineCount, std::vector<std::string>& globalSymboles, Common::CompileData& data) {
		std::string value;
		switch (GetValue(line, charEnd + 1, value)) {
		case Status::Absolut:
			data.steps.push_back({ StepType::OP, (uint32_t)Instruction::LDA_ABS });
			data.steps.push_back({ StepType::WordValue, std::stoul(value, nullptr, 16) });
			return true;
		case Status::Immediate:
			data.steps.push_back({ StepType::OP, (uint32_t)Instruction::LDA_IM });
			data.steps.push_back({ StepType::ByteValue, std::stoul(value) });
			return true;
		case Status::ZeroPage:
			data.steps.push_back({ StepType::OP, (uint32_t)Instruction::LDA_ZP });
			data.steps.push_back({ StepType::ByteValue, std::stoul(value, nullptr, 16) });
			return true;
		case Status::ZeroPageX:
			data.steps.push_back({ StepType::OP, (uint32_t)Instruction::LDA_ZPX });
			data.steps.push_back({ StepType::ByteValue, std::stoul(value, nullptr, 16) });
			return true;
		case Status::Error:
			ERROR_C_R("Cannot parse value in line: %u\n\t> %s\n", lineCount, line.c_str());
			return true;
		default:
			ERROR_C_R("Unexpected value for lda instruction in line: %u\n\t> %s\n", lineCount, line.c_str());
			return true;
		}
	}
	bool Instruction_JSR(const std::string& line, size_t charEnd, const std::string& fileName, uint32_t lineCount, std::vector<std::string>& globalSymboles, Common::CompileData& data) {
		std::string value;
		switch (GetValue(line, charEnd + 1, value)) {
		case Status::Absolut:
			data.steps.push_back({ StepType::OP, (uint32_t)Instruction::JSR });
			data.steps.push_back({ StepType::WordValue, std::stoul(value, nullptr, 16) });
			return true;
		case Status::Label:
			data.steps.push_back({ StepType::OP, (uint32_t)Instruction::JSR });
			data.steps.push_back({ StepType::SymboleValue, CompilerSymID(value, fileName, globalSymboles) });
			return true;
		case Status::Error:
			ERROR_C_R("Cannot parse value in line: %u\n\t> %s\n", lineCount, line.c_str());
			return true;
		default:
			ERROR_C_R("Expected an absolut value for JSR instruction in line: %u\n\t> %s\n", lineCount, line.c_str());
			return true;
		}
	}
	bool Instruction_JMP(const std::string& line, size_t charEnd, const std::string& fileName, uint32_t lineCount, std::vector<std::string>& globalSymboles, Common::CompileData& data) {
		std::string value;
		switch (GetValue(line, charEnd + 1, value)) {
		case Status::Absolut:
			data.steps.push_back({ StepType::OP, (uint32_t)Instruction::JMP_ABS });
			data.steps.push_back({ StepType::WordValue, std::stoul(value, nullptr, 16) });
			return true;
		case Status::Label:
			data.steps.push_back({ StepType::OP, (uint32_t)Instruction::JMP_ABS });
			data.steps.push_back({ StepType::SymboleValue, CompilerSymID(value, fileName, globalSymboles) });
			return true;
		case Status::Error:
			ERROR_C_R("Cannot parse value in line: %u\n\t> %s\n", lineCount, line.c_str());
			return true;
		default:
			ERROR_C_R("Expected an absolut or indirect value for JMP instruction in line: %u\n\t> %s\n", lineCount, line.c_str());
			return true;
		}
	}
	bool Instruction_PLA(const std::string& line, size_t charEnd, const std::string& fileName, uint32_t lineCount, std::vector<std::string>& globalSymboles, Common::CompileData& data) {
		data.steps.push_back({ StepType::OP, (uint32_t)Instruction::PLA });
		return true;
	}
	bool Instruction_PHA(const std::string& line, size_t charEnd, const std::string& fileName, uint32_t lineCount, std::vector<std::string>& globalSymboles, Common::CompileData& data) {
		data.steps.push_back({ StepType::OP, (uint32_t)Instruction::PHA });
		return true;
	}
	bool Instruction_BRK(const std::string& line, size_t charEnd, const std::string& fileName, uint32_t lineCount, std::vector<std::string>& globalSymboles, Common::CompileData& data) {
		data.steps.push_back({ StepType::OP, (uint32_t)Instruction::BRK });
		return true;
	}
	bool Instruction_RTS(const std::string& line, size_t charEnd, const std::string& fileName, uint32_t lineCount, std::vector<std::string>& globalSymboles, Common::CompileData& data) {
		data.steps.push_back({ StepType::OP, (uint32_t)Instruction::RTS });
		return true;
	}


	bool Instruction_EXTERN(const std::string& line, size_t charEnd, const std::string& fileName, uint32_t lineCount, std::vector<std::string>& globalSymboles, Common::CompileData& data) {
		size_t start = line.find_first_not_of("\t ", charEnd + 1);
		size_t end = line.find_first_of(" \t", start + 1);
		if (start == line.npos || end < start) {
			ERROR_C_R("Cannot parse name in line: %u\n\t> %s\n", lineCount, line.c_str());
			return true;
		}
		std::string name = line.substr(start, end - start);
		globalSymboles.push_back(name);
		return true;
	}
	bool Instruction_GLOBAL(const std::string& line, size_t charEnd, const std::string& fileName, uint32_t lineCount, std::vector<std::string>& globalSymboles, Common::CompileData& data) {
		size_t start = line.find_first_not_of("\t ", charEnd + 1);
		size_t end = line.find_first_of(" \t", start + 1);
		if (start == line.npos || end < start) {
			ERROR_C_R("Cannot parse name in line: %u\n\t> %s\n", lineCount, line.c_str());
			return true;
		}
		std::string name = line.substr(start, end - start);
		globalSymboles.push_back(name);
		return true;
	}

	using instructionFunc = std::function<bool(const std::string&, size_t, const std::string&, uint32_t, std::vector<std::string>&, Common::CompileData&)>;

	static std::unordered_map<std::string, instructionFunc> inst_map = {
		{"sta", Instruction_STA },
		{"lda", Instruction_LDA },
		{"jsr", Instruction_JSR },
		{"jmp", Instruction_JMP },
		{"pha", Instruction_PHA },
		{"pla", Instruction_PLA },
		{"brk", Instruction_BRK },
		{"rts", Instruction_RTS },

		{"extern", Instruction_EXTERN },
		{"global", Instruction_GLOBAL },
	};

	Common::CompileData CompileASM(const std::string& fileName) {
		printf("Compiling %s\n", fileName.c_str());

		CompileData data;
		data.origFile = fileName;

		uint32_t lineCount = 0;

		std::ifstream fileStream;
		fileStream.open(fileName);

		std::vector<std::string> globalSymboles;


		std::string line;
		while (std::getline(fileStream, line)) {
			lineCount++;
			toLower(line);

			size_t start = line.find_first_not_of("\t ");
			//empty
			if (line.empty() || start == line.npos) {
				continue;
			}

			// comment
			if (line[start] == ';') {
				continue;
			}

			// symbole
			if (line[start] == ':') {
				size_t charStart = line.find_first_not_of("\t ", start + 1);
				size_t charEnd = line.find_first_of(" \t", charStart + 1);
				if (charStart == line.npos || charEnd < charStart) {
					ERROR_C("Cannot parse name in line: %u\n\t> %s\n", lineCount, line.c_str());
				}
				std::string symbole = line.substr(charStart, charEnd - charStart);
				std::string name = SymName(symbole, fileName, globalSymboles);
				uint32_t id = MakeSymboleID(name);
				Common::GetGData().symboles[id] = name;
				data.steps.push_back({ StepType::Symbole, id });
				continue;
			}
			// variable
			if (line[start] == '.') {
				size_t charStart = line.find_first_not_of("\t ", start + 1);
				size_t charEnd = line.find_first_of(" \t", charStart + 1);
				if (charStart == line.npos || charEnd < charStart) {
					ERROR_C("Cannot parse name in line: %u\n\t> %s\n", lineCount, line.c_str());
				}
				std::string typeStr = line.substr(charStart, charEnd - charStart);
				StepType type = StepType::ByteValue;
				if (typeStr == "byte" || typeStr == "db")
					type = StepType::ByteValue;
				else if (typeStr == "word" || typeStr == "dw")
					type = StepType::WordValue;
				else
					ERROR_C("Undefined type \"%s\" in line: %u\n\t> %s\n", typeStr.c_str(), lineCount, line.c_str())

				std::string value;
				switch (GetValue(line, charEnd + 1, value)) {
				case Status::Absolut:
					data.steps.push_back({ type, std::stoul(value, nullptr, 16) });
					continue;
				case Status::Immediate:
					data.steps.push_back({ type, std::stoul(value) });
					continue;
				case Status::Error:
					ERROR_C("Cannot parse value in line: %u\n\t> %s\n", lineCount, line.c_str());
				default:
					ERROR_C("Expected an absolut or immediate value for variable in line: %u\n\t> %s\n", lineCount, line.c_str());
				}
				continue;
			}
			size_t charEnd = line.find_first_of(" \t", start + 1);
			if (start == line.npos || charEnd < start) {
				ERROR_C("Cannot parse instruction in line: %u\n\t> %s\n", lineCount, line.c_str());
			}
			// instruction
			std::string inst = line.substr(start, charEnd - start);
			auto loc = inst_map.find(inst);
			if (loc != inst_map.end()) {
				if ((*loc).second(line, charEnd, fileName, lineCount, globalSymboles, data))
					continue;
			}
			
		}

		printf("Finished compiling %s\n", fileName.c_str());

		return data;
	}
}

