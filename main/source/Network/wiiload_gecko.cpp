
#include <network.h>
#include <ogcsys.h>
#include <unistd.h>
#include <zlib.h>
#include <fcntl.h>
#include <ogc/lwp_watchdog.h>

#include "libwiigui/gui.h"
#include "main.h"
#include "BootHomebrew/BootHomebrew.h"
#include "Prompts/prompts.h"
#include "xprintf.h"

#define READ_SIZE	(1 << 10)

u8 *gdata = (u8 *)0x92000000;

/*** Extern variables ***/
extern GuiWindow * mainWindow;
extern bool boot_buffer;
extern bool wiiload;

/*** Extern functions ***/
extern void ResumeGui();
extern void HaltGui();

static lwp_t geckothread = LWP_THREAD_NULL;
static bool geckoHalt = true;
char GArgtemp[1024];


int read_gecko_data(int s,     	/* connected socket */
              char *buf, 		/* pointer to the buffer */
              int n,			/* number of characters (bytes) we want */
			  u32 timeout		/* maximum time of no reception */
	      )
{
	int bcount; /* counts bytes read */
	int br;     /* bytes read this pass */
	u32 t;

	t = ticks_to_millisecs(gettime()) + timeout;

	bcount= 0;
	br= 0;
	while (bcount < n)
	{             /* loop until full buffer */
		if ((br= usb_recvbuffer_safe_ex(s,buf,n-bcount,100)) > 0)
		{
			bcount += br;                /* increment byte counter */
			buf += br;                   /* move buffer ptr for next read */

		}
		else if (br < 0)               /* signal an error to the caller */
		{
			xprintf("GeckoRead failure\n");
			return br;
		}
		if (ticks_to_millisecs(gettime()) > t)
			return 0;

	}
	return bcount;
}

void ResumeGeckoThread()
{
	if(geckoHalt)
	{
		geckoHalt = false;
		LWP_ResumeThread(geckothread);
	}
}

void HaltGeckoThread()
{
	if(!geckoHalt)
	{
		geckoHalt = true;

		// wait for thread to finish
		while(!LWP_ThreadIsSuspended(geckothread))
			usleep(100);
	}
}

static void * gecko_l_callback(void *arg)
{
	u32 offset, read, size, channel, uncfilesize;
	u8 *bfr[READ_SIZE];
	u32 tms;
	bool compress = false;

	channel = EXI_CHANNEL_1;

    while(1)
	{
		if(geckoHalt)
		{
            LWP_SuspendThread(geckothread);
			usleep(100);
			continue;
		}
		else
		{
			//xprintf("wiiload_gecko thread running\n");
			int temp = usb_recvbuffer_safe_ex(channel,(char *)&read,4,100);
			if(temp == 4)
			{

				xprintf("4 bytes received from usb buffer\n");
				GuiImage * progressImg = new GuiImage(new GuiImageData(Theme.progress));
				progressImg->SetAlignment(ALIGN_CENTRE, ALIGN_MIDDLE);

				GXColor ImgColor[4];

				ImgColor[0] = (GXColor){Theme.progressbar_color1_1, Theme.progressbar_color1_2, Theme.progressbar_color1_3, 200};		// oben links
				ImgColor[1] = ImgColor[0];		// oben rechts
				ImgColor[2] = (GXColor){Theme.progressbar_color2_1, Theme.progressbar_color2_2, Theme.progressbar_color2_3, 200};		// unten rechts
				ImgColor[3] = ImgColor[2];		// unten links


				GuiImage * progressbarImg = new GuiImage(0, 38, (GXColor *) &ImgColor);
				progressbarImg->SetAlignment(ALIGN_LEFT, ALIGN_MIDDLE);
				progressbarImg->SetPosition((screenwidth - progressImg->GetWidth()) /2 +36, 2);

				GuiText * PercentTxt = new GuiText("0%", 20, (GXColor){Theme.text_1, Theme.text_2, Theme.text_3, 255});
				PercentTxt->SetAlignment(ALIGN_RIGHT, ALIGN_MIDDLE);
				PercentTxt->SetPosition(-((screenwidth - progressImg->GetWidth()) /2 +36), 38);


				float Percent = 0.0f;

				HaltGui();
				mainWindow->SetState(STATE_DISABLED);
				mainWindow->Append(progressImg);
				mainWindow->Append(progressbarImg);
				mainWindow->Append(PercentTxt);
				ResumeGui();

				if(read == 1212242008) // 1212242008 -> 48415858 -> HAXX -> wiiload
				{
					compress = true;
		//			printf("HAXX\n");

					xprintf("reading version\n");

					read_gecko_data(channel, (char *)&read, 4,1000);
		//			int WIILOAD_VERSION_MAYOR	= (u8)(((u16)(read >> 16)) >> 8);
		//			int WIILOAD_VERSION_MINOR	= (u8)(((u16)(read >> 16)) & 0xFF);
		//			int a						= (u8)(((u16)(read & 0xFFFF)) >> 8);
		//			int b						= (u8)(((u16)(read & 0xFFFF)) & 0xFF);

		//			printf("wiiload v%x.%x\n", WIILOAD_VERSION_MAYOR, WIILOAD_VERSION_MINOR);
		//			printf("args %x08\n", read);
		//			printf("args a=%x  b=%x\n", a, b);

					xprintf("reading size\n");

					read_gecko_data(channel, (char *)&size, 4, 1000);

					xprintf("reading uncompressed size\n");

					read_gecko_data(channel, (char *)&uncfilesize, 4, 1000);
				}
				else
					size = read;

				offset = 0;
				while(offset < size && (read = read_gecko_data(channel, (char *)bfr, (size - offset) > READ_SIZE ? READ_SIZE : (size - offset), 2000)) > 0)
				{
					xprintf("finished reading block at offset %x\n",offset);
					memcpy(gdata + offset, bfr, READ_SIZE);
					offset += read;

					Percent = 100.0f * offset/size;
					progressbarImg->SetSize(Percent*3.27f, 38);

					char buffer[6];
					sprintf(buffer, "%i %%", (int)Percent);
					PercentTxt->SetText(buffer);
				}
				if (offset >= size)
				{
//again, free gift from postloader

					// These are the arguments....
					tms = ticks_to_millisecs(gettime());
					xprintf("timer1 = %d\n",tms);
					int ret = read_gecko_data(channel, (char *) (GArgtemp), 1023, 1000);
					tms = ticks_to_millisecs(gettime());
					xprintf("timer2 = %d\n",tms);
					GArgtemp[ret] = 0;
					//xprintf("all arguments = %s\n",GArgtemp);
					if (ret > 2 && GArgtemp[ret - 1] == '\0' && GArgtemp[ret - 2] == '\0') // Check if it is really an arg
					{
						CopyArgs((u8*)(GArgtemp),ret);
					}
					usleep(100000);
					if(compress)
					{
						u8 *zdata = (u8 *) malloc(uncfilesize);
						if(!zdata)
							return NULL;

						uLongf zdatalen = uncfilesize;

						int res = uncompress (zdata, &zdatalen, gdata, (uLongf)size);

						if (res != Z_OK)
						{
							free(zdata);
			//				fprintf (stderr, "error decompressing data: %d\n", res);
							return NULL;
						}
						else
						{
							gdata = zdata;
							size = zdatalen;
						//	free(zdata);
						}
					}

					CopyHomebrewMemory(gdata, 0, size);
					if(gdata)
						free(gdata);
		//			if(zdata)
		//				free(zdata);

					mainWindow->Remove(PercentTxt);
					mainWindow->Remove(progressbarImg);
					mainWindow->Remove(progressImg);
					mainWindow->SetState(STATE_DEFAULT);
					ResumeGui();

					boot_buffer = true;
					wiiload = true;
					break;
				}
				else
				{
					//receive timeout
					//we shouldn't hang, but resume
					//hanging is unprofessional
					mainWindow->Remove(PercentTxt);
					mainWindow->Remove(progressbarImg);
					mainWindow->Remove(progressImg);
					mainWindow->SetState(STATE_DEFAULT);
					ResumeGui();

				}
			}
			else
			{
				usleep(250 * 1000);
				//xprintf(".");
			}
		}
	}
	return NULL;
}

void InitGeckoThread()
{
	LWP_CreateThread (&geckothread, gecko_l_callback, NULL, NULL, 32768, 60);
}

/****************************************************************************
 * ShutdownThread
 ***************************************************************************/
void ShutdownGeckoThread()
{
    if(geckothread != LWP_THREAD_NULL)
    {
        ResumeGeckoThread();
        LWP_JoinThread (geckothread, NULL);
        geckothread = LWP_THREAD_NULL;
    }
}
