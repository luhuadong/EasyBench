#ifndef EB_PCM_UTIL_H
#define EB_PCM_UTIL_H

#include <QAudioFormat>
#include <QByteArray>
#include <QStringList>

namespace EbPcm {

QAudioFormat defaultFormat();

QByteArray generateSineWave(int sampleRate, int durationMs, double frequencyHz, double volume);
QByteArray generateDualTone(int sampleRate, int durationMs, double freq1, double freq2, double volume);
QByteArray generateSweep(int sampleRate, int durationMs, double startHz, double endHz, double volume);

QStringList builtinToneNames();
QByteArray builtinTonePcm(int index);

} // namespace EbPcm

#endif /* EB_PCM_UTIL_H */
