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
#include "aboutdialog.h"
#include <QApplication>
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
#include <QDirIterator>
#include <QtConcurrent/QtConcurrent>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setupUi();
    setupMenuBar();
    createModels();

    mediaPlayer = new MediaPlayer(this);
    connect(mediaPlayer, &MediaPlayer::currentMediaChanged, this, &MainWindow::updateTrackInfo);
    connect(mediaPlayer, &MediaPlayer::durationChanged, this, &MainWindow::updateDuration);
    connect(mediaPlayer, &MediaPlayer::positionChanged, this, &MainWindow::updatePosition);
    connect(timeSlider, &QSlider::sliderMoved, this, &MainWindow::seek);
}

MainWindow::~MainWindow()
{
}

void MainWindow::setupUi()
{
    setWindowTitle("Music");
    resize(1024, 768);

    // Main layout
    mainSplitter = new QSplitter(Qt::Horizontal, this);
    setCentralWidget(mainSplitter);

    // Sidebar
    sidebar = new QTreeView(mainSplitter);
    sidebar->setHeaderHidden(true);
    sidebar->setFixedWidth(200);

    // Right side (Cover Flow and Track List)
    rightSplitter = new QSplitter(Qt::Vertical, mainSplitter);

    // Cover Flow
    coverFlow = new KDFM::Flow(rightSplitter);

    // Track List
    trackListView = new QTableView(rightSplitter);
    trackListView->setSelectionBehavior(QAbstractItemView::SelectRows);
    trackListView->horizontalHeader()->setStretchLastSection(true);
    trackListView->verticalHeader()->setVisible(false);
    trackListView->setShowGrid(false);

    rightSplitter->addWidget(coverFlow);
    rightSplitter->addWidget(trackListView);
    rightSplitter->setStretchFactor(0, 1);
    rightSplitter->setStretchFactor(1, 1);

    mainSplitter->addWidget(sidebar);
    mainSplitter->addWidget(rightSplitter);
    mainSplitter->setStretchFactor(0, 0);
    mainSplitter->setStretchFactor(1, 1);

    // Status Bar
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
    // The flow widget from DocSurf seems to use a specific model type.
    // This will need careful integration. For now, we'll use a standard model.
    // coverFlow->setModel(coverFlowModel);
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
    AboutDialog aboutDialog(this);
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
