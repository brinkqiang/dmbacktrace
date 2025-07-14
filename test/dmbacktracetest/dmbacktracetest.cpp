#include "dmbacktrace.h"
#include <iostream>
#include <thread>
#include <vector>
#include <numeric>
#include <future>
// --- 场景一代码 (不变) ---
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

// --- 场景二代码 (重构) ---
class ComplexCalculator {
    // ... 内部代码不变 ...
public:
    template<typename T>
    DM_NO_INLINE void logAndExecute(T value) {
        std::cout << "\n[Worker Thread] Logging calculated value: " << value << std::endl;
        getTraceHelper();
    }
    DM_NO_INLINE void performStep() {
        std::vector<int> data = {10, 20, 30, 40, 50};
        int sum = std::accumulate(data.begin(), data.end(), 0);
        logAndExecute(sum);
    }
    DM_NO_INLINE void calculate() {
        performStep();
    }
private:
    static void getTraceHelper() {
        dmbacktracePtr module(dmbacktraceGetModule());
        if (module) {
            std::cout << "--- Stack Trace from Worker Thread ---" << std::endl;
            std::cout << module->GetBackTrace(1);
            std::cout << "--------------------------------------" << std::endl;
        }
    }
};

/**
 * @brief 将核心业务逻辑封装成一个独立的函数
 */
void run_complex_scenario() {
    std::cout << "[Worker Thread] Business logic started." << std::endl;
    ComplexCalculator calculator;
    calculator.calculate();
}

/**
 * @brief 新的线程入口点，接收一个不透明的函数指针
 * @param task_func 一个指向无参、无返回值函数的指针
 */
void thread_entry_point(void (*task_func)()) {
    std::cout << "[Worker Thread] Thread entry point reached." << std::endl;
    if (task_func) {
        // 调用这个函数指针。LTO无法优化掉这一步。
        task_func();
    }
}

// --- main 函数 (修改) ---
int main(int argc, char* argv[]) {
    std::cout << "======= Main function started. =======\n" << std::endl;

    // --- 执行场景一 ---
    std::cout << "Executing Scenario 1: Simple chain in main thread..." << std::endl;
    function_a();

    std::cout << "\n========================================\n" << std::endl;

    // --- 执行场景二 ---
    std::cout << "Executing Scenario 2: Using opaque function pointer in a new thread..." << std::endl;
    
    // 创建线程，入口是 thread_entry_point，参数是 run_complex_scenario 的地址
    std::thread worker_thread(thread_entry_point, &run_complex_scenario);

    // 等待工作线程执行完毕
    worker_thread.join();
    
    std::cout << "\n======= All scenarios completed. =======\n" << std::endl;

    return 0;
}