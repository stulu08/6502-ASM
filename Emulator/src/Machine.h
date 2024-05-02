#pragma once

#include <vector>
#include "Device.h"
#include "6502.h"

namespace E6502 {
	class Machine {
	public:
		~Machine() {
			for (Device* device : devices) {
				if (device) {
					device->Disconnect();
					delete device;
				}
			}
		}

		CPU cpu;
		Memory memory;


		void Reset() {
			Machine::s_instance = this;

			cpu.Reset(memory);
			cpu.interruptCallback = &OnInterrupt;
			memory.writeCallback = &OnMemoryWritten;
		}

		void Load(const char* fPath) {
			memory.Load(fPath);
		}

		void Execute() {
			while (true) {
				if (cpu.PC == Memory::InterruptVector)
					break;

				cpu.ExecuteOne(memory);

			}
		}

		static inline void OnMemoryWritten(DWord location) {
			for (Device* device : Machine::Get().devices) {
				if (device) {
					device->OnMemoryWrite(location);
				}
			}
		}

		static inline void OnInterrupt(Byte num) {
			for (Device* device : Machine::Get().devices) {
				if (device) {
					device->OnInterrupt(num);
				}
			}
		}

		void DumpInfo() {
			memory.Dump("MemDump.bin");
			cpu.Dump("CpuDump.txt");
		}

		template<class T = Device>
		void AddDevic() {
			T* device = new T();
			devices.push_back(device);
			device->memory = &memory;
			device->cpu = &cpu;
			device->Connect();
		}

		static inline Machine& Get() {
			return *s_instance;
		}
	private:
		static inline Machine* s_instance;

		std::vector<Device*> devices;
	};
}