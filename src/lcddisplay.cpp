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
#include <QPainterPath>

LcdDisplay::LcdDisplay(QWidget *parent)
    : QWidget(parent)
    , duration(0)
    , position(0)
    , isSeeking(false)
    , seekSliderHeight(6)
    , lcdBackgroundGradient(nullptr)
    , lcdGlowGradient(nullptr)
    , lcdFont(new QFont(qApp->property("ms.lcdfamily").toString().isEmpty() ? QStringLiteral("Monospace") : qApp->property("ms.lcdfamily").toString(), 12, QFont::Bold))
    , timeFont(new QFont(qApp->property("ms.lcdfamily").toString().isEmpty() ? QStringLiteral("Monospace") : qApp->property("ms.lcdfamily").toString(), 10, QFont::Bold))
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

    // Rounded bezel and inner fill (iTunes 10 style)
    const int radius = 8;
    QRect outer = rect().adjusted(0,0,-1,-1);
    QPainterPath bezelPath; bezelPath.addRoundedRect(outer, radius, radius);
    painter.setPen(QPen(QColor(150,150,130), 1));
    painter.setBrush(Qt::NoBrush);
    painter.drawPath(bezelPath);

    QRect inner = outer.adjusted(2,2,-2,-2);
    QPainterPath innerPath; innerPath.addRoundedRect(inner, radius-2, radius-2);
    painter.setClipPath(innerPath);
    painter.fillRect(inner, *lcdBackgroundGradient);
    painter.setClipping(false);

    // Gloss overlay at top half
    QRect glossRect = QRect(inner.left(), inner.top(), inner.width(), inner.height()/2);
    QLinearGradient gloss(inner.topLeft(), inner.bottomLeft());
    gloss.setColorAt(0.0, QColor(255,255,255,90));
    gloss.setColorAt(0.6, QColor(255,255,255,20));
    gloss.setColorAt(1.0, QColor(255,255,255,0));
    painter.setClipPath(innerPath);
    painter.fillRect(glossRect, gloss);
    painter.setClipping(false);

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
    int playIconSize = 18;
    QRect playIconRect(inner.left()+6, inner.center().y()-playIconSize/2, playIconSize, playIconSize);
    painter.setBrush(QColor(220, 230, 210));
    painter.setPen(QPen(QColor(120, 140, 110), 1));
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
    QFont dynTitle = *lcdFont; dynTitle.setPixelSize(qMax(10, height()/3));
    painter.setFont(dynTitle);
    painter.setPen(QPen(lcdTextColor, 1));
    QString displayText = displayArtist;
    if (!displayTitle.isEmpty()) {
        displayText += " — " + displayTitle;
    }
    QFontMetrics fm(dynTitle);
    displayText = fm.elidedText(displayText, Qt::ElideRight, width() - (playIconRect.right()+20));
    painter.drawText(inner.adjusted(playIconRect.right()+10, 6, -10, -inner.height()/2), Qt::AlignHCenter | Qt::AlignVCenter, displayText);

    // Draw elapsed and right time (remaining/total)
    QFont dynTime = *timeFont; dynTime.setPixelSize(qMax(9, height()/4));
    painter.setFont(dynTime);
    painter.setPen(QPen(lcdTextColor, 1));
    auto toMinSec = [](qint64 ms){ int s = int(ms/1000); return QString("%1:%2").arg(s/60,2,10,QChar('0')).arg(s%60,2,10,QChar('0')); };
    QString elapsed = toMinSec(position);
    QString right = duration>0 ? (showRemainingNotTotal ? QString("-")+toMinSec(duration-position) : toMinSec(duration)) : QString();
    QFontMetrics tfm(dynTime);
    int lwidth = tfm.horizontalAdvance(elapsed);
    int rwidth = tfm.horizontalAdvance(right);
    leftTimeRect  = QRect(inner.left()+playIconRect.width()+14, inner.center().y(), lwidth, tfm.height());
    rightTimeRect = QRect(inner.right()-rwidth-10, inner.center().y(), rwidth, tfm.height());
    painter.drawText(leftTimeRect, Qt::AlignLeft|Qt::AlignVCenter, elapsed);
    if (!right.isEmpty()) painter.drawText(rightTimeRect, Qt::AlignRight|Qt::AlignVCenter, right);

    // Draw seek slider with knob
    if (duration > 0) {
        // Update seek slider area between time labels
        int leftEdge = leftTimeRect.right() + 12;
        int rightEdge = rightTimeRect.left() - 12;
        if (rightEdge - leftEdge < 40) { leftEdge = inner.left()+40; rightEdge = inner.right()-40; }
        seekSliderRect = QRect(leftEdge, inner.bottom()- (seekSliderHeight+8), rightEdge-leftEdge, seekSliderHeight);
        // Groove
        QPainterPath groovePath; groovePath.addRoundedRect(seekSliderRect, 3, 3);
        painter.setClipPath(groovePath);
        painter.fillRect(seekSliderRect, QColor(230, 235, 220));
        int progressWidth = (position * seekSliderRect.width()) / duration;
        QRect progressRect = seekSliderRect; progressRect.setWidth(progressWidth);
        painter.fillRect(progressRect, QColor(150, 170, 140));
        painter.setClipping(false);
        painter.setPen(QPen(QColor(120, 135, 110), 1));
        painter.drawPath(groovePath);
        // Knob
        int knobX = seekSliderRect.x() + progressWidth;
        QRect knobRect(knobX-4, seekSliderRect.y()-3, 8, seekSliderRect.height()+6);
        painter.fillRect(knobRect, QColor(90,110,90));
    }
}

void LcdDisplay::mousePressEvent(QMouseEvent *event)
{
    int playIconSize = 18;
    QRect playIconRect(6, (height()-playIconSize)/2, playIconSize, playIconSize);
    if (event->button() == Qt::LeftButton && playIconRect.contains(event->pos())) {
        visualizerActive = !visualizerActive;
        updateDisplay();
        return;
    }
    if (event->button() == Qt::LeftButton && rightTimeRect.contains(event->pos())) {
        showRemainingNotTotal = !showRemainingNotTotal;
        update();
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
