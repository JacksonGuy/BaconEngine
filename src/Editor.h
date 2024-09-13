#pragma once

#include "util.h"

namespace Editor {
    const int BUFFSIZE = 512;
    const int TEXTBUFFSIZE = 16384; // 1024 * 16

    // External UI Variables
    extern char addVariableName[BUFFSIZE];
    extern i32 addVariableType;
    extern f64 addVariableNumberVal;
    extern char addVariableStringVal[BUFFSIZE];
}