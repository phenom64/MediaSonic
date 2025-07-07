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

#include "sidebar.h"
#include <QPainter>
#include <QMouseEvent>
#include <QApplication>
#include <QStyleOptionViewItem>
#include <QFontMetrics>

// ============================================================================
// Sidebar Implementation
// ============================================================================

Sidebar::Sidebar(QWidget *parent)
    : QTreeView(parent)
    , delegate(new SidebarDelegate(this))
    , model(new QStandardItemModel(this))
    , backgroundGradient(nullptr)
    , selectionGradient(nullptr)
    , headerFont(new QFont("Lucida Grande", 10, QFont::Bold))
    , itemFont(new QFont("Lucida Grande", 9))
{
    // Initialize colors
    backgroundColor = QColor(240, 245, 250);
    selectionColor = QColor(100, 150, 255);
    headerColor = QColor(80, 100, 120);
    itemColor = QColor(60, 60, 60);
    borderColor = QColor(180, 180, 180);

    setupModel();
    setupStyling();
    createGradients();
    setupDefaultItems();
}

Sidebar::~Sidebar()
{
    delete headerFont;
    delete itemFont;
    delete backgroundGradient;
    delete selectionGradient;
}

void Sidebar::setupModel()
{
    setModel(model);
    setItemDelegate(delegate);
    setHeaderHidden(true);
    setRootIsDecorated(false);
    setIndentation(10);
    setUniformRowHeights(true);
}

void Sidebar::setupStyling()
{
    // Set selection behavior
    setSelectionBehavior(QAbstractItemView::SelectRows);
    setSelectionMode(QAbstractItemView::SingleSelection);
    
    // Disable focus rectangle
    setFocusPolicy(Qt::NoFocus);
    
    // Set background
    setAutoFillBackground(true);
    QPalette pal = palette();
    pal.setColor(QPalette::Window, backgroundColor);
    setPalette(pal);
}

void Sidebar::createGradients()
{
    // Background gradient (blueish to grey)
    backgroundGradient = new QLinearGradient(0, 0, 0, height());
    backgroundGradient->setColorAt(0, QColor(245, 250, 255));
    backgroundGradient->setColorAt(1, QColor(235, 240, 245));

    // Selection gradient
    selectionGradient = new QLinearGradient(0, 0, 0, 25);
    selectionGradient->setColorAt(0, QColor(120, 170, 255));
    selectionGradient->setColorAt(1, QColor(100, 150, 255));
}

void Sidebar::setupDefaultItems()
{
    // LIBRARY section
    QStandardItem *libraryHeader = new QStandardItem("LIBRARY");
    libraryHeader->setFlags(libraryHeader->flags() & ~Qt::ItemIsSelectable);
    model->appendRow(libraryHeader);

    // Add default library items
    addLibraryItem("Music", QIcon(":/gfx/icons/music.png"));
    addLibraryItem("Movies", QIcon(":/gfx/icons/movies.png"));
    addLibraryItem("TV Shows", QIcon(":/gfx/icons/tv.png"));
    addLibraryItem("Radio", QIcon(":/gfx/icons/radio.png"));

    // SHARED section
    QStandardItem *sharedHeader = new QStandardItem("SHARED");
    sharedHeader->setFlags(sharedHeader->flags() & ~Qt::ItemIsSelectable);
    model->appendRow(sharedHeader);
    QStandardItem *homeSharing = new QStandardItem(QIcon(":/gfx/icons/home_sharing.png"), "Home Sharing");
    homeSharing->setData("shared", Qt::UserRole);
    sharedHeader->appendRow(homeSharing);

    // GENIUS section
    QStandardItem *geniusHeader = new QStandardItem("GENIUS");
    geniusHeader->setFlags(geniusHeader->flags() & ~Qt::ItemIsSelectable);
    model->appendRow(geniusHeader);
    QStandardItem *genius = new QStandardItem(QIcon(":/gfx/icons/genius.png"), "Genius");
    genius->setData("genius", Qt::UserRole);
    geniusHeader->appendRow(genius);

    // PLAYLISTS section
    QStandardItem *playlistsHeader = new QStandardItem("PLAYLISTS");
    playlistsHeader->setFlags(playlistsHeader->flags() & ~Qt::ItemIsSelectable);
    model->appendRow(playlistsHeader);

    // Add default playlists (with icons)
    addPlaylistItem("iTunes DJ", QIcon(":/gfx/icons/dj.png"));
    addPlaylistItem("90's Music", QIcon(":/gfx/icons/90s.png"));
    addPlaylistItem("Classical Music", QIcon(":/gfx/icons/classical.png"));
    addPlaylistItem("Music Videos", QIcon(":/gfx/icons/videos.png"));
    addPlaylistItem("My Top Rated", QIcon(":/gfx/icons/top_rated.png"));
    addPlaylistItem("Recently Added", QIcon(":/gfx/icons/recent.png"));
    addPlaylistItem("Recently Played", QIcon(":/gfx/icons/recent_played.png"));
    addPlaylistItem("Top 25 Most Played", QIcon(":/gfx/icons/top25.png"));

    // Example: Playlist folder (collapsed by default)
    QStandardItem *folder = new QStandardItem(QIcon(":/gfx/icons/folder.png"), "My Playlists");
    folder->setData("playlist_folder", Qt::UserRole);
    playlistsHeader->appendRow(folder);
    QStandardItem *customPlaylist = new QStandardItem(QIcon(":/gfx/icons/playlist.png"), "Custom Playlist");
    customPlaylist->setData("playlist", Qt::UserRole);
    folder->appendRow(customPlaylist);
}

void Sidebar::addLibraryItem(const QString &name, const QIcon &icon)
{
    QStandardItem *item = new QStandardItem(icon, name);
    item->setData("library", Qt::UserRole);
    model->item(0)->appendRow(item); // LIBRARY header is always at index 0
}

void Sidebar::addPlaylistItem(const QString &name, const QIcon &icon)
{
    QStandardItem *item = new QStandardItem(icon, name);
    item->setData("playlist", Qt::UserRole);
    // PLAYLISTS header index is now dynamic, so find it:
    for (int i = 0; i < model->rowCount(); ++i) {
        if (model->item(i)->text() == "PLAYLISTS") {
            model->item(i)->appendRow(item);
            break;
        }
    }
}

void Sidebar::paintEvent(QPaintEvent *event)
{
    QTreeView::paintEvent(event);
    
    QPainter painter(viewport());
    painter.setRenderHint(QPainter::Antialiasing);

    // Draw background gradient
    painter.fillRect(viewport()->rect(), *backgroundGradient);
}

void Sidebar::mousePressEvent(QMouseEvent *event)
{
    QTreeView::mousePressEvent(event);
    
    QModelIndex index = indexAt(event->pos());
    if (index.isValid()) {
        QString category = index.data(Qt::UserRole).toString();
        QString itemName = index.data(Qt::DisplayRole).toString();
        
        if (!category.isEmpty() && !itemName.isEmpty()) {
            emit itemSelected(category, itemName);
        }
    }
}

// ============================================================================
// SidebarDelegate Implementation
// ============================================================================

SidebarDelegate::SidebarDelegate(QObject *parent)
    : QStyledItemDelegate(parent)
    , headerGradient(nullptr)
    , itemGradient(nullptr)
    , selectionGradient(nullptr)
    , headerFont(new QFont("Lucida Grande", 10, QFont::Bold))
    , itemFont(new QFont("Lucida Grande", 9))
{
    // Initialize colors
    headerColor = QColor(80, 100, 120);
    itemColor = QColor(60, 60, 60);
    selectionColor = QColor(100, 150, 255);
    selectionTextColor = QColor(255, 255, 255);

    // Create gradients
    headerGradient = new QLinearGradient(0, 0, 0, 25);
    headerGradient->setColorAt(0, QColor(220, 225, 230));
    headerGradient->setColorAt(1, QColor(200, 205, 210));

    itemGradient = new QLinearGradient(0, 0, 0, 25);
    itemGradient->setColorAt(0, QColor(245, 250, 255));
    itemGradient->setColorAt(1, QColor(235, 240, 245));

    selectionGradient = new QLinearGradient(0, 0, 0, 25);
    selectionGradient->setColorAt(0, QColor(120, 170, 255));
    selectionGradient->setColorAt(1, QColor(100, 150, 255));
}

SidebarDelegate::~SidebarDelegate()
{
    delete headerFont;
    delete itemFont;
    delete headerGradient;
    delete itemGradient;
    delete selectionGradient;
}

void SidebarDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, 
                           const QModelIndex &index) const
{
    painter->setRenderHint(QPainter::Antialiasing);

    QRect rect = option.rect;
    bool isSelected = option.state & QStyle::State_Selected;
    bool isHeader = !index.parent().isValid(); // Root items are headers

    if (isHeader) {
        paintHeader(painter, option, index);
    } else {
        paintItem(painter, option, index);
    }
}

void SidebarDelegate::paintHeader(QPainter *painter, const QStyleOptionViewItem &option, 
                                 const QModelIndex &index) const
{
    QRect rect = option.rect;
    
    // Draw header background
    painter->fillRect(rect, *headerGradient);
    
    // Draw header text
    painter->setFont(*headerFont);
    painter->setPen(QPen(headerColor));
    
    QString text = index.data(Qt::DisplayRole).toString();
    painter->drawText(rect.adjusted(10, 0, -10, 0), 
                     Qt::AlignLeft | Qt::AlignVCenter, text);
    
    // Draw subtle border
    painter->setPen(QPen(QColor(180, 180, 180), 1));
    painter->drawLine(rect.left(), rect.bottom(), rect.right(), rect.bottom());
}

void SidebarDelegate::paintItem(QPainter *painter, const QStyleOptionViewItem &option, 
                               const QModelIndex &index) const
{
    QRect rect = option.rect;
    bool isSelected = option.state & QStyle::State_Selected;
    
    // Draw background
    if (isSelected) {
        painter->fillRect(rect, *selectionGradient);
    } else {
        painter->fillRect(rect, *itemGradient);
    }
    
    // Draw icon if available
    QIcon icon = index.data(Qt::DecorationRole).value<QIcon>();
    if (!icon.isNull()) {
        QRect iconRect = rect.adjusted(5, 2, -rect.width() + 25, -2);
        icon.paint(painter, iconRect);
    }
    
    // Draw text
    painter->setFont(*itemFont);
    if (isSelected) {
        painter->setPen(QPen(selectionTextColor));
    } else {
        painter->setPen(QPen(itemColor));
    }
    
    QString text = index.data(Qt::DisplayRole).toString();
    QRect textRect = rect.adjusted(30, 0, -10, 0);
    painter->drawText(textRect, Qt::AlignLeft | Qt::AlignVCenter, text);
}

QSize SidebarDelegate::sizeHint(const QStyleOptionViewItem &option, 
                                const QModelIndex &index) const
{
    Q_UNUSED(option)
    
    bool isHeader = !index.parent().isValid();
    if (isHeader) {
        return QSize(200, 25);
    } else {
        return QSize(200, 22);
    }
} 