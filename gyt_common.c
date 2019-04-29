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
#include <time.h>
#include <sys/time.h>

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
#if 0
    strcpy(BACKLIGHT_NAME, getBacklightNodeName());
#else
    strcpy(BACKLIGHT_NAME, "backlight.22");
#endif

    TOUCH_TYPE = TOUCH_RESISTIVE;
    strcpy(gSerialPortStr, "/dev/ttymxc1");
    strcpy(gVideoInputStr, "/dev/video0");
}

unsigned char calcCheckSum(unsigned char *data, unsigned int len)
{
    unsigned char sum = 0x00;
    while(len--) {
        sum += data[len];
    }
    return sum;
}

bool is_singleton(void)
{
    int fd = -1;
    char buf[32];

    fd = open(DEFAULT_FILE, O_WRONLY | O_CREAT, 0666);
    if(fd < 0) {
        printf("(E) Open %s failed.\n", DEFAULT_FILE);
        return false;
    }

    struct flock lock;
    bzero(&lock, sizeof(lock));

    if(fcntl(fd, F_GETLK, &lock) < 0) {
        printf("(E) Fail to fcntl F_GETLK.\n");
        return false;
    }

    lock.l_type = F_WRLCK;
    lock.l_whence = SEEK_SET;

    if(fcntl(fd, F_SETLK, &lock) < 0) {
        printf("(E) Fail to fcntl F_SETLK.\n");
        return false;
    }

    pid_t pid = getpid();
    int len = snprintf(buf, 32, "%d\n", (int)pid);

    // Write pid to the file, don't close
    if(-1 == write(fd, buf, len)) {
        printf("(E) Fail to write pid into file.\n");
        return false;
    }

    return true;
}


int get_timezone()
{
    struct timezone tz;

    gettimeofday(NULL, &tz);

    return tz.tz_minuteswest / -60;
}



