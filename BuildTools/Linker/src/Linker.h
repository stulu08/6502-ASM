#pragma once
#include <memory>
#include <Common.h>
#include <6502.h>

namespace Linker {
	std::shared_ptr<E6502::Memory> LinkASM(const std::vector<Common::CompileData>& data);
}