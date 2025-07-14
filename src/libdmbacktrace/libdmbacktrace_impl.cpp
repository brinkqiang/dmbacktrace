#include "libdmbacktrace_impl.h"
#include <iostream>
#include "dmformat.h"
#ifdef _WIN32
#include "stackwalker.h"
#include <mutex>

namespace {

class StackWalkerStringHelper : private StackWalker {
public:
    StackWalkerStringHelper() : skipframes_(0) {}

    std::string getStackTrace(int skipframes, HANDLE hThread) {
        str_.clear();
        skipframes_ = skipframes;
        
        ShowCallstack(hThread);
        return str_;
    }

private:
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
        if ((gle == 487 && strcmp(szFuncName, "SymGetLineFromAddr64") == 0) ||
            (gle == 487 && strcmp(szFuncName, "SymGetSymFromAddr64") == 0)) {
        } else {
            StackWalker::OnDbgHelpErr(szFuncName, gle, addr);
        }
    }

    std::string str_;
    int skipframes_;
};

StackWalkerStringHelper g_swsi;
std::mutex g_mutex;

}

#else
#include <execinfo.h> // for backtrace()
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sstream>
#include <cxxabi.h>
#include <memory>

static std::string demangle_symbol(const std::string& symbol_line) {

    size_t mangled_start_pos = symbol_line.find('(');
    size_t offset_start_pos = symbol_line.rfind('+');

    if (mangled_start_pos == std::string::npos || offset_start_pos == std::string::npos || mangled_start_pos >= offset_start_pos) {
        return symbol_line;
    }

    std::string mangled_name = symbol_line.substr(mangled_start_pos + 1, offset_start_pos - (mangled_start_pos + 1));

    int status = 0;
    std::unique_ptr<char, decltype(&std::free)> demangled_name_ptr(
        abi::__cxa_demangle(mangled_name.c_str(), nullptr, nullptr, &status),
        &std::free
    );

    if (status == 0 && demangled_name_ptr) {
        std::string prefix = symbol_line.substr(0, mangled_start_pos + 1);
        std::string suffix = symbol_line.substr(offset_start_pos);
        return prefix + demangled_name_ptr.get() + suffix;
    }

    return symbol_line;
}
#endif

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

extern "C" DMEXPORT_DLL Idmbacktrace* DMAPI dmbacktraceGetModule() {
    return new DmbacktraceImpl();
}