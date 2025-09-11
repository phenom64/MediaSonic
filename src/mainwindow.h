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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStandardItemModel>
#include "flow.h"
#include "mediaplayer.h"
#include "topbar.h"
#include <QStackedWidget>
#include <QSortFilterProxyModel>

// Forward declarations for MS namespace types used as pointers
namespace MS { class TrackModel; class VisualizerBridge; class Scanner; }

class QTableView;
class QSplitter;
class QTreeView;
class QLabel;
class QSlider;
class QListView;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void addFiles();
    void addFolder();
    void addToLibrary();
    void about();

private:
    void setupUi();
    void setupMenuBar();
    void createModels();
    void scanDirectory(const QString &path);
    void updateStatusSummary();

    // Main UI components
    TopBar *topBar;
    QWidget *viewHeader;
    QPushButton *albumsButton;
    QPushButton *artistsButton;
    QPushButton *genresButton;
    QPushButton *composersButton;
    QSplitter *mainSplitter;
    QSplitter *rightSplitter;
    QTreeView *sidebar;
    QStackedWidget *mainViewStack;
    QTableView *trackListView;
    QListView *albumListView;
    QTableView *coverFlowTrackList;
    Flow *coverFlow;
    QGridLayout *albumGridLayout; // legacy, unused for list view

    // Models
    QStandardItemModel *sidebarModel;
    MS::TrackModel *trackListModel;
    QSortFilterProxyModel *trackProxyModel;
    QStandardItemModel *coverFlowModel;
    QStandardItemModel *albumViewModel;

    // Media Player & services
    MediaPlayer *mediaPlayer;
    MS::VisualizerBridge *visualizer;
    MS::Scanner *scanner;

    // Status bar widgets
    QLabel *trackInfoLabel;
    QLabel *timeLabel;
    QSlider *timeSlider;
    QLabel *statusSummaryLabel;
};

#endif // MAINWINDOW_H
