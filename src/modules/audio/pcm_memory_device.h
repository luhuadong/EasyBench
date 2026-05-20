#ifndef PCM_MEMORY_DEVICE_H
#define PCM_MEMORY_DEVICE_H

#include <QIODevice>

class PcmMemoryDevice : public QIODevice
{
    Q_OBJECT
public:
    explicit PcmMemoryDevice(const QByteArray &pcm, QObject *parent = nullptr);

    bool isSequential() const override;
    qint64 readData(char *data, qint64 maxSize) override;
    qint64 writeData(const char *data, qint64 maxSize) override;

private:
    QByteArray m_pcm;
    qint64 m_pos;
};

#endif /* PCM_MEMORY_DEVICE_H */
