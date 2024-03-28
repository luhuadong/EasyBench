#ifndef CPUSTATTHREAD_H
#define CPUSTATTHREAD_H

#include <QThread>

class CpuStatThread : public QThread
{
    Q_OBJECT
public:
    explicit CpuStatThread(QObject *parent = 0);

signals:

protected:
    void run();

public slots:

};

#endif // CPUSTATTHREAD_H
