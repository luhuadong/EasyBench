#include "tb_pcm_util.h"

#include <QAudioFormat>
#include <QtMath>
#include <QVector>

namespace TbPcm {

QAudioFormat defaultFormat()
{
    QAudioFormat format;
    format.setSampleRate(44100);
    format.setChannelCount(1);
    format.setSampleSize(16);
    format.setCodec(QStringLiteral("audio/pcm"));
    format.setByteOrder(QAudioFormat::LittleEndian);
    format.setSampleType(QAudioFormat::SignedInt);
    return format;
}

static QByteArray generateWaveform(int sampleRate, int durationMs,
                                   const QVector<double> &frequencies,
                                   double volume)
{
    const int sampleCount = sampleRate * durationMs / 1000;
    QByteArray data;
    data.resize(sampleCount * static_cast<int>(sizeof(qint16)));

    qint16 *samples = reinterpret_cast<qint16 *>(data.data());
    const double amplitude = qBound(0.0, volume, 1.0) * 32767.0;

    for (int i = 0; i < sampleCount; ++i) {
        const double t = static_cast<double>(i) / sampleRate;
        double value = 0.0;
        for (double freq : frequencies) {
            value += qSin(2.0 * M_PI * freq * t);
        }
        value /= qMax(1, frequencies.size());
        samples[i] = static_cast<qint16>(qBound(-32767.0, value * amplitude, 32767.0));
    }
    return data;
}

QByteArray generateSineWave(int sampleRate, int durationMs, double frequencyHz, double volume)
{
    QVector<double> freqs;
    freqs.append(frequencyHz);
    return generateWaveform(sampleRate, durationMs, freqs, volume);
}

QByteArray generateDualTone(int sampleRate, int durationMs, double freq1, double freq2, double volume)
{
    QVector<double> freqs;
    freqs.append(freq1);
    freqs.append(freq2);
    return generateWaveform(sampleRate, durationMs, freqs, volume);
}

QByteArray generateSweep(int sampleRate, int durationMs, double startHz, double endHz, double volume)
{
    const int sampleCount = sampleRate * durationMs / 1000;
    QByteArray data;
    data.resize(sampleCount * static_cast<int>(sizeof(qint16)));

    qint16 *samples = reinterpret_cast<qint16 *>(data.data());
    const double amplitude = qBound(0.0, volume, 1.0) * 32767.0;

    for (int i = 0; i < sampleCount; ++i) {
        const double progress = static_cast<double>(i) / qMax(1, sampleCount - 1);
        const double freq = startHz + (endHz - startHz) * progress;
        const double t = static_cast<double>(i) / sampleRate;
        const double value = qSin(2.0 * M_PI * freq * t);
        samples[i] = static_cast<qint16>(qBound(-32767.0, value * amplitude, 32767.0));
    }
    return data;
}

QStringList builtinToneNames()
{
    return QStringList()
        << QStringLiteral("纯音 440Hz (1秒)")
        << QStringLiteral("双音提示 (1秒)")
        << QStringLiteral("扫频 200Hz–4kHz (2秒)")
        << QStringLiteral("铃声风格 (1.5秒)");
}

QByteArray builtinTonePcm(int index)
{
    const int sampleRate = defaultFormat().sampleRate();
    switch (index) {
    case 0:
        return generateSineWave(sampleRate, 1000, 440.0, 0.7);
    case 1:
        return generateDualTone(sampleRate, 1000, 880.0, 660.0, 0.65);
    case 2:
        return generateSweep(sampleRate, 2000, 200.0, 4000.0, 0.6);
    case 3: {
        QByteArray part1 = generateDualTone(sampleRate, 400, 1046.5, 1318.5, 0.55);
        QByteArray part2 = generateSineWave(sampleRate, 1100, 988.0, 0.55);
        return part1 + part2;
    }
    default:
        return QByteArray();
    }
}

} // namespace TbPcm
