#if !defined(TRACE_H)
#define TRACE_H

VOID
//__cdecl
TraceEvent (__in const char *file,
	         __in const char *function,
            __in int  line,
            //__in_z __drv_formatString(printf) PCSTR logMessageFormat,
            char *debugMessageFormat,
            ...
            );
#undef TRACE
#define TRACE(...) TraceEvent( __FILE__, __FUNCTION__, __LINE__, __VA_ARGS__);
#define TRACE_LOG(...) TraceEvent(__FILE__, __FUNCTION__, __LINE__, __VA_ARGS__); 

#endif