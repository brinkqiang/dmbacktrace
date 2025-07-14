#ifndef __DMBACKTRACE_H_INCLUDE__
#define __DMBACKTRACE_H_INCLUDE__

#include "dmos.h"
#include "dmmoduleptr.h"
#include <string>
#include <vector>

// 接口类前置声明
class Idmbacktrace;

// 为接口定义智能指针
typedef DmModulePtr<Idmbacktrace> dmbacktracePtr;

/**
 * @brief backtrace接口类
 */
class Idmbacktrace {
public:
    virtual ~Idmbacktrace() {}

    /**
     * @brief 释放模块实例
     * @details 调用此方法来销毁由 dmbacktraceGetModule 创建的对象
     */
    virtual void DMAPI Release(void) = 0;

    /**
     * @brief 获取当前线程的调用堆栈
     * @param skipframes 要从堆栈顶部跳过的帧数
     * @return 返回一个包含堆栈信息的字符串
     */
    virtual std::string DMAPI GetBackTrace(int skipframes = 0) = 0;
};

/**
 * @brief C-Style的工厂函数，用于创建模块实例
 * @return Idmbacktrace 接口指针
 */
extern "C" DMEXPORT_DLL Idmbacktrace* DMAPI dmbacktraceGetModule();
typedef Idmbacktrace* (DMAPI* PFN_dmbacktraceGetModule)();

#endif // __DMBACKTRACE_H_INCLUDE__