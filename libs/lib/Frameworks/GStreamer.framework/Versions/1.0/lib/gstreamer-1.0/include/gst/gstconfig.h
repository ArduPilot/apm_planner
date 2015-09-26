#ifdef __i386__
#include "i386/gstconfig.h"
#elif defined(__ppc__)
#include "ppc/gstconfig.h"
#elif defined(__x86_64__)
#include "x86_64/gstconfig.h"
#elif defined(__arm__)
#include "arm/gstconfig.h"
#elif defined(__arm64__)
#include "arm64/gstconfig.h"
#else
#error "Unsupported Architecture"
#endif
