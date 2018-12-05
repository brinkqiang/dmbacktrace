#include "dmbacktrace.h"

#ifdef WIN32

std::string prettyBackTrace(int skipframes)
{
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

std::string StackWalkerString::getStackTrace(int skipframes, HANDLE hThread /*= GetCurrentThread()*/)
{
    static StackWalkerStringHelper swsi;
    static std::mutex mymutex;

    std::unique_lock<std::mutex> l(mymutex);

    return swsi.getStackTrace(skipframes, hThread);
}

std::string StackWalkerStringHelper::getStackTrace(int skipframes, HANDLE hThread) // = GetCurrentThread())
{
    fflush(stdout);
    if (!str_.empty())
        str_ += "\n\n";
    str_.clear();
    skipframes_ = skipframes;
    ShowCallstack(hThread);
    return str_;
}

void StackWalkerStringHelper::OnOutput(LPCSTR szText)
{
    //fputs(szText, stderr);

    //StackWalker::OnOutput(szText);
}

void StackWalkerStringHelper::OnCallStackOutput(LPCSTR szText)
{
    if (0 < skipframes_)
        --skipframes_;
    else
        str_ += szText;
}

void StackWalkerStringHelper::OnDbgHelpErr(LPCSTR szFuncName, DWORD gle, DWORD64 addr)
{
    if (gle == 487 && 0 == strcmp(szFuncName, "SymGetLineFromAddr64"))
        ; // ignore
    else if (gle == 487 && 0 == strcmp(szFuncName, "SymGetSymFromAddr64"))
        ; // ignore
    else
        StackWalker::OnDbgHelpErr(szFuncName, gle, addr);
}
#else
#include <string>
#include "stacktrace.h"
std::string prettyBackTrace(int skipframes)
{
    std::string strTrace;
    struct stacktrace *trace = stacktrace_get(skipframes);
    stacktrace_string(trace, &strTrace);
    stacktrace_free(trace);

    return strTrace;
}
#endif
