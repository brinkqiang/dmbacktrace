#include "dmbacktrace.h"

#ifdef WIN32

std::string DMGetBackTrace(int skipframes)
{
    return StackWalkerString::getStackTrace(skipframes + 1);
    //std::stringstream str;
    //str << StackWalkerString::getStackTrace(skipframes + 1);

    // Get the backtrace of all other threads in this process as well

    //DWORD currentProcessId = GetCurrentProcessId();
    //DWORD currentThreadId = GetCurrentThreadId();
    //HANDLE h = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
    //if (h != INVALID_HANDLE_VALUE) {
    //    THREADENTRY32 te;
    //    te.dwSize = sizeof(te);
    //    if (Thread32First(h, &te)) {
    //        do {
    //            if (te.dwSize >= FIELD_OFFSET(THREADENTRY32, th32OwnerProcessID) +
    //                sizeof(te.th32OwnerProcessID)) {
    //                if (currentProcessId == te.th32OwnerProcessID && currentThreadId != te.th32ThreadID)
    //                {
    //                    HANDLE hThread = OpenThread(THREAD_ALL_ACCESS, TRUE, te.th32ThreadID);
    //                    str << "Thread " << te.th32ThreadID << " is at: " << std::endl << StackWalkerString::getStackTrace(0, hThread);
    //                    CloseHandle(hThread);
    //                    fflush(stdout);
    //                }
    //            }
    //            te.dwSize = sizeof(te);
    //        } while (Thread32Next(h, &te));
    //    }
    //    CloseHandle(h);
    //}

    //return str.str();
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
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>	    /* for signal */
#include <execinfo.h> 	/* for backtrace() */

#include <string>

#include "dmformat.h"

#define BACKTRACE_SIZE 20

std::string DMGetBackTrace(int skipframes)
{
    std::string strTrace;

    int j, nptrs;
    void *buffer[BACKTRACE_SIZE];
    char **strings;

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
}
#endif
