#include "cpustatthread.h"
#include "sub_page/monitorpage.h"
#include <QString>
#include <QFile>

extern "C"
{
#include <stdio.h>
#include <unistd.h>
}

CpuStatThread::CpuStatThread(QObject *parent) :
    QThread(parent)
{
    printf("CpuStatThread\n");
}

void CpuStatThread::run()
{
    float cpuTotalDuty;
    MonitorPage *page = (MonitorPage *)parent();
    QString tmpStr;
    QStringList tmpStrList;
    QFile tmpFile;

    int tt = 2;
    int index = 0;
    int cpuInfo[2][7];
    int cpuTotal[2][2];
    int a = 0, b = 0;
    int a0 = 0, a1 = 0, b0 = 0, b1 = 0;

    while(1)
    {
//        index = 0;
//        while(index < 2) {
//            tmpFile.setFileName(CPU_FILE);
//            if(tmpFile.open(QIODevice::ReadOnly)) {

//                tmpStr = tmpFile.readLine();
//                if(-1 != tmpStr.indexOf("cpu")) {

//                    tmpStrList = tmpStr.split(QRegExp("\\s+"));
//                    for(int i=0; i<7; i++) {
//                        cpuInfo[index][i] = tmpStrList.at(i+1).toInt();
//                        cpuTotal[index][0] +=
//                    }
//                    index++;


//                }
//                else {
//                    break;
//                }
//            }
//        }



        tt = 2;
        cpuTotal[0][0] = 0;
        cpuTotal[0][1] = 0;
        cpuTotal[1][0] = 0;
        cpuTotal[1][1] = 0;

        //tmpFile.setFileName(CPU_FILE);
        while(tt) {

            //printf("open %s\n", CPU_FILE);
            tmpFile.setFileName(CPU_FILE);
            if(tmpFile.open(QIODevice::ReadOnly)) {
                tmpStr = tmpFile.readLine();
                for(int i=0; i<7; i++) {
                    cpuInfo[2-tt][i] = tmpStr.section(" ", i+2, i+2).toInt();
                    cpuTotal[1][2-tt] += cpuInfo[2-tt][i];
                    if(i == 3) {
                        cpuTotal[0][2-tt] += cpuInfo[2-tt][i];
                    }
                }
                tt--;
                tmpFile.close();
                usleep(500000);
            }
        }
        a = cpuTotal[0][1] - cpuTotal[0][0];    // idle_2   - idle_1
        b = cpuTotal[1][1] - cpuTotal[1][0];    // total_2 - total_1
        a = a<0 ? -a : a;
        b = b<0 ? -b : b;
        printf("a=%d, b=%d\n", a, b);
        printf("cpuTotal[0][1] = %d, cpuTotal[0][0] = %d\n", cpuTotal[0][1], cpuTotal[0][0]);
        printf("cpuTotal[1][1] = %d, cpuTotal[1][0] = %d\n", cpuTotal[1][1], cpuTotal[1][0]);
        if(0 == b) {
            cpuTotalDuty = 0;
        }
        else {
            cpuTotalDuty = 100 - a*100/b;
        }
        //cpuBar->setValue(a*100/b);

        /*
        //tmpFile.setFileName(CPU_FILE);
        if(tmpFile.open(QIODevice::ReadOnly)) {
            tmpStr = tmpFile.readLine();
            a0 = a1;
            b0 = b1;
            a1 = b1 = 0;
            int gg;
            for(int i=0; i<7; i++) {
                b1 += tmpStr.section(" ", i+2, i+2).toInt();
                gg = b1;
                if(i == 3) {
                    a1 += tmpStr.section(" ", i+2, i+2).toInt();
                }
            }
            int m, n;
            m = a1 - a0;
            n = b1 - b0;
            m = m<0 ? -m : m;
            n = n<0 ? -n : n;
            printf("m=%d, n=%d\n", m, n);
            //cpuBar->setValue((n-m)*100/n);
            tmpFile.close();
        }
        */

        page->setCpuDuty(cpuTotalDuty);
        usleep(500000);
    }
}
