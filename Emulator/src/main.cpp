#include <Machine.h>
using namespace E6502;


int main(int argc, char** argv) {
	Machine* machine = new Machine();

	machine->Reset();
	if(argc <= 1) {
		fprintf(stderr, "Error: No Input file given!");
		return -1;
	}
	
	const char* file = argv[1];

	if (FILE* f = fopen(file, "r")) {
		fclose(f);
	}
	else {
		fprintf(stderr, "Error: Could not open file: %s", file);
	}

	machine->Load(file);

	machine->AddDevic<Terminal>();

	machine->Execute();
	
	machine->DumpInfo();

	Byte re = machine->cpu.A;

	delete machine;

	return re;
}