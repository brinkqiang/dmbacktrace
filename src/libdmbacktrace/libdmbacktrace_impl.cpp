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
#endif

// --- DmbacktraceImpl 方法实现 ---

void DmbacktraceImpl::Release(void) {
    delete this;
}

std::string DmbacktraceImpl::GetBackTrace(int skipframes) {
#ifdef WIN32
    // 使用锁确保来自不同线程的调用不会交错执行
    std::unique_lock<std::mutex> lock(g_mutex);
    // +1 是为了跳过 GetBackTrace 这层函数调用本身
    return g_swsi.getStackTrace(skipframes + 1, GetCurrentThread());
#else
    std::string strTrace;

    int j, nptrs;
    void* buffer[BACKTRACE_SIZE];
    char** strings;

    nptrs = backtrace(buffer, BACKTRACE_SIZE);

    strings = backtrace_symbols(buffer, nptrs);
    if (strings == NULL) {
        perror("backtrace_symbols");
        exit(EXIT_FAILURE);
    }

    for (j = 0; j < nptrs; j++)
    {
        std::string strLine = fmt::format("[{0:02}] {1}\n", j, strings[j]);
        strTrace.append(strLine);
    }

    free(strings);

    return strTrace;

#endif
}

// --- 工厂函数实现 ---

extern "C" DMEXPORT_DLL Idmbacktrace* DMAPI dmbacktraceGetModule() {
    return new DmbacktraceImpl();
}