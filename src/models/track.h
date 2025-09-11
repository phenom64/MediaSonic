/*
 * Core track metadata structure
 */
#ifndef MEDIASONIC_MODELS_TRACK_H
#define MEDIASONIC_MODELS_TRACK_H

#include <QString>
#include <QUrl>
#include <QImage>
#include <QMetaType>

namespace MS {

struct Track
{
    QUrl url;
    QString title;
    QString artist;
    QString album;
    QString genre;
    int year = 0;
    int trackNumber = 0;
    int discNumber = 0;
    qint64 durationMs = 0;
    int bitrateKbps = 0;
    int sampleRate = 0;
    int rating = 0; // 0..5
    int playCount = 0;
};

}

#include <QMetaType>
Q_DECLARE_METATYPE(MS::Track)

#endif // MEDIASONIC_MODELS_TRACK_H
