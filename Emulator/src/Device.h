#pragma once
#include "Memory.h"
#include "6502.h"

#include <thread>

#include <windows.h>

namespace E6502 {
	class Device {
	public:
		void SetAddress(DWord address) {
			this->address = address;
		}
		DWord GetAddress() const {
			return address;
		}

		virtual void Connect() {
			
		}
		virtual void Disconnect() {
			
		}
		virtual void OnMemoryWrite(DWord location) {

		}
		virtual void OnInterrupt(Byte num) {

		}

		virtual Byte GetSize() const = 0;
		virtual bool AlreadyDedicatedAddress() const {
			return true;
		};

		DWord address;
		Memory* memory;
		CPU* cpu;
	};

	class Terminal : public Device {
	public:
		virtual Byte GetSize() const override {
			return 0;
		}
		virtual void Connect() override {

		}
		virtual void Disconnect() override {
		}

		virtual void OnInterrupt(Byte num) {
			Byte data = cpu->A;
			putchar(data);
		}
		virtual void OnMemoryWrite(DWord location) {
			if (location == Memory::IOPeripherals + 1) {
				Byte data = memory->Read(location);
				putchar(data);
			}
		}
	};
}