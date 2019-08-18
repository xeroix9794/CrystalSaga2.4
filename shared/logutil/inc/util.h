#if !defined(UTIL_H_)
#define UTIL_H_

#include "./util2.h"
#include "./TryUtil.h"
#include "./log.h"
#include "./log2.h"
#include "./playsound.h"
#include "./singleton.h"
#include "./matrix_stuff.h"
#include "./db.h"
#include "./db3.h"
#include "./algo.h"
#include "./fifo.h"
#include "./bill.h"

#include "./PrivilegeCheck.h"
#include "./KopLicense.h"
#include "./Win32Guid.h"

#ifdef LG_PRINTF
#define printf myprintf
#endif

#ifdef LG_FPRINTF
#define fprintf myfprintf
#endif

#endif
