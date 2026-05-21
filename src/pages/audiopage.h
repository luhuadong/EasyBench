#ifndef AUDIOPAGE_H
#define AUDIOPAGE_H

#include "widgets/pagewidget.h"
#include "eb_qt_compat.h"

#include <QAudioFormat>
#include <QByteArray>
#include <QComboBox>
#include <QGroupBox>
#include <QLabel>
#include <QPushButton>
#include <QSlider>

class PcmMemoryDevice;
class QAudioDecoder;

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

    QString defaultStatusHint() const override;

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
    void onTonePlayBtnClicked();

private:
    void buildUi();
    void updateTonePlayButton();
    void setSpeakerStatus(const QString &text);
    void setMicStatus(const QString &text);
    void updateStatusBar();
    void stopSpeakerOutput();
    void stopMicInput();
    void stopToneDecoder();
    void playPcmOnOutput(const QByteArray &pcm, const QAudioFormat &format,
                         const QString &statusText = QString());
    void onResourceToneDecoded();
    QByteArray currentTonePcm() const;
    QString currentResourceToneUrl() const;
    bool isResourceToneSelected() const;
    void playResourceTone(const QString &resourceUrl);
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
    QPushButton *tonePlayBtn;
    QSlider *volumeSlider;
    QLabel *volumeValueLabel;

    QGroupBox *micGroup;
    QComboBox *inputDeviceBox;
    QPushButton *refreshInputBtn;
    QPushButton *recordBtn;
    QPushButton *playRecordBtn;

    QString speakerStatusHint;
    QString micStatusHint;

    QAudioFormat playbackFormat;
    QAudioFormat captureFormat;
    QAudioFormat recordedFormat;
    QByteArray recordedPcm;
    bool isRecording;
    bool isPlayingTone;

    PcmMemoryDevice *playbackDevice;
    QAudioDecoder *toneDecoder = nullptr;
    QByteArray resourceDecodeBuffer;
#if EB_QT_VERSION_MAJOR >= 6
    QAudioSink *audioSink;
    QAudioSource *audioSource;
#else
    QAudioOutput *audioOutput;
    QAudioInput *audioInput;
#endif
};

#endif /* AUDIOPAGE_H */
