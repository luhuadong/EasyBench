#include "audiopage.h"
#include "eb_qt_compat.h"
#include "modules/audio/eb_pcm_util.h"
#include "modules/audio/pcm_memory_device.h"
#include "widgets/eb_widget_util.h"

#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QTimer>
#include <QBuffer>
#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QAudioBuffer>
#include <QAudioDecoder>
#include <QStandardPaths>
#include <QUrl>

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

constexpr char kForEliseTuneUrl[] = ":/sounds/for_elise_tune.mp3";

QString normalizeQtResourcePath(const QString &url)
{
    if (url.startsWith(QLatin1String("qrc:"))) {
        QString path = url.mid(3);
        if (!path.startsWith(QLatin1Char(':'))) {
            path.prepend(QLatin1Char(':'));
        }
        return path;
    }
    return url;
}

QUrl fileUrlIfExists(const QString &localPath)
{
    if (!QFile::exists(localPath)) {
        return QUrl();
    }
    return QUrl::fromLocalFile(QFileInfo(localPath).absoluteFilePath());
}

/** GStreamer 无法播放 qrc:/，须转为本地绝对路径的 file:// URL */
QUrl mediaUrlForResourceTone(const QString &resourceUrl)
{
    const QString qtRes = normalizeQtResourcePath(resourceUrl);
    const QString fileName = QFileInfo(qtRes).fileName();
    if (fileName.isEmpty()) {
        return QUrl();
    }

    QStringList candidates;

    if (QFile::exists(qtRes)) {
        QString cacheDir = QStandardPaths::writableLocation(QStandardPaths::CacheLocation);
        if (cacheDir.isEmpty()) {
            cacheDir = QStandardPaths::writableLocation(QStandardPaths::TempLocation);
        }
        if (!cacheDir.isEmpty()) {
            QDir().mkpath(cacheDir);
            const QString cached = cacheDir + QStringLiteral("/easybench_") + fileName;
            if (QFile::exists(cached)) {
                QFile::remove(cached);
            }
            if (QFile::copy(qtRes, cached)) {
                candidates << cached;
            }
        }
    }

#ifdef EB_PROJECT_ROOT
    candidates << QStringLiteral(EB_PROJECT_ROOT) + QStringLiteral("/resource/sounds/")
                      + fileName;
#endif
    candidates << QDir::currentPath() + QStringLiteral("/resource/sounds/") + fileName;
    if (QCoreApplication::instance()) {
        const QString appDir = QCoreApplication::applicationDirPath();
        candidates << appDir + QStringLiteral("/../resource/sounds/") + fileName;
        candidates << appDir + QStringLiteral("/../../resource/sounds/") + fileName;
    }

    for (const QString &path : candidates) {
        const QUrl url = fileUrlIfExists(path);
        if (url.isValid() && !url.toLocalFile().isEmpty()) {
            return url;
        }
    }
    return QUrl();
}

void applySmallActionButtonWidth(const QList<QPushButton *> &buttons, const QString &referenceText)
{
    if (buttons.isEmpty()) {
        return;
    }
    const QFontMetrics fm(buttons.first()->font());
    const int width = fm.horizontalAdvance(referenceText) + 32;
    for (QPushButton *btn : buttons) {
        btn->setFixedWidth(width);
        btn->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    }
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
    , micGroup(nullptr)
    , inputDeviceBox(nullptr)
    , refreshInputBtn(nullptr)
    , recordBtn(nullptr)
    , playRecordBtn(nullptr)
    , speakerStatusHint(tr("未播放"))
    , micStatusHint(tr("未录音"))
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

    updateStatusBar();
}

QString AudioPage::defaultStatusHint() const
{
    return tr("扬声器：未播放 · 麦克风：未录音");
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
    refreshOutputBtn = new QPushButton(tr("刷新"), speakerGroup);
    refreshOutputBtn->setObjectName(QStringLiteral("functionBtn_small"));

    toneBox = new QComboBox(speakerGroup);
    tonePlayBtn = new QPushButton(tr("播放"), speakerGroup);
    tonePlayBtn->setObjectName(QStringLiteral("functionBtn_small"));

    applySmallActionButtonWidth({refreshOutputBtn, tonePlayBtn}, tr("刷新"));

    volumeSlider = new QSlider(Qt::Horizontal, speakerGroup);
    volumeSlider->setRange(0, 100);
    volumeSlider->setValue(80);
    volumeValueLabel = new QLabel(QStringLiteral("80%"), speakerGroup);
    volumeValueLabel->setMinimumWidth(40);
    volumeValueLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

    auto makeFieldRow = [](QWidget *parent, QComboBox *combo, QPushButton *actionBtn) -> QWidget * {
        QWidget *rowWidget = new QWidget(parent);
        rowWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        QHBoxLayout *row = new QHBoxLayout(rowWidget);
        row->setContentsMargins(0, 0, 0, 0);
        row->setSpacing(8);
        row->setAlignment(Qt::AlignVCenter);
        EbWidget::applyComboBoxStyle(combo);
        row->addWidget(combo, 1);
        row->addWidget(actionBtn, 0, Qt::AlignVCenter);
        return rowWidget;
    };

    QWidget *outputDeviceRow = makeFieldRow(speakerGroup, outputDeviceBox, refreshOutputBtn);
    QWidget *toneRow = makeFieldRow(speakerGroup, toneBox, tonePlayBtn);

    QWidget *volumeRowWidget = new QWidget(speakerGroup);
    volumeRowWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    QHBoxLayout *volumeRow = new QHBoxLayout(volumeRowWidget);
    volumeRow->setContentsMargins(0, 0, 0, 0);
    volumeRow->setSpacing(8);
    volumeRow->setAlignment(Qt::AlignVCenter);
    volumeRow->addWidget(volumeSlider, 1);
    volumeRow->addWidget(volumeValueLabel, 0, Qt::AlignVCenter);

    QGridLayout *speakerGrid = new QGridLayout(speakerGroup);
    speakerGrid->setContentsMargins(12, 16, 12, 12);
    speakerGrid->setHorizontalSpacing(12);
    speakerGrid->setVerticalSpacing(6);
    speakerGrid->setColumnStretch(1, 1);

    auto addSpeakerRow = [&](int row, const QString &labelText, QWidget *field) {
        field->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        speakerGrid->addWidget(EbWidget::createFormLabel(speakerGroup, labelText), row, 0);
        speakerGrid->addWidget(field, row, 1);
    };

    addSpeakerRow(0, tr("输出设备"), outputDeviceRow);
    addSpeakerRow(1, tr("测试音频"), toneRow);
    addSpeakerRow(2, tr("音量"), volumeRowWidget);

    micGroup = new QGroupBox(tr("麦克风"), content);
    inputDeviceBox = new QComboBox(micGroup);
    refreshInputBtn = new QPushButton(tr("刷新"), micGroup);
    refreshInputBtn->setObjectName(QStringLiteral("functionBtn_small"));
    applySmallActionButtonWidth({refreshInputBtn}, tr("刷新"));

    recordBtn = new QPushButton(tr("开始录音 (5秒)"), micGroup);
    recordBtn->setObjectName(QStringLiteral("functionBtn_small"));
    playRecordBtn = new QPushButton(tr("回放录音"), micGroup);
    playRecordBtn->setObjectName(QStringLiteral("functionBtn_small"));
    playRecordBtn->setEnabled(false);

    QWidget *micBtnRowWidget = new QWidget(micGroup);
    micBtnRowWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    QHBoxLayout *micBtnRow = new QHBoxLayout(micBtnRowWidget);
    micBtnRow->setContentsMargins(0, 0, 0, 0);
    micBtnRow->setSpacing(8);
    micBtnRow->setAlignment(Qt::AlignVCenter);
    micBtnRow->addWidget(recordBtn);
    micBtnRow->addWidget(playRecordBtn);
    micBtnRow->addStretch();

    QWidget *inputDeviceRow = makeFieldRow(micGroup, inputDeviceBox, refreshInputBtn);

    QGridLayout *micGrid = new QGridLayout(micGroup);
    micGrid->setContentsMargins(12, 16, 12, 12);
    micGrid->setHorizontalSpacing(12);
    micGrid->setVerticalSpacing(6);
    micGrid->setColumnStretch(1, 1);

    auto addMicRow = [&](int row, const QString &labelText, QWidget *field) {
        field->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        micGrid->addWidget(EbWidget::createFormLabel(micGroup, labelText), row, 0);
        micGrid->addWidget(field, row, 1);
    };

    addMicRow(0, tr("输入设备"), inputDeviceRow);
    micGrid->addWidget(micBtnRowWidget, 1, 1);

    QVBoxLayout *pageLayout = new QVBoxLayout(content);
    pageLayout->setContentsMargins(16, 12, 16, 12);
    pageLayout->setSpacing(16);
    pageLayout->addWidget(speakerGroup);
    pageLayout->addWidget(micGroup);
    pageLayout->addStretch();

    connect(tonePlayBtn, &QPushButton::clicked, this, &AudioPage::onTonePlayBtnClicked);
    updateTonePlayButton();
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

void AudioPage::updateTonePlayButton()
{
    if (!tonePlayBtn) {
        return;
    }
    const bool busy = isPlayingTone || toneDecoder != nullptr;
    tonePlayBtn->setText(busy ? tr("停止") : tr("播放"));
}

void AudioPage::onTonePlayBtnClicked()
{
    if (isPlayingTone || toneDecoder) {
        stopPlayback();
    } else {
        playTestTone();
    }
}

void AudioPage::refreshToneList()
{
    toneBox->clear();
    const QStringList names = EbPcm::builtinToneNames();
    for (int i = 0; i < names.size(); ++i) {
        toneBox->addItem(names.at(i), i);
    }
    toneBox->addItem(tr("致爱丽丝（铃声）"), QString::fromLatin1(kForEliseTuneUrl));
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
        setSpeakerStatus(tr("未检测到音频输出设备。"));
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
        setMicStatus(tr("未检测到音频输入设备。"));
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

void AudioPage::setSpeakerStatus(const QString &text)
{
    speakerStatusHint = text;
    updateStatusBar();
}

void AudioPage::setMicStatus(const QString &text)
{
    micStatusHint = text;
    updateStatusBar();
}

void AudioPage::updateStatusBar()
{
    setStatusMessage(tr("扬声器：%1 · 麦克风：%2").arg(speakerStatusHint, micStatusHint));
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

bool AudioPage::isResourceToneSelected() const
{
    return toneBox && toneBox->currentData().type() == QVariant::String;
}

QString AudioPage::currentResourceToneUrl() const
{
    if (!isResourceToneSelected()) {
        return QString();
    }
    return toneBox->currentData().toString();
}

QByteArray AudioPage::currentTonePcm() const
{
    if (isResourceToneSelected()) {
        return QByteArray();
    }
    const int index = toneBox->currentData().toInt();
    if (index < 0) {
        return QByteArray();
    }
    return EbPcm::builtinTonePcm(index);
}

void AudioPage::stopToneDecoder()
{
    if (toneDecoder) {
        toneDecoder->stop();
        toneDecoder->deleteLater();
        toneDecoder = nullptr;
    }
    resourceDecodeBuffer.clear();
}

void AudioPage::playPcmOnOutput(const QByteArray &pcm, const QAudioFormat &format,
                              const QString &statusText)
{
    if (pcm.isEmpty()) {
        QMessageBox::information(this, tr("扬声器"), tr("无效的测试音。"));
        return;
    }

    playbackFormat = format;
    if (!ensureOutputAvailable()) {
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
    if (statusText.isEmpty()) {
        setSpeakerStatus(tr("正在通过「%1」播放：%2")
                             .arg(outputDeviceBox->currentText(), toneBox->currentText()));
    } else {
        setSpeakerStatus(statusText);
    }
    updateTonePlayButton();
}

void AudioPage::onResourceToneDecoded()
{
    QAudioDecoder *decoder = toneDecoder;
    toneDecoder = nullptr;

    if (!decoder) {
        return;
    }

    const bool decodeError = decoder->error() != QAudioDecoder::NoError;
    const QString decodeErrorText = decoder->errorString();
    const QByteArray pcm = resourceDecodeBuffer;
    resourceDecodeBuffer.clear();
    decoder->deleteLater();

    if (decodeError || pcm.isEmpty()) {
        QMessageBox::warning(
            this, tr("扬声器"),
            decodeError ? decodeErrorText : tr("铃声解码结果为空，无法播放。"));
        setSpeakerStatus(tr("播放失败"));
        updateTonePlayButton();
        return;
    }

    playPcmOnOutput(pcm, EbPcm::defaultFormat());
}

void AudioPage::playResourceTone(const QString &url)
{
    if (url.isEmpty()) {
        QMessageBox::information(this, tr("扬声器"), tr("无效的测试音。"));
        return;
    }
    const QString qtRes = normalizeQtResourcePath(url);
    if (!QFile::exists(qtRes)) {
        QMessageBox::warning(this, tr("扬声器"),
                             tr("找不到铃声资源：%1\n请确认已重新编译并包含 sounds.qrc。").arg(qtRes));
        return;
    }

    const QUrl playUrl = mediaUrlForResourceTone(url);
    const QString localFile = playUrl.toLocalFile();
    if (!playUrl.isValid() || playUrl.scheme() != QStringLiteral("file") || localFile.isEmpty()) {
        QMessageBox::warning(this, tr("扬声器"), tr("无法准备铃声文件，无法播放。"));
        return;
    }

    if (!ensureOutputAvailable()) {
        return;
    }

    stopMicRecord();
    stopSpeakerOutput();

    resourceDecodeBuffer.clear();
    toneDecoder = new QAudioDecoder(this);
    toneDecoder->setAudioFormat(EbPcm::defaultFormat());
#if EB_QT_VERSION_MAJOR >= 6
    toneDecoder->setSource(playUrl);
#else
    toneDecoder->setSourceFilename(localFile);
#endif

#if EB_QT_VERSION_MAJOR >= 6
    connect(toneDecoder, &QAudioDecoder::bufferReady, this,
            [this](const QAudioBuffer &buffer) {
                if (!buffer.isValid()) {
                    return;
                }
                resourceDecodeBuffer.append(
                    static_cast<const char *>(buffer.constData()), buffer.byteCount());
            });
#else
    connect(toneDecoder, &QAudioDecoder::bufferReady, this, [this]() {
        if (!toneDecoder) {
            return;
        }
        const QAudioBuffer buffer = toneDecoder->read();
        if (!buffer.isValid()) {
            return;
        }
        resourceDecodeBuffer.append(
            static_cast<const char *>(buffer.constData()), buffer.byteCount());
    });
#endif
    connect(toneDecoder, &QAudioDecoder::finished, this, &AudioPage::onResourceToneDecoded);
    connect(toneDecoder, QOverload<QAudioDecoder::Error>::of(&QAudioDecoder::error), this,
            [this](QAudioDecoder::Error error) {
                Q_UNUSED(error);
                if (!toneDecoder) {
                    return;
                }
                QMessageBox::warning(this, tr("扬声器"), toneDecoder->errorString());
                stopSpeakerOutput();
                setSpeakerStatus(tr("播放失败"));
                updateTonePlayButton();
            });

    setSpeakerStatus(tr("正在加载铃声…"));
    toneDecoder->start();
    updateTonePlayButton();
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
    stopToneDecoder();
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
    updateTonePlayButton();
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
    stopPlayback();
    stopMicRecord();

    if (isResourceToneSelected()) {
        playResourceTone(currentResourceToneUrl());
        return;
    }

    const QByteArray pcm = currentTonePcm();
    stopSpeakerOutput();
    playPcmOnOutput(pcm, EbPcm::defaultFormat());
}

void AudioPage::stopPlayback()
{
    stopSpeakerOutput();
    setSpeakerStatus(tr("播放已停止"));
    updateTonePlayButton();
}

void AudioPage::onPlaybackFinished()
{
    if (isPlayingTone) {
        stopSpeakerOutput();
        setSpeakerStatus(tr("播放完成"));
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
    setMicStatus(tr("正在通过「%1」录音… 最长 5 秒").arg(inputDeviceBox->currentText()));

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
        setMicStatus(tr("未录到数据，请检查麦克风权限或所选设备。"));
        playRecordBtn->setEnabled(false);
    } else {
        const int bytesPerFrame = recordedFormat.channelCount() * recordedFormat.sampleSize() / 8;
        const int ms = bytesPerFrame > 0
            ? recordedPcm.size() * 1000 / (recordedFormat.sampleRate() * bytesPerFrame)
            : 0;
        setMicStatus(tr("已通过「%1」录音约 %2 毫秒（仅内存，未保存文件）")
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

    const QAudioFormat playFormat = recordedFormat;
#if EB_QT_VERSION_MAJOR >= 6
    if (!currentOutputDevice().isFormatSupported(playFormat)) {
#else
    if (!currentOutputDevice().isFormatSupported(playFormat)) {
#endif
        QMessageBox::warning(
            this, tr("扬声器"),
            tr("当前所选扬声器不支持录音采样格式，请更换输出设备或重新录音。"));
        return;
    }

    stopSpeakerOutput();
    setMicStatus(tr("回放中…"));
    playPcmOnOutput(
        recordedPcm, playFormat,
        tr("正在通过「%1」回放麦克风录音").arg(outputDeviceBox->currentText()));
}
