#include "main.h"
#include "gecko.h"
#include "Network/wifi_gecko.h"

extern "C" void sdprintf(const char * format, ...)
{
	char * tmp = NULL;
	va_list va;
	va_start(va, format);
	if((vasprintf(&tmp, format, va) >= 0) && tmp)
	{
		FILE * file;

		file = fopen((Settings.device_dat + ":/config/HBF/debug.txt").c_str(), "a");
		if(file)
		{
			fprintf(file, "%s", tmp);
			fclose(file);
		}
	}
	va_end(va);

	if(tmp)
		free(tmp);
}

extern "C" void xprintf(const char *str, ...)
{
	if(Options.sdgecko)
		sdprintf(str);
	else if(Options.wifigecko)
		wifi_printf(str);
#ifndef VWII
	else
		gprintf(str);
#endif
}
