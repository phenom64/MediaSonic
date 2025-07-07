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

#include "lcddisplay.h"
#include <QPainter>
#include <QMouseEvent>
#include <QResizeEvent>
#include <QFontMetrics>
#include <QApplication>

LcdDisplay::LcdDisplay(QWidget *parent)
    : QWidget(parent)
    , duration(0)
    , position(0)
    , isSeeking(false)
    , seekSliderHeight(6)
    , lcdBackgroundGradient(nullptr)
    , lcdGlowGradient(nullptr)
    , lcdFont(new QFont("Monaco", 9, QFont::Bold))
    , timeFont(new QFont("Monaco", 8, QFont::Bold))
    , visualizerActive(false)
{
    // Initialize colors
    lcdBackgroundColor = QColor(20, 40, 20);
    lcdTextColor = QColor(0, 255, 0);
    lcdGlowColor = QColor(0, 200, 0);
    seekSliderColor = QColor(0, 255, 0);
    seekSliderBackgroundColor = QColor(10, 20, 10);

    createLcdGradients();
    setupSeekSlider();
    
    setMouseTracking(true);
}

LcdDisplay::~LcdDisplay()
{
    delete lcdFont;
    delete timeFont;
    delete lcdBackgroundGradient;
    delete lcdGlowGradient;
}

void LcdDisplay::setTrackInfo(const QString &title, const QString &artist)
{
    displayTitle = title;
    displayArtist = artist;
    updateDisplay();
}

void LcdDisplay::setDuration(qint64 dur)
{
    duration = dur;
    updateDisplay();
}

void LcdDisplay::setPosition(qint64 pos)
{
    position = pos;
    updateDisplay();
}

void LcdDisplay::setElapsedTime(const QString &time)
{
    elapsedTime = time;
    updateDisplay();
}

void LcdDisplay::setupSeekSlider()
{
    seekSliderRect = QRect(5, height() - seekSliderHeight - 5, width() - 10, seekSliderHeight);
}

void LcdDisplay::updateDisplay()
{
    update();
}

void LcdDisplay::createLcdGradients()
{
    // LCD background gradient
    lcdBackgroundGradient = new QLinearGradient(0, 0, 0, height());
    lcdBackgroundGradient->setColorAt(0, QColor(30, 50, 30));
    lcdBackgroundGradient->setColorAt(0.5, QColor(20, 40, 20));
    lcdBackgroundGradient->setColorAt(1, QColor(10, 30, 10));

    // LCD glow gradient
    lcdGlowGradient = new QRadialGradient(width() / 2, height() / 2, width() / 2);
    lcdGlowGradient->setColorAt(0, QColor(0, 255, 0, 50));
    lcdGlowGradient->setColorAt(1, QColor(0, 255, 0, 0));
}

void LcdDisplay::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // Draw LCD background
    painter.fillRect(rect(), *lcdBackgroundGradient);

    // Draw LCD border
    painter.setPen(QPen(QColor(60, 80, 60), 2));
    painter.drawRect(rect().adjusted(1, 1, -1, -1));

    // Draw gloss effect (subtle white gradient at top)
    QLinearGradient gloss(rect().topLeft(), rect().bottomLeft());
    gloss.setColorAt(0, QColor(255,255,255,80));
    gloss.setColorAt(0.3, QColor(255,255,255,30));
    gloss.setColorAt(1, QColor(255,255,255,0));
    QRect glossRect = rect().adjusted(2, 2, -2, -height()/2);
    painter.fillRect(glossRect, gloss);

    // If no track is playing, show centered Syndromatic logo
    if (displayTitle.isEmpty() && displayArtist.isEmpty()) {
        QPixmap logo(":/gfx/icons/syndromatic_logo.png");
        int logoW = width() / 3;
        int logoH = height() * 0.7;
        QRect logoRect((width()-logoW)/2, (height()-logoH)/2, logoW, logoH);
        painter.drawPixmap(logoRect, logo);
        return;
    }

    // Draw play icon (clickable for visualizer)
    int playIconSize = 24;
    QRect playIconRect(10, (height()-playIconSize)/2, playIconSize, playIconSize);
    painter.setBrush(QColor(200, 220, 180));
    painter.setPen(Qt::NoPen);
    painter.drawEllipse(playIconRect);
    painter.setPen(QPen(QColor(60, 80, 60), 2));
    QPointF points[3] = {
        QPointF(playIconRect.left()+7, playIconRect.top()+5),
        QPointF(playIconRect.left()+7, playIconRect.bottom()-5),
        QPointF(playIconRect.right()-5, playIconRect.center().y())
    };
    painter.setBrush(QColor(60, 80, 60));
    painter.drawPolygon(points, 3);

    // If visualizer is active, draw a placeholder digital visualizer
    if (visualizerActive) {
        int visW = width() - playIconRect.right() - 30;
        int visH = height() / 2;
        QRect visRect(playIconRect.right()+10, height()/2-visH/2, visW, visH);
        painter.setPen(Qt::NoPen);
        for (int i = 0; i < 32; ++i) {
            int barH = qrand() % visH;
            QRect bar(visRect.left()+i*(visW/32), visRect.bottom()-barH, visW/32-2, barH);
            painter.setBrush(QColor(0,255,0,180));
            painter.drawRect(bar);
        }
        return;
    }

    // Draw track info (title, artist)
    painter.setFont(*lcdFont);
    painter.setPen(QPen(lcdTextColor, 1));
    QString displayText = displayArtist;
    if (!displayTitle.isEmpty()) {
        displayText += " — " + displayTitle;
    }
    QFontMetrics fm(*lcdFont);
    if (fm.horizontalAdvance(displayText) > width() - 80) {
        displayText = fm.elidedText(displayText, Qt::ElideRight, width() - 80);
    }
    painter.drawText(rect().adjusted(playIconRect.right()+10, 5, -10, -height()/2), Qt::AlignLeft | Qt::AlignVCenter, displayText);

    // Draw elapsed and remaining time
    painter.setFont(*timeFont);
    painter.setPen(QPen(lcdTextColor, 1));
    QString timeStr = elapsedTime;
    if (duration > 0) {
        int secs = (duration-position)/1000;
        QString rem = QString("-%1:%2").arg(secs/60,2,10,QChar('0')).arg(secs%60,2,10,QChar('0'));
        timeStr += "  " + rem;
    }
    painter.drawText(rect().adjusted(playIconRect.right()+10, height()/2, -10, -5), Qt::AlignLeft | Qt::AlignBottom, timeStr);

    // Draw seek slider
    if (duration > 0) {
        painter.fillRect(seekSliderRect, seekSliderBackgroundColor);
        int progressWidth = (position * seekSliderRect.width()) / duration;
        QRect progressRect = seekSliderRect;
        progressRect.setWidth(progressWidth);
        painter.fillRect(progressRect, seekSliderColor);
        painter.setPen(QPen(QColor(100, 120, 100), 1));
        painter.drawRect(seekSliderRect);
    }
}

void LcdDisplay::mousePressEvent(QMouseEvent *event)
{
    int playIconSize = 24;
    QRect playIconRect(10, (height()-playIconSize)/2, playIconSize, playIconSize);
    if (event->button() == Qt::LeftButton && playIconRect.contains(event->pos())) {
        visualizerActive = !visualizerActive;
        updateDisplay();
        return;
    }
    if (event->button() == Qt::LeftButton && seekSliderRect.contains(event->pos())) {
        isSeeking = true;
        int x = event->pos().x() - seekSliderRect.x();
        qint64 newPosition = (x * duration) / seekSliderRect.width();
        newPosition = qBound(qint64(0), newPosition, duration);
        emit seekChanged(newPosition);
    }
}

void LcdDisplay::mouseMoveEvent(QMouseEvent *event)
{
    if (isSeeking) {
        int x = event->pos().x() - seekSliderRect.x();
        qint64 newPosition = (x * duration) / seekSliderRect.width();
        newPosition = qBound(qint64(0), newPosition, duration);
        emit seekChanged(newPosition);
    }
}

void LcdDisplay::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event)
    isSeeking = false;
}

void LcdDisplay::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    setupSeekSlider();
    
    // Update gradients for new size
    if (lcdBackgroundGradient) {
        lcdBackgroundGradient->setFinalStop(0, height());
    }
    if (lcdGlowGradient) {
        lcdGlowGradient->setCenter(width() / 2, height() / 2);
        lcdGlowGradient->setRadius(width() / 2);
    }
} 