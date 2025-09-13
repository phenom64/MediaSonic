/*
 * TM & (C) 2025 Syndromatic Ltd. All rights reserved.
 * Designed by Kavish Krishnakumar in Manchester.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 */

#include "application.h"
#include "mainwindow.h"
#include <QStylePlugin>
#include <QPluginLoader>
#include <QStyleFactory>
#include <QFontDatabase>
#include <QDir>
#include <KLocalizedString>
#include "models/track.h"
#include <QDateTime>
#include <QTextStream>
#include <QFile>
#include <QStandardPaths>
#include <QDir>

static void msMessageHandler(QtMsgType type, const QMessageLogContext &ctx, const QString &msg)
{
    const char *typeStr = "INFO";
    switch (type) {
    case QtDebugMsg: typeStr = "DEBUG"; break;
    case QtInfoMsg: typeStr = "INFO"; break;
    case QtWarningMsg: typeStr = "WARN"; break;
    case QtCriticalMsg: typeStr = "CRIT"; break;
    case QtFatalMsg: typeStr = "FATAL"; break;
    }
    QString where;
    if (ctx.file)
        where = QString::fromUtf8(ctx.file) + ":" + QString::number(ctx.line);
    QTextStream(stderr) << QDateTime::currentDateTime().toString(Qt::ISODate) << " [" << typeStr
                        << "] " << where << ": " << msg << '\n';
    if (type == QtFatalMsg)
        abort();
}

int main(int argc, char *argv[])
{
    qInstallMessageHandler(msMessageHandler);
    qRegisterMetaType<MS::Track>("MS::Track");
    qRegisterMetaType<QVector<float>>("QVector<float>");
    Application app(argc, argv);
    KLocalizedString::setApplicationDomain("mediasonic");

    // Load DS-Digital font for LCD display. Try embedded first, then local build dir fallback
    int fontId = QFontDatabase::addApplicationFont(":/fonts/fonts/DS-DIGII.TTF");
    if (fontId == -1) {
        QString local1 = QCoreApplication::applicationDirPath() + "/fonts/DS-DIGII.TTF";
        fontId = QFontDatabase::addApplicationFont(local1);
    }
    if (fontId == -1) {
        QString local2 = QCoreApplication::applicationDirPath() + "/../share/mediasonic/fonts/DS-DIGII.TTF";
        fontId = QFontDatabase::addApplicationFont(local2);
    }
    if (fontId != -1) {
        const QStringList fams = QFontDatabase::applicationFontFamilies(fontId);
        if (!fams.isEmpty()) qApp->setProperty("ms.lcdfamily", fams.first());
    } else {
        qDebug() << "Failed to load DS-Digital font, using system fallback";
    }

    // Apply Atmo NSE style if available (pretty everywhere; native on SynOS)
    try {
        // Allow users to opt-out via env var
        if (qEnvironmentVariable("MS_NO_NSE").isEmpty()) {
            // Try to load the Atmo style plugin dynamically from the build tree
            QString base = QCoreApplication::applicationDirPath() + "/../NSE/atmo";
            QStringList candidates;
#if defined(Q_OS_WIN)
            candidates << base + "/atmo.dll";
#elif defined(Q_OS_MAC)
            candidates << base + "/libatmo.dylib";
#else
            candidates << base + "/libatmo.so";
#endif
            bool styled = false;
            for (const QString &lib : candidates) {
                if (!QFile::exists(lib)) continue;
                QPluginLoader loader(lib);
                QObject *inst = loader.instance();
                if (!inst) continue;
                if (QStylePlugin *sp = qobject_cast<QStylePlugin*>(inst)) {
                    if (QStyle *s = sp->create(QStringLiteral("Atmo"))) {
                        qApp->setStyle(s);
                        styled = true;
                        break;
                    }
                }
            }
            if (!styled) {
                // Fallback to factory if plugin is installed in a standard path
                if (QStyle *s = QStyleFactory::create(QStringLiteral("Atmo"))) {
                    qApp->setStyle(s);
                }
            }
        }
        // Ensure a config is present for NSE so the look matches expectations
        const QString userDir = QDir::homePath() + "/.config/NSE";
        const QString userConf = userDir + "/NSE.conf";
        if (!QFile::exists(userConf)) {
            QDir().mkpath(userDir);
            // Try to copy from build path (NSE/atmo) or the fetched source dir
            const QString buildNSE = QCoreApplication::applicationDirPath() + "/../NSE/atmo/NSE.conf";
            QString sourceDir;
#ifdef MS_ATMO_SOURCE_DIR
            sourceDir = QStringLiteral(MS_ATMO_SOURCE_DIR);
#endif
            const QString srcNSE = sourceDir.isEmpty() ? QString() : (sourceDir + "/NSE.conf");
            const QString fallback = QStringLiteral("%1/NSE.conf").arg(QStringLiteral("/home/phenom/Atmo-Desktop"));
            QString src;
            if (QFile::exists(buildNSE)) src = buildNSE;
            else if (!srcNSE.isEmpty() && QFile::exists(srcNSE)) src = srcNSE;
            else if (QFile::exists(fallback)) src = fallback;
            if (!src.isEmpty()) QFile::copy(src, userConf);
        }
    } catch (...) {
        // Fallback silently to platform default style
    }

    // Ensure only one instance of the application is running
    if (app.isRunning()) {
        return 0;
    }

    MainWindow win;
    win.show();

    try {
        return app.exec();
    } catch (const std::exception &e) {
        qCritical() << "Unhandled exception:" << e.what();
        return 1;
    } catch (...) {
        qCritical() << "Unhandled unknown exception";
        return 2;
    }
}
