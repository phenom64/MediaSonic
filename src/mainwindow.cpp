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
#include <QListView>
#include "sidebar.h"
#include "lcddisplay.h"
#include "dialogs/filepicker.h"
#include <QMediaPlayer>
#include <QMediaPlaylist>
#include <QStyledItemDelegate>
#include <QPainter>
#include <QImageReader>
#include <QSortFilterProxyModel>
#include <QRegularExpression>
#include "models/trackmodel.h"
#include "services/scanner.h"
#include "visualizer/visualizerbridge.h"
#include <KLocalizedString>

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
    scanner = nullptr;
    trackProxyModel = nullptr;
    visualizer = nullptr;
    
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
    connect(mediaPlayer, &MediaPlayer::stateChanged, topBar, [this](QMediaPlayer::State st){
        topBar->setPlayState(st == QMediaPlayer::PlayingState);
    });
    // Connect LcdDisplay seekChanged to mediaPlayer->setPosition using public getter
    connect(topBar->getLcdDisplay(), &LcdDisplay::seekChanged, mediaPlayer, &MediaPlayer::setPosition);
    // Wire TopBar controls
    connect(topBar, &TopBar::playClicked,   mediaPlayer, &MediaPlayer::play);
    connect(topBar, &TopBar::pauseClicked,  mediaPlayer, &MediaPlayer::pause);
    connect(topBar, &TopBar::stopClicked,   mediaPlayer, &MediaPlayer::stop);
    connect(topBar, &TopBar::previousClicked, mediaPlayer, &MediaPlayer::previous);
    connect(topBar, &TopBar::nextClicked,     mediaPlayer, &MediaPlayer::next);
    connect(topBar, &TopBar::volumeChanged,   mediaPlayer, &MediaPlayer::setVolume);
    // Visualizer bridge
    visualizer = new MS::VisualizerBridge(mediaPlayer, this);
    connect(visualizer, &MS::VisualizerBridge::levelsUpdated, topBar->getLcdDisplay(), &LcdDisplay::setLevels);
    
    setupMenuBar();
    createModels();

    // Search integration once models exist
    connect(topBar, &TopBar::searchTextChanged, this, [this](const QString &text){
        if (trackProxyModel) {
            trackProxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
            trackProxyModel->setFilterKeyColumn(-1);
            trackProxyModel->setFilterRegularExpression(QRegularExpression(QRegularExpression::escape(text), QRegularExpression::CaseInsensitiveOption));
        }
    });

    // Connect media player to update status bar (mediaPlayer is valid here)
    connect(mediaPlayer, &MediaPlayer::currentMediaChanged, this, &MainWindow::updateStatusSummary);
    connect(mediaPlayer, &MediaPlayer::durationChanged, this, &MainWindow::updateStatusSummary);
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

    // View Header (Albums, Artists, Genres, Composers buttons)
    viewHeader = new QWidget(this);
    QHBoxLayout *viewHeaderLayout = new QHBoxLayout(viewHeader);
    viewHeaderLayout->setContentsMargins(10, 5, 10, 5);
    viewHeaderLayout->setSpacing(10);
    
    // Create view toggle buttons
    albumsButton = new QPushButton("Albums", viewHeader);
    artistsButton = new QPushButton("Artists", viewHeader);
    genresButton = new QPushButton("Genres", viewHeader);
    composersButton = new QPushButton("Composers", viewHeader);
    
    // Style the buttons to match iTunes 9
    QString buttonStyle = 
        "QPushButton {"
        "    background: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
        "        stop:0 #f8f8f8, stop:1 #e8e8e8);"
        "    border: 1px solid #c0c0c0;"
        "    border-radius: 3px;"
        "    padding: 4px 12px;"
        "    color: #404040;"
        "    font-size: 11px;"
        "    font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "    background: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
        "        stop:0 #f0f0f0, stop:1 #e0e0e0);"
        "}"
        "QPushButton:pressed {"
        "    background: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
        "        stop:0 #e0e0e0, stop:1 #d0d0d0);"
        "}"
        "QPushButton:checked {"
        "    background: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
        "        stop:0 #0078d4, stop:1 #0063b1);"
        "    color: white;"
        "    border: 1px solid #005a9e;"
        "}";
    
    albumsButton->setStyleSheet(buttonStyle);
    artistsButton->setStyleSheet(buttonStyle);
    genresButton->setStyleSheet(buttonStyle);
    composersButton->setStyleSheet(buttonStyle);
    
    // Make buttons checkable
    albumsButton->setCheckable(true);
    artistsButton->setCheckable(true);
    genresButton->setCheckable(true);
    composersButton->setCheckable(true);
    
    // Set Albums as default selected
    albumsButton->setChecked(true);
    
    // Add buttons to layout
    viewHeaderLayout->addWidget(albumsButton);
    viewHeaderLayout->addWidget(artistsButton);
    viewHeaderLayout->addWidget(genresButton);
    viewHeaderLayout->addWidget(composersButton);
    viewHeaderLayout->addStretch();
    
    // Connect button signals
    connect(albumsButton, &QPushButton::clicked, this, [this]() {
        albumsButton->setChecked(true);
        artistsButton->setChecked(false);
        genresButton->setChecked(false);
        composersButton->setChecked(false);
    });
    connect(artistsButton, &QPushButton::clicked, this, [this]() {
        albumsButton->setChecked(false);
        artistsButton->setChecked(true);
        genresButton->setChecked(false);
        composersButton->setChecked(false);
    });
    connect(genresButton, &QPushButton::clicked, this, [this]() {
        albumsButton->setChecked(false);
        artistsButton->setChecked(false);
        genresButton->setChecked(true);
        composersButton->setChecked(false);
    });
    connect(composersButton, &QPushButton::clicked, this, [this]() {
        albumsButton->setChecked(false);
        artistsButton->setChecked(false);
        genresButton->setChecked(false);
        composersButton->setChecked(true);
    });
    
    mainLayout->addWidget(viewHeader);

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

    // --- Album View (QListView in IconMode) ---
    QWidget *albumView = new QWidget();
    QVBoxLayout *albumLayout = new QVBoxLayout(albumView);
    albumLayout->setContentsMargins(10, 10, 10, 10);
    albumLayout->setSpacing(10);
    albumListView = new QListView(albumView);
    albumListView->setViewMode(QListView::IconMode);
    albumListView->setResizeMode(QListView::Adjust);
    albumListView->setMovement(QListView::Static);
    albumListView->setSpacing(12);
    albumListView->setIconSize(QSize(128,128));
    albumLayout->addWidget(albumListView);
    mainViewStack->addWidget(albumView);

    // --- Cover Flow View ---
    QWidget *coverFlowView = new QWidget();
    QVBoxLayout *coverFlowLayout = new QVBoxLayout(coverFlowView);
    coverFlowLayout->setContentsMargins(0, 0, 0, 0);
    coverFlowLayout->setSpacing(0);
    
    // Create vertical splitter for Cover Flow view
    QSplitter *coverFlowSplitter = new QSplitter(Qt::Vertical, coverFlowView);
    coverFlowSplitter->setChildrenCollapsible(false);
    
    // Flow widget (top)
    coverFlow = new Flow();
    coverFlowSplitter->addWidget(coverFlow);
    
    // Track list (bottom)
    coverFlowTrackList = new QTableView();
    // Model is set later in createModels()
    coverFlowTrackList->setSelectionBehavior(QAbstractItemView::SelectRows);
    coverFlowTrackList->horizontalHeader()->setStretchLastSection(true);
    coverFlowTrackList->verticalHeader()->setVisible(false);
    coverFlowTrackList->setShowGrid(false);
    coverFlowSplitter->addWidget(coverFlowTrackList);
    
    // Set splitter proportions (70% Flow, 30% Track List)
    coverFlowSplitter->setSizes(QList<int>() << 400 << 200);
    
    coverFlowLayout->addWidget(coverFlowSplitter);
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
    
    // Create summary label
    statusSummaryLabel = new QLabel("No music loaded", statusBar);
    statusBar->addWidget(statusSummaryLabel);
    
    // Add speaker icon on the right
    QPushButton *speakerIcon = new QPushButton(statusBar);
    speakerIcon->setIcon(QIcon(":/gfx/icons/volume.png"));
    speakerIcon->setFixedSize(20, 20);
    speakerIcon->setStyleSheet(
        "QPushButton {"
        "    border: none;"
        "    background: transparent;"
        "    padding: 0px;"
        "}"
        "QPushButton:hover {"
        "    background-color: rgba(0, 0, 0, 0.1);"
        "    border-radius: 2px;"
        "}"
    );
    statusBar->addPermanentWidget(speakerIcon);

    connect(topBar, &TopBar::viewSwitched, this, [this](int idx) {
        mainViewStack->setCurrentIndex(idx);
    });
    
    connect(trackListView, &QTableView::doubleClicked, this, [this](const QModelIndex &index) {
        if (!index.isValid()) return;
        QModelIndex srcIdx = index;
        if (trackProxyModel) srcIdx = trackProxyModel->mapToSource(index);
        int row = srcIdx.row();
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
    QAction *addLibraryAction = new QAction(tr("Add to Library..."), this);
    addLibraryAction->setShortcut(QKeySequence::Open);
    connect(addLibraryAction, &QAction::triggered, this, &MainWindow::addToLibrary);
    fileMenu->addAction(addLibraryAction);
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
    // Sidebar Model handled by Sidebar widget itself
    // Track List Model (typed)
    trackListModel = new MS::TrackModel(this);
    trackProxyModel = new QSortFilterProxyModel(this);
    trackProxyModel->setSourceModel(trackListModel);
    trackProxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    trackProxyModel->setDynamicSortFilter(true);
    trackListView->setModel(trackProxyModel);
    if (coverFlowTrackList) coverFlowTrackList->setModel(trackProxyModel);
    trackListView->setAlternatingRowColors(true);
    trackListView->setSortingEnabled(true);
    trackListView->setSelectionBehavior(QAbstractItemView::SelectRows);
    trackListView->setShowGrid(false);
    trackListView->horizontalHeader()->setStretchLastSection(true);
    trackListView->verticalHeader()->setDefaultSectionSize(22); // Compact row height
    
    // Style the track list to match iTunes 9
    trackListView->setStyleSheet(
        "QTableView {"
        "    background-color: white;"
        "    alternate-background-color: #f8f8f8;"
        "    gridline-color: #e0e0e0;"
        "    selection-background-color: #0078d4;"
        "    selection-color: white;"
        "}"
        "QTableView::item {"
        "    padding: 2px;"
        "    border: none;"
        "}"
        "QTableView::item:selected {"
        "    background-color: #0078d4;"
        "    color: white;"
        "}"
        "QHeaderView::section {"
        "    background-color: #f0f0f0;"
        "    border: 1px solid #d0d0d0;"
        "    padding: 4px;"
        "    font-weight: bold;"
        "    color: #404040;"
        "}"
    );
    if (coverFlowTrackList)
        coverFlowTrackList->setStyleSheet(trackListView->styleSheet());
    
    // Add custom delegate for star rating in Rating column (proxy column index preserved)
    trackListView->setItemDelegateForColumn(MS::TrackModel::ColRating, new StarRatingDelegate(trackListView));
    if (coverFlowTrackList)
        coverFlowTrackList->setItemDelegateForColumn(MS::TrackModel::ColRating, new StarRatingDelegate(coverFlowTrackList));

    // Album View Model (simple: album name, artist, cover art)
    albumViewModel = new QStandardItemModel(this);
    if (albumListView) albumListView->setModel(albumViewModel);

    // Cover Flow Model
    coverFlowModel = new QStandardItemModel(this);
    coverFlowModel->setHorizontalHeaderLabels({"Album", "Artist", "Year"});
    coverFlow->setModel(coverFlowModel);
    // Opened album -> play first track matching album
    connect(coverFlow, &Flow::opened, this, [this](const QModelIndex &idx){
        if (!idx.isValid()) return;
        QString album = coverFlowModel->index(idx.row(), 0).data().toString();
        // Find first matching track
        int srcRow = -1;
        for (int r = 0; r < trackListModel->rowCount(); ++r) {
            QModelIndex aIdx = trackListModel->index(r, MS::TrackModel::ColAlbum);
            if (aIdx.data().toString() == album) { srcRow = r; break; }
        }
        if (srcRow >= 0) {
            QMediaPlaylist *pl = mediaPlayer->getPlaylist();
            if (pl && srcRow < pl->mediaCount()) {
                pl->setCurrentIndex(srcRow);
                mediaPlayer->play();
            }
        }
    });
}

void MainWindow::addFiles()
{
    QStringList files = QFileDialog::getOpenFileNames(
        this,
        tr("Add Files"),
        QDir::homePath(),
        tr("Audio Files (*.mp3 *.flac *.m4a *.wav *.ogg *.aac *.opus *.aiff *.wma);;All Files (*)"));
    if (!files.isEmpty()) {
        if (!scanner) {
            scanner = new MS::Scanner(this);
            connect(scanner, &MS::Scanner::trackDiscovered, this, [this](const MS::Track &t){
                mediaPlayer->addToPlaylist(t.url);
                trackListModel->addTrack(t);
                // Basic album & Cover Flow population
                QString album = t.album;
                QString artist = t.artist;
                QString year = t.year ? QString::number(t.year) : QString();
                if (!album.isEmpty()) {
                    bool found=false; for (int r=0;r<albumViewModel->rowCount();++r) if (albumViewModel->item(r)->text()==album) {found=true;break;}
                    if (!found) {
                        QStandardItem *albumItem = new QStandardItem(album);
                        albumItem->setData(artist, Qt::UserRole + 1);
                        albumItem->setIcon(QPixmap(":/gfx/icons/music.png").scaled(64,64));
                        albumViewModel->appendRow(albumItem);
                    }
                }
                QList<QStandardItem*> coverRow;
                coverRow << new QStandardItem(album);
                coverRow << new QStandardItem(artist);
                coverRow << new QStandardItem(year);
                coverFlowModel->appendRow(coverRow);
                updateStatusSummary();
            });
        }
        scanner->scanPaths(files);
    }
}

void MainWindow::addFolder()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Add Folder"), QDir::homePath(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if (!dir.isEmpty()) {
        scanDirectory(dir);
    }
}

void MainWindow::addToLibrary()
{
    FilePickerDialog dlg(this);
    if (dlg.exec() != QDialog::Accepted) return;
    QStringList files = dlg.selectedPaths();
    if (files.isEmpty()) return;
    if (!scanner) {
        scanner = new MS::Scanner(this);
        connect(scanner, &MS::Scanner::trackDiscovered, this, [this](const MS::Track &t){
            mediaPlayer->addToPlaylist(t.url);
            trackListModel->addTrack(t);
            QString album = t.album;
            QString artist = t.artist;
            QString year = t.year ? QString::number(t.year) : QString();
            if (!album.isEmpty()) {
                bool found=false; for (int r=0;r<albumViewModel->rowCount();++r) if (albumViewModel->item(r)->text()==album) {found=true;break;}
                if (!found) {
                    QStandardItem *albumItem = new QStandardItem(album);
                    albumItem->setData(artist, Qt::UserRole + 1);
                    albumItem->setIcon(QPixmap(":/gfx/icons/music.png").scaled(128,128, Qt::KeepAspectRatio, Qt::SmoothTransformation));
                    albumViewModel->appendRow(albumItem);
                }
            }
            QList<QStandardItem*> coverRow;
            QStandardItem *albumCell = new QStandardItem(album);
            albumCell->setData(QIcon(":/gfx/icons/music.png"), Qt::DecorationRole);
            coverRow << albumCell;
            coverRow << new QStandardItem(artist);
            coverRow << new QStandardItem(year);
            coverFlowModel->appendRow(coverRow);
            updateStatusSummary();
        });
    }
    scanner->scanPaths(files);
}

void MainWindow::scanDirectory(const QString &path)
{
    if (!scanner) {
        scanner = new MS::Scanner(this);
        connect(scanner, &MS::Scanner::trackDiscovered, this, [this](const MS::Track &t){
            // Add to playlist in same order
            mediaPlayer->addToPlaylist(t.url);
            trackListModel->addTrack(t);
            // Album/CoverFlow population (simple for now)
            QString album = t.album;
            QString artist = t.artist;
            QString year = t.year ? QString::number(t.year) : QString();
            if (!album.isEmpty()) {
                bool found=false; for (int r=0;r<albumViewModel->rowCount();++r) if (albumViewModel->item(r)->text()==album) {found=true;break;}
                if (!found) {
                    QStandardItem *albumItem = new QStandardItem(album);
                    albumItem->setData(artist, Qt::UserRole + 1);
                    albumItem->setIcon(QPixmap(":/gfx/icons/music.png").scaled(64,64));
                    albumViewModel->appendRow(albumItem);
                }
            }
            QList<QStandardItem*> coverRow;
            QStandardItem *albumCell = new QStandardItem(album);
            albumCell->setData(QIcon(":/gfx/icons/music.png"), Qt::DecorationRole);
            coverRow << albumCell;
            coverRow << new QStandardItem(artist);
            coverRow << new QStandardItem(year);
            coverFlowModel->appendRow(coverRow);
            updateStatusSummary();
        });
    }
    scanner->scanDirectory(path);
}

void MainWindow::about()
{
    AboutInfo aboutDialog(this);
    aboutDialog.exec();
}

void MainWindow::updateStatusSummary()
{
    if (!statusSummaryLabel) return;
    
    int trackCount = trackListModel ? trackListModel->rowCount() : 0;
    if (trackCount == 0) {
        statusSummaryLabel->setText(i18n("No music loaded"));
        return;
    }
    
    // Calculate total duration from model
    qint64 totalDuration = trackListModel->totalDurationMs();
    
    int totalMinutes = totalDuration / 60000;
    int totalSeconds = (totalDuration % 60000) / 1000;
    
    // Calculate total size (placeholder until file sizes are stored)
    qint64 totalSize = trackListModel->totalSizeBytes();
    double totalSizeMB = totalSize / (1024.0 * 1024.0);
    
    QString summary = QString("%1 %2, %3:%4, %5 MB")
                     .arg(trackCount)
                     .arg(trackCount == 1 ? i18n("track") : i18n("tracks"))
                     .arg(totalMinutes)
                     .arg(totalSeconds, 2, 10, QChar('0'))
                     .arg(QString::number(totalSizeMB, 'f', 1));
    
    statusSummaryLabel->setText(summary);
}
