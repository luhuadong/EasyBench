#include "pcm_memory_device.h"

PcmMemoryDevice::PcmMemoryDevice(const QByteArray &pcm, QObject *parent)
    : QIODevice(parent)
    , m_pcm(pcm)
    , m_pos(0)
{
}

bool PcmMemoryDevice::isSequential() const
{
    return true;
}

qint64 PcmMemoryDevice::readData(char *data, qint64 maxSize)
{
    if (m_pos >= m_pcm.size()) {
        return 0;
    }
    const qint64 remaining = m_pcm.size() - m_pos;
    const qint64 toRead = qMin(maxSize, remaining);
    memcpy(data, m_pcm.constData() + m_pos, static_cast<size_t>(toRead));
    m_pos += toRead;
    return toRead;
}

qint64 PcmMemoryDevice::writeData(const char *data, qint64 maxSize)
{
    Q_UNUSED(data);
    Q_UNUSED(maxSize);
    return -1;
}
