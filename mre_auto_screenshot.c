#include "mre_auto_screenshot.h"

const int BYTES_PER_PIXEL = 3;
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

    layer_hdl[0] = -1;
    vm_reg_sysevt_callback(handle_sysevt);
    //width = vm_graphic_get_screen_width();
    //height = vm_graphic_get_screen_height();

}

void handle_sysevt(VMINT message, VMINT param) {

    switch (message) {
    case VM_MSG_CREATE:
        break;
    case VM_MSG_PAINT:
        layer_hdl[0] = vm_graphic_create_layer(0, 0, vm_graphic_get_screen_width(), vm_graphic_get_screen_height(), -1);
        vm_graphic_set_clip(0, 0, vm_graphic_get_screen_width(), vm_graphic_get_screen_height());
        buffer = vm_graphic_get_layer_buffer(layer_hdl[0]);

        if (!x) { vm_pmng_set_bg(); }
        x = VM_TRUE;
        if (p) {
            counter = 10;
        }
        if (pp) {
            pp = VM_FALSE;
            timer_id = vm_create_timer_ex(6000, timer);
        }
        break;
    case VM_MSG_HIDE:
        break;
    case VM_MSG_QUIT:
        if (layer_hdl[0] != -1) {
            vm_graphic_delete_layer(layer_hdl[0]);
            layer_hdl[0] = -1;
        }
        break;
    }
}

void timer(VMINT a){

	p = VM_TRUE;
	if (counter >= 10 && timer_id != -1) {
            vm_delete_timer_ex(timer_id);
            timer_id = -1;
            vm_exit_app();
	} else {
            vm_audio_play_beep();
            vm_graphic_flush_layer(layer_hdl, 1);
            //vm_graphic_flush_layer_ex(layer_hdl, vm_graphic_get_blt_layer_count(), 240, 320, 240, 320);
            screenshot();                   // kazkas su permatomumu turbut
        }
	counter = counter + 1;
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

int screenshot(void) {

    VMWCHAR e[100];
    int i;
    int x;
    int y;

    create_auto_filename(e, "bmp");
    create_auto_full_path_name(e, e); // overwrite with full path
    int width = vm_graphic_get_screen_width();
    int height = vm_graphic_get_screen_height();
    int row_stride = (width * 3 + 3) & ~3;  // padding to 4 bytes
    int pixel_data_size = row_stride * height;
    int g_bmp_size = FILE_HEADER_SIZE + INFO_HEADER_SIZE + pixel_data_size;
    unsigned char* g_bmp_buffer = vm_malloc(g_bmp_size);
    if (!g_bmp_buffer) return -1;

    // Fill headers
    createBitmapFileHeader(g_bmp_buffer, g_bmp_size);
    createBitmapInfoHeader(g_bmp_buffer + FILE_HEADER_SIZE, width, height);

    // Fill pixel data (bottom-up BMP)
    unsigned char* dst = g_bmp_buffer + FILE_HEADER_SIZE + INFO_HEADER_SIZE;
    unsigned short* src = (unsigned short*)buffer;

    for (y = height - 1; y >= 0; --y) {
        for (x = 0; x < width; ++x) {
            unsigned short color = src[y * width + x];

            // RGB565 to 24-bit conversion
            unsigned char r = VM_COLOR_GET_RED(color);
            unsigned char g = VM_COLOR_GET_GREEN(color);
            unsigned char b = VM_COLOR_GET_BLUE(color);

            *dst++ = b;
            *dst++ = g;
            *dst++ = r;
        }

        // Padding
        int padding = row_stride - (width * 3);
        for (i = 0; i < padding; ++i) {
            *dst++ = 0x00;
        }
    }

    // Write file
    VMUINT written;
    VMFILE f = vm_file_open((VMWSTR)e, MODE_CREATE_ALWAYS_WRITE, 1);
    if (f < 0) {
        vm_free(g_bmp_buffer);
        return -1;
    }

    vm_file_write(f, g_bmp_buffer, g_bmp_size, &written);
    vm_file_close(f);

    vm_free(g_bmp_buffer);
    return 0;
}

void createBitmapFileHeader(unsigned char* fileHeader, int fileSize) {
    fileHeader[0] = 'B';
    fileHeader[1] = 'M';
    fileHeader[2] = (unsigned char)(fileSize);
    fileHeader[3] = (unsigned char)(fileSize >> 8);
    fileHeader[4] = (unsigned char)(fileSize >> 16);
    fileHeader[5] = (unsigned char)(fileSize >> 24);
    fileHeader[6] = 0;
    fileHeader[7] = 0;
    fileHeader[8] = 0;
    fileHeader[9] = 0;
    fileHeader[10] = FILE_HEADER_SIZE + INFO_HEADER_SIZE;  // Pixel data offset = 54
    fileHeader[11] = 0;
    fileHeader[12] = 0;
    fileHeader[13] = 0;
}

void createBitmapInfoHeader(unsigned char* infoHeader, int width, int height) {
    int i;
    for (i = 0; i < INFO_HEADER_SIZE; ++i) infoHeader[i] = 0;

    infoHeader[0] = INFO_HEADER_SIZE;  // Header size
    infoHeader[4] = (unsigned char)(width);
    infoHeader[5] = (unsigned char)(width >> 8);
    infoHeader[6] = (unsigned char)(width >> 16);
    infoHeader[7] = (unsigned char)(width >> 24);
    infoHeader[8] = (unsigned char)(height);
    infoHeader[9] = (unsigned char)(height >> 8);
    infoHeader[10] = (unsigned char)(height >> 16);
    infoHeader[11] = (unsigned char)(height >> 24);
    infoHeader[12] = 1; // planes
    infoHeader[14] = 24; // bits per pixel (RGB24)
    int imageSize = ((width * 3 + 3) & ~3) * height;
    infoHeader[20] = (unsigned char)(imageSize);
    infoHeader[21] = (unsigned char)(imageSize >> 8);
    infoHeader[22] = (unsigned char)(imageSize >> 16);
    infoHeader[23] = (unsigned char)(imageSize >> 24);
    infoHeader[24] = 0x13; // 2835 pixels/meter = 72 DPI
    infoHeader[25] = 0x0B;
    infoHeader[28] = 0x13;
    infoHeader[29] = 0x0B;
}

