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

#include "mainwindow.h"
#include "dialogs/aboutInfo.h"
#include <QApplication>
#include <QMainWindow>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QFileDialog>
#include <QSplitter>
#include <QTreeView>
#include <QTableView>
#include <QHeaderView>
#include <QStandardItemModel>
#include <QStatusBar>
#include <QLabel>
#include <QSlider>
#include <QHBoxLayout>
#include <QMediaMetaData>
#include <QMediaContent>
#include <QTime>
#include <QDirIterator>
#include <QtConcurrent/QtConcurrent>
#include "topbar.h"
#include <QStackedWidget>
#include <QWidget>
#include <QVBoxLayout>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setupUi();
    
    // Create MediaPlayer BEFORE setupMenuBar
    mediaPlayer = new MediaPlayer(this);
    connect(mediaPlayer, &MediaPlayer::currentMediaChanged, this, &MainWindow::updateTrackInfo);
    connect(mediaPlayer, &MediaPlayer::durationChanged, this, &MainWindow::updateDuration);
    connect(mediaPlayer, &MediaPlayer::positionChanged, this, &MainWindow::updatePosition);
    connect(timeSlider, &QSlider::sliderMoved, this, &MainWindow::seek);
    
    setupMenuBar();
    createModels();
}

MainWindow::~MainWindow()
{
}

void MainWindow::setupUi()
{
    setWindowTitle("MediaSonic");
    resize(1200, 800);

    // Central widget and main layout
    QWidget *central = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(central);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // TopBar (new skeuomorphic control bar)
    topBar = new TopBar(this);
    mainLayout->addWidget(topBar);

    // Main splitter (sidebar + main view)
    mainSplitter = new QSplitter(Qt::Horizontal, this);
    mainSplitter->setChildrenCollapsible(false);

    // Sidebar
    sidebar = new QTreeView(mainSplitter);
    sidebar->setHeaderHidden(true);
    sidebar->setFixedWidth(220);
    // TODO: Custom delegate for skeuomorphic gradient and selection

    // Main view stack (List, Album, Cover Flow)
    mainViewStack = new QStackedWidget(mainSplitter);

    // --- List View ---
    QWidget *listView = new QWidget();
    QVBoxLayout *listLayout = new QVBoxLayout(listView);
    // TODO: Add column browser (genre/artist/album) here
    trackListView = new QTableView();
    trackListView->setSelectionBehavior(QAbstractItemView::SelectRows);
    trackListView->horizontalHeader()->setStretchLastSection(true);
    trackListView->verticalHeader()->setVisible(false);
    trackListView->setShowGrid(false);
    listLayout->addWidget(trackListView);
    mainViewStack->addWidget(listView);

    // --- Album View ---
    QWidget *albumView = new QWidget();
    // TODO: Implement grid of album covers
    mainViewStack->addWidget(albumView);

    // --- Cover Flow View ---
    QWidget *coverFlowView = new QWidget();
    QVBoxLayout *coverFlowLayout = new QVBoxLayout(coverFlowView);
    coverFlow = new Flow();
    QTableView *coverFlowTrackList = new QTableView();
    coverFlowLayout->addWidget(coverFlow);
    coverFlowLayout->addWidget(coverFlowTrackList);
    mainViewStack->addWidget(coverFlowView);

    // Add sidebar and main view stack to splitter
    mainSplitter->addWidget(sidebar);
    mainSplitter->addWidget(mainViewStack);
    mainSplitter->setStretchFactor(0, 0);
    mainSplitter->setStretchFactor(1, 1);

    mainLayout->addWidget(mainSplitter, 1);
    setCentralWidget(central);

    // Status Bar (optional, for legacy info)
    QStatusBar *statusBar = new QStatusBar(this);
    setStatusBar(statusBar);

    trackInfoLabel = new QLabel("No song playing", statusBar);
    timeSlider = new QSlider(Qt::Horizontal, statusBar);
    timeLabel = new QLabel("00:00 / 00:00", statusBar);

    QWidget *statusBarWidget = new QWidget();
    QHBoxLayout *statusBarLayout = new QHBoxLayout(statusBarWidget);
    statusBarLayout->addWidget(trackInfoLabel, 1);
    statusBarLayout->addWidget(timeSlider, 2);
    statusBarLayout->addWidget(timeLabel);
    statusBar->addPermanentWidget(statusBarWidget, 1);

    // Connect TopBar view switcher
    connect(topBar, &TopBar::viewSwitched, this, [this](int idx) {
        mainViewStack->setCurrentIndex(idx);
    });

    // TODO: Connect TopBar playback controls, volume, LCD, search, etc.
    // TODO: Implement skeuomorphic QSS and custom painting for all widgets
}

void MainWindow::setupMenuBar()
{
    QMenuBar *menuBar = new QMenuBar(this);
    setMenuBar(menuBar);

    // File Menu
    QMenu *fileMenu = menuBar->addMenu(tr("&File"));
    QAction *openAction = fileMenu->addAction(tr("&Add Folder to Library..."));
    connect(openAction, &QAction::triggered, this, &MainWindow::openFiles);
    fileMenu->addSeparator();
    QAction *exitAction = fileMenu->addAction(tr("E&xit"));
    connect(exitAction, &QAction::triggered, qApp, &QApplication::quit);

    // Edit Menu
    menuBar->addMenu(tr("&Edit"));

    // View Menu
    menuBar->addMenu(tr("&View"));

    // Controls Menu
    QMenu *controlsMenu = menuBar->addMenu(tr("&Controls"));
    QAction *playAction = controlsMenu->addAction(tr("Play"));
    connect(playAction, &QAction::triggered, mediaPlayer, &MediaPlayer::play);
    QAction *pauseAction = controlsMenu->addAction(tr("Pause"));
    connect(pauseAction, &QAction::triggered, mediaPlayer, &MediaPlayer::pause);
    QAction *stopAction = controlsMenu->addAction(tr("Stop"));
    connect(stopAction, &QAction::triggered, mediaPlayer, &MediaPlayer::stop);


    // Share Menu
    menuBar->addMenu(tr("S&hare"));

    // Store Menu
    menuBar->addMenu(tr("S&tore"));

    // Advanced Menu
    menuBar->addMenu(tr("&Advanced"));

    // Window Menu
    menuBar->addMenu(tr("&Window"));

    // Help Menu
    QMenu *helpMenu = menuBar->addMenu(tr("&Help"));
    QAction *aboutAction = helpMenu->addAction(tr("&About SynOS Music Player"));
    connect(aboutAction, &QAction::triggered, this, &MainWindow::about);
}

void MainWindow::createModels()
{
    // Sidebar Model
    sidebarModel = new QStandardItemModel(this);
    QStandardItem *library = new QStandardItem("LIBRARY");
    library->setFlags(library->flags() & ~Qt::ItemIsEditable);
    sidebarModel->appendRow(library);

    QStandardItem *music = new QStandardItem("Music");
    music->setFlags(music->flags() & ~Qt::ItemIsEditable);
    library->appendRow(music);

    sidebar->setModel(sidebarModel);

    // Track List Model
    trackListModel = new QStandardItemModel(this);
    trackListModel->setHorizontalHeaderLabels({"Name", "Time", "Artist", "Album"});
    trackListView->setModel(trackListModel);

    // Cover Flow Model
    coverFlowModel = new QStandardItemModel(this);
    coverFlowModel->setHorizontalHeaderLabels({"Album", "Artist", "Year"});
    coverFlow->setModel(coverFlowModel);
}

void MainWindow::openFiles()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"), "/home", QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if (!dir.isEmpty()) {
        QtConcurrent::run(this, &MainWindow::scanDirectory, dir);
    }
}

void MainWindow::scanDirectory(const QString &path)
{
    QDirIterator it(path, {"*.mp3", "*.flac", "*.m4a"}, QDir::Files, QDirIterator::Subdirectories);
    while (it.hasNext()) {
        QString filePath = it.next();
        mediaPlayer->addToPlaylist(QUrl::fromLocalFile(filePath));

        QFileInfo fileInfo(filePath);
        QList<QStandardItem *> rowItems;
        rowItems << new QStandardItem(fileInfo.baseName());
        rowItems << new QStandardItem(""); // Duration placeholder
        rowItems << new QStandardItem(""); // Artist placeholder
        rowItems << new QStandardItem(""); // Album placeholder

        QMetaObject::invokeMethod(trackListModel, "appendRow", Qt::QueuedConnection, Q_ARG(QList<QStandardItem*>, rowItems));
    }
}


void MainWindow::about()
{
    AboutInfo aboutDialog(this);
    aboutDialog.exec();
}

void MainWindow::updateTrackInfo(const QMediaContent &content)
{
    if (!content.isNull()) {
        QString title = mediaPlayer->metaData(QMediaMetaData::Title).toString();
        QString artist = mediaPlayer->metaData(QMediaMetaData::AlbumArtist).toString();
        if (title.isEmpty()) {
            title = content.request().url().fileName();
        }
        trackInfoLabel->setText(QString("%1 by %2").arg(title, artist));
    } else {
        trackInfoLabel->setText("No song playing");
    }
}

void MainWindow::updateDuration(qint64 duration)
{
    timeSlider->setRange(0, duration / 1000);
    updatePosition(mediaPlayer->position());
}

void MainWindow::updatePosition(qint64 position)
{
    qint64 duration = mediaPlayer->duration();
    if (position > duration) {
        position = duration;
    }
    timeSlider->setValue(position / 1000);

    QTime currentTime((position / 3600000) % 60, (position / 60000) % 60, (position / 1000) % 60);
    QTime totalTime((duration / 3600000) % 60, (duration / 60000) % 60, (duration / 1000) % 60);
    QString format = "mm:ss";
    if (duration > 3600000) {
        format = "hh:mm:ss";
    }
    timeLabel->setText(currentTime.toString(format) + " / " + totalTime.toString(format));
}

void MainWindow::seek(int seconds)
{
    mediaPlayer->setPosition(seconds * 1000);
}
