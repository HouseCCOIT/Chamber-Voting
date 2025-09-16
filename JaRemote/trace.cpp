#include <windows.h>
#include <stdio.h>

#define DEBUG_BUFFER_SIZE 1024

VOID __cdecl
TraceEvent(__in const char *file,
	       __in const char *function,
           __in const int  line,
           //__in_z __drv_formatString(printf) PCSTR debugMessageFormat,
           char *debugMessageFormat,
           ...
           )
{
   va_list  list;
   CHAR     debugMessageBuffer[DEBUG_BUFFER_SIZE];
   CHAR     debugMsg[DEBUG_BUFFER_SIZE];

   va_start(list, debugMessageFormat);

   if (debugMessageFormat) {
      vsprintf_s(debugMessageBuffer, debugMessageFormat, list);
      //sprintf(debugMsg, "%s: %s(%d) - %s\n", file, function, line, debugMessageBuffer);
      sprintf_s(debugMsg, "%s(%d) - %s\n", function, line, debugMessageBuffer);
	  OutputDebugStringA(debugMsg);
   }

   va_end(list);

   return;
}