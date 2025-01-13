# dmbacktrace

Copyright (c) 2013-2018 brinkqiang (brink.qiang@gmail.com)

[dmbacktrace GitHub](https://github.com/brinkqiang/dmbacktrace)

## Build status
| [Linux][lin-link] | [MacOSX][osx-link] | [Windows][win-link] |
| :---------------: | :----------------: | :-----------------: |
| ![lin-badge]      | ![osx-badge]       | ![win-badge]        |

[lin-badge]: https://travis-ci.org/brinkqiang/dmbacktrace.svg?branch=master "Travis build status"
[lin-link]:  https://travis-ci.org/brinkqiang/dmbacktrace "Travis build status"
[osx-badge]: https://travis-ci.org/brinkqiang/dmbacktrace.svg?branch=master "Travis build status"
[osx-link]:  https://travis-ci.org/brinkqiang/dmbacktrace "Travis build status"
[win-badge]: https://ci.appveyor.com/api/projects/status/github/brinkqiang/dmbacktrace?branch=master&svg=true "AppVeyor build status"
[win-link]:  https://ci.appveyor.com/project/brinkqiang/dmbacktrace "AppVeyor build status"

## Intro
dmbacktrace
```cpp
#include "dmbacktrace.h"
#include <iostream>
#include <string>
void test2()
{
    std::string strTrace = DMGetBackTrace();
    std::cout << strTrace << std::endl;
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
e:\git\dmbacktrace\bin\Debug>dmbacktracetest.exe
e:\git\dmbacktrace\src\dmbacktrace.cpp (58): StackWalkerStringHelper::getStackTrace
e:\git\dmbacktrace\src\dmbacktrace.cpp (47): StackWalkerString::getStackTrace
e:\git\dmbacktrace\src\dmbacktrace.cpp (7): DMGetBackTrace
e:\git\dmbacktrace\test\dmbacktracetest.cpp (7): test2
e:\git\dmbacktrace\test\dmbacktracetest.cpp (14): test
e:\git\dmbacktrace\test\dmbacktracetest.cpp (19): main
d:\agent\_work\1\s\src\vctools\crt\vcstartup\src\startup\exe_common.inl (79): invoke_main
d:\agent\_work\1\s\src\vctools\crt\vcstartup\src\startup\exe_common.inl (288): __scrt_common_main_seh
d:\agent\_work\1\s\src\vctools\crt\vcstartup\src\startup\exe_common.inl (331): __scrt_common_main
d:\agent\_work\1\s\src\vctools\crt\vcstartup\src\startup\exe_main.cpp (17): mainCRTStartup
00000000773659ED (kernel32): (filename not available): BaseThreadInitThunk
000000007759C541 (ntdll): (filename not available): RtlUserThreadStart
```

linux(centos)
```
[00] ./dmbacktracetest(_Z14DMGetBackTracei+0x38) [0x411b68]
[01] ./dmbacktracetest(_Z5test2v+0x12) [0x4119f2]
[02] ./dmbacktracetest(main+0x9) [0x411879]
[03] /lib64/libc.so.6(__libc_start_main+0xf5) [0x7fab965a6495]
[04] ./dmbacktracetest() [0x411925]
```
## Contacts


## Thanks
