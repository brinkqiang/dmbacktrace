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

win
```
dmbacktrace\src\libdmbacktrace\libdmbacktrace_impl.cpp (116): DmbacktraceImpl::GetBackTrace
dmbacktrace\test\dmbacktrace_easytest\dmbacktrace_easytest.cpp (9): test2
dmbacktrace\test\dmbacktrace_easytest\dmbacktrace_easytest.cpp (22): main
D:\a\_work\1\s\src\vctools\crt\vcstartup\src\startup\exe_common.inl (288): __scrt_common_main_seh
00007FF80F277374 (KERNEL32): (filename not available): BaseThreadInitThunk
00007FF81113CC91 (ntdll): (filename not available): RtlUserThreadStart
```

linux(centos)
```
--- Stack Trace from Main Thread ---
[00] ./dmbacktrace_easytest(test2()+0x94) [0x5607579eb924]
[01] ./dmbacktrace_easytest(main+0xd) [0x5607579eb72d]
[02] /lib/x86_64-linux-gnu/libc.so.6(+0x29d90) [0x7f385a402d90]
[03] /lib/x86_64-linux-gnu/libc.so.6(__libc_start_main+0x80) [0x7f385a402e40]
[04] ./dmbacktrace_easytest(_start+0x25) [0x5607579eb7c5]
```
## Contacts


## Thanks
