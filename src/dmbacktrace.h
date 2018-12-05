
#ifndef __DMBACKTRACE_H_INCLUDE__
#define __DMBACKTRACE_H_INCLUDE__

#if defined(_WIN32)

#include "StackWalker.h"
#include "TlHelp32.h"

#include <string>
#include <sstream>
#include <mutex>
#include <thread>

class StackWalkerStringHelper : private StackWalker
{
public:
    std::string getStackTrace(int skipframes, HANDLE hThread) // = GetCurrentThread())
    {
        fflush(stdout);
        if (!str_.empty())
            str_ += "\n\n";
        str_.clear();
        skipframes_ = skipframes;
        ShowCallstack(hThread);
        return str_;
    }

    std::string str() { return str_; }

private:
    virtual void OnOutput(LPCSTR szText)
    {
        //fputs(szText, stderr);

        //StackWalker::OnOutput(szText);
    }

    virtual void OnCallStackOutput(LPCSTR szText)
    {
        if (0 < skipframes_)
            --skipframes_;
        else
            str_ += szText;
    }

    virtual void OnDbgHelpErr(LPCSTR szFuncName, DWORD gle, DWORD64 addr)
    {
        if (gle == 487 && 0 == strcmp(szFuncName, "SymGetLineFromAddr64"))
            ; // ignore
        else if (gle == 487 && 0 == strcmp(szFuncName, "SymGetSymFromAddr64"))
            ; // ignore
        else
            StackWalker::OnDbgHelpErr(szFuncName, gle, addr);
    }

    std::string str_;
    int skipframes_;
};


class StackWalkerString {
public:
    static std::string getStackTrace(int skipframes, HANDLE hThread = GetCurrentThread())
    {
        static StackWalkerStringHelper swsi;
        static std::mutex mymutex;

        std::unique_lock<std::mutex> l(mymutex);

        return swsi.getStackTrace(skipframes, hThread);
    }
};


std::string prettyBackTrace(int skipframes)
{
    // http://stackoverflow.com/questions/590160/how-to-log-stack-frames-with-windows-x64
    // http://www.codeproject.com/Articles/11132/Walking-the-callstack
    // http://msdn.microsoft.com/en-us/library/windows/desktop/ms684335%28v=vs.85%29.aspx
    // http://stackoverflow.com/questions/9965784/how-to-obtain-list-of-thread-handles-from-a-win32-process

    std::stringstream str;
    str << StackWalkerString::getStackTrace(skipframes + 1);

    // Get the backtrace of all other threads in this process as well

    DWORD currentProcessId = GetCurrentProcessId();
    DWORD currentThreadId = GetCurrentThreadId();
    HANDLE h = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
    if (h != INVALID_HANDLE_VALUE) {
        THREADENTRY32 te;
        te.dwSize = sizeof(te);
        if (Thread32First(h, &te)) {
            do {
                if (te.dwSize >= FIELD_OFFSET(THREADENTRY32, th32OwnerProcessID) +
                    sizeof(te.th32OwnerProcessID)) {
                    if (currentProcessId == te.th32OwnerProcessID && currentThreadId != te.th32ThreadID)
                    {
                        HANDLE hThread = OpenThread(THREAD_ALL_ACCESS, TRUE, te.th32ThreadID);
                        str << "Thread " << te.th32ThreadID << " is at: " << std::endl << StackWalkerString::getStackTrace(0, hThread);
                        CloseHandle(hThread);
                        fflush(stdout);
                    }
                }
                te.dwSize = sizeof(te);
            } while (Thread32Next(h, &te));
        }
        CloseHandle(h);
    }

    return str.str();
}

#endif

#endif // __DMBACKTRACE_H_INCLUDE__
