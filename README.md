# dmbacktrace

Copyright (c) 2013-2018 brinkqiang (brink.qiang@gmail.com)

[dmbacktrace GitHub](https://github.com/brinkqiang/dmbacktrace)

## Build status
| [Linux][lin-link] | [Mac][mac-link] | [Windows][win-link] |
| :---------------: | :----------------: | :-----------------: |
| ![lin-badge]      | ![mac-badge]       | ![win-badge]        |

[lin-badge]: https://github.com/brinkqiang/dmbacktrace/workflows/linux/badge.svg "linux build status"
[lin-link]:  https://github.com/brinkqiang/dmbacktrace/actions/workflows/linux.yml "linux build status"
[mac-badge]: https://github.com/brinkqiang/dmbacktrace/workflows/mac/badge.svg "mac build status"
[mac-link]:  https://github.com/brinkqiang/dmbacktrace/actions/workflows/mac.yml "mac build status"
[win-badge]: https://github.com/brinkqiang/dmbacktrace/workflows/win/badge.svg "win build status"
[win-link]:  https://github.com/brinkqiang/dmbacktrace/actions/workflows/win.yml "win build status"

## Intro
dmbacktrace
```cpp
#include "dmbacktrace.h"
#include <iostream>
#include <string>
void test2()
{
    dmbacktracePtr module(dmbacktraceGetModule());
    if (module) {
        std::cout << "--- Stack Trace from Main Thread ---" << std::endl;
        std::cout << module->GetBackTrace(0);
        std::cout << "------------------------------------" << std::endl;
    }
}

void test()
{
    test2();
}


int main( int argc, char* argv[] ) {
    test();
    return 0;
}
```

## Stack Traces in Debug vs. Release Builds

:warning: **Please note:** Stack traces generated from **Release** (optimized) builds will be less detailed than those from **Debug** builds. This is expected behavior due to compiler optimizations.

  * **Function Inlining:** In Release builds, simple functions are often inlined into their callers and will **not appear** in the stack trace. This can make the call stack look shorter or incomplete.

  * **For Debugging:** To get the most accurate stack traces, compile in **Debug** mode (e.g., `g++ -g -O0`).

  * **For Release (Linux):** To get meaningful stack traces from a Release build on Linux, you **must** use the following flags:

      * Compile with `-g` to include symbol information.
      * Link with `-rdynamic` to make symbols available at runtime.

    <!-- end list -->

    ```bash
    # Recommended release build command for Linux
    g++ -O2 -g -rdynamic ...
    ```

-----


## 关于 Debug 和 Release 版本中的堆栈差异

:warning: **请注意：** 从 **Release** (优化) 版本中获取的调用堆栈会比 **Debug** 版本的信息更少。这是编译器优化导致的正常现象。

  * **函数内联 (Function Inlining):** 在 Release 模式下，简单的函数会被编译器内联，导致它们**不会出现**在最终的调用堆栈中。这会让调用链看起来比实际的短。

  * **用于调试:** 如需最完整的堆栈信息，请在 **Debug** 模式下编译 (例如 `g++ -g -O0`)。

  * **用于 Release (Linux):** 如果希望在 Release 版本中也能获取有意义的堆栈，**必须**使用以下编译和链接选项：

      * 编译时使用 `-g` 加入符号信息。
      * 链接时使用 `-rdynamic` 使符号在运行时可见。

    <!-- end list -->

    ```bash
    # 推荐的 Linux Release 构建命令
    g++ -O2 -g -rdynamic ...
    ```
## output

win
```
dmbacktrace\src\libdmbacktrace\libdmbacktrace_impl.cpp (116): DmbacktraceImpl::GetBackTrace
dmbacktrace\test\dmbacktrace_easytest\dmbacktrace_easytest.cpp (9): test2
dmbacktrace\test\dmbacktrace_easytest\dmbacktrace_easytest.cpp (16): test
dmbacktrace\test\dmbacktrace_easytest\dmbacktrace_easytest.cpp (22): main
D:\a\_work\1\s\src\vctools\crt\vcstartup\src\startup\exe_common.inl (79): invoke_main
D:\a\_work\1\s\src\vctools\crt\vcstartup\src\startup\exe_common.inl (288): __scrt_common_main_seh
D:\a\_work\1\s\src\vctools\crt\vcstartup\src\startup\exe_common.inl (331): __scrt_common_main
D:\a\_work\1\s\src\vctools\crt\vcstartup\src\startup\exe_main.cpp (17): mainCRTStartup
00007FF80F277374 (KERNEL32): (filename not available): BaseThreadInitThunk
00007FF81113CC91 (ntdll): (filename not available): RtlUserThreadStart
```

linux(centos)
```
--- Stack Trace from Main Thread ---
[00] ./dmbacktrace_easytest(test2()+0x9a) [0x5591b3686a03]
[01] ./dmbacktrace_easytest(test()+0xd) [0x5591b3686ab8]
[02] ./dmbacktrace_easytest(main+0x18) [0x5591b3686ad3]
[03] /lib/x86_64-linux-gnu/libc.so.6(+0x29d90) [0x7f9540342d90]
[04] /lib/x86_64-linux-gnu/libc.so.6(__libc_start_main+0x80) [0x7f9540342e40]
[05] ./dmbacktrace_easytest(_start+0x25) [0x5591b36868a5]
```
## Contacts


## Thanks
