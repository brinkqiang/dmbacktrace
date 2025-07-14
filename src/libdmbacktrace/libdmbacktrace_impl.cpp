#include "libdmbacktrace_impl.h"
#include <iostream>
#include "dmformat.h"
#ifdef _WIN32
#include "stackwalker.h"
#include <mutex>

namespace { // 使用匿名命名空间隐藏实现细节

// 在Windows平台上，用于将堆栈跟踪捕获为字符串的辅助类
class StackWalkerStringHelper : private StackWalker {
public:
    StackWalkerStringHelper() : skipframes_(0) {}

    std::string getStackTrace(int skipframes, HANDLE hThread) {
        // 清理旧数据并设置参数
        str_.clear();
        skipframes_ = skipframes;
        
        // 执行堆栈回溯
        ShowCallstack(hThread);
        return str_;
    }

private:
    // 重载 StackWalker 的虚函数以捕获输出
    virtual void OnOutput(LPCSTR szText) override {
        // 我们不需要默认的输出（如模块加载信息等）
        // StackWalker::OnOutput(szText);
    }

    virtual void OnCallStackOutput(LPCSTR szText) override {
        if (skipframes_ > 0) {
            --skipframes_;
        } else {
            str_ += szText;
        }
    }

    virtual void OnDbgHelpErr(LPCSTR szFuncName, DWORD gle, DWORD64 addr) override {
        // 忽略一些已知的、通常无害的错误
        if ((gle == 487 && strcmp(szFuncName, "SymGetLineFromAddr64") == 0) ||
            (gle == 487 && strcmp(szFuncName, "SymGetSymFromAddr64") == 0)) {
            // 忽略
        } else {
            StackWalker::OnDbgHelpErr(szFuncName, gle, addr);
        }
    }

    std::string str_;
    int skipframes_;
};

// 辅助类的静态实例和用于保证线程安全的互斥锁
StackWalkerStringHelper g_swsi;
std::mutex g_mutex;

} // 匿名命名空间结束

#else // 非 Windows 平台实现
#include <execinfo.h> // for backtrace()
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sstream>
#include <cxxabi.h>
#include <memory>

static std::string demangle_symbol(const char* symbol_line) {
    const char* mangled_start = nullptr;
    const char* offset_start = nullptr;

    for (const char* p = symbol_line; *p; ++p) {
        if (*p == '(') {
            mangled_start = p + 1;
        } else if (*p == '+') {
            offset_start = p;
        }
    }

    if (!mangled_start || !offset_start) {
        return symbol_line;
    }

    std::string mangled_name(mangled_start, offset_start - mangled_start);

    int status = 0;
    std::unique_ptr<char, decltype(&std::free)> demangled_name_ptr(
        abi::__cxa_demangle(mangled_name.c_str(), nullptr, nullptr, &status),
        &std::free
    );

    if (status == 0 && demangled_name_ptr) {
        std::string prefix(symbol_line, mangled_start - symbol_line);
        std::string suffix(offset_start);
        return prefix + demangled_name_ptr.get() + suffix;
    }

    return symbol_line;
}
#endif

// --- DmbacktraceImpl 方法实现 ---

void DmbacktraceImpl::Release(void) {
    delete this;
}



std::string DmbacktraceImpl::GetBackTrace(int skipframes) {
#ifdef _WIN32
    std::unique_lock<std::mutex> lock(g_mutex);
    return g_swsi.getStackTrace(skipframes + 1, GetCurrentThread());
#else
    const int BACKTRACE_SIZE = 100;
    std::string strTrace;

    void* buffer[BACKTRACE_SIZE];
    int nptrs = backtrace(buffer, BACKTRACE_SIZE);

    std::unique_ptr<char*, decltype(&std::free)> strings(
        backtrace_symbols(buffer, nptrs),
        &std::free
    );

    if (strings == nullptr) {
        perror("backtrace_symbols");
        return "Error: backtrace_symbols failed.";
    }
    for (int j = skipframes + 1; j < nptrs; j++)
    {
        std::string demangled_line = demangle_symbol(strings.get()[j]);
        
        std::string strLine = fmt::format("[{0:02}] {1}\n", j - (skipframes + 1), demangled_line);
        strTrace.append(strLine);
    }

    return strTrace;
#endif
}

// --- 工厂函数实现 ---

extern "C" DMEXPORT_DLL Idmbacktrace* DMAPI dmbacktraceGetModule() {
    return new DmbacktraceImpl();
}