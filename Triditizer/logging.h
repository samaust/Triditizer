#ifndef _LOGGING_H
#define _LOGGING_H

//#define LOG_TO_FILE // comment to disable logging to file
//#define LOG_PIXELSHADERS // comment to disable logging to file of pixel shaders
//#define LOG_VERTEXSHADERS // comment to disable logging to file of vertex shaders

void __cdecl open_log(void);
void __cdecl add_log(const char * fmt, ...);
void __cdecl close_log(void);

#ifdef LOG_TO_FILE
#define ADD_LOG(fmt, ...) { add_log(fmt, __VA_ARGS__); }
#else
#define ADD_LOG(fmt, ...) {}
#endif

#endif
