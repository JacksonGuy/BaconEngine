cmake_minimum_required(VERSION 3.15)
project(BaconEngine
    VERSION 0.1
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Third Party
add_subdirectory(extern/raylib)
add_subdirectory(extern/box2d)
add_subdirectory(extern/nfd)
add_subdirectory(extern/lua)

# Main project files
set(SOURCE_FILES
    extern/rlimgui/rlImGui.cpp
    extern/imgui/imgui.cpp
    extern/imgui/imgui_draw.cpp
    extern/imgui/imgui_tables.cpp
    extern/imgui/imgui_widgets.cpp
    extern/imgui/

    src/core/game_object.cpp
    src/core/entity.cpp
    src/core/game_manager.cpp
    src/engine/engine.cpp
    src/file/file.cpp

    src/main.cpp
)
add_executable(main ${SOURCE_FILES})

# Options
target_compile_options(main PRIVATE
    $<$<CONFIG:Release>:-O3>
    $<$<CONFIG:Debug>:
        -O0 -g3 -pg -Wall
        -fsanitize=address -fno-omit-frame-pointer
        -fmacro-prefix-map=${CMAKE_SOURCE_DIR}/=
    >
)
target_compile_definitions(main PRIVATE
    $<$<CONFIG:Debug>:DEBUG_BUILD>
)
target_link_options(main PRIVATE
    $<$<CONFIG:Debug>: -fsanitize=address>
)

# Link
target_link_libraries(main
    PRIVATE
        raylib
        box2d
        nfd
        lua_static
)
target_include_directories(main
    PRIVATE
        extern/imgui
        extern/rlimgui
        extern/json
        extern/lua
        extern/sol2/include
        src
        include
)
