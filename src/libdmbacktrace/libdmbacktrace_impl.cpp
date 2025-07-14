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

static std::string demangle_symbol(const std::string& symbol_line) {
    // 1. 查找左括号 '(' 和加号 '+' 的位置
    // 格式通常是: executable_path(mangled_symbol+offset) [address]
    size_t mangled_start_pos = symbol_line.find('(');
    size_t offset_start_pos = symbol_line.rfind('+'); // 使用 rfind 从右向左找，更准确

    // 2. 如果找不到符合格式的括号和加号，则认为这不是一个可 demangle 的 C++ 符号，返回原始行
    if (mangled_start_pos == std::string::npos || offset_start_pos == std::string::npos || mangled_start_pos >= offset_start_pos) {
        return symbol_line;
    }

    // 3. 提取出 mangled name
    // 从左括号后一位开始，长度为二者位置之差
    std::string mangled_name = symbol_line.substr(mangled_start_pos + 1, offset_start_pos - (mangled_start_pos + 1));

    int status = 0;
    // 4. 使用智能指针来管理 __cxa_demangle 分配的内存，确保自动释放
    std::unique_ptr<char, decltype(&std::free)> demangled_name_ptr(
        abi::__cxa_demangle(mangled_name.c_str(), nullptr, nullptr, &status),
        &std::free
    );

    // 5. 如果 demangle 成功，则拼接成新的可读字符串
    if (status == 0 && demangled_name_ptr) {
        // 截取原始字符串的前缀和后缀
        std::string prefix = symbol_line.substr(0, mangled_start_pos + 1);
        std::string suffix = symbol_line.substr(offset_start_pos);
        // 组合成最终结果
        return prefix + demangled_name_ptr.get() + suffix;
    }

    // demangle 失败或无需 demangle，返回原始行
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
    return g_swsi.getStackTrace(skipframes + 2, GetCurrentThread());
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