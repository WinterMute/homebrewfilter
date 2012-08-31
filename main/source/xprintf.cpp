#include "main.h"
#include "gecko.h"
#include "Network/wifi_gecko.h"

void xprintf(const char *str, ...)
{
	if(Options.wifigecko)
		wifi_printf(str);
	else
		gprintf(str);
}
