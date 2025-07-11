#ifndef _VRE_APP_WIZARDTEMPLATE_
#define	_VRE_APP_WIZARDTEMPLATE_

#define		SUPPORT_BG

#include "vmsys.h"
#include "vmio.h"
#include "vmtimer.h"
#include "vmchset.h"
#include "vmpromng.h"
#include "vmmm.h"
//#include <string>
#include "vmgraph.h"
#include "vmstdlib.h"
#include <math.h>
#include <stdio.h>
#include "stdint.h"
#include "ResID.h"
#include "vm4res.h"
#include "vmres.h"
#include "vmstatusbar.h"
#include "vmbtcm.h"
#include "vmlog.h"
#include <time.h>
#include <string.h>

VMINT		layer_hdl[1];

void handle_sysevt(VMINT message, VMINT param);
void timer(VMINT a);
int screenshot(void);
void create_auto_filename(VMWSTR text, VMSTR extt);
void create_auto_full_path_name(VMWSTR result, VMWSTR fname);
unsigned char* createBitmapFileHeader(int fileSize);
unsigned char* createBitmapInfoHeader(int height, int width);

#endif

