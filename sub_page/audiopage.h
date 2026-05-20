#ifndef AUDIOPAGE_H
#define AUDIOPAGE_H

#include "custom_widget/pagewidget.h"
#include "eb_qt_compat.h"

#include <QAudioFormat>
#include <QByteArray>
#include <QComboBox>
#include <QGroupBox>
#include <QLabel>
#include <QPushButton>
#include <QSlider>

class PcmMemoryDevice;

#if EB_QT_VERSION_MAJOR >= 6
class QAudioSink;
class QAudioSource;
class QAudioDevice;
#else
class QAudioOutput;
class QAudioInput;
class QAudioDeviceInfo;
#endif

class AudioPage : public PageWidget
{
    Q_OBJECT
public:
    explicit AudioPage(EbOptions *options, QWidget *parent = 0);
    ~AudioPage();

private slots:
    void playTestTone();
    void stopPlayback();
    void onVolumeChanged(int value);
    void startMicRecord();
    void stopMicRecord();
    void playMicRecording();
    void onPlaybackFinished();
    void refreshOutputDevices();
    void refreshInputDevices();
    void onOutputDeviceChanged(int index);
    void onInputDeviceChanged(int index);

private:
    void buildUi();
    void setStatus(const QString &text);
    void stopSpeakerOutput();
    void stopMicInput();
    QByteArray currentTonePcm() const;
    void refreshToneList();
    void updatePlaybackFormat();
    void updateCaptureFormat();
    bool ensureOutputAvailable();
    bool ensureInputAvailable();

#if EB_QT_VERSION_MAJOR >= 6
    QAudioDevice currentOutputDevice() const;
    QAudioDevice currentInputDevice() const;
#else
    QAudioDeviceInfo currentOutputDevice() const;
    QAudioDeviceInfo currentInputDevice() const;
#endif

    QGroupBox *speakerGroup;
    QComboBox *outputDeviceBox;
    QPushButton *refreshOutputBtn;
    QComboBox *toneBox;
    QSlider *volumeSlider;
    QLabel *volumeValueLabel;
    QLabel *speakerStatusLabel;

    QGroupBox *micGroup;
    QComboBox *inputDeviceBox;
    QPushButton *refreshInputBtn;
    QPushButton *recordBtn;
    QPushButton *playRecordBtn;
    QLabel *micStatusLabel;

    QAudioFormat playbackFormat;
    QAudioFormat captureFormat;
    QAudioFormat recordedFormat;
    QByteArray recordedPcm;
    bool isRecording;
    bool isPlayingTone;

    PcmMemoryDevice *playbackDevice;
#if EB_QT_VERSION_MAJOR >= 6
    QAudioSink *audioSink;
    QAudioSource *audioSource;
#else
    QAudioOutput *audioOutput;
    QAudioInput *audioInput;
#endif
};

#endif /* AUDIOPAGE_H */
