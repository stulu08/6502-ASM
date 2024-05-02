#pragma once

#include <stdint.h>

#ifndef E6502_EMULATE_ON_STACK
#define E6502_EMULATE_ON_STACK 0
#endif

// 64kb memory
#ifndef E6502_MEM_SIZE
#define E6502_MEM_SIZE 0xffffu /* 1024 * 64 */
#endif
