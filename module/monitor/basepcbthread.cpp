#include "basepcbthread.h"
#include "sub_page/monitorpage.h"
#include <QString>
//#include <QChar>

extern "C"
{
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <errno.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
}

BasePcbThread::BasePcbThread(const int arg, QObject *parent) :
    QThread(parent)
{
    sockfd = arg;
}

void BasePcbThread::run()
{
    int len;
    MonitorPage *page = (MonitorPage *)parent();

    while(1)
    {
        bzero(buffer, MAXBUF + 1);
        len = recv(sockfd, buffer, MAXBUF, 0);

        if(len > 0) {
            if(len == sizeof(readStatusCMD)) {
                readStatusCMD cmd;
                memcpy(&cmd, buffer, sizeof(cmd));

                if(cmd.head == 0xAA && cmd.tail == 0xEE \
                        && cmd.size == sizeof(readStatusCMD) \
                        && cmd.csum == calcCheckSum(buffer, cmd.size-2)) {
                    printf("+++ Right! read status +++\n");
                    page->setBasePcbTemp1Text(cmd.temperature1);
                    page->setBasePcbTemp2Text(cmd.temperature2);
                    page->setFanSpeedText(cmd.fanSpeed);

                }
            }
            else if(len == sizeof(readVersionCMD)) {
                readVersionCMD cmd;
                memcpy(&cmd, buffer, sizeof(cmd));

                if(cmd.head == 0xAA && cmd.tail == 0xEE \
                        && cmd.size == sizeof(readVersionCMD) \
                        && cmd.csum == calcCheckSum(buffer, cmd.size-2)) {
                    printf("+++ Right! read version +++\n");

                    page->setFwVerText(QString("Firmware Ver : v %1.%2.%3.%4").arg(cmd.firmwareVer[0]).arg(cmd.firmwareVer[1]).arg(cmd.firmwareVer[2]).arg(cmd.firmwareVer[3]));
                    page->setHwVerText(QString("Hardware Ver : v %1.%2").arg(cmd.hardwareVer[0]).arg(cmd.hardwareVer[1]));
                }
            }
        }
        else if(len < 0) {
            printf("recv failure! errno code is %d, errno message is '%s'\n", errno, strerror(errno));
            continue;
        }
        else {
            printf("the other one close quit\n");
            continue;
        }
    }
}
