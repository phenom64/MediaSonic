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

#include "topbar.h"
#include "lcddisplay.h"
#include <QApplication>
#include <QStyle>
#include <QStyleOption>
#include <QMouseEvent>
#include <QResizeEvent>
#include <QPainterPath>
#include <QPen>
#include <QBrush>
#include <QDebug>
#include <QPropertyAnimation>
#include <QEasingCurve>
#include <QGraphicsDropShadowEffect>
#include <QGraphicsOpacityEffect>
#include <QTime>

// ============================================================================
// TopBar Implementation
// ============================================================================

TopBar::TopBar(QWidget *parent)
    : QWidget(parent)
    , isPlaying(false)
    , currentDuration(0)
    , currentPosition(0)
    , currentVolume(50)
    , currentView(0)
    , elapsedTimer(new QTimer(this))
    , buttonAnimation(new QPropertyAnimation(this, "geometry"))
    , backgroundGradient(nullptr)
    , buttonGradient(nullptr)
    , buttonPressedGradient(nullptr)
    , lcdFont(new QFont("Monaco", 10, QFont::Bold))
    , buttonFont(new QFont("Lucida Grande", 9))
    , searchFont(new QFont("Lucida Grande", 9))
{
    // Initialize colors (iTunes 9 inspired)
    backgroundColor = QColor(240, 240, 240);
    buttonColor = QColor(220, 220, 220);
    buttonPressedColor = QColor(180, 180, 180);
    textColor = QColor(60, 60, 60);
    lcdBackgroundColor = QColor(20, 40, 20);
    lcdTextColor = QColor(0, 255, 0);
    lcdGlowColor = QColor(0, 200, 0);

    setupUi();
    setupStyling();
    createGradients();

    // Connect timer for elapsed time updates
    connect(elapsedTimer, &QTimer::timeout, this, &TopBar::updateElapsedTime);
    elapsedTimer->start(1000); // Update every second
}

TopBar::~TopBar()
{
    delete lcdFont;
    delete buttonFont;
    delete searchFont;
    delete backgroundGradient;
    delete buttonGradient;
    delete buttonPressedGradient;
}

void TopBar::setupUi()
{
    // Main layout
    mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(10, 8, 10, 8);
    mainLayout->setSpacing(8);

    setupPlaybackControls();
    setupVolumeControl();
    setupLcdDisplay();
    setupViewSwitcher();
    setupSearchBar();

    // Set fixed height for consistent appearance
    setFixedHeight(60);
}

void TopBar::setupPlaybackControls()
{
    // Previous button
    previousButton = new QPushButton(this);
    previousButton->setText("⏮");
    previousButton->setFixedSize(32, 32);
    previousButton->setFont(*buttonFont);
    connect(previousButton, &QPushButton::clicked, this, &TopBar::onPreviousClicked);

    // Play/Pause button
    playPauseButton = new QPushButton(this);
    playPauseButton->setText("▶");
    playPauseButton->setFixedSize(40, 32);
    playPauseButton->setFont(*buttonFont);
    connect(playPauseButton, &QPushButton::clicked, this, &TopBar::onPlayPauseClicked);

    // Next button
    nextButton = new QPushButton(this);
    nextButton->setText("⏭");
    nextButton->setFixedSize(32, 32);
    nextButton->setFont(*buttonFont);
    connect(nextButton, &QPushButton::clicked, this, &TopBar::onNextClicked);

    // Stop button
    stopButton = new QPushButton(this);
    stopButton->setText("⏹");
    stopButton->setFixedSize(32, 32);
    stopButton->setFont(*buttonFont);
    connect(stopButton, &QPushButton::clicked, this, &TopBar::stopClicked);

    // Add to layout
    mainLayout->addWidget(previousButton);
    mainLayout->addWidget(playPauseButton);
    mainLayout->addWidget(nextButton);
    mainLayout->addWidget(stopButton);
    mainLayout->addSpacing(10);
}

void TopBar::setupVolumeControl()
{
    volumeSlider = new VolumeSlider(this);
    volumeSlider->setOrientation(Qt::Horizontal);
    volumeSlider->setRange(0, 100);
    volumeSlider->setValue(currentVolume);
    volumeSlider->setFixedSize(80, 20);
    connect(volumeSlider, &VolumeSlider::valueChanged, this, &TopBar::onVolumeChanged);

    mainLayout->addWidget(volumeSlider);
    mainLayout->addSpacing(15);
}

void TopBar::setupLcdDisplay()
{
    lcdDisplay = new LcdDisplay(this);
    lcdDisplay->setFixedSize(300, 40);
    connect(lcdDisplay, &LcdDisplay::seekChanged, this, &TopBar::seekChanged);

    mainLayout->addWidget(lcdDisplay);
    mainLayout->addSpacing(15);
}

void TopBar::setupViewSwitcher()
{
    viewSwitcher = new ViewSwitcher(this);
    viewSwitcher->setFixedSize(180, 32);
    connect(viewSwitcher, &ViewSwitcher::viewChanged, this, &TopBar::onViewButtonClicked);

    mainLayout->addWidget(viewSwitcher);
    mainLayout->addSpacing(15);
}

void TopBar::setupSearchBar()
{
    searchBar = new QLineEdit(this);
    searchBar->setPlaceholderText("Search...");
    searchBar->setFont(*searchFont);
    searchBar->setFixedSize(150, 24);
    connect(searchBar, &QLineEdit::textChanged, this, &TopBar::onSearchTextChanged);

    mainLayout->addWidget(searchBar);
    mainLayout->addStretch();
}

void TopBar::setupStyling()
{
    // Set background color
    setAutoFillBackground(true);
    QPalette pal = palette();
    pal.setColor(QPalette::Window, backgroundColor);
    setPalette(pal);

    // Apply styling to buttons
    QString buttonStyle = 
        "QPushButton {"
        "    background: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
        "        stop:0 #f0f0f0, stop:1 #d0d0d0);"
        "    border: 1px solid #a0a0a0;"
        "    border-radius: 4px;"
        "    color: #404040;"
        "    font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "    background: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
        "        stop:0 #e8e8e8, stop:1 #c8c8c8);"
        "}"
        "QPushButton:pressed {"
        "    background: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
        "        stop:0 #c0c0c0, stop:1 #a0a0a0);"
        "}";

    previousButton->setStyleSheet(buttonStyle);
    playPauseButton->setStyleSheet(buttonStyle);
    nextButton->setStyleSheet(buttonStyle);
    stopButton->setStyleSheet(buttonStyle);

    // Search bar styling
    searchBar->setStyleSheet(
        "QLineEdit {"
        "    background: white;"
        "    border: 1px solid #a0a0a0;"
        "    border-radius: 3px;"
        "    padding: 2px 6px;"
        "    color: #404040;"
        "}"
        "QLineEdit:focus {"
        "    border: 1px solid #0078d4;"
        "}"
    );
}

void TopBar::createGradients()
{
    // Background gradient
    backgroundGradient = new QLinearGradient(0, 0, 0, height());
    backgroundGradient->setColorAt(0, QColor(245, 245, 245));
    backgroundGradient->setColorAt(1, QColor(235, 235, 235));

    // Button gradient
    buttonGradient = new QLinearGradient(0, 0, 0, 32);
    buttonGradient->setColorAt(0, QColor(240, 240, 240));
    buttonGradient->setColorAt(1, QColor(220, 220, 220));

    // Button pressed gradient
    buttonPressedGradient = new QLinearGradient(0, 0, 0, 32);
    buttonPressedGradient->setColorAt(0, QColor(200, 200, 200));
    buttonPressedGradient->setColorAt(1, QColor(180, 180, 180));
}

void TopBar::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // Draw background gradient
    painter.fillRect(rect(), *backgroundGradient);

    // Draw subtle border
    painter.setPen(QPen(QColor(180, 180, 180), 1));
    painter.drawLine(0, height() - 1, width(), height() - 1);
}

void TopBar::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    
    // Update gradients for new size
    if (backgroundGradient) {
        backgroundGradient->setFinalStop(0, height());
    }
}

// Public interface methods
void TopBar::setTrackInfo(const QString &title, const QString &artist)
{
    currentTitle = title;
    currentArtist = artist;
    lcdDisplay->setTrackInfo(title, artist);
}

void TopBar::setDuration(qint64 duration)
{
    currentDuration = duration;
    lcdDisplay->setDuration(duration);
}

void TopBar::setPosition(qint64 position)
{
    currentPosition = position;
    lcdDisplay->setPosition(position);
}

void TopBar::setVolume(int volume)
{
    currentVolume = volume;
    volumeSlider->setValue(volume);
}

void TopBar::setPlayState(bool playing)
{
    isPlaying = playing;
    if (playing) {
        playPauseButton->setText("⏸");
    } else {
        playPauseButton->setText("▶");
    }
}

void TopBar::setCurrentView(int viewIndex)
{
    currentView = viewIndex;
    viewSwitcher->setCurrentView(viewIndex);
}

// Private slots
void TopBar::onPlayPauseClicked()
{
    if (isPlaying) {
        emit pauseClicked();
    } else {
        emit playClicked();
    }
}

void TopBar::onPreviousClicked()
{
    emit previousClicked();
}

void TopBar::onNextClicked()
{
    emit nextClicked();
}

void TopBar::onVolumeChanged(int value)
{
    currentVolume = value;
    emit volumeChanged(value);
}

void TopBar::onViewButtonClicked(int id)
{
    currentView = id;
    emit viewSwitched(id);
}

void TopBar::onSearchTextChanged(const QString &text)
{
    emit searchTextChanged(text);
}

void TopBar::updateElapsedTime()
{
    if (isPlaying && currentDuration > 0) {
        QTime elapsed = QTime(0, 0).addMSecs(currentPosition);
        QString timeStr = elapsed.toString("mm:ss");
        lcdDisplay->setElapsedTime(timeStr);
    }
}

void TopBar::updateButtonStates()
{
    // Update button enabled states based on current state
    bool hasMedia = currentDuration > 0;
    previousButton->setEnabled(hasMedia);
    playPauseButton->setEnabled(hasMedia);
    nextButton->setEnabled(hasMedia);
    stopButton->setEnabled(hasMedia);
}

void TopBar::animateButton(QPushButton *button)
{
    // Create a simple scale animation
    QPropertyAnimation *anim = new QPropertyAnimation(button, "geometry");
    anim->setDuration(100);
    anim->setStartValue(button->geometry());
    anim->setEndValue(button->geometry().adjusted(1, 1, -1, -1));
    anim->setEasingCurve(QEasingCurve::OutQuad);
    anim->start(QAbstractAnimation::DeleteWhenStopped);
}

void TopBar::mousePressEvent(QMouseEvent *event)
{
    QWidget::mousePressEvent(event);
}

void TopBar::mouseReleaseEvent(QMouseEvent *event)
{
    QWidget::mouseReleaseEvent(event);
}

void TopBar::mouseMoveEvent(QMouseEvent *event)
{
    QWidget::mouseMoveEvent(event);
}

// ============================================================================
// VolumeSlider Implementation
// ============================================================================

// ============================================================================
// VolumeSlider Implementation
// ============================================================================

VolumeSlider::VolumeSlider(QWidget *parent)
    : QSlider(parent)
    , isDragging(false)
    , sliderBackgroundGradient(nullptr)
    , sliderHandleGradient(nullptr)
    , sliderProgressGradient(nullptr)
{
    // Initialize colors
    sliderBackgroundColor = QColor(200, 200, 200);
    sliderHandleColor = QColor(180, 180, 180);
    sliderProgressColor = QColor(100, 150, 255);
    sliderBorderColor = QColor(120, 120, 120);

    createSliderGradients();
    setMouseTracking(true);
}

VolumeSlider::~VolumeSlider()
{
    delete sliderBackgroundGradient;
    delete sliderHandleGradient;
    delete sliderProgressGradient;
}

void VolumeSlider::createSliderGradients()
{
    // Background gradient
    sliderBackgroundGradient = new QLinearGradient(0, 0, 0, height());
    sliderBackgroundGradient->setColorAt(0, QColor(220, 220, 220));
    sliderBackgroundGradient->setColorAt(1, QColor(200, 200, 200));

    // Handle gradient
    sliderHandleGradient = new QLinearGradient(0, 0, 0, height());
    sliderHandleGradient->setColorAt(0, QColor(240, 240, 240));
    sliderHandleGradient->setColorAt(1, QColor(200, 200, 200));

    // Progress gradient
    sliderProgressGradient = new QLinearGradient(0, 0, 0, height());
    sliderProgressGradient->setColorAt(0, QColor(120, 170, 255));
    sliderProgressGradient->setColorAt(1, QColor(100, 150, 255));
}

void VolumeSlider::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    int handleWidth = 12;
    int handleHeight = height() - 4;
    int sliderWidth = width() - handleWidth;
    
    // Calculate handle position
    int handleX = (value() * sliderWidth) / (maximum() - minimum());

    // Draw background
    painter.fillRect(rect(), *sliderBackgroundGradient);

    // Draw progress
    QRect progressRect = rect();
    progressRect.setWidth(handleX + handleWidth / 2);
    painter.fillRect(progressRect, *sliderProgressGradient);

    // Draw handle
    QRect handleRect(handleX, 2, handleWidth, handleHeight);
    painter.fillRect(handleRect, *sliderHandleGradient);

    // Draw borders
    painter.setPen(QPen(sliderBorderColor, 1));
    painter.drawRect(rect().adjusted(0, 0, -1, -1));
    painter.drawRect(handleRect);
}

void VolumeSlider::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        isDragging = true;
        int x = event->pos().x();
        int newValue = ((x - 6) * (maximum() - minimum())) / (width() - 12);
        newValue = qBound(minimum(), newValue, maximum());
        setValue(newValue);
    }
}

void VolumeSlider::mouseMoveEvent(QMouseEvent *event)
{
    if (isDragging) {
        int x = event->pos().x();
        int newValue = ((x - 6) * (maximum() - minimum())) / (width() - 12);
        newValue = qBound(minimum(), newValue, maximum());
        setValue(newValue);
    }
}

void VolumeSlider::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event)
    isDragging = false;
}

// ============================================================================
// ViewSwitcher Implementation
// ============================================================================

ViewSwitcher::ViewSwitcher(QWidget *parent)
    : QWidget(parent)
    , buttonGroup(new QButtonGroup(this))
    , listButton(new QPushButton("List", this))
    , albumButton(new QPushButton("Album", this))
    , coverFlowButton(new QPushButton("Cover Flow", this))
    , buttonBackgroundGradient(nullptr)
    , buttonSelectedGradient(nullptr)
    , buttonHoverGradient(nullptr)
{
    // Initialize colors
    buttonBackgroundColor = QColor(220, 220, 220);
    buttonSelectedColor = QColor(180, 200, 255);
    buttonHoverColor = QColor(200, 220, 240);
    buttonTextColor = QColor(60, 60, 60);
    buttonBorderColor = QColor(150, 150, 150);

    setupButtons();
    createButtonGradients();
}

ViewSwitcher::~ViewSwitcher()
{
    delete buttonBackgroundGradient;
    delete buttonSelectedGradient;
    delete buttonHoverGradient;
}

void ViewSwitcher::setupButtons()
{
    // Configure buttons
    listButton->setFixedSize(60, 30);
    albumButton->setFixedSize(60, 30);
    coverFlowButton->setFixedSize(60, 30);

    // Add to button group
    buttonGroup->addButton(listButton, 0);
    buttonGroup->addButton(albumButton, 1);
    buttonGroup->addButton(coverFlowButton, 2);

    // Set exclusive
    buttonGroup->setExclusive(true);

    // Connect signals
    connect(buttonGroup, QOverload<int>::of(&QButtonGroup::idClicked), 
            this, &ViewSwitcher::viewChanged);

    // Layout
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->addWidget(listButton);
    layout->addWidget(albumButton);
    layout->addWidget(coverFlowButton);

    // Set initial state
    listButton->setChecked(true);
}

void ViewSwitcher::createButtonGradients()
{
    // Background gradient
    buttonBackgroundGradient = new QLinearGradient(0, 0, 0, 30);
    buttonBackgroundGradient->setColorAt(0, QColor(240, 240, 240));
    buttonBackgroundGradient->setColorAt(1, QColor(220, 220, 220));

    // Selected gradient
    buttonSelectedGradient = new QLinearGradient(0, 0, 0, 30);
    buttonSelectedGradient->setColorAt(0, QColor(200, 220, 255));
    buttonSelectedGradient->setColorAt(1, QColor(180, 200, 255));

    // Hover gradient
    buttonHoverGradient = new QLinearGradient(0, 0, 0, 30);
    buttonHoverGradient->setColorAt(0, QColor(220, 240, 255));
    buttonHoverGradient->setColorAt(1, QColor(200, 220, 240));
}

void ViewSwitcher::setCurrentView(int viewIndex)
{
    switch (viewIndex) {
        case 0:
            listButton->setChecked(true);
            break;
        case 1:
            albumButton->setChecked(true);
            break;
        case 2:
            coverFlowButton->setChecked(true);
            break;
    }
}

void ViewSwitcher::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // Draw background
    painter.fillRect(rect(), *buttonBackgroundGradient);

    // Draw borders
    painter.setPen(QPen(buttonBorderColor, 1));
    painter.drawRect(rect().adjusted(0, 0, -1, -1));
} 