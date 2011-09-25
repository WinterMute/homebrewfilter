
#include <ogcsys.h>
#include <unistd.h>

#include "libwiigui/gui.h"
#include "main.h"
//#include "Prompts/prompts.h"

/*** Extern variables ***/
extern GuiWindow * mainWindow;

/*** Extern functions ***/
extern void ResumeGui();
extern void HaltGui();

GuiImage * throbberImg = NULL;
static lwp_t throbberthread = LWP_THREAD_NULL;
static bool throbberHalt = true;
int spin = 0;

void ResumeThrobberThread()
{
	if(throbberHalt)
	{
		throbberHalt = false;
		
		throbberImg = new GuiImage(new GuiImageData(Theme.throbber));
		throbberImg->SetAlignment(ALIGN_CENTRE, ALIGN_MIDDLE);
		
		HaltGui();
		mainWindow->SetState(STATE_DISABLED);
		mainWindow->Append(throbberImg);
		ResumeGui();
		
		LWP_ResumeThread(throbberthread);
	}
}

void HaltThrobberThread()
{
	if(!throbberHalt)
	{
		spin = 0;
		throbberHalt = true;
		
		HaltGui();
		mainWindow->Remove(throbberImg);
		mainWindow->SetState(STATE_DEFAULT);
		ResumeGui();
	}
}

static void * throbber_update(void *arg)
{
    while(1)
	{
		if(throbberHalt)
		{
			LWP_SuspendThread(throbberthread);
		}
		else
		{
			usleep(2000);
			spin += 1;
			if(spin >= 360)
				spin = 0;
			throbberImg->SetAngle(spin);
		}
	}
	return NULL;
}

void InitThrobberThread()
{
	LWP_CreateThread (&throbberthread, throbber_update, NULL, NULL, 32768, 60);
}
