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

    // Draw text with glow effect
    painter.setFont(*lcdFont);
    
    // Draw artist/title line
    QString displayText = displayArtist;
    if (!displayTitle.isEmpty()) {
        displayText += " - " + displayTitle;
    }
    
    // Truncate text if too long
    QFontMetrics fm(*lcdFont);
    if (fm.horizontalAdvance(displayText) > width() - 20) {
        displayText = fm.elidedText(displayText, Qt::ElideRight, width() - 20);
    }

    // Draw glow effect
    painter.setPen(QPen(lcdGlowColor, 3));
    painter.drawText(rect().adjusted(10, 5, -10, -15), Qt::AlignLeft | Qt::AlignTop, displayText);

    // Draw main text
    painter.setPen(QPen(lcdTextColor, 1));
    painter.drawText(rect().adjusted(10, 5, -10, -15), Qt::AlignLeft | Qt::AlignTop, displayText);

    // Draw elapsed time
    painter.setFont(*timeFont);
    painter.setPen(QPen(lcdTextColor, 1));
    painter.drawText(rect().adjusted(10, 5, -10, -5), Qt::AlignRight | Qt::AlignBottom, elapsedTime);

    // Draw seek slider
    if (duration > 0) {
        // Background
        painter.fillRect(seekSliderRect, seekSliderBackgroundColor);
        
        // Progress
        int progressWidth = (position * seekSliderRect.width()) / duration;
        QRect progressRect = seekSliderRect;
        progressRect.setWidth(progressWidth);
        painter.fillRect(progressRect, seekSliderColor);
        
        // Border
        painter.setPen(QPen(QColor(100, 120, 100), 1));
        painter.drawRect(seekSliderRect);
    }
}

void LcdDisplay::mousePressEvent(QMouseEvent *event)
{
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