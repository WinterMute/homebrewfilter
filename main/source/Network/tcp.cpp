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
#include "gecko.h"

#define READ_SIZE (1 << 10)

u8 *data = (u8 *)0x92000000;

/*** Extern variables ***/
extern GuiWindow * mainWindow;
extern bool boot_buffer;
extern bool wiiload;

/*** Extern functions ***/
extern void ResumeGui();
extern void HaltGui();

static lwp_t tcpthread = LWP_THREAD_NULL;
static bool tcpHalt = true;
char Argtemp[1024];

//read with timeout. Comes from postloader
static int NetRead(int connection, u8 *buf, u32 len, u32 tout) // timeout in msec
{
	u32 read = 0;
	s32 ret = 0;
	u32 t;

	t = ticks_to_millisecs(gettime()) + tout;

	while (read < len)
	{
		ret = net_read(connection, buf + read, len - read);

		if (ret <= 0)
			usleep (10 * 1000);
		else
			read += ret;

		if (ticks_to_millisecs(gettime()) > t)
			break;
	}
	return read;
}

// code to oport a socket; originally from bzs@bu-cs.bu.edu
int oport(unsigned short portnum)
{
	int    s;
	struct sockaddr_in sa;
	int err;

	net_init();
	memset(&sa, 0, sizeof(struct sockaddr_in)); /* clear our address */
	sa.sin_family= AF_INET;              /* this is our host address */
	sa.sin_port= htons(portnum);                /* this is our port number */
	sa.sin_addr.s_addr = net_gethostip();
	sa.sin_len = 8;
	if ((s= net_socket(AF_INET, SOCK_STREAM, 0)) < 0) /* create socket */
	{
		gprintf("net_socket failed\n");
 		return(-1);
	}

	int one = 1;
	err = net_setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (const char *)&one, sizeof(one));
	if (err < 0)
	{
		gprintf("net_setsockopt SO_REUSEADDR error");
		return (-1);
	}

	//this makes the read timeout if no data available
/*
	int flags = net_fcntl(s, F_GETFL, 0);
	flags = net_fcntl(s, F_SETFL, flags|4); // Set non blocking
*/

	if (net_bind(s,(struct sockaddr *)&sa,8) < 0)
	{
		net_close(s);
		return(-1);                               /* bind address to socket */
	}

	net_listen(s, 3);                               /* max # of queued connects */

/*
	u32 buflen = 8;
	int t = net_accept(s,(struct sockaddr *)&sa, &buflen);
	gprintf("Dummy net_accept returned %d\n",t);
*/

	return(s);
}

// wait for a connection to occur on a socket created with oport()
int get_connection(int s, struct sockaddr_in *sa)
{
	int t;                  /* socket of connection */
	//  struct sockaddr_in sa;
	sa->sin_len = 8;
	sa->sin_family = AF_INET;
	u32 buflen = 8;
	t = net_accept(s,(struct sockaddr *)sa, &buflen);

/*
	debug("Incoming connection");
	printf("Incoming connection from %d.%d.%d.%d\n",
		(sa->sin_addr.s_addr >> 24) & 0xFF,
		(sa->sin_addr.s_addr >> 16) & 0xFF,
		(sa->sin_addr.s_addr >> 8) & 0xFF,
		(sa->sin_addr.s_addr) & 0xFF);
*/
	return t;
}

int read_data(int s,     /* connected socket */
              char *buf, /* pointer to the buffer */
              int n      /* number of characters (bytes) we want */
	      )
{
	int bcount; /* counts bytes read */
	int br;     /* bytes read this pass */

	bcount= 0;
	br= 0;
	while (bcount < n)
	{             /* loop until full buffer */
		if ((br= net_read(s,buf,n-bcount)) > 0)
		{
			bcount += br;                /* increment byte counter */
			buf += br;                   /* move buffer ptr for next read */
		}
		else if (br < 0)               /* signal an error to the caller */
		{
			gprintf("NetRead failure\n");
			return br;
		}
	}
	return bcount;
}

void ResumeTcpThread()
{
	if(tcpHalt)
	{
		tcpHalt = false;
		LWP_ResumeThread(tcpthread);
	}
}

void HaltTcpThread()
{
	if(!tcpHalt)
	{
		tcpHalt = true;

		// wait for thread to finish
		while(!LWP_ThreadIsSuspended(tcpthread))
			usleep(100);
	}
}

static void * tcp_callback(void *arg)
{
	u32 offset, read, size, client, uncfilesize;
	u8 *bfr[READ_SIZE];
	bool compress = false;
	struct sockaddr_in addr;

	redo:
	s32 listen = oport(4299);
	while(1)
	{
		if(tcpHalt)
		{
			LWP_SuspendThread(tcpthread);
			usleep(100);
			continue;
		}
		else
		{
		//	Waiting for connection
			client = get_connection(listen, &addr);

			gprintf("After get_connection\n");
			if(client > 0)
			{
		//		client connected

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

				//wait 2 milliseconds hopefully fixes the error we see
				//usleep (100000);
				gprintf("reading protocol id\n");
				int temp = read_data(client, (char *)&read, 4);
				if(temp < 0)
				{
					//printf("read_data() error while reading protocol id\n");
					net_close(client);
					net_close(listen);
					goto redo;
				}
				else
				{
					HaltGui();
					mainWindow->SetState(STATE_DISABLED);
					mainWindow->Append(progressImg);
					mainWindow->Append(progressbarImg);
					mainWindow->Append(PercentTxt);
					ResumeGui();
				}

				if(read == 1212242008) // 1212242008 -> 48415858 -> HAXX -> wiiload
				{
					compress = true;
					//printf("HAXX\n");

					gprintf("reading version\n");
					read_data(client, (char *)&read, 4);
					/*int WIILOAD_VERSION_MAYOR	= (u8)(((u16)(read >> 16)) >> 8);
					int WIILOAD_VERSION_MINOR	= (u8)(((u16)(read >> 16)) & 0xFF);
					int a						= (u8)(((u16)(read & 0xFFFF)) >> 8);
					int b						= (u8)(((u16)(read & 0xFFFF)) & 0xFF);

					printf("wiiload v%x.%x\n", WIILOAD_VERSION_MAYOR, WIILOAD_VERSION_MINOR);
					printf("args %x08\n", read);
					printf("args a=%x  b=%x\n", a, b);*/

					gprintf("reading size\n");
					read_data(client, (char *)&size, 4);

					gprintf("reading uncompressed size\n");
					read_data(client, (char *)&uncfilesize, 4);
				}
				else
					size = read;

				offset = 0;
				while(offset < size && (read = read_data(client, (char *)bfr, (size - offset) > READ_SIZE ? READ_SIZE : (size - offset))) > 0)
				{
					gprintf("finished reading block at offset %x\n",offset);
					memcpy(data + offset, bfr, READ_SIZE);
					offset += read;

					Percent = 100.0f * offset/size;
					progressbarImg->SetSize(Percent*3.27f, 38);

					char buffer[6];
					sprintf(buffer, "%i %%", (int)Percent);
					PercentTxt->SetText(buffer);
				}

				// These are the arguments....
				int ret = NetRead(client, (u8 *) Argtemp, 1023, 250);
				Argtemp[ret] = 0;
				//gprintf("all arguments = %s\n",Argtemp);
				if (ret > 2 && Argtemp[ret - 1] == '\0' && Argtemp[ret - 2] == '\0') // Check if it is really an arg
				{
					CopyArgs((u8*)(Argtemp),ret);
				}

				usleep(100000);
				net_close(client);
				net_close(listen);

				if(compress)
				{
					u8 *zdata = (u8 *) malloc(uncfilesize);
					if(!zdata)
						return NULL;

					uLongf zdatalen = uncfilesize;

					int res = uncompress (zdata, &zdatalen, data, (uLongf)size);

					if (res != Z_OK)
					{
						free(zdata);
						//fprintf (stderr, "error decompressing data: %d\n", res);
						return NULL;
					}
					else
					{
						data = zdata;
						size = zdatalen;
					//	free(zdata);
					}
				}

				CopyHomebrewMemory(data, 0, size);
				if(data)
					free(data);
				//if(zdata)
				//	free(zdata);

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
				usleep(250 * 1000);
			}
		}
	}
	return NULL;
}

void InitTcpThread()
{
	LWP_CreateThread (&tcpthread, tcp_callback, NULL, NULL, 32768, 60);
}

/****************************************************************************
 * ShutdownThread
 ***************************************************************************/
void ShutdownTcpThread()
{
    if(tcpthread != LWP_THREAD_NULL)
    {
        ResumeTcpThread();
        LWP_JoinThread (tcpthread, NULL);
        tcpthread = LWP_THREAD_NULL;
    }
}
