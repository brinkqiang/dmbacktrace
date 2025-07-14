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


// --- 场景二代码 (修改) ---

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
 * @brief 工作线程的入口函数，增加了同步机制
 * @param ready_promise 一个承诺，用于通知外界本线程已准备就绪
 */
void thread_worker_function(std::promise<void> ready_promise) {
    // 1. 通过 set_value() “兑现承诺”，发出“我已就绪”的信号
    //    这表明线程已经成功启动，并且 C++ 运行时环境已建立。
    ready_promise.set_value();

    std::cout << "[Worker Thread] Thread signaled ready, beginning calculation." << std::endl;
    ComplexCalculator calculator;
    calculator.calculate();
}

// --- main 函数 (修改) ---
int main(int argc, char* argv[]) {
    std::cout << "======= Main function started. =======\n" << std::endl;
    
    // --- 执行场景一 ---
    std::cout << "Executing Scenario 1: Simple chain in main thread..." << std::endl;
    function_a();

    std::cout << "\n========================================\n" << std::endl;

    // --- 执行场景二 ---
    std::cout << "Executing Scenario 2: Complex chain in a new worker thread..." << std::endl;
    
    // 2. 创建 promise 对象
    std::promise<void> ready_promise;
    // 3. 从 promise 获取 future 对象
    std::future<void> ready_future = ready_promise.get_future();

    // 4. 启动新线程，并将 promise 的所有权转移(std::move)给它
    std::thread worker_thread(thread_worker_function, std::move(ready_promise));

    // 5. 主线程等待，直到收到工作线程“就绪”的信号
    //    这个 get() 会阻塞，直到工作线程调用 ready_promise.set_value()
    ready_future.get();
    std::cout << "[Main Thread] Received ready signal from worker. Waiting for it to finish." << std::endl;

    // 6. 等待工作线程执行完毕
    worker_thread.join();
    
    std::cout << "\n======= All scenarios completed. =======\n" << std::endl;

    return 0;
}