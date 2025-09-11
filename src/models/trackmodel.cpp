#include "models/trackmodel.h"
#include <QLocale>
#include <QTime>

using namespace MS;

TrackModel::TrackModel(QObject *parent)
    : QAbstractTableModel(parent)
{
}

int TrackModel::rowCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : m_tracks.size();
}

int TrackModel::columnCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : ColCount;
}

QVariant TrackModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= m_tracks.size()) return QVariant();
    const Track &t = m_tracks.at(index.row());
    if (role == Qt::DisplayRole) {
        switch (index.column()) {
        case ColName: return t.title.isEmpty() ? t.url.fileName() : t.title;
        case ColTime: {
            int secs = int(t.durationMs / 1000);
            return QString("%1:%2").arg(secs/60).arg(secs%60, 2, 10, QChar('0'));
        }
        case ColArtist: return t.artist;
        case ColAlbum: return t.album;
        case ColGenre: return t.genre;
        case ColRating: return t.rating;
        case ColPlays: return t.playCount;
        default: return QVariant();
        }
    } else if (role == Qt::EditRole) {
        if (index.column() == ColRating) return t.rating;
    } else if (role == Qt::UserRole) {
        // Provide raw duration and URL
        if (index.column() == ColTime) return t.durationMs;
        if (index.column() == ColName) return t.url.toString();
    }
    return QVariant();
}

QVariant TrackModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        switch (section) {
        case ColName: return QStringLiteral("Name");
        case ColTime: return QStringLiteral("Time");
        case ColArtist: return QStringLiteral("Artist");
        case ColAlbum: return QStringLiteral("Album");
        case ColGenre: return QStringLiteral("Genre");
        case ColRating: return QStringLiteral("Rating");
        case ColPlays: return QStringLiteral("Plays");
        }
    }
    return QAbstractTableModel::headerData(section, orientation, role);
}

Qt::ItemFlags TrackModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags f = QAbstractTableModel::flags(index);
    if (index.column() == ColRating) f |= Qt::ItemIsEditable;
    return f;
}

bool TrackModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid() || index.row() < 0 || index.row() >= m_tracks.size()) return false;
    Track &t = m_tracks[index.row()];
    if (role == Qt::EditRole || role == Qt::DisplayRole) {
        if (index.column() == ColRating) {
            t.rating = value.toInt();
            emit dataChanged(index, index, {Qt::DisplayRole, Qt::EditRole});
            return true;
        }
    }
    return false;
}

void TrackModel::addTrack(const Track &t)
{
    const int row = m_tracks.size();
    beginInsertRows(QModelIndex(), row, row);
    m_tracks.push_back(t);
    endInsertRows();
    emit trackAdded(t, row);
}

const Track &TrackModel::trackAt(int row) const
{
    return m_tracks.at(row);
}

Track &TrackModel::trackAtMutable(int row)
{
    return m_tracks[row];
}

QList<int> TrackModel::columnRoles() const
{
    return { Qt::DisplayRole };
}

qint64 TrackModel::totalDurationMs() const
{
    qint64 sum = 0;
    for (const auto &t : m_tracks) sum += t.durationMs;
    return sum;
}

qint64 TrackModel::totalSizeBytes() const
{
    // Placeholder until file sizes are stored; assume 5MB per track
    return qint64(m_tracks.size()) * 5 * 1024 * 1024;
}

