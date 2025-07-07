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

#ifndef TOPBAR_H
#define TOPBAR_H

#include <QWidget>
#include <QPushButton>
#include <QSlider>
#include <QLineEdit>
#include <QLabel>
#include <QHBoxLayout>
#include <QButtonGroup>
#include <QTime>
#include <QTimer>
#include <QPainter>
#include <QLinearGradient>
#include <QRadialGradient>
#include <QConicalGradient>
#include <QFont>
#include <QFontMetrics>
#include <QPropertyAnimation>
#include <QEasingCurve>

class LcdDisplay;
class VolumeSlider;
class SeekSlider;
class ViewSwitcher;

/**
 * @brief TopBar - The main control bar for MediaSonic
 * 
 * This widget replicates the iTunes 9 top control bar with:
 * - Playback controls (previous, play/pause, next)
 * - Volume slider with custom styling
 * - LCD display showing track info and elapsed time
 * - Integrated seek slider within the LCD display
 * - View switcher (List, Album, Cover Flow)
 * - Search bar for library filtering
 * 
 * The design follows skeuomorphic principles with:
 * - Light grey gradient background
 * - Glossy button effects
 * - Custom LCD display with dark greenish background
 * - Smooth animations and transitions
 */
class TopBar : public QWidget
{
    Q_OBJECT

public:
    explicit TopBar(QWidget *parent = nullptr);
    ~TopBar();

    // Public interface for external control
    void setTrackInfo(const QString &title, const QString &artist);
    void setDuration(qint64 duration);
    void setPosition(qint64 position);
    void setVolume(int volume);
    void setPlayState(bool isPlaying);
    void setCurrentView(int viewIndex);
    LcdDisplay* getLcdDisplay() { return lcdDisplay; }

signals:
    // Playback control signals
    void playClicked();
    void pauseClicked();
    void stopClicked();
    void previousClicked();
    void nextClicked();
    void volumeChanged(int volume);
    void seekChanged(qint64 position);
    
    // View switching signals
    void viewSwitched(int viewIndex);
    
    // Search signals
    void searchTextChanged(const QString &text);

protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

private slots:
    void onPlayPauseClicked();
    void onPreviousClicked();
    void onNextClicked();
    void onVolumeChanged(int value);
    void onViewButtonClicked(int id);
    void onSearchTextChanged(const QString &text);
    void updateElapsedTime();

private:
    void setupUi();
    void setupPlaybackControls();
    void setupVolumeControl();
    void setupLcdDisplay();
    void setupViewSwitcher();
    void setupSearchBar();
    void setupStyling();
    void createGradients();
    void updateButtonStates();
    void animateButton(QPushButton *button);

    // Main layout
    QHBoxLayout *mainLayout;
    
    // Playback controls
    QPushButton *previousButton;
    QPushButton *playPauseButton;
    QPushButton *nextButton;
    QPushButton *stopButton;
    
    // Volume control
    VolumeSlider *volumeSlider;
    
    // LCD display with seek slider
    LcdDisplay *lcdDisplay;
    
    // View switcher
    ViewSwitcher *viewSwitcher;
    
    // Search bar
    QLineEdit *searchBar;
    
    // State variables
    bool isPlaying;
    QString currentTitle;
    QString currentArtist;
    qint64 currentDuration;
    qint64 currentPosition;
    int currentVolume;
    int currentView;
    
    // Animation and styling
    QTimer *elapsedTimer;
    QPropertyAnimation *buttonAnimation;
    QLinearGradient *backgroundGradient;
    QLinearGradient *buttonGradient;
    QLinearGradient *buttonPressedGradient;
    
    // Fonts
    QFont *lcdFont;
    QFont *buttonFont;
    QFont *searchFont;
    
    // Colors (iTunes 9 inspired)
    QColor backgroundColor;
    QColor buttonColor;
    QColor buttonPressedColor;
    QColor textColor;
    QColor lcdBackgroundColor;
    QColor lcdTextColor;
    QColor lcdGlowColor;
};

/**
 * @brief VolumeSlider - Custom volume control slider
 * 
 * Features:
 * - iTunes 9-style horizontal slider
 * - Custom painting with gradients
 * - Smooth animations
 * - Visual feedback
 */
class VolumeSlider : public QSlider
{
    Q_OBJECT

public:
    explicit VolumeSlider(QWidget *parent = nullptr);
    virtual ~VolumeSlider();

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    void createSliderGradients();
    
    bool isDragging;
    QLinearGradient *sliderBackgroundGradient;
    QLinearGradient *sliderHandleGradient;
    QLinearGradient *sliderProgressGradient;
    
    QColor sliderBackgroundColor;
    QColor sliderHandleColor;
    QColor sliderProgressColor;
    QColor sliderBorderColor;
};

/**
 * @brief ViewSwitcher - Button group for view switching
 * 
 * Contains three buttons for:
 * - List View
 * - Album View  
 * - Cover Flow View
 * 
 * Features:
 * - iTunes 9-style button group
 * - Custom painting
 * - Smooth transitions
 */
class ViewSwitcher : public QWidget
{
    Q_OBJECT

public:
    explicit ViewSwitcher(QWidget *parent = nullptr);
    virtual ~ViewSwitcher();
    
    void setCurrentView(int viewIndex);

signals:
    void viewChanged(int viewIndex);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    void setupButtons();
    void createButtonGradients();
    
    QButtonGroup *buttonGroup;
    QPushButton *listButton;
    QPushButton *albumButton;
    QPushButton *coverFlowButton;
    
    QLinearGradient *buttonBackgroundGradient;
    QLinearGradient *buttonSelectedGradient;
    QLinearGradient *buttonHoverGradient;
    
    QColor buttonBackgroundColor;
    QColor buttonSelectedColor;
    QColor buttonHoverColor;
    QColor buttonTextColor;
    QColor buttonBorderColor;
};

#endif // TOPBAR_H 