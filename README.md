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
    std::string strTrace = prettyBackTrace(0);
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
## Contacts
[![Join the chat](https://badges.gitter.im/brinkqiang/dmbacktrace/Lobby.svg)](https://gitter.im/brinkqiang/dmbacktrace)

## Thanks
