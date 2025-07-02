#include "mre_auto_screenshot.h"

const int BYTES_PER_PIXEL = 3;  /// red, green, & blue
const int FILE_HEADER_SIZE = 14;
const int INFO_HEADER_SIZE = 40;

VMUINT8 *buffer;
VMBOOL p = VM_FALSE;
VMBOOL pp = VM_TRUE;
VMBOOL x = VM_FALSE;
VMBOOL z = VM_FALSE;
VMINT counter = 0;
static VMINT timer_id = -1;


void vm_main(void) {
	vm_reg_sysevt_callback(handle_sysevt);
}

void handle_sysevt(VMINT message, VMINT param) {
	switch (message) {
	case VM_MSG_CREATE:
		break;
	case VM_MSG_PAINT:
	layer_hdl[0] = vm_graphic_create_layer(0, 0, vm_graphic_get_screen_width(), vm_graphic_get_screen_height(), -1);
		vm_graphic_set_clip(0, 0, vm_graphic_get_screen_width(), vm_graphic_get_screen_height());
                buffer = vm_graphic_get_layer_buffer(layer_hdl[0]);

                if (!x) {vm_pmng_set_bg();}
                x = VM_TRUE;
		if (p) {
                   vm_exit_app();}
		if (pp) {
	           pp = VM_FALSE;
	           timer_id = vm_create_timer_ex(6000, timer);
                }

		break;
	case VM_MSG_HIDE:	
		//if( layer_hdl[0] != -1 )
		//{
		//	vm_graphic_delete_layer(layer_hdl[0]);
		//	layer_hdl[0] = -1;
		//}
		break;
	case VM_MSG_QUIT:
		{
			vm_graphic_delete_layer(layer_hdl[0]);
			layer_hdl[0] = -1;
		}
		break;
	}
}

void timer(VMINT a){
	p = VM_TRUE;
	if(counter >= 10 && timer_id != -1)
	{
		vm_delete_timer_ex(timer_id);
		timer_id = -1;
                vm_exit_app();

	}
	vm_audio_play_beep();
        vm_graphic_flush_layer(layer_hdl, 1);
        screenshot();
        counter = counter + 1;

}

int screenshot(void) {

    VMWCHAR s[100];
    VMWCHAR e[100];
    VMUINT p;
    char ss[100];
    int i, j, q, w, h;
    VMFILE f_read;

    create_auto_filename(e, "bmp");
    create_auto_full_path_name(s, e);

    f_read = vm_file_open((VMWSTR)s, MODE_CREATE_ALWAYS_WRITE, 1);
    w = vm_graphic_get_screen_width();
    h = vm_graphic_get_screen_height();

    vm_file_write(f_read, createBitmapFileHeader(FILE_HEADER_SIZE + INFO_HEADER_SIZE + vm_graphic_get_screen_width() * vm_graphic_get_screen_height() * 3), FILE_HEADER_SIZE, &p);
    vm_file_write(f_read, createBitmapInfoHeader(vm_graphic_get_screen_height(), vm_graphic_get_screen_width()), INFO_HEADER_SIZE, &p);

    unsigned char *temp_line_buf = vm_malloc(w * 3);

    for (i = h - 1; i > -1; --i) {
        for (j = 0; j < w; ++j) {
            unsigned short color = ((unsigned short*)buffer)[i * w + j];
            temp_line_buf[j * 3 + 0] = VM_COLOR_GET_BLUE(color);
            temp_line_buf[j * 3 + 1] = VM_COLOR_GET_GREEN(color);
            temp_line_buf[j * 3 + 2] = VM_COLOR_GET_RED(color);
        }
        vm_file_write(f_read, temp_line_buf, w * 3, &p);
    }

    vm_file_close(f_read);

    vm_free(temp_line_buf);
    temp_line_buf = NULL;

    return 0;
}

void create_auto_filename(VMWSTR text, VMSTR extt) {

    struct vm_time_t curr_time;
    VMCHAR fAutoFileName[100];

    vm_get_time(&curr_time);
    sprintf(fAutoFileName, "%02d%02d%02d%02d%02d.%s", curr_time.mon, curr_time.day, curr_time.hour, curr_time.min, curr_time.sec, extt);
    vm_ascii_to_ucs2(text, (strlen(fAutoFileName) + 1) * 2, fAutoFileName);

}

void create_auto_full_path_name(VMWSTR result, VMWSTR fname) {

    VMINT drv;
    VMCHAR fAutoFileName[100];
    VMWCHAR wAutoFileName[100];

    if ((drv = vm_get_removable_driver()) < 0) {
       drv = vm_get_system_driver();
    }

    sprintf(fAutoFileName, "%c:\\", drv);
    vm_ascii_to_ucs2(wAutoFileName, (strlen(fAutoFileName) + 1) * 2, fAutoFileName);
    vm_wstrcat(wAutoFileName, fname);
    vm_wstrcpy(result, wAutoFileName);

}

unsigned char* createBitmapFileHeader(int fileSize) {

    static unsigned char fileHeader[] = {
        0, 0,        /// signature
        0, 0, 0, 0,  /// image file size in bytes
        0, 0, 0, 0,  /// reserved
        0, 0, 0, 0,  /// start of pixel array
    };

    fileHeader[0] = (unsigned char)('B');
    fileHeader[1] = (unsigned char)('M');
    fileHeader[2] = (unsigned char)(fileSize);
    fileHeader[3] = (unsigned char)(fileSize >> 8);
    fileHeader[4] = (unsigned char)(fileSize >> 16);
    fileHeader[5] = (unsigned char)(fileSize >> 24);
    fileHeader[10] = (unsigned char)(FILE_HEADER_SIZE + INFO_HEADER_SIZE);

    return fileHeader;
}

unsigned char* createBitmapInfoHeader(int height, int width) {

    static unsigned char infoHeader[] = {
        0, 0, 0, 0,  /// header size
        0, 0, 0, 0,  /// image width
        0, 0, 0, 0,  /// image height
        0, 0,        /// number of color planes
        0, 0,        /// bits per pixel
        0, 0, 0, 0,  /// compression
        0, 0, 0, 0,  /// image size
        0, 0, 0, 0,  /// horizontal resolution
        0, 0, 0, 0,  /// vertical resolution
        0, 0, 0, 0,  /// colors in color table
        0, 0, 0, 0,  /// important color count
    };

    infoHeader[0] = (unsigned char)(INFO_HEADER_SIZE);
    infoHeader[4] = (unsigned char)(width);
    infoHeader[5] = (unsigned char)(width >> 8);
    infoHeader[6] = (unsigned char)(width >> 16);
    infoHeader[7] = (unsigned char)(width >> 24);
    infoHeader[8] = (unsigned char)(height);
    infoHeader[9] = (unsigned char)(height >> 8);
    infoHeader[10] = (unsigned char)(height >> 16);
    infoHeader[11] = (unsigned char)(height >> 24);
    infoHeader[12] = (unsigned char)(1);
    infoHeader[14] = (unsigned char)(BYTES_PER_PIXEL * 8);

    return infoHeader;
}