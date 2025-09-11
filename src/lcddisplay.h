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

#ifndef LCDDISPLAY_H
#define LCDDISPLAY_H

#include <QWidget>
#include <QPainter>
#include <QLinearGradient>
#include <QRadialGradient>
#include <QFont>
#include <QMouseEvent>
#include <QTimer>
#include <QVector>

class LcdDisplay : public QWidget
{
    Q_OBJECT

public:
    explicit LcdDisplay(QWidget *parent = nullptr);
    ~LcdDisplay();

    void setTrackInfo(const QString &title, const QString &artist);
    void setDuration(qint64 duration);
    void setPosition(qint64 position);
    void setElapsedTime(const QString &time);
    void setLevels(const QVector<float> &levels);

signals:
    void seekChanged(qint64 position);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    void setupSeekSlider();
    void updateDisplay();
    void createLcdGradients();
    
    QString displayTitle;
    QString displayArtist;
    QString elapsedTime;
    qint64 duration;
    qint64 position;
    
    // Seek slider
    QRect seekSliderRect;
    bool isSeeking;
    int seekSliderHeight;
    
    // LCD styling
    QLinearGradient *lcdBackgroundGradient;
    QRadialGradient *lcdGlowGradient;
    QFont *lcdFont;
    QFont *timeFont;
    
    // Colors
    QColor lcdBackgroundColor;
    QColor lcdTextColor;
    QColor lcdGlowColor;
    QColor seekSliderColor;
    QColor seekSliderBackgroundColor;

    // Visualizer state
    bool visualizerActive;
    QVector<float> levelBins;
};

#endif // LCDDISPLAY_H 
