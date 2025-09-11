#include "services/scanner.h"
#include <QtConcurrent>
#include <QFileInfo>
#include <QDebug>

#ifdef HAVE_TAGLIB
#include <taglib/fileref.h>
#include <taglib/tag.h>
#include <taglib/audioproperties.h>
#endif

using namespace MS;

Scanner::Scanner(QObject *parent)
    : QObject(parent)
{
}

void Scanner::cancel()
{
    m_cancelled = true;
}

void Scanner::scanDirectory(const QString &path)
{
    m_cancelled = false;
    // Run on a background thread to avoid blocking UI
    QtConcurrent::run([this, path]() {
        qInfo() << "Scanner: scanning directory" << path;
        QDirIterator it(path, QStringList() << "*.mp3" << "*.flac" << "*.m4a" << "*.wav" << "*.ogg" << "*.aac" << "*.opus" << "*.aiff" << "*.wma",
                         QDir::Files, QDirIterator::Subdirectories);
        while (it.hasNext() && !m_cancelled) {
            const QString filePath = it.next();
            Track t; t.url = QUrl::fromLocalFile(filePath);
#ifdef HAVE_TAGLIB
            try {
                TagLib::FileRef f(filePath.toUtf8().constData());
                if (!f.isNull()) {
                    if (f.tag()) {
                        t.title = QString::fromUtf8(f.tag()->title().toCString(true));
                        t.artist = QString::fromUtf8(f.tag()->artist().toCString(true));
                        t.album = QString::fromUtf8(f.tag()->album().toCString(true));
                        t.genre = QString::fromUtf8(f.tag()->genre().toCString(true));
                        t.year = int(f.tag()->year());
                        t.trackNumber = int(f.tag()->track());
                    }
                    if (f.audioProperties()) {
                        t.durationMs = qint64(f.audioProperties()->length()) * 1000;
                        t.bitrateKbps = f.audioProperties()->bitrate();
                        t.sampleRate = f.audioProperties()->sampleRate();
                    }
                }
            } catch (...) {
                // Fallbacks below
            }
#endif
            if (t.title.isEmpty()) {
                QFileInfo fi(filePath);
                t.title = fi.completeBaseName();
            }
            emit trackDiscovered(t);
        }
        qInfo() << "Scanner: finished" << path;
        emit finished();
    });
}

void Scanner::scanPaths(const QStringList &paths)
{
    m_cancelled = false;
    const QStringList items = paths; // copy for lambda capture
    QtConcurrent::run([this, items]() {
        qInfo() << "Scanner: scanning paths" << items;
        for (const QString &p : items) {
            if (m_cancelled) break;
            QFileInfo fi(p);
            if (!fi.exists()) continue;
            if (fi.isDir()) {
                QDirIterator it(p, QStringList() << "*.mp3" << "*.flac" << "*.m4a" << "*.wav" << "*.ogg" << "*.aac" << "*.opus" << "*.aiff" << "*.wma",
                                QDir::Files, QDirIterator::Subdirectories);
                while (it.hasNext() && !m_cancelled) {
                    const QString filePath = it.next();
                    Track t; t.url = QUrl::fromLocalFile(filePath);
#ifdef HAVE_TAGLIB
                    try {
                        TagLib::FileRef f(filePath.toUtf8().constData());
                        if (!f.isNull()) {
                            if (f.tag()) {
                                t.title = QString::fromUtf8(f.tag()->title().toCString(true));
                                t.artist = QString::fromUtf8(f.tag()->artist().toCString(true));
                                t.album = QString::fromUtf8(f.tag()->album().toCString(true));
                                t.genre = QString::fromUtf8(f.tag()->genre().toCString(true));
                                t.year = int(f.tag()->year());
                                t.trackNumber = int(f.tag()->track());
                            }
                            if (f.audioProperties()) {
                                t.durationMs = qint64(f.audioProperties()->length()) * 1000;
                                t.bitrateKbps = f.audioProperties()->bitrate();
                                t.sampleRate = f.audioProperties()->sampleRate();
                            }
                        }
                    } catch (...) {}
#endif
                    if (t.title.isEmpty()) t.title = QFileInfo(filePath).completeBaseName();
                    emit trackDiscovered(t);
                }
            } else if (fi.isFile()) {
                const QString filePath = fi.absoluteFilePath();
                Track t; t.url = QUrl::fromLocalFile(filePath);
#ifdef HAVE_TAGLIB
                try {
                    TagLib::FileRef f(filePath.toUtf8().constData());
                    if (!f.isNull()) {
                        if (f.tag()) {
                            t.title = QString::fromUtf8(f.tag()->title().toCString(true));
                            t.artist = QString::fromUtf8(f.tag()->artist().toCString(true));
                            t.album = QString::fromUtf8(f.tag()->album().toCString(true));
                            t.genre = QString::fromUtf8(f.tag()->genre().toCString(true));
                            t.year = int(f.tag()->year());
                            t.trackNumber = int(f.tag()->track());
                        }
                        if (f.audioProperties()) {
                            t.durationMs = qint64(f.audioProperties()->length()) * 1000;
                            t.bitrateKbps = f.audioProperties()->bitrate();
                            t.sampleRate = f.audioProperties()->sampleRate();
                        }
                    }
                } catch (...) {}
#endif
                if (t.title.isEmpty()) t.title = fi.completeBaseName();
                emit trackDiscovered(t);
            }
        }
        qInfo() << "Scanner: finished paths";
        emit finished();
    });
}
