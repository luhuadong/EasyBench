#ifdef __cplusplus
extern "C"{
#endif

#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>
#include <fcntl.h>
#include <linux/fb.h>
//#include <sys/mman.h>
#include <sys/ioctl.h>

#ifdef __cplusplus
}
#endif

#define GY_GLOBAL
#include "gyt_common.h"

char *getBacklightNodeName(void)
{
    DIR *dir;
    struct dirent *ent;
    char *retName = NULL;

    dir = opendir("/sys/class/backlight/");
    if(NULL == dir)
        return NULL;

    while((ent = readdir(dir)) != NULL) {
        if((0 == strcmp(ent->d_name, ".")) || (0 == strcmp(ent->d_name, ".."))) {
            continue;
        }else {
            retName = ent->d_name;
            break;
        }
    }
    closedir(dir);
    return retName;
}

int getScreenInfo(struct fb_var_screeninfo *vinfo)
{
    int fbfd = 0;
    fbfd = open("/dev/fb0", O_RDONLY);
    if (!fbfd) {
        //printf("Error: cannot open framebuffer device.\n");
        return -1;
    }
    //printf("The framebuffer device was opened successfully.\n");

    if (ioctl(fbfd, FBIOGET_VSCREENINFO, vinfo)) {
        //printf("Error reading variable information.\n");
        return -1;
    }
    close(fbfd);
    return 0;
}

void detectDevices(void)
{
    struct fb_var_screeninfo vinfo;
    if(0 == getScreenInfo(&vinfo)) {
        LCD_WIDTH = vinfo.xres;
        LCD_HEIGHT = vinfo.yres;
    }else {
        LCD_WIDTH = 800;
        LCD_HEIGHT = 600;
    }


    memset(BACKLIGHT_NAME, 0, sizeof(BACKLIGHT_NAME));
    strcpy(BACKLIGHT_NAME, getBacklightNodeName());

    TOUCH_TYPE = TOUCH_CAPACITIVE;
}

unsigned char calcCheckSum(unsigned char *data, unsigned int len)
{
    unsigned char sum = 0x00;
    while(len--) {
        sum += data[len];
    }
    return sum;
}





