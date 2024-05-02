#include "Compiler.h"
#include <S-Bin/File.h>
#include <atomic>
#include <execution>


int main(int argc, char** argv) {
	std::vector<std::string> inFiles;
	std::filesystem::path outPath;
	std::filesystem::path gDataFile;

	if (argc == 1) {
		printf("usage: %s in.asm in2.asm -o /out/ -g globalData.comp\n", argv[0]);
		return -1;
	}
	
	std::vector<std::string>* argWriter = &inFiles;
	for (int i = 1; i < argc; i++) {
		if (std::string(argv[i]) == "-o" && i < (argc - 1)) {
			outPath = argv[i + 1];
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

	printf("Compiler Configuration: \n");
	printf("\tIn:\n");
	for (int i = 0; i < inFiles.size(); i++) {
		printf("\t\t- %s\n", inFiles[i].c_str());
	}
	printf("\tInt: %s\n", outPath.string().c_str());
	printf("\tGData: %s\n", gDataFile.string().c_str());

	printf("\n---- Compiling ----\n");
	Common::CreateGData();

	std::vector<Common::CompileData> results;
	results.resize(inFiles.size());

	std::for_each(std::execution::par, inFiles.begin(), inFiles.end(), [&results, &inFiles](const std::string& file) {
		uint64_t i = &file - &inFiles[0];
		results[i] = Compiler::CompileASM(file);
	});


	if (Common::GetGData().errorCount != 0) {
		fprintf(stderr, "Compiling failed with %u error(s)", Common::GetGData().errorCount);
		return -2;
	}

	std::filesystem::create_directories(outPath);

	for (auto& result : results) {
		auto file = SBin::File::CreateBinary(outPath.string() + "/" + std::filesystem::path(result.origFile).stem().string() + ".obj");
		auto stream = file->OpenWrite();
		stream << result;
		file->Close();
	}

	auto dataFile = SBin::File::CreateBinary(gDataFile);
	auto stream = dataFile->OpenWrite();
	stream << Common::GetGData();
	dataFile->Close();


	Common::DestroyGData();
	printf("\n---- Done Compiling ----\n");
	return 0;
	
}