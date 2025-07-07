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

class QTableView;
class QSplitter;
class QTreeView;
class QLabel;
class QSlider;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void openFiles();
    void about();

private:
    void setupUi();
    void setupMenuBar();
    void createModels();
    void scanDirectory(const QString &path);

    // Main UI components
    TopBar *topBar;
    QSplitter *mainSplitter;
    QSplitter *rightSplitter;
    QTreeView *sidebar;
    QStackedWidget *mainViewStack;
    QTableView *trackListView;
    Flow *coverFlow;

    // Models
    QStandardItemModel *sidebarModel;
    QStandardItemModel *trackListModel;
    QStandardItemModel *coverFlowModel;
    QStandardItemModel *albumViewModel;

    // Media Player
    MediaPlayer *mediaPlayer;

    // Status bar widgets
    QLabel *trackInfoLabel;
    QLabel *timeLabel;
    QSlider *timeSlider;
};

#endif // MAINWINDOW_H
