#include "dmbacktrace.h"
#include <iostream>
#include <thread>   // 用于 std::thread
#include <vector>   // 用于演示业务逻辑
#include <numeric>  // 用于 std::accumulate

// =================================================================
// 场景一：在主线程中的简单函数调用链 (与之前相同)
// =================================================================
void function_c() {
    dmbacktracePtr module(dmbacktraceGetModule());
    if (module) {
        std::cout << "--- Stack Trace from Main Thread ---" << std::endl;
        std::cout << module->GetBackTrace(0);
        std::cout << "------------------------------------" << std::endl;
    }
}

DM_NO_INLINE void function_b() { function_c(); }
DM_NO_INLINE void function_a() { function_b(); }


// =================================================================
// 场景二：在工作线程中的复杂类调用链
// =================================================================

class ComplexCalculator {
public:
    // 3. 调用一个模板成员函数
    template<typename T>
    DM_NO_INLINE void logAndExecute(T value) {
        std::cout << "\n[Worker Thread] Logging calculated value: " << value << std::endl;
        
        // 4. 最后调用一个静态辅助函数来获取堆栈信息
        getTraceHelper();
    }

    // 2. 调用一个普通的成员函数
    DM_NO_INLINE void performStep() {
        std::vector<int> data = {10, 20, 30, 40, 50};
        int sum = std::accumulate(data.begin(), data.end(), 0);
        
        logAndExecute(sum);
    }

    // 1. 对象逻辑的入口成员函数
    DM_NO_INLINE void calculate() {
        performStep();
    }

private:
    // 5. 静态成员函数，无需对象实例即可调用
    static void getTraceHelper() {
        dmbacktracePtr module(dmbacktraceGetModule());
        if (module) {
            std::cout << "--- Stack Trace from Worker Thread ---" << std::endl;
            // 跳过1层，隐藏 getTraceHelper 自身
            std::cout << module->GetBackTrace(1); 
            std::cout << "--------------------------------------" << std::endl;
        }
    }
};

/**
 * @brief 这个函数将作为新线程的入口点
 */
void thread_worker_function() {
    std::cout << "[Worker Thread] Thread started, beginning calculation." << std::endl;
    ComplexCalculator calculator;
    
    // 调用链: thread_worker_function() -> calculator.calculate() -> ... -> getTraceHelper()
    calculator.calculate();
}


// =================================================================
// main 函数，负责调度两个场景
// =================================================================
int main(int argc, char* argv[]) {
    std::cout << "======= Main function started. =======\n" << std::endl;
    
    // --- 执行场景一 ---
    std::cout << "Executing Scenario 1: Simple chain in main thread..." << std::endl;
    function_a();

    std::cout << "\n========================================\n" << std::endl;

    // --- 执行场景二 ---
    std::cout << "Executing Scenario 2: Complex chain in a new worker thread..." << std::endl;
    // 创建并启动新线程，入口函数是 thread_worker_function
    std::thread worker_thread(thread_worker_function);

    // 等待工作线程执行完毕
    worker_thread.join();
    
    std::cout << "\n======= All scenarios completed. =======\n" << std::endl;

    return 0;
}