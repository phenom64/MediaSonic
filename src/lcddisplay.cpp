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
#include <QRandomGenerator>
#include "ui/atmo_style.h"
#include <QVector>

LcdDisplay::LcdDisplay(QWidget *parent)
    : QWidget(parent)
    , duration(0)
    , position(0)
    , isSeeking(false)
    , seekSliderHeight(6)
    , lcdBackgroundGradient(nullptr)
    , lcdGlowGradient(nullptr)
    , lcdFont(new QFont("DS-Digital", 12, QFont::Bold))
    , timeFont(new QFont("DS-Digital", 10, QFont::Bold))
    , visualizerActive(false)
{
    // Initialize colors - iTunes 9 style khaki-green LCD
    lcdBackgroundColor = QColor(215, 220, 200); // Light khaki start
    lcdTextColor = QColor(50, 59, 40); // Dark olive green text
    lcdGlowColor = QColor(50, 59, 40);
    seekSliderColor = QColor(50, 59, 40);
    seekSliderBackgroundColor = QColor(200, 205, 185); // Darker khaki end

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

void LcdDisplay::setLevels(const QVector<float> &levels)
{
    levelBins = levels;
    if (visualizerActive) update();
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
    MS::AtmoStyle style = MS::AtmoStyle::fromPalette(palette());
    QLinearGradient g = style.lcdBackground(height());
    lcdBackgroundGradient = new QLinearGradient(g);
    QRadialGradient rg = style.overlayGlow(width(), height());
    lcdGlowGradient = new QRadialGradient(rg);
}

void LcdDisplay::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // Draw LCD background
    painter.fillRect(rect(), *lcdBackgroundGradient);

    // Draw LCD border - subtle iTunes style
    painter.setPen(QPen(QColor(180, 185, 170), 1));
    painter.drawRect(rect().adjusted(1, 1, -1, -1));

    // Draw gloss effect (subtle highlight at top)
    QLinearGradient gloss(rect().topLeft(), rect().bottomLeft());
    gloss.setColorAt(0, QColor(255,255,255,60));
    gloss.setColorAt(0.2, QColor(255,255,255,20));
    gloss.setColorAt(1, QColor(255,255,255,0));
    QRect glossRect = rect().adjusted(2, 2, -2, -height()/3);
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

    // If visualizer is active, draw bars using live data
    if (visualizerActive) {
        int visW = width() - playIconRect.right() - 30;
        int visH = height() / 2;
        QRect visRect(playIconRect.right()+10, height()/2-visH/2, visW, visH);
        painter.setPen(Qt::NoPen);
        const int bins = levelBins.isEmpty() ? 32 : levelBins.size();
        for (int i = 0; i < bins; ++i) {
            float level = levelBins.isEmpty() ? QRandomGenerator::global()->bounded(1.0f) : levelBins.at(i);
            int barH = int(level * visH);
            int barW = qMax(2, visW / bins - 2);
            int x = visRect.left() + i * (visW / bins);
            QRect bar(x, visRect.bottom() - barH, barW, barH);
            painter.setBrush(QColor(0,255,0,160));
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
