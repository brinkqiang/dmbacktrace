#include "dmbacktrace.h"
#include <iostream>

// 函数 C
void function_c() {
    dmbacktracePtr module(dmbacktraceGetModule());
    if (module) {
        std::cout << "--- Stack Trace from function_c ---" << std::endl;
        // 获取堆栈，跳过0层 (GetBackTrace内部会+1，跳过它自己)
        std::cout << module->GetBackTrace();
        std::cout << "-----------------------------------" << std::endl;
    }
}

// 函数 B
DM_NO_INLINE void function_b() {
    function_c();
}

// 函数 A

DM_NO_INLINE void function_a() {
    function_b();
}


int main(int argc, char* argv[]) {
    // 调用函数链以产生调用堆栈
    function_a();
    return 0;
}