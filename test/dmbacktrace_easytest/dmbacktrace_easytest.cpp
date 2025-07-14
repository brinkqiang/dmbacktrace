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