#include "Linker.h"

#include <sstream>
#include <fstream>

using namespace E6502;
namespace Linker {

	std::shared_ptr<Memory> LinkASM(const std::vector<Common::CompileData>& dataList) {
		std::shared_ptr<Memory> memPtr = std::make_shared<Memory>();
		// jump to main first
		Memory& memory = *memPtr.get();
		memory.Initilize();

		DWord counter = Memory::ROM;
		#define WriteWord(_counter, _word) {memory.WriteWord(_counter, _word); _counter+=2;}
		#define WriteByte(_counter, _byte) {memory.Write(_counter, _byte); _counter++;}
		#define WriteInst(_counter, _ins) {memory.Write(_counter, (Byte)_ins); _counter++;}


		std::unordered_map<std::string, DWord> symboles;
		std::vector <std::pair<DWord,std::pair<std::string, std::string>>> writtenSymboles;
		for (const auto& data : dataList) {
			for (auto& [type, value] : data.steps) {
				if (type == Common::StepType::SymboleValue) {
					if (Common::GetGData().symboles.find(value) == Common::GetGData().symboles.end()) {
						ERROR_L("Invalid symbole declared in file %s with hash: %u\n", data.origFile.c_str(), value);
					}
					const std::string& name = Common::GetGData().symboles.at(value);

					writtenSymboles.push_back({ counter, {name, data.origFile} });
					WriteWord(counter, 0);
					continue;
				}
				if (type == Common::StepType::Symbole) {
					if (Common::GetGData().symboles.find(value) == Common::GetGData().symboles.end()) {
						ERROR_L("Invalid symbole declared in file %s with hash id: %u\n", data.origFile.c_str(), value);
					}
					const std::string& name = Common::GetGData().symboles.at(value);
					if (symboles.find(name) != symboles.end()) {
						ERROR_L("%s: Symbole %s already defined\n", data.origFile.c_str(), name.c_str());
					}
					symboles[name] = counter;
					continue;
				}
				if (type == Common::StepType::ByteValue) {
					WriteByte(counter, (Byte)value);
					continue;
				}
				if (type == Common::StepType::WordValue) {
					WriteWord(counter, (DWord)value);
					continue;
				}
				if (type == Common::StepType::OP) {
					WriteByte(counter, (Byte)value);
					continue;
				}
			}
			#undef WriteWord
		}
		// write all symboles
		for (auto& [location, entry] : writtenSymboles) {
			auto name = entry.first;
			auto file = entry.second;
			if (symboles.find(name) == symboles.end()) {
				fprintf(stderr, "Unresolved symbole %s in file %s\n", name.c_str(), file.c_str());
			}
			memory.WriteWord(location, symboles[name]);
		}
		// currently the reset is done in the emulator so we just jump to main
		const std::string& mainName = Common::MakeSymboleName("main", "_global");
		memory.Write(Memory::ResetVector, (Byte)Instruction::JMP_ABS);

		if (symboles.find(mainName) != symboles.end()) {
			DWord location = symboles.at(mainName);
			printf("Gloabal Main function found at: %X\n", (uint32_t)location);
			memory.WriteWord(Memory::ResetVector + 1, location);
		}
		else {
			printf("Main function not found, setting ROM as start: %X\n", (uint32_t)Memory::ROM);
			memory.WriteWord(Memory::ResetVector + 1, Memory::ROM);
		}
		

		return Common::GetGData().errorCount == 0 ? memPtr : nullptr;
	}
}

