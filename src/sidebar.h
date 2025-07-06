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

#ifndef SIDEBAR_H
#define SIDEBAR_H

#include <QTreeView>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QStyledItemDelegate>
#include <QPainter>
#include <QLinearGradient>
#include <QFont>
#include <QIcon>
#include <QMouseEvent>

class SidebarDelegate;

/**
 * @brief Sidebar - iTunes 9-style sidebar with skeuomorphic design
 * 
 * Features:
 * - Blueish-to-grey vertical gradient background
 * - Category headers (LIBRARY, PLAYLISTS)
 * - Custom selection with solid blue background
 * - Icons for different media types
 * - Smooth animations and transitions
 */
class Sidebar : public QTreeView
{
    Q_OBJECT

public:
    explicit Sidebar(QWidget *parent = nullptr);
    ~Sidebar();

    void setupModel();
    void addLibraryItem(const QString &name, const QIcon &icon = QIcon());
    void addPlaylistItem(const QString &name, const QIcon &icon = QIcon());

signals:
    void itemSelected(const QString &category, const QString &item);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

private:
    void setupStyling();
    void createGradients();
    void setupDefaultItems();

    SidebarDelegate *delegate;
    QStandardItemModel *model;
    
    // Styling
    QLinearGradient *backgroundGradient;
    QLinearGradient *selectionGradient;
    QFont *headerFont;
    QFont *itemFont;
    
    // Colors
    QColor backgroundColor;
    QColor selectionColor;
    QColor headerColor;
    QColor itemColor;
    QColor borderColor;
};

/**
 * @brief SidebarDelegate - Custom delegate for sidebar items
 * 
 * Handles:
 * - Custom painting for headers vs items
 * - Gradient backgrounds
 * - Selection highlighting
 * - Icon positioning
 */
class SidebarDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    explicit SidebarDelegate(QObject *parent = nullptr);
    virtual ~SidebarDelegate();

protected:
    void paint(QPainter *painter, const QStyleOptionViewItem &option, 
               const QModelIndex &index) const override;
    QSize sizeHint(const QStyleOptionViewItem &option, 
                   const QModelIndex &index) const override;

private:
    void paintHeader(QPainter *painter, const QStyleOptionViewItem &option, 
                     const QModelIndex &index) const;
    void paintItem(QPainter *painter, const QStyleOptionViewItem &option, 
                   const QModelIndex &index) const;
    
    QLinearGradient *headerGradient;
    QLinearGradient *itemGradient;
    QLinearGradient *selectionGradient;
    
    QFont *headerFont;
    QFont *itemFont;
    
    QColor headerColor;
    QColor itemColor;
    QColor selectionColor;
    QColor selectionTextColor;
};

#endif // SIDEBAR_H 