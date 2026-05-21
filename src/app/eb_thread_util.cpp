#include "eb_thread_util.h"

#include <QThread>

#ifdef __linux__
#include <pthread.h>
#endif

namespace EbThread {

void setCurrentThreadName(const char *name)
{
    if (!name || name[0] == '\0') {
        return;
    }

    if (QThread *const current = QThread::currentThread()) {
        current->setObjectName(QString::fromLatin1(name));
    }

#ifdef __linux__
    pthread_setname_np(pthread_self(), name);
#endif
}

void nameQThread(QThread *thread, const char *name)
{
    if (!thread || !name || name[0] == '\0') {
        return;
    }
    thread->setObjectName(QString::fromLatin1(name));
}

} // namespace EbThread
