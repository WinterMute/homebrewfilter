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

		file = fopen("sd1:/config/HBF/debug.txt", "a");
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
#ifndef VWII
	if(Options.sdgecko)
		sdprintf(str);
	else if
#else
	if
#endif

	(Options.wifigecko)
		wifi_printf(str);
	else
		gprintf(str);

}
