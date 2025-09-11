/*
 * VisualizerBridge - feeds audio probe data into visualizer bars for LCD
 */
#ifndef MEDIASONIC_VISUALIZER_BRIDGE_H
#define MEDIASONIC_VISUALIZER_BRIDGE_H

#include <QObject>
#include <QAudioProbe>
#include <QAudioBuffer>
#include <QVector>

class MediaPlayer;

namespace MS {

class VisualizerBridge : public QObject
{
    Q_OBJECT
public:
    explicit VisualizerBridge(MediaPlayer *player, QObject *parent = nullptr);

signals:
    void levelsUpdated(const QVector<float> &levels);

private slots:
    void processBuffer(const QAudioBuffer &buffer);

private:
    QAudioProbe m_probe;
    int m_bins = 32;
    void computeFFT(const float *samples, int count, QVector<float> &out);
};

}

#endif // MEDIASONIC_VISUALIZER_BRIDGE_H

