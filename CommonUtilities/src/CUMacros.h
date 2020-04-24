/* ========================================================================
   $Creator: Patrik Fjellstedt $
   ======================================================================== */
#ifndef _MACROS_H_
#define _MACROS_H_
#define MAX(x, y) (x > y ? x : y)
#define MIN(x, y) (x < y ? x : y)
#define PI 3.14159265F

#define RAD2DEG 180.F / PI

#define KB(Value) ((Value)*1024LL)
#define MB(Value) (KB(Value)*1024LL)
#define GB(Value) (MB(Value)*1024LL)
#define TB(Value) (GB(Value)*1024LL)
#define ArrayCount(x) (sizeof(x)/sizeof(x[0]))

#ifdef _DEBUG
#define Assert(x) if(!(x)) \
			{ \
				__debugbreak(); \
			}
#define InvalidCodePath Assert(false)
   // NOTE(pf): this is used to force the debugger to stay in the scope so we can inspect the last element........
static int a = 4;
#define DEBUG_LEDGE a > 0 ? a + 2 : a + 4
#else
#define DEBUG_LEDGE
#define InvalidCodePath
#define Assert(x) 
#endif
#endif // !_MACROS_H_