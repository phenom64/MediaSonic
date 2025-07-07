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
#include "sidebar.h"
#include "lcddisplay.h"
#include <QMediaPlayer>
#include <QMediaPlaylist>
#include <QStyledItemDelegate>
#include <QPainter>
#include <QImageReader>

// StarRatingDelegate for the Rating column
class StarRatingDelegate : public QStyledItemDelegate {
public:
    StarRatingDelegate(QObject *parent = nullptr) : QStyledItemDelegate(parent) {}

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override {
        int rating = index.data().toInt();
        int starCount = 5;
        int starSize = option.rect.height() - 4;
        int spacing = 2;
        int x = option.rect.x() + (option.rect.width() - (starCount * starSize + (starCount - 1) * spacing)) / 2;
        int y = option.rect.y() + 2;
        for (int i = 0; i < starCount; ++i) {
            QRect starRect(x + i * (starSize + spacing), y, starSize, starSize);
            if (i < rating) {
                painter->setBrush(QColor(255, 200, 40));
            } else {
                painter->setBrush(QColor(220, 220, 220));
            }
            painter->setPen(Qt::NoPen);
            drawStar(painter, starRect);
        }
    }

    bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index) override {
        if (event->type() == QEvent::MouseButtonPress || event->type() == QEvent::MouseButtonRelease) {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
            int starCount = 5;
            int starSize = option.rect.height() - 4;
            int spacing = 2;
            int x = option.rect.x() + (option.rect.width() - (starCount * starSize + (starCount - 1) * spacing)) / 2;
            int clicked = (mouseEvent->pos().x() - x) / (starSize + spacing) + 1;
            if (clicked >= 1 && clicked <= 5) {
                model->setData(index, clicked);
            }
            return true;
        }
        return false;
    }

private:
    void drawStar(QPainter *painter, const QRect &rect) const {
        static const QPointF points[10] = {
            QPointF(0.5, 0), QPointF(0.618, 0.382), QPointF(1, 0.382), QPointF(0.691, 0.618), QPointF(0.809, 1),
            QPointF(0.5, 0.764), QPointF(0.191, 1), QPointF(0.309, 0.618), QPointF(0, 0.382), QPointF(0.382, 0.382)
        };
        QPolygonF star;
        for (int i = 0; i < 10; ++i) {
            star << QPointF(rect.x() + points[i].x() * rect.width(), rect.y() + points[i].y() * rect.height());
        }
        painter->drawPolygon(star);
    }
};

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setupUi();
    
    // Create MediaPlayer BEFORE setupMenuBar
    mediaPlayer = new MediaPlayer(this);
    // Connect MediaPlayer signals to TopBar
    connect(mediaPlayer, &MediaPlayer::currentMediaChanged, topBar, [this](const QMediaContent &content) {
        QString title = mediaPlayer->metaData(QMediaMetaData::Title).toString();
        QString artist = mediaPlayer->metaData(QMediaMetaData::AlbumArtist).toString();
        if (title.isEmpty() && !content.isNull()) {
            title = content.request().url().fileName();
        }
        topBar->setTrackInfo(title, artist);
    });
    connect(mediaPlayer, &MediaPlayer::durationChanged, topBar, [this](qint64 duration) {
        topBar->setDuration(duration);
    });
    connect(mediaPlayer, &MediaPlayer::positionChanged, topBar, [this](qint64 position) {
        topBar->setPosition(position);
    });
    // Connect LcdDisplay seekChanged to mediaPlayer->setPosition using public getter
    connect(topBar->getLcdDisplay(), &LcdDisplay::seekChanged, mediaPlayer, &MediaPlayer::setPosition);
    
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

    // TopBar (all playback controls/info go here)
    topBar = new TopBar(this);
    mainLayout->addWidget(topBar);

    // Main splitter (sidebar + main view)
    mainSplitter = new QSplitter(Qt::Horizontal, this);
    mainSplitter->setChildrenCollapsible(false);

    // Sidebar (use custom Sidebar widget)
    sidebar = new Sidebar(mainSplitter);
    sidebar->setFixedWidth(220);
    connect(static_cast<Sidebar*>(sidebar), &Sidebar::itemSelected, this, [this](const QString &category, const QString &item) {
        // TODO: Handle sidebar item selection
    });

    // Main view stack (List, Album, Cover Flow)
    mainViewStack = new QStackedWidget(mainSplitter);

    // --- List View ---
    QWidget *listView = new QWidget();
    QVBoxLayout *listLayout = new QVBoxLayout(listView);
    trackListView = new QTableView();
    trackListView->setSelectionBehavior(QAbstractItemView::SelectRows);
    trackListView->horizontalHeader()->setStretchLastSection(true);
    trackListView->verticalHeader()->setVisible(false);
    trackListView->setShowGrid(false);
    listLayout->addWidget(trackListView);
    mainViewStack->addWidget(listView);

    // --- Album View ---
    QWidget *albumView = new QWidget();
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

    // Status Bar (summary only)
    QStatusBar *statusBar = new QStatusBar(this);
    setStatusBar(statusBar);
    statusBar->showMessage("No music loaded"); // Placeholder, update as needed

    connect(topBar, &TopBar::viewSwitched, this, [this](int idx) {
        mainViewStack->setCurrentIndex(idx);
    });

    connect(trackListView, &QTableView::doubleClicked, this, [this](const QModelIndex &index) {
        if (!index.isValid()) return;
        int row = index.row();
        QMediaPlaylist* playlist = mediaPlayer->getPlaylist();
        if (playlist && row >= 0 && row < playlist->mediaCount()) {
            playlist->setCurrentIndex(row);
            mediaPlayer->play();
        }
    });
}

void MainWindow::setupMenuBar()
{
    QMenuBar *menuBar = new QMenuBar(this);
    setMenuBar(menuBar);

    // File Menu
    QMenu *fileMenu = menuBar->addMenu(tr("&File"));
    QAction *addToLibraryAction = new QAction(tr("Add to Library..."), this);
    addToLibraryAction->setShortcut(QKeySequence::Open);
    connect(addToLibraryAction, &QAction::triggered, this, &MainWindow::openFiles);
    fileMenu->addAction(addToLibraryAction);
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
    // Sidebar Model is now handled by Sidebar widget itself
    // Track List Model
    trackListModel = new QStandardItemModel(this);
    trackListModel->setHorizontalHeaderLabels({"Name", "Time", "Artist", "Album", "Genre", "Rating", "Plays"});
    trackListView->setModel(trackListModel);
    trackListView->setAlternatingRowColors(true);
    trackListView->setSortingEnabled(true);
    trackListView->setSelectionBehavior(QAbstractItemView::SelectRows);
    trackListView->setShowGrid(false);
    trackListView->horizontalHeader()->setStretchLastSection(true);
    trackListView->verticalHeader()->setDefaultSectionSize(22); // Compact row height
    // Add custom delegate for star rating in Rating column
    trackListView->setItemDelegateForColumn(5, new StarRatingDelegate(trackListView));

    // Album View Model (simple: album name, artist, cover art)
    albumViewModel = new QStandardItemModel(this);
    // For now, just one column for album art and info
    // TODO: Use a QListView or QTableView in album view and set this model

    // Cover Flow Model
    coverFlowModel = new QStandardItemModel(this);
    coverFlowModel->setHorizontalHeaderLabels({"Album", "Artist", "Year"});
    coverFlow->setModel(coverFlowModel);
}

void MainWindow::openFiles()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"), QDir::homePath(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if (!dir.isEmpty()) {
        scanDirectory(dir);
    }
}

void MainWindow::scanDirectory(const QString &path)
{
    QDirIterator it(path, QStringList() << "*.mp3" << "*.flac" << "*.m4a" << "*.wav" << "*.ogg", QDir::Files, QDirIterator::Subdirectories);
    QSet<QString> albumsAdded;
    while (it.hasNext()) {
        QString filePath = it.next();
        mediaPlayer->addToPlaylist(QUrl::fromLocalFile(filePath));

        QFileInfo fileInfo(filePath);
        QList<QStandardItem *> rowItems;
        rowItems << new QStandardItem(fileInfo.baseName()); // Name
        rowItems << new QStandardItem(""); // Time
        rowItems << new QStandardItem(""); // Artist
        rowItems << new QStandardItem(""); // Album
        rowItems << new QStandardItem(""); // Genre
        rowItems << new QStandardItem(""); // Rating
        rowItems << new QStandardItem("0"); // Plays
        trackListModel->appendRow(rowItems);

        // --- Album View & Cover Flow population ---
        // Try to extract metadata (album, artist, cover art)
        QMediaPlayer tempPlayer;
        tempPlayer.setMedia(QUrl::fromLocalFile(filePath));
        tempPlayer.stop(); // Just to load metadata
        QString album = tempPlayer.metaData(QMediaMetaData::AlbumTitle).toString();
        QString artist = tempPlayer.metaData(QMediaMetaData::AlbumArtist).toString();
        QString year = tempPlayer.metaData(QMediaMetaData::Year).toString();
        QVariant coverArt = tempPlayer.metaData(QMediaMetaData::CoverArtImage);
        if (!album.isEmpty() && !albumsAdded.contains(album)) {
            QStandardItem *albumItem = new QStandardItem(album);
            albumItem->setData(artist, Qt::UserRole + 1);
            if (coverArt.isValid()) {
                QPixmap pixmap = coverArt.value<QPixmap>();
                if (!pixmap.isNull()) {
                    albumItem->setIcon(pixmap.scaled(64, 64, Qt::KeepAspectRatio, Qt::SmoothTransformation));
                } else {
                    // Use placeholder
                    albumItem->setIcon(QPixmap(":/gfx/icons/music.png").scaled(64, 64));
                }
            } else {
                albumItem->setIcon(QPixmap(":/gfx/icons/music.png").scaled(64, 64));
            }
            albumViewModel->appendRow(albumItem);
            albumsAdded.insert(album);
        }
        // Cover Flow
        QList<QStandardItem*> coverRow;
        coverRow << new QStandardItem(album);
        coverRow << new QStandardItem(artist);
        coverRow << new QStandardItem(year);
        coverFlowModel->appendRow(coverRow);
    }
}

void MainWindow::about()
{
    AboutInfo aboutDialog(this);
    aboutDialog.exec();
}
