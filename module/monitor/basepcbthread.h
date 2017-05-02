#ifndef BASEPCBTHREAD_H
#define BASEPCBTHREAD_H

#include "gyt_common.h"
#include <QWidget>
#include <QThread>

#define MAXBUF 256

class BasePcbThread : public QThread
{
    Q_OBJECT
public:
    explicit BasePcbThread(const int arg, QObject *parent = 0);

signals:

public slots:

protected:
    void run();

private:
    int sockfd;
    unsigned char buffer[MAXBUF + 1];

};

#endif // BASEPCBTHREAD_H
