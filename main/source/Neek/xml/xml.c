#include <stdio.h>
#include <stdlib.h>
#include <gccore.h>
#include <wiiuse/wpad.h>

/*Include these for mxml*/
#include <fat.h>
#include <mxml.h>
#include "../common.h"

/*
struct SSettingsNeek2o {
	char type[8];
	char dipath[64];
	char nandbootdelay[4];
	char neekbootdelay[4]
	char nandbootapplication[64];
	char neekbootapplication[64];
	char neekbootchannel[16];
	char neekselection[2];
};
*/

extern struct SSettingsNeek2o n2oSettings;





void CreateXmlFile(const char* filename, struct SSettingsNeek2o *Settings)
{
	mxml_node_t *xml;
	mxml_node_t *data;

	xml = mxmlNewXML("1.0");

	data = mxmlNewElement(xml, "neek2oconfig");

   //Create config value
   //mxmlElementSetAttr(data, "type",Settings->type);
	mxmlElementSetAttr(data, "neeknandpath",Settings->neeknandpath);
	mxmlElementSetAttr(data, "neekdipath",Settings->neekdipath);
    mxmlElementSetAttr(data, "nandbootdelay",Settings->nandbootdelay);
    mxmlElementSetAttr(data, "neekbootdelay",Settings->neekbootdelay);

    mxmlElementSetAttr(data, "nandbootapplication",Settings->nandbootapplication);
    mxmlElementSetAttr(data, "neekbootapplication",Settings->neekbootapplication);

    mxmlElementSetAttr(data, "neekbootchannel",Settings->neekbootchannel);
    mxmlElementSetAttr(data, "neekselection",Settings->neekselection);

    mxmlElementSetAttr(data, "nandbootdescription",Settings->nandbootdescription);
    mxmlElementSetAttr(data, "neekbootdescription",Settings->neekbootdescription);

    mxmlElementSetAttr(data, "nandbootargument",Settings->nandbootargument);
    mxmlElementSetAttr(data, "neekbootargument",Settings->neekbootargument);


   /*save the xml file to a file*/
   FILE *fp;
   fp = fopen(filename, "w");

   mxmlSaveFile(xml, fp, MXML_NO_CALLBACK);

   /*clean up*/
   fclose(fp);
   mxmlDelete(data);
   mxmlDelete(xml);
}

int LoadXmlFile(const char* filename, struct SSettingsNeek2o *Settings)
{
	FILE *fp;
	mxml_node_t *tree;
	mxml_node_t *data;
	u32 counter, count2;
	u32 slashfound;
	char temppath[64];
	//u32 last,teller;
	/*Load our xml file! */
	fp = fopen(filename, "r");

	if (fp==NULL)
	{
	//WindowPrompt(filename, "Fehler", "Back",0);
	fclose(fp);
	return -1;
	}
	tree = mxmlLoadFile(NULL, fp, MXML_NO_CALLBACK);
	fclose(fp);

	/*Load and printf our values! */
	/* As a note, its a good idea to normally check if node* is NULL */
	data = mxmlFindElement(tree, tree, "neek2oconfig", NULL, NULL, MXML_DESCEND);

	//snprintf(Settings->type,8,"%s",mxmlElementGetAttr(data,"type"));
	snprintf(Settings->neeknandpath,64,"%s",mxmlElementGetAttr(data,"neeknandpath"));
	snprintf(Settings->neekdipath,64,"%s",mxmlElementGetAttr(data,"neekdipath"));
	snprintf(Settings->nandbootdelay,4,"%s",mxmlElementGetAttr(data,"nandbootdelay"));
	snprintf(Settings->neekbootdelay,4,"%s",mxmlElementGetAttr(data,"neekbootdelay"));
	snprintf(Settings->nandbootapplication,64,"%s",mxmlElementGetAttr(data,"nandbootapplication"));
	snprintf(Settings->neekbootapplication,64,"%s",mxmlElementGetAttr(data,"neekbootapplication"));
	snprintf(Settings->neekbootchannel,24,"%s",mxmlElementGetAttr(data,"neekbootchannel"));
	snprintf(Settings->neekselection,2,"%s",mxmlElementGetAttr(data,"neekselection"));
	snprintf(Settings->nandbootdescription,80,"%s",mxmlElementGetAttr(data,"nandbootdescription"));
	snprintf(Settings->neekbootdescription,80,"%s",mxmlElementGetAttr(data,"neekbootdescription"));
	snprintf(Settings->nandbootargument,80,"%s",mxmlElementGetAttr(data,"nandbootargument"));
	snprintf(Settings->neekbootargument,80,"%s",mxmlElementGetAttr(data,"neekbootargument"));

	//strcpy(Settings->SMB_PWD,mxmlElementGetAttr(data,"password"));
	//snprintf(Settings->SMB_PWD,64,"%s",mxmlElementGetAttr(data,"password"));
	//strcpy(Settings->SMB_SHARE,mxmlElementGetAttr(data,"smbsharename"));
	//snprintf(Settings->SMB_SHARE,64,"%s",mxmlElementGetAttr(data,"smbsharename"));
	//strcpy(Settings->SMB_IP,(char*)mxmlElementGetAttr(data,"smbip"));
	//snprintf(Settings->SMB_IP,20,"%s",mxmlElementGetAttr(data,"smbip"));

	mxmlDelete(data);
	mxmlDelete(tree);

//we should convert neeknandpath in nanddisk and nandfolder
	if (Settings->neeknandpath[0] != 0)
	{
		strncpy(temppath,Settings->neeknandpath,3);
		temppath[3]=0;
		for (counter=0;counter<3;counter++)
		{
			temppath[counter] = toupper((int)(temppath[counter]));
		}
		if (strncmp(temppath,"USB",3)==0)
		{
			strcpy(Settings->nanddisc,"usb1:/");
			count2 = 3;
			slashfound = 0;
			while((count2 < strlen(Settings->neeknandpath))&&(slashfound ==0))
			{
				if (Settings->neeknandpath[count2] == '/')
				{
					slashfound = 1;
				}
				else
				{
					count2++;
				}

			}
			if (slashfound != 0)
			{
				if (Settings->neeknandpath[count2+1]=='/')
				{
					count2++;
				}
				strcpy(Settings->nandfolder,Settings->neeknandpath+count2);
			}
			else
			{
				//default to root
				strcpy(Settings->nandfolder,"/");
			}
		}
		else if (strncmp(temppath,"SD",2)==0)
		{
			strcpy(Settings->nanddisc,"sd1:/");
			count2 = 2;
			slashfound = 0;
			while((count2 < strlen(Settings->neeknandpath))&&(slashfound == 0))
			{
				if (Settings->neeknandpath[count2] == '/')
				{
					slashfound = 1;
				}
				else
				{
					count2++;
				}

			}
			if (slashfound != 0)
			{
				if (Settings->neeknandpath[count2+1]=='/')
				{
					count2++;
				}
				strcpy(Settings->nandfolder,Settings->neeknandpath+count2);
			}
			else
			{
				//default to root
				strcpy(Settings->nandfolder,"/");
			}

		}
	}
	else
	{
		//default to root of usb
		//not sure this is needed
		strcpy(Settings->nanddisc,"usb1:/");
		strcpy(Settings->nandfolder,"/");
	}







//obcd
//Somehow, the first dot from the ip adress is changed into a ,
//This dirty hack ensures that only dots are present in the ip adress
/*
	last = strlen(SMBSettings.SMB_IP);
	for (teller = 0;teller < last;teller++)
	{
		if (SMBSettings.SMB_IP[teller] == ',')
		{
			SMBSettings.SMB_IP[teller] = '.';
		}
	}
*/
	return 1;
}
