#pragma once

#include "util.h"

namespace Editor {
    const int BUFFSIZE = 512;

    // External UI Variables
    extern char addVariableName[BUFFSIZE];
    extern i32 addVariableType;
    extern f64 addVariableNumberVal;
    extern char addVariableStringVal[BUFFSIZE];
}