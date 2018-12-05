
#include "dmbacktrace.h"

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
