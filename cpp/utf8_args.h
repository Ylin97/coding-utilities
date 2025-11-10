#pragma once

#include <string>
#include <vector>

#if defined(_WIN32)
#include <windows.h>
#include <shellapi.h>
#include <iostream>

namespace utf8_args {

// 将宽字符串转换为 UTF-8 编码的 std::string。注意：返回的字符串不包含 null terminator（已由 std::string 管理）
inline std::string WideToUTF8(const wchar_t* wstr) {
    if (!wstr) return "";
    int size = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, nullptr, 0, nullptr, nullptr);
    if (size <= 0) return "";
    std::string utf8(size - 1, '\0'); // 不包括末尾的'\0'
    WideCharToMultiByte(CP_UTF8, 0, wstr, -1, utf8.data(), size, nullptr, nullptr);
    return utf8;
}

// 获取 UTF-8 编码的命令行参数（替代 char* argv[]）
inline std::vector<std::string> GetUTF8CommandLineArgs() {
    int argc = 0;
    wchar_t** argv_w = CommandLineToArgvW(GetCommandLineW(), &argc);
    std::vector<std::string> utf8_args;

    if (!argv_w) {
        fprintf(stderr, "[utf8_args] Failed to parse command line.\n");
        return utf8_args;
    }

    utf8_args.reserve(argc);
    for (int i = 0; i < argc; ++i) {
        utf8_args.emplace_back(WideToUTF8(argv_w[i]));
    }

    LocalFree(argv_w);
    return utf8_args;
}

} // namespace utf8_args

#else // 非 Windows 平台

namespace utf8_args {
inline std::vector<std::string> GetUTF8CommandLineArgs(int argc, char* argv[]) {
    return std::vector<std::string>(argv, argv + argc);
}
} // namespace utf8_args

#endif

/**********************************************************
 ============== 用法示例 ============== 
 
#include "utf8_args.h"
#include <iostream>
#include <windows.h>

int main(int argc, char* argv[]) {
    // 设置控制台输出为 UTF-8（Windows 10+ 支持）
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    auto args =
    #ifdef _WIN32
        utf8_args::GetUTF8CommandLineArgs();
    #else
        utf8_args::GetUTF8CommandLineArgs(argc, argv);
    #endif

    for (const auto& arg : args) {
        std::cout << arg << "\n";
    }

    return 0;
}

**********************************************************/