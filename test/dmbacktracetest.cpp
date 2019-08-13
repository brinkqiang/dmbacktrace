
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
