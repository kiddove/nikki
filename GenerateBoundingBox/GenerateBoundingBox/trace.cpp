#include "stdafx.h"
#include "trace.h"
#ifdef _DEBUG
bool _trace(TCHAR *format, ...)
{
   TCHAR buffer[1000];

   va_list argptr;
   va_start(argptr, format);
   vsprintf_s(buffer, 1000, format, argptr);
   va_end(argptr);

   OutputDebugString(buffer);

   return true;
}
#endif