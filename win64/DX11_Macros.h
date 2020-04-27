/* ========================================================================
   $Creator: Patrik Fjellstedt $
   ======================================================================== */

#ifndef _DX11_MACROS_H_
#define _DX11_MACROS_H_

#define DX_RELEASE(a) if(a)\
{\
	a->Release(); \
	a = nullptr; \
}\

   // TODO(pf): Logging system.
#define DX_HR(a, msg) if(FAILED((a))) \
{ \
	MessageBoxW(0, msg, L"Error", MB_OK);\
	return false;\
} \

#endif _DX11_MACROS_H_