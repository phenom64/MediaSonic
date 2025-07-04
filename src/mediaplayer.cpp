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

#include "mediaplayer.h"

MediaPlayer::MediaPlayer(QObject *parent) : QObject(parent)
{
    player = new QMediaPlayer(this);
    playlist = new QMediaPlaylist(this);
    player->setPlaylist(playlist);

    connect(player, &QMediaPlayer::currentMediaChanged, this, &MediaPlayer::currentMediaChanged);
    connect(player, &QMediaPlayer::durationChanged, this, &MediaPlayer::durationChanged);
    connect(player, &QMediaPlayer::positionChanged, this, &MediaPlayer::positionChanged);
}

void MediaPlayer::addToPlaylist(const QUrl &url)
{
    playlist->addMedia(url);
}

void MediaPlayer::play()
{
    player->play();
}

void MediaPlayer::pause()
{
    player->pause();
}

void MediaPlayer::stop()
{
    player->stop();
}

QVariant MediaPlayer::metaData(QMediaMetaData::Key key) const
{
    return player->metaData(key);
}

qint64 MediaPlayer::duration() const
{
    return player->duration();
}

qint64 MediaPlayer::position() const
{
    return player->position();
}

void MediaPlayer::setPosition(qint64 position)
{
    player->setPosition(position);
}
