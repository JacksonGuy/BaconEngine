#pragma once

#include <cstdarg>
#include <cstdio>
#include <utility>

namespace bacon {
    inline void _debug_log_impl(
        const char* level,
        const char* file,
        int line,
        const char* text,
        ...
    ) {
        std::fprintf(stderr, "[%s][%s:%d] ", level, file, line);

        va_list args;
        va_start(args, text);
        std::vfprintf(stderr, text, args);
        va_end(args);

        std::fprintf(stderr, "\n");
    }

    #ifdef DEBUG_BUILD
        #define debug_log(text, ...) \
            _debug_log_impl("INFO", __FILE__, __LINE__, text, ##__VA_ARGS__)

        #define debug_warn(text, ...) \
            _debug_log_impl("WARN", __FILE__, __LINE__, text, ##__VA_ARGS__)

        #define debug_error(text, ...) \
            _debug_log_impl("ERROR", __FILE__, __LINE__, text, ##__VA_ARGS__)
    #else
        #define debug_log(...) ((void)0);
        #define debug_warn(...) ((void)0);
        #define debug_error(...) ((void)0);
    #endif
}
