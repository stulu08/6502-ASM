#include "Linker.h"

#include <atomic>
#include <execution>
#include "main.h"


int main(int argc, char** argv) {
	std::vector<std::string> inFiles;
	std::string outFile;
	std::string gDataFile;

	if (argc == 1) {
		printf("usage: %s in.obj in2.obj -o out.bin -g globalData.comp\n", argv[0]);
		return -1;
	}

	std::vector<std::string>* argWriter = &inFiles;
	for (int i = 1; i < argc; i++) {

		if (std::string(argv[i]) == "-o" && i < (argc - 1)) {
			outFile = argv[i + 1];
			i++;
			continue;
		}

		if (std::string(argv[i]) == "-g" && i < (argc - 1)) {
			gDataFile = argv[i + 1];
			i++;
			continue;
		}

		argWriter->push_back(std::string(argv[i]));
	}

	printf("Linker Configuration: \n");
	printf("\tIn:\n");
	for (int i = 0; i < inFiles.size(); i++) {
		printf("\t\t- %s\n", inFiles[i].c_str());
	}
	printf("\tOut: %s\n", outFile.c_str());
	printf("\tGData: %s\n", gDataFile.c_str());

	return Linker::MainLink(inFiles, outFile, gDataFile);
}

namespace Linker {
	int Linker::MainLink(const std::vector<std::string>& inFiles, const std::string& outFile, const std::string& gData) {
		printf("\n---- Linking ----\n");
		Common::CreateGData();

		auto dataFile = SBin::File::CreateBinary(gData);
		auto stream = dataFile->OpenRead();
		stream >> Common::GetGData();
		dataFile->Close();

		std::vector<Common::CompileData> data;

		for (auto& fileName : inFiles) {
			auto file = SBin::File::CreateBinary(fileName);
			auto stream = file->OpenRead();

			Common::CompileData cData;
			stream >> cData;
			file->Close();

			data.push_back(cData);

		}


		auto result = Linker::LinkASM(data);
		if (!result) {
			fprintf(stderr, "Linking failed with %u error(s)", Common::GetGData().errorCount);
			return -2;
		}
		Common::DestroyGData();
		result->Dump(outFile.c_str());

		printf("\n---- Done Linking ----\n");
		printf("\t-> %s\n", outFile.c_str());

		return 0;
	}
}