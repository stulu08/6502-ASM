#pragma once
#include <memory>
#include <Common.h>
#include <6502.h>

namespace Compiler {
	Common::CompileData CompileASM(const std::string& fileName);
}