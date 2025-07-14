#ifndef __LIBDMBACKTRACE_IMPL_H_INCLUDE__
#define __LIBDMBACKTRACE_IMPL_H_INCLUDE__

#include "dmbacktrace.h"

/**
 * @brief Idmbacktrace 接口的实现类
 */
class DmbacktraceImpl : public Idmbacktrace {
public:
    virtual ~DmbacktraceImpl() {}

    // Idmbacktrace 接口实现
    virtual void DMAPI Release(void) override;
    virtual std::string DMAPI GetBackTrace(int skipframes = 0) override;
};

#endif // __LIBDMBACKTRACE_IMPL_H_INCLUDE__