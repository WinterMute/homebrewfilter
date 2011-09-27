
#include <network.h>
#include <ogcsys.h>
#include <unistd.h>
#include <zlib.h>

#include "libwiigui/gui.h"
#include "main.h"
#include "BootHomebrew/BootHomebrew.h"
#include "Prompts/prompts.h"

#define READ_SIZE	(1 << 10)

u8 *data = (u8 *)0x92000000;

/*** Extern variables ***/
extern GuiWindow * mainWindow;
extern bool boot_buffer;

/*** Extern functions ***/
extern void ResumeGui();
extern void HaltGui();

static lwp_t tcpthread = LWP_THREAD_NULL;
static bool tcpHalt = true;

// code to oport a socket; originally from bzs@bu-cs.bu.edu
int oport(unsigned short portnum)
{
	int    s;
	struct sockaddr_in sa;

	memset(&sa, 0, sizeof(struct sockaddr_in)); /* clear our address */
	sa.sin_family= AF_INET;              /* this is our host address */
	sa.sin_port= htons(portnum);                /* this is our port number */
	sa.sin_addr.s_addr = net_gethostip();
	sa.sin_len = 8;
	if ((s= net_socket(AF_INET, SOCK_STREAM, 0)) < 0) /* create socket */
		return(-1);
	if (net_bind(s,(struct sockaddr *)&sa,8) < 0)
	{
		net_close(s);
		return(-1);                               /* bind address to socket */
	}
	net_listen(s, 3);                               /* max # of queued connects */
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
			return br;
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
				int temp = read_data(client, (char *)&read, 4);
				if(temp < 0)
				{
		//			printf("read_data() error while reading check\n");
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
		//			printf("HAXX\n");
					
					read_data(client, (char *)&read, 4);
		//			int WIILOAD_VERSION_MAYOR	= (u8)(((u16)(read >> 16)) >> 8);
		//			int WIILOAD_VERSION_MINOR	= (u8)(((u16)(read >> 16)) & 0xFF);
		//			int a						= (u8)(((u16)(read & 0xFFFF)) >> 8);
		//			int b						= (u8)(((u16)(read & 0xFFFF)) & 0xFF);
					
		//			printf("wiiload v%x.%x\n", WIILOAD_VERSION_MAYOR, WIILOAD_VERSION_MINOR);
		//			printf("args %x08\n", read);
		//			printf("args a=%x  b=%x\n", a, b);
					
					read_data(client, (char *)&size, 4);
					read_data(client, (char *)&uncfilesize, 4);
				}
				else
					size = read;
					
				offset = 0;
				while(offset < size && (read = read_data(client, (char *)bfr, (size - offset) > READ_SIZE ? READ_SIZE : (size - offset))) > 0)
				{
					memcpy(data + offset, bfr, READ_SIZE);
					offset += read;
					
					Percent = 100.0f * offset/size;
					progressbarImg->SetSize(Percent*3.27f, 38);
					
					char buffer[6];
					sprintf(buffer, "%i %%", (int)Percent);
					PercentTxt->SetText(buffer);
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
			//			fprintf (stderr, "error decompressing data: %d\n", res);
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
		//		if(zdata)
		//			free(zdata);
					
				mainWindow->Remove(PercentTxt);
				mainWindow->Remove(progressbarImg);
				mainWindow->Remove(progressImg);
				mainWindow->SetState(STATE_DEFAULT);
				ResumeGui();

				boot_buffer = true;
				break;
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
