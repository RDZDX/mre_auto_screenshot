#ifndef _VRE_APP_WIZARDTEMPLATE_
#define	_VRE_APP_WIZARDTEMPLATE_

#define		SUPPORT_BG

#include "vmsys.h"
#include "vmio.h"
#include "vmtimer.h"
#include "vmchset.h"
#include "vmpromng.h"
#include "vmmm.h"
#include "vmgraph.h"
#include "vmstdlib.h"
#include <stdio.h>
#include "stdint.h"
#include <time.h>
#include <string.h>

VMINT		layer_hdl[1];

void handle_sysevt(VMINT message, VMINT param);
void timer(VMINT a);
int screenshot(void);
void create_auto_filename(VMWSTR text, VMSTR extt);
void create_auto_full_path_name(VMWSTR result, VMWSTR fname);
void createBitmapInfoHeader(unsigned char* infoHeader, int width, int height);
void createBitmapFileHeader(unsigned char* fileHeader, int fileSize);

#endif

