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

#ifndef MEDIAPLAYER_H
#define MEDIAPLAYER_H

#include <QObject>
#include <QMediaPlayer>
#include <QMediaPlaylist>

class MediaPlayer : public QObject
{
    Q_OBJECT
public:
    explicit MediaPlayer(QObject *parent = nullptr);

    void addToPlaylist(const QUrl &url);
    QVariant metaData(QMediaMetaData::Key key) const;
    qint64 duration() const;
    qint64 position() const;


public slots:
    void play();
    void pause();
    void stop();
    void setPosition(qint64 position);

signals:
    void currentMediaChanged(const QMediaContent &content);
    void durationChanged(qint64 duration);
    void positionChanged(qint64 position);


private:
    QMediaPlayer *player;
    QMediaPlaylist *playlist;
};

#endif // MEDIAPLAYER_H
