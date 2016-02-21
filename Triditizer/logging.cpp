
#include <windows.h>
#include <iostream>
#include <fstream>
#include <string>
#include <stdio.h>

#include "logging.h"

va_list va_alist;
std::ofstream ofile;
static bool logOpened = false;

void __cdecl open_log(void)
{
	if (!logOpened)
	{
		logOpened = true;
		static char dir[] = "C:\\StereoScreenshots\\stereoLog.txt"; // hardcoded path
		ofile.open(dir, std::ios::app);
	}
}

void __cdecl add_log(const char *fmt, ...)
{
	char logbuf[50000] = { 0 };

	if (!fmt) { return; }

	va_start(va_alist, fmt);
	_vsnprintf(logbuf + strlen(logbuf), sizeof(logbuf) - strlen(logbuf), fmt, va_alist);
	va_end(va_alist);

	if (ofile)
	{
		ofile << logbuf << std::endl;
	}
}

void __cdecl close_log(void)
{
	if (ofile) { ofile.close(); }
}