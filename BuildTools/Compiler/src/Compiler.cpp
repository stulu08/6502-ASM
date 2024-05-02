#include "Compiler.h"

#include <sstream>
#include <fstream>

using namespace E6502;
using namespace Common;

namespace Compiler {

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
				if (typeStr == "byte")
					type = StepType::ByteValue;
				else if (typeStr == "word")
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
			std::string inst = line.substr(start, charEnd - start);

			// global/extern
			if (inst == "global") {
				size_t start = line.find_first_not_of("\t ", charEnd + 1);
				size_t end = line.find_first_of(" \t", start + 1);
				if (start == line.npos || end < start) {
					ERROR_C("Cannot parse name in line: %u\n\t> %s\n", lineCount, line.c_str());
				}
				std::string name = line.substr(start, end - start);
				globalSymboles.push_back(name);
				continue;
			}
			if (inst == "extern") {
				size_t start = line.find_first_not_of("\t ", charEnd + 1);
				size_t end = line.find_first_of(" \t", start + 1);
				if (start == line.npos || end < start) {
					ERROR_C("Cannot parse name in line: %u\n\t> %s\n", lineCount, line.c_str());
				}
				std::string name = line.substr(start, end - start);
				globalSymboles.push_back(name);
				continue;
			}

			// instruction
			if (inst == "brk") {
				data.steps.push_back({ StepType::OP, (uint32_t)Instruction::BRK });
				continue;
			}
			if (inst == "rts") {
				data.steps.push_back({ StepType::OP, (uint32_t)Instruction::RTS });
				continue;
			}
			if (inst == "jsr") {
				std::string value;
				switch (GetValue(line, charEnd + 1, value)) {
				case Status::Absolut:
					data.steps.push_back({ StepType::OP, (uint32_t)Instruction::JSR });
					data.steps.push_back({ StepType::WordValue, std::stoul(value, nullptr, 16) });
					continue;
				case Status::Label:
					data.steps.push_back({ StepType::OP, (uint32_t)Instruction::JSR });
					data.steps.push_back({ StepType::SymboleValue, CompilerSymID(value, fileName, globalSymboles) });
					continue;
				case Status::Error:
					ERROR_C("Cannot parse value in line: %u\n\t> %s\n", lineCount, line.c_str());
				default:
					ERROR_C("Expected an absolut value for JSR instruction in line: %u\n\t> %s\n", lineCount, line.c_str());
				}
			}
			if (inst == "int") {
				std::string value;
				switch (GetValue(line, charEnd + 1, value)) {
				case Status::Immediate:
					data.steps.push_back({ StepType::OP, (uint32_t)Instruction::INT_IM });
					data.steps.push_back({ StepType::ByteValue, std::stoul(value) });
					continue;
				case Status::Error:
					ERROR_C("Cannot parse value in line: %u\n\t> %s\n", lineCount, line.c_str());
				default:
					ERROR_C("Expected an immediate value for int instruction in line: %u\n\t> %s\n", lineCount, line.c_str());
				}
			}
			if (inst == "jmp") {
				std::string value;
				switch (GetValue(line, charEnd + 1, value)) {
				case Status::Absolut:
					data.steps.push_back({ StepType::OP, (uint32_t)Instruction::JMP_ABS });
					data.steps.push_back({ StepType::WordValue, std::stoul(value, nullptr, 16) });
					continue;
				case Status::Label:
					data.steps.push_back({ StepType::OP, (uint32_t)Instruction::JMP_ABS });
					data.steps.push_back({ StepType::SymboleValue, CompilerSymID(value, fileName, globalSymboles) });
					continue;
				case Status::Error:
					ERROR_C("Cannot parse value in line: %u\n\t> %s\n", lineCount, line.c_str());
				default:
					ERROR_C("Expected an absolut or indirect value for JMP instruction in line: %u\n\t> %s\n", lineCount, line.c_str());
				}
				continue;
			}
			if (inst == "lda") {
				std::string value;
				switch (GetValue(line, charEnd + 1, value)) {
				case Status::Absolut:
					data.steps.push_back({ StepType::OP, (uint32_t)Instruction::LDA_ABS });
					data.steps.push_back({ StepType::WordValue, std::stoul(value, nullptr, 16) });
					continue;
					break;
				case Status::Immediate:
					data.steps.push_back({ StepType::OP, (uint32_t)Instruction::LDA_IM });
					data.steps.push_back({ StepType::ByteValue, std::stoul(value) });
					continue;
					break;
				case Status::ZeroPage:
					data.steps.push_back({ StepType::OP, (uint32_t)Instruction::LDA_ZP });
					data.steps.push_back({ StepType::ByteValue, std::stoul(value, nullptr, 16) });
					continue;
					break;
				case Status::ZeroPageX:
					data.steps.push_back({ StepType::OP, (uint32_t)Instruction::LDA_ZPX });
					data.steps.push_back({ StepType::ByteValue, std::stoul(value, nullptr, 16) });
					continue;
					break;
				case Status::Error:
					ERROR_C("Cannot parse value in line: %u\n\t> %s\n", lineCount, line.c_str());
				default:
					ERROR_C("Unexpected value for lda instruction in line: %u\n\t> %s\n", lineCount, line.c_str());
				}
			}
			if (inst == "sta") {
				std::string value;
				switch (GetValue(line, charEnd + 1, value)) {
				case Status::Absolut:
					data.steps.push_back({ StepType::OP, (uint32_t)Instruction::STA_ABS });
					data.steps.push_back({ StepType::WordValue, std::stoul(value, nullptr, 16) });
					continue;
				case Status::ZeroPage:
					data.steps.push_back({ StepType::OP, (uint32_t)Instruction::STA_ABS });
					data.steps.push_back({ StepType::ByteValue, std::stoul(value, nullptr, 16) });
					continue;
				case Status::Error:
					ERROR_C("Cannot parse value in line: %u\n\t> %s\n", lineCount, line.c_str());
				default:
					ERROR_C("Unexpected value for sta instruction in line: %u\n\t> %s\n", lineCount, line.c_str());
				}
			}
		}

		printf("Finished compiling %s\n", fileName.c_str());

		return data;
	}
}

