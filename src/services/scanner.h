/*
 * Scanner - asynchronous library scanner with optional TagLib metadata
 */
#ifndef MEDIASONIC_SERVICES_SCANNER_H
#define MEDIASONIC_SERVICES_SCANNER_H

#include <QObject>
#include <QString>
#include <QFutureWatcher>
#include <QDirIterator>
#include "models/track.h"

namespace MS {

class Scanner : public QObject
{
    Q_OBJECT
public:
    explicit Scanner(QObject *parent = nullptr);

    void scanDirectory(const QString &path);
    void scanPaths(const QStringList &paths);
    void cancel();

signals:
    void trackDiscovered(const MS::Track &track);
    void finished();

private:
    bool m_cancelled = false;
};

}

#endif // MEDIASONIC_SERVICES_SCANNER_H
