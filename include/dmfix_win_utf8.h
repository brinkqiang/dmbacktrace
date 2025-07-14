
// Copyright (c) 2018 brinkqiang (brink.qiang@gmail.com)
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

// cmake env
// add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")

#ifndef __DMFIX_WIN_UTF8_H_INCLUDE__
#define __DMFIX_WIN_UTF8_H_INCLUDE__

#include <cstdint>
#include <iostream>
#ifdef _WIN32
#include <windows.h>
#include <fcntl.h>
#include <io.h>
#else
#include <clocale>
#endif

class ConsoleEncoding {
public:
    // 禁止实例化
    ConsoleEncoding() = delete;
    ~ConsoleEncoding() = delete;
    ConsoleEncoding(const ConsoleEncoding&) = delete;
    ConsoleEncoding& operator=(const ConsoleEncoding&) = delete;

private:
    // 静态成员变量，用于初始化代码页
    static inline uint32_t codePage = 65001; // 默认 UTF-8

    // 静态对象的构造函数，用于设置控制台代码页
    struct Initializer {
        Initializer() {
#ifdef _WIN32
        SetConsoleOutputCP(codePage);
        SetConsoleCP(codePage);
#else
        // 设置 Linux/macOS 区域设置
        std::setlocale(LC_ALL, "en_US.utf8");
#endif
        }
    };

    // 静态成员变量：自动执行的初始化器
    static inline Initializer initializer;
};

#endif // __DMFIX_WIN_UTF8_H_INCLUDE__