#ifndef EB_THREAD_UTIL_H
#define EB_THREAD_UTIL_H

class QThread;

namespace EbThread {

/** 设置当前线程名（QThread::objectName + Linux pthread 名，供 GDB info threads 识别） */
void setCurrentThreadName(const char *name);

/** 在 start() 之前为 QThread 设置 objectName（主线程尚未运行时仅影响 Qt 调试名） */
void nameQThread(QThread *thread, const char *name);

} // namespace EbThread

#endif // EB_THREAD_UTIL_H
