#pragma once

#include "GameObject.h"
#include "util.h"

namespace Editor {
    const int BUFFSIZE = 512;
    const int TEXTBUFFSIZE = 16384; // 1024 * 16

    extern GameObject* viewPropertiesObject;

    extern char addVariableName[BUFFSIZE];
    extern i32 addVariableType;
    extern f64 addVariableNumberVal;
    extern char addVariableStringVal[BUFFSIZE];
    extern i32 addVariableBoolVal;
}