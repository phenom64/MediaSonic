/*
 * TrackModel - strongly-typed table model for tracks
 */
#ifndef MEDIASONIC_MODELS_TRACKMODEL_H
#define MEDIASONIC_MODELS_TRACKMODEL_H

#include <QAbstractTableModel>
#include <QVector>
#include <QHash>
#include <QByteArray>
#include "models/track.h"

namespace MS {

class TrackModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    enum Columns {
        ColName = 0,
        ColTime,
        ColArtist,
        ColAlbum,
        ColGenre,
        ColRating,
        ColPlays,
        ColCount
    };

    explicit TrackModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role) override;

    void addTrack(const Track &t);
    const Track &trackAt(int row) const;
    Track &trackAtMutable(int row);
    QList<int> columnRoles() const;

    qint64 totalDurationMs() const;
    qint64 totalSizeBytes() const; // placeholder until we store file size

signals:
    void trackAdded(const MS::Track &track, int row);

private:
    QVector<Track> m_tracks;
};

}

#endif // MEDIASONIC_MODELS_TRACKMODEL_H

