#include "visualizer/visualizerbridge.h"
#include "mediaplayer.h"
#include <QtMath>
#include <QMediaObject>
#include <complex>
#include <cmath>
#include <algorithm>

using namespace MS;

namespace {
template<typename T>
inline float sampleToFloat(T v) { return 0.0f; }

template<>
inline float sampleToFloat<qint16>(qint16 v) { return float(v) / 32768.0f; }

template<>
inline float sampleToFloat<qint8>(qint8 v) { return float(v) / 128.0f; }

template<>
inline float sampleToFloat<float>(float v) { return v; }
}

VisualizerBridge::VisualizerBridge(MediaPlayer *player, QObject *parent)
    : QObject(parent)
{
    if (player && player->backend()) {
        m_probeOk = m_probe.setSource(player->backend());
    }
    connect(&m_probe, &QAudioProbe::audioBufferProbed, this, &VisualizerBridge::processBuffer);

    if (!m_probeOk) {
        // Fallback animated levels to avoid blank visualizer on backends without probe support
        m_fallbackTimer = new QTimer(this);
        connect(m_fallbackTimer, &QTimer::timeout, this, [this]() {
            QVector<float> bins(m_bins);
            static float phase = 0.0f; phase += 0.08f;
            for (int i = 0; i < m_bins; ++i) {
                float v = 0.5f + 0.5f * std::sin(phase + i * 0.3f);
                bins[i] = v;
            }
            emit levelsUpdated(bins);
        });
        m_fallbackTimer->start(33);
    }
}

void VisualizerBridge::processBuffer(const QAudioBuffer &buffer)
{
    if (!buffer.isValid()) return;
    const auto fmt = buffer.format();
    const int channels = fmt.channelCount();
    const int frames = buffer.frameCount();
    QVector<float> mono; mono.reserve(frames);
    auto read = [&](auto *ptr){
        for (int i = 0; i < frames; ++i) {
            float sum = 0.0f;
            for (int c = 0; c < channels; ++c) sum += sampleToFloat<std::remove_pointer_t<decltype(ptr)>>(ptr[i*channels+c]);
            mono.push_back(sum / channels);
        }
    };
    switch (fmt.sampleType()) {
    case QAudioFormat::SignedInt:
        if (fmt.sampleSize() == 16)
            read(buffer.constData<qint16>());
        else if (fmt.sampleSize() == 8)
            read(buffer.constData<qint8>());
        break;
    case QAudioFormat::Float:
        read(buffer.constData<float>());
        break;
    default:
        return;
    }

    QVector<float> bins(m_bins);
    computeFFT(mono.constData(), mono.size(), bins);
    emit levelsUpdated(bins);
}

// Simple radix-2 FFT magnitude computation into fixed number of bins
void VisualizerBridge::computeFFT(const float *samples, int count, QVector<float> &out)
{
    if (count <= 0) return;
    int N = 1; while (N < count) N <<= 1; // Next power of two
    QVector<std::complex<float>> a(N);
    for (int i = 0; i < count; ++i) a[i] = std::complex<float>(samples[i], 0.0f);
    for (int i = count; i < N; ++i) a[i] = {0.0f, 0.0f};

    // Bit-reversal permutation
    int j = 0;
    for (int i = 1; i < N; ++i) {
        int bit = N >> 1;
        for (; j & bit; bit >>= 1) j ^= bit;
        j ^= bit;
        if (i < j) std::swap(a[i], a[j]);
    }
    for (int len = 2; len <= N; len <<= 1) {
        float ang = -2.0f * float(M_PI) / len;
        std::complex<float> wlen(cosf(ang), sinf(ang));
        for (int i = 0; i < N; i += len) {
            std::complex<float> w(1.0f, 0.0f);
            for (int k = 0; k < len/2; ++k) {
                std::complex<float> u = a[i+k];
                std::complex<float> v = a[i+k+len/2] * w;
                a[i+k] = u + v;
                a[i+k+len/2] = u - v;
                w *= wlen;
            }
        }
    }
    int half = N / 2;
    out.fill(0.0f);
    for (int i = 0; i < half; ++i) {
        float mag = std::abs(a[i]) / (N/2);
        int bin = int(float(i) / half * out.size());
        if (bin >= 0 && bin < out.size()) out[bin] = std::max(out[bin], mag);
    }
    for (int i = 0; i < out.size(); ++i) {
        out[i] = qBound(0.0f, sqrtf(out[i]) * 1.5f, 1.0f);
    }
}
