
#ifndef __DMBACKTRACE_H_INCLUDE__
#define __DMBACKTRACE_H_INCLUDE__

#if defined(_WIN32)

#include "StackWalker.h"
#include "TlHelp32.h"

#include <string>
#include <sstream>
#include <iostream>
#include <mutex>
#include <thread>

class StackWalkerStringHelper : private StackWalker
{
public:
    std::string getStackTrace(int skipframes, HANDLE hThread); // = GetCurrentThread())

    std::string str() { return str_; }

private:
    virtual void OnOutput(LPCSTR szText);

    virtual void OnCallStackOutput(LPCSTR szText);

    virtual void OnDbgHelpErr(LPCSTR szFuncName, DWORD gle, DWORD64 addr);

    std::string str_;
    int skipframes_;
};


class StackWalkerString {
public:
    static std::string getStackTrace(int skipframes, HANDLE hThread = GetCurrentThread());
};


std::string prettyBackTrace(int skipframes);

#endif

#endif // __DMBACKTRACE_H_INCLUDE__
