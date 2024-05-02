#include <filesystem>
#include <sstream>
#include <algorithm>

inline std::string CleanPath(const std::string& target) {
	std::string newStr;
	newStr.resize(target.size());
	std::transform(target.begin(), target.end(), newStr.begin(), [](char c) -> char { return c == '\\' ? '/' : c; });
	return newStr;
}

inline std::string CleanPath(std::string& target) {
	std::transform(target.begin(), target.end(), target.begin(), [](char c) -> char { return c == '\\' ? '/' : c; });
	return target;
}

int main(int argc, char** argv) {
#ifdef _WIN32
	const std::filesystem::path compilerPath = "Compiler/Compiler.exe";
	const std::filesystem::path linkerPath = "Linker/Linker.exe";
#endif

	auto directory = std::filesystem::current_path();
	for (int i = 1; i < argc; i++) {

		if (std::string(argv[i]) == "-o" && i < (argc - 1)) {
			i++;
			continue;
		}

		if (std::string(argv[i]) == "-g" && i < (argc - 1)) {
			i++;
			continue;
		}

		std::string possibleDir = argv[1];
		if (std::filesystem::is_directory(possibleDir) && std::filesystem::exists(possibleDir)) {
			directory = possibleDir;
		}
		else {
			printf("Error: %s is not a directory", possibleDir.c_str());
		}
	}
	directory = CleanPath(directory.string());
	printf("Building directory: %s\n", directory.string().c_str());

	
	std::vector<std::string> files;
	for (auto& entry : std::filesystem::recursive_directory_iterator(directory)) {
		if (!entry.is_regular_file())
			continue;
		if (entry.path().extension() != ".asm") {
			continue;
		}
		files.push_back(CleanPath(entry.path().string()));
	}

	printf("\n---- Build starting ----\n");

	if (files.size() <= 0) {
		printf("Warning: No files found, building empty rom\n");
	}

	std::filesystem::path intDir = directory.string() + "/" + directory.stem().string() + "-int";
	std::filesystem::path gFile = intDir.string() + "/globalData.comp";
	std::filesystem::path outFile = directory.string() + "/" + directory.stem().string() + ".bin";
	{
		std::stringstream compilerCmd;
		compilerCmd << "call " << compilerPath;
		for (const auto& file : files) {
			compilerCmd << " " << std::filesystem::path(file);
		}
		compilerCmd << " -o " << intDir;
		compilerCmd << " -g " << gFile;


		int compileStatus = system(compilerCmd.str().c_str());
		if (compileStatus != 0) {
			printf("Build failed at compiling!");
			return compileStatus;
		}
	}
	
	{
		std::stringstream linkerCmd;
		linkerCmd << "call " << linkerPath;
		for (const auto& file : files) {
			auto path = intDir.string() + "/" + std::filesystem::path(file).stem().string() + ".obj";
			linkerCmd << " " << path;
		}
		linkerCmd << " -o " << outFile;
		linkerCmd << " -g " << gFile;

		int linkStatus = system(linkerCmd.str().c_str());
		if (linkStatus != 0) {
			printf("Build failed at linking!");
			return linkStatus;
		}
	}
	

	printf("\n---- Build finished ----\n");
	return 0;
}