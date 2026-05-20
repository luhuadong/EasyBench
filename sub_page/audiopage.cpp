#include "audiopage.h"
#include "eb_qt_compat.h"
#include "module/audio/eb_pcm_util.h"
#include "module/audio/pcm_memory_device.h"

#include <QFormLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QTimer>
#include <QBuffer>

#if EB_QT_VERSION_MAJOR >= 6
#include <QMediaDevices>
#include <QAudioSink>
#include <QAudioSource>
Q_DECLARE_METATYPE(QAudioDevice)
#else
#include <QAudio>
#include <QAudioDeviceInfo>
#include <QAudioOutput>
#include <QAudioInput>
#endif

namespace {

QAudioFormat formatForDevice(const QAudioFormat &desired,
#if EB_QT_VERSION_MAJOR >= 6
                             const QAudioDevice &device)
#else
                             const QAudioDeviceInfo &device)
#endif
{
#if EB_QT_VERSION_MAJOR >= 6
    if (device.isFormatSupported(desired)) {
        return desired;
    }
    return device.preferredFormat();
#else
    if (device.isFormatSupported(desired)) {
        return desired;
    }
    return device.nearestFormat(desired);
#endif
}

} // namespace

AudioPage::AudioPage(EbOptions *options, QWidget *parent)
    : PageWidget(options, parent)
    , speakerGroup(nullptr)
    , outputDeviceBox(nullptr)
    , refreshOutputBtn(nullptr)
    , toneBox(nullptr)
    , volumeSlider(nullptr)
    , volumeValueLabel(nullptr)
    , speakerStatusLabel(nullptr)
    , micGroup(nullptr)
    , inputDeviceBox(nullptr)
    , refreshInputBtn(nullptr)
    , recordBtn(nullptr)
    , playRecordBtn(nullptr)
    , micStatusLabel(nullptr)
    , isRecording(false)
    , isPlayingTone(false)
    , playbackDevice(nullptr)
#if EB_QT_VERSION_MAJOR >= 6
    , audioSink(nullptr)
    , audioSource(nullptr)
#else
    , audioOutput(nullptr)
    , audioInput(nullptr)
#endif
{
    setTitleLabelText(tr("声音测试"));
    const QAudioFormat target = EbPcm::defaultFormat();
    playbackFormat = target;
    captureFormat = target;
    recordedFormat = target;

    buildUi();
    refreshToneList();
    refreshOutputDevices();
    refreshInputDevices();

#if EB_QT_VERSION_MAJOR >= 6
    audioSink = nullptr;
    audioSource = nullptr;
#else
    audioOutput = nullptr;
    audioInput = nullptr;
#endif

    setStatus(tr("就绪。请选择设备后测试；内置测试音由程序生成。"));
}

AudioPage::~AudioPage()
{
    stopMicRecord();
    stopPlayback();
}

void AudioPage::buildUi()
{
    QWidget *content = contentArea();

    speakerGroup = new QGroupBox(tr("扬声器"), content);
    outputDeviceBox = new QComboBox(speakerGroup);
    outputDeviceBox->setMinimumWidth(260);
    refreshOutputBtn = new QPushButton(tr("刷新设备"), speakerGroup);
    refreshOutputBtn->setObjectName(QStringLiteral("functionBtn_small"));

    toneBox = new QComboBox(speakerGroup);
    toneBox->setMinimumWidth(260);

    volumeSlider = new QSlider(Qt::Horizontal, speakerGroup);
    volumeSlider->setRange(0, 100);
    volumeSlider->setValue(80);
    volumeValueLabel = new QLabel(QStringLiteral("80%"), speakerGroup);
    volumeValueLabel->setMinimumWidth(40);
    volumeValueLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    speakerStatusLabel = new QLabel(tr("未播放"), speakerGroup);
    speakerStatusLabel->setWordWrap(true);

    QPushButton *playBtn = new QPushButton(tr("播放测试音"), speakerGroup);
    playBtn->setObjectName(QStringLiteral("functionBtn_small"));
    QPushButton *stopBtn = new QPushButton(tr("停止"), speakerGroup);
    stopBtn->setObjectName(QStringLiteral("functionBtn_small"));

    QHBoxLayout *speakerBtnRow = new QHBoxLayout;
    speakerBtnRow->addWidget(playBtn);
    speakerBtnRow->addWidget(stopBtn);
    speakerBtnRow->addStretch();

    QHBoxLayout *outputDeviceRow = new QHBoxLayout;
    outputDeviceRow->setContentsMargins(0, 0, 0, 0);
    outputDeviceRow->setSpacing(8);
    outputDeviceRow->addWidget(outputDeviceBox, 1);
    outputDeviceRow->addWidget(refreshOutputBtn);

    QHBoxLayout *volumeRow = new QHBoxLayout;
    volumeRow->setContentsMargins(0, 0, 0, 0);
    volumeRow->setSpacing(8);
    volumeRow->addWidget(volumeSlider, 1);
    volumeRow->addWidget(volumeValueLabel);

    QFormLayout *speakerForm = new QFormLayout;
    speakerForm->setContentsMargins(12, 16, 12, 12);
    speakerForm->addRow(tr("输出设备"), outputDeviceRow);
    speakerForm->addRow(tr("测试音"), toneBox);
    speakerForm->addRow(tr("音量"), volumeRow);
    speakerForm->addRow(tr("状态"), speakerStatusLabel);
    speakerForm->addRow(QString(), speakerBtnRow);
    speakerGroup->setLayout(speakerForm);

    micGroup = new QGroupBox(tr("麦克风"), content);
    inputDeviceBox = new QComboBox(micGroup);
    inputDeviceBox->setMinimumWidth(260);
    refreshInputBtn = new QPushButton(tr("刷新设备"), micGroup);
    refreshInputBtn->setObjectName(QStringLiteral("functionBtn_small"));

    recordBtn = new QPushButton(tr("开始录音 (5秒)"), micGroup);
    recordBtn->setObjectName(QStringLiteral("functionBtn_small"));
    playRecordBtn = new QPushButton(tr("回放录音"), micGroup);
    playRecordBtn->setObjectName(QStringLiteral("functionBtn_small"));
    playRecordBtn->setEnabled(false);
    micStatusLabel = new QLabel(tr("未录音"), micGroup);
    micStatusLabel->setWordWrap(true);

    QHBoxLayout *micBtnRow = new QHBoxLayout;
    micBtnRow->addWidget(recordBtn);
    micBtnRow->addWidget(playRecordBtn);
    micBtnRow->addStretch();

    QHBoxLayout *inputDeviceRow = new QHBoxLayout;
    inputDeviceRow->setContentsMargins(0, 0, 0, 0);
    inputDeviceRow->setSpacing(8);
    inputDeviceRow->addWidget(inputDeviceBox, 1);
    inputDeviceRow->addWidget(refreshInputBtn);

    QFormLayout *micForm = new QFormLayout;
    micForm->setContentsMargins(12, 16, 12, 12);
    micForm->addRow(tr("输入设备"), inputDeviceRow);
    micForm->addRow(QString(), micBtnRow);
    micForm->addRow(tr("状态"), micStatusLabel);
    micGroup->setLayout(micForm);

    QVBoxLayout *pageLayout = new QVBoxLayout(content);
    pageLayout->setContentsMargins(16, 12, 16, 12);
    pageLayout->setSpacing(16);
    pageLayout->addWidget(speakerGroup);
    pageLayout->addWidget(micGroup);
    pageLayout->addStretch();

    connect(playBtn, &QPushButton::clicked, this, &AudioPage::playTestTone);
    connect(stopBtn, &QPushButton::clicked, this, &AudioPage::stopPlayback);
    connect(volumeSlider, &QSlider::valueChanged, this, &AudioPage::onVolumeChanged);
    connect(recordBtn, &QPushButton::clicked, this, &AudioPage::startMicRecord);
    connect(playRecordBtn, &QPushButton::clicked, this, &AudioPage::playMicRecording);
    connect(refreshOutputBtn, &QPushButton::clicked, this, &AudioPage::refreshOutputDevices);
    connect(refreshInputBtn, &QPushButton::clicked, this, &AudioPage::refreshInputDevices);
    connect(outputDeviceBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &AudioPage::onOutputDeviceChanged);
    connect(inputDeviceBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &AudioPage::onInputDeviceChanged);
}

void AudioPage::refreshToneList()
{
    toneBox->clear();
    const QStringList names = EbPcm::builtinToneNames();
    for (int i = 0; i < names.size(); ++i) {
        toneBox->addItem(names.at(i), i);
    }
}

void AudioPage::refreshOutputDevices()
{
    const int previousIndex = outputDeviceBox->currentIndex();
    outputDeviceBox->blockSignals(true);
    outputDeviceBox->clear();

#if EB_QT_VERSION_MAJOR >= 6
    const QAudioDevice defaultDevice = QMediaDevices::defaultAudioOutput();
    const QList<QAudioDevice> devices = QMediaDevices::audioOutputs();
    for (const QAudioDevice &device : devices) {
        QString label = device.description();
        if (!defaultDevice.isNull() && device.id() == defaultDevice.id()) {
            label += tr(" (系统默认)");
        }
        outputDeviceBox->addItem(label, QVariant::fromValue(device));
    }
#else
    const QAudioDeviceInfo defaultDevice = QAudioDeviceInfo::defaultOutputDevice();
    const QList<QAudioDeviceInfo> devices =
        QAudioDeviceInfo::availableDevices(QAudio::AudioOutput);
    for (const QAudioDeviceInfo &device : devices) {
        QString label = device.deviceName();
        if (device == defaultDevice) {
            label += tr(" (系统默认)");
        }
        outputDeviceBox->addItem(label, QVariant::fromValue(device));
    }
#endif

    outputDeviceBox->blockSignals(false);

    if (outputDeviceBox->count() == 0) {
        setStatus(tr("未检测到音频输出设备。"));
        return;
    }

    if (previousIndex >= 0 && previousIndex < outputDeviceBox->count()) {
        outputDeviceBox->setCurrentIndex(previousIndex);
    } else {
        outputDeviceBox->setCurrentIndex(0);
    }
    onOutputDeviceChanged(outputDeviceBox->currentIndex());
}

void AudioPage::refreshInputDevices()
{
    const int previousIndex = inputDeviceBox->currentIndex();
    inputDeviceBox->blockSignals(true);
    inputDeviceBox->clear();

#if EB_QT_VERSION_MAJOR >= 6
    const QAudioDevice defaultDevice = QMediaDevices::defaultAudioInput();
    const QList<QAudioDevice> devices = QMediaDevices::audioInputs();
    for (const QAudioDevice &device : devices) {
        QString label = device.description();
        if (!defaultDevice.isNull() && device.id() == defaultDevice.id()) {
            label += tr(" (系统默认)");
        }
        inputDeviceBox->addItem(label, QVariant::fromValue(device));
    }
#else
    const QAudioDeviceInfo defaultDevice = QAudioDeviceInfo::defaultInputDevice();
    const QList<QAudioDeviceInfo> devices =
        QAudioDeviceInfo::availableDevices(QAudio::AudioInput);
    for (const QAudioDeviceInfo &device : devices) {
        QString label = device.deviceName();
        if (device == defaultDevice) {
            label += tr(" (系统默认)");
        }
        inputDeviceBox->addItem(label, QVariant::fromValue(device));
    }
#endif

    inputDeviceBox->blockSignals(false);

    if (inputDeviceBox->count() == 0) {
        micStatusLabel->setText(tr("未检测到音频输入设备。"));
        return;
    }

    if (previousIndex >= 0 && previousIndex < inputDeviceBox->count()) {
        inputDeviceBox->setCurrentIndex(previousIndex);
    } else {
        inputDeviceBox->setCurrentIndex(0);
    }
    onInputDeviceChanged(inputDeviceBox->currentIndex());
}

void AudioPage::onOutputDeviceChanged(int index)
{
    Q_UNUSED(index);
    updatePlaybackFormat();
}

void AudioPage::onInputDeviceChanged(int index)
{
    Q_UNUSED(index);
    updateCaptureFormat();
}

void AudioPage::updatePlaybackFormat()
{
    /* 内置测试音按 EbPcm::defaultFormat() 生成，播放格式须与其一致 */
    playbackFormat = EbPcm::defaultFormat();
}

void AudioPage::updateCaptureFormat()
{
#if EB_QT_VERSION_MAJOR >= 6
    const QAudioDevice device = currentInputDevice();
    if (device.isNull()) {
        return;
    }
    captureFormat = formatForDevice(EbPcm::defaultFormat(), device);
#else
    const QAudioDeviceInfo device = currentInputDevice();
    if (device.isNull()) {
        return;
    }
    captureFormat = formatForDevice(EbPcm::defaultFormat(), device);
#endif
}

#if EB_QT_VERSION_MAJOR >= 6
QAudioDevice AudioPage::currentOutputDevice() const
{
    if (outputDeviceBox->currentIndex() < 0) {
        return QAudioDevice();
    }
    return outputDeviceBox->currentData().value<QAudioDevice>();
}

QAudioDevice AudioPage::currentInputDevice() const
{
    if (inputDeviceBox->currentIndex() < 0) {
        return QAudioDevice();
    }
    return inputDeviceBox->currentData().value<QAudioDevice>();
}
#else
QAudioDeviceInfo AudioPage::currentOutputDevice() const
{
    if (outputDeviceBox->currentIndex() < 0) {
        return QAudioDeviceInfo();
    }
    return outputDeviceBox->currentData().value<QAudioDeviceInfo>();
}

QAudioDeviceInfo AudioPage::currentInputDevice() const
{
    if (inputDeviceBox->currentIndex() < 0) {
        return QAudioDeviceInfo();
    }
    return inputDeviceBox->currentData().value<QAudioDeviceInfo>();
}
#endif

void AudioPage::setStatus(const QString &text)
{
    speakerStatusLabel->setText(text);
}

bool AudioPage::ensureOutputAvailable()
{
    const auto device = currentOutputDevice();
    if (device.isNull()) {
        QMessageBox::warning(this, tr("扬声器"), tr("请先选择有效的音频输出设备。"));
        return false;
    }
    updatePlaybackFormat();
#if EB_QT_VERSION_MAJOR >= 6
    if (!device.isFormatSupported(playbackFormat)) {
#else
    if (!device.isFormatSupported(playbackFormat)) {
#endif
        QMessageBox::warning(
            this, tr("扬声器"),
            tr("所选扬声器不支持程序测试音格式（%1 Hz / %2 声道），请更换设备。")
                .arg(playbackFormat.sampleRate())
                .arg(playbackFormat.channelCount()));
        return false;
    }
    return true;
}

bool AudioPage::ensureInputAvailable()
{
#if EB_QT_VERSION_MAJOR >= 6
    if (currentInputDevice().isNull()) {
#else
    if (currentInputDevice().isNull()) {
#endif
        QMessageBox::warning(this, tr("麦克风"), tr("请先选择有效的音频输入设备。"));
        return false;
    }
    updateCaptureFormat();
    return true;
}

QByteArray AudioPage::currentTonePcm() const
{
    const int index = toneBox->currentData().toInt();
    if (index < 0) {
        return QByteArray();
    }
    return EbPcm::builtinTonePcm(index);
}

void AudioPage::onVolumeChanged(int value)
{
    volumeValueLabel->setText(QStringLiteral("%1%").arg(value));
#if EB_QT_VERSION_MAJOR >= 6
    if (audioSink) {
        audioSink->setVolume(value / 100.0f);
    }
#else
    if (audioOutput) {
        audioOutput->setVolume(value / 100.0);
    }
#endif
}

void AudioPage::stopSpeakerOutput()
{
#if EB_QT_VERSION_MAJOR >= 6
    if (audioSink) {
        audioSink->stop();
        audioSink->deleteLater();
        audioSink = nullptr;
    }
#else
    if (audioOutput) {
        audioOutput->stop();
        audioOutput->deleteLater();
        audioOutput = nullptr;
    }
#endif
    if (playbackDevice) {
        playbackDevice->close();
        playbackDevice->deleteLater();
        playbackDevice = nullptr;
    }
    isPlayingTone = false;
}

void AudioPage::stopMicInput()
{
    if (!isRecording) {
        return;
    }
#if EB_QT_VERSION_MAJOR >= 6
    if (audioSource) {
        audioSource->stop();
    }
#else
    if (audioInput) {
        audioInput->stop();
    }
#endif
    isRecording = false;
    recordBtn->setText(tr("开始录音 (5秒)"));
}

void AudioPage::playTestTone()
{
    if (!ensureOutputAvailable()) {
        return;
    }

    stopPlayback();
    stopMicRecord();

    const QByteArray pcm = currentTonePcm();
    if (pcm.isEmpty()) {
        QMessageBox::information(this, tr("扬声器"), tr("无效的测试音。"));
        return;
    }

    playbackDevice = new PcmMemoryDevice(pcm, this);
    playbackDevice->open(QIODevice::ReadOnly);

    const qreal volume = volumeSlider->value() / 100.0;

#if EB_QT_VERSION_MAJOR >= 6
    const QAudioDevice device = currentOutputDevice();
    audioSink = new QAudioSink(device, playbackFormat, this);
    audioSink->setVolume(static_cast<float>(volume));
    connect(audioSink, &QAudioSink::stateChanged, this, [this](QAudio::State state) {
        if (state == QAudio::IdleState && isPlayingTone) {
            onPlaybackFinished();
        }
    });
    audioSink->start(playbackDevice);
#else
    audioOutput = new QAudioOutput(currentOutputDevice(), playbackFormat, this);
    audioOutput->setVolume(volume);
    connect(audioOutput, &QAudioOutput::stateChanged, this, [this](QAudio::State state) {
        if (state == QAudio::IdleState && isPlayingTone) {
            onPlaybackFinished();
        }
    });
    audioOutput->start(playbackDevice);
#endif

    isPlayingTone = true;
    setStatus(tr("正在通过「%1」播放：%2")
                  .arg(outputDeviceBox->currentText(), toneBox->currentText()));
}

void AudioPage::stopPlayback()
{
    stopSpeakerOutput();
    setStatus(tr("播放已停止"));
}

void AudioPage::onPlaybackFinished()
{
    if (isPlayingTone) {
        stopSpeakerOutput();
        setStatus(tr("播放完成"));
    }
}

void AudioPage::startMicRecord()
{
    if (isRecording) {
        stopMicRecord();
        return;
    }

    if (!ensureInputAvailable()) {
        return;
    }

    stopPlayback();

    recordedPcm.clear();
    recordedFormat = captureFormat;
    QBuffer *buffer = new QBuffer(&recordedPcm, this);
    buffer->open(QIODevice::WriteOnly);

#if EB_QT_VERSION_MAJOR >= 6
    audioSource = new QAudioSource(currentInputDevice(), captureFormat, this);
    audioSource->start(buffer);
#else
    audioInput = new QAudioInput(currentInputDevice(), captureFormat, this);
    audioInput->start(buffer);
#endif

    isRecording = true;
    recordBtn->setText(tr("停止录音"));
    playRecordBtn->setEnabled(false);
    micStatusLabel->setText(tr("正在通过「%1」录音… 最长 5 秒").arg(inputDeviceBox->currentText()));

    QTimer::singleShot(5000, this, &AudioPage::stopMicRecord);
}

void AudioPage::stopMicRecord()
{
    if (!isRecording) {
        return;
    }

#if EB_QT_VERSION_MAJOR >= 6
    if (audioSource) {
        audioSource->stop();
        audioSource->deleteLater();
        audioSource = nullptr;
    }
#else
    if (audioInput) {
        audioInput->stop();
        audioInput->deleteLater();
        audioInput = nullptr;
    }
#endif

    isRecording = false;
    recordBtn->setText(tr("开始录音 (5秒)"));

    if (recordedPcm.isEmpty()) {
        micStatusLabel->setText(tr("未录到数据，请检查麦克风权限或所选设备。"));
        playRecordBtn->setEnabled(false);
    } else {
        const int bytesPerFrame = recordedFormat.channelCount() * recordedFormat.sampleSize() / 8;
        const int ms = bytesPerFrame > 0
            ? recordedPcm.size() * 1000 / (recordedFormat.sampleRate() * bytesPerFrame)
            : 0;
        micStatusLabel->setText(
            tr("已通过「%1」录音约 %2 毫秒（仅内存，未保存文件）")
                .arg(inputDeviceBox->currentText())
                .arg(ms));
        playRecordBtn->setEnabled(true);
    }
}

void AudioPage::playMicRecording()
{
    if (recordedPcm.isEmpty()) {
        QMessageBox::information(this, tr("麦克风"), tr("请先录音。"));
        return;
    }
    if (!ensureOutputAvailable()) {
        return;
    }

    stopPlayback();
    stopMicRecord();

    playbackDevice = new PcmMemoryDevice(recordedPcm, this);
    playbackDevice->open(QIODevice::ReadOnly);

    const qreal volume = volumeSlider->value() / 100.0;
    const QAudioFormat playFormat = recordedFormat;

#if EB_QT_VERSION_MAJOR >= 6
    const QAudioDevice device = currentOutputDevice();
    if (!device.isFormatSupported(playFormat)) {
        QMessageBox::warning(
            this, tr("扬声器"),
            tr("当前所选扬声器不支持录音采样格式，请更换输出设备或重新录音。"));
        playbackDevice->deleteLater();
        playbackDevice = nullptr;
        return;
    }
    audioSink = new QAudioSink(device, playFormat, this);
    audioSink->setVolume(static_cast<float>(volume));
    connect(audioSink, &QAudioSink::stateChanged, this, [this](QAudio::State state) {
        if (state == QAudio::IdleState && isPlayingTone) {
            onPlaybackFinished();
        }
    });
    audioSink->start(playbackDevice);
#else
    const QAudioDeviceInfo device = currentOutputDevice();
    if (!device.isFormatSupported(playFormat)) {
        QMessageBox::warning(
            this, tr("扬声器"),
            tr("当前所选扬声器不支持录音采样格式，请更换输出设备或重新录音。"));
        playbackDevice->deleteLater();
        playbackDevice = nullptr;
        return;
    }
    audioOutput = new QAudioOutput(device, playFormat, this);
    audioOutput->setVolume(volume);
    connect(audioOutput, &QAudioOutput::stateChanged, this, [this](QAudio::State state) {
        if (state == QAudio::IdleState && isPlayingTone) {
            onPlaybackFinished();
        }
    });
    audioOutput->start(playbackDevice);
#endif

    isPlayingTone = true;
    setStatus(tr("正在通过「%1」回放麦克风录音").arg(outputDeviceBox->currentText()));
    micStatusLabel->setText(tr("回放中…"));
}
