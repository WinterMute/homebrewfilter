
#include <gccore.h>

int main(int argc, char **argv)
{
	VIDEO_Init();

	WII_LaunchTitle(0x0001000154484246);

	return 0;
}