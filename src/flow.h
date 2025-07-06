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

#ifndef FLOW_H
#define FLOW_H

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QGraphicsPixmapItem>
#include <QGraphicsSimpleTextItem>
#include <QGraphicsProxyWidget>
#include <QGraphicsDropShadowEffect>
#include <QTimeLine>
#include <QGraphicsItemAnimation>
#include <QScrollBar>
#include <QStandardItemModel>
#include <QModelIndex>
#include <QItemSelectionModel>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QResizeEvent>
#include <QShowEvent>
#include <QEnterEvent>
#include <QPainter>
#include <QPainterPath>
#include <QTransform>
#include <QColor>
#include <QBrush>
#include <QPen>
#include <QFont>
#include <QRadialGradient>
#include <QList>
#include <QPoint>
#include <QPointF>
#include <QRect>
#include <QRectF>
#include <QUrl>
#include <QElapsedTimer>
#include <QApplication>
#include <QStyle>
#include <QStyleOption>
#include <QBitmap>
#include <QRegion>
#include <QHash>
#include <QQueue>
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QSurfaceFormat>

/**
 * @brief Flow - Sophisticated 3D Cover Flow widget for album art display
 * 
 * This widget provides a 3D carousel view of album covers,
 * similar to iTunes 9's Cover Flow feature. It includes:
 * - 3D perspective rendering of album covers with reflections
 * - Smooth animations and transitions with QTimeLine
 * - Mouse interaction for navigation
 * - Integration with album metadata
 * - Custom scrollbar with visual effects
 * - Graphics scene with proper item management
 * - OpenGL acceleration for smooth rendering
 */
// Forward declarations
class Flow;
class FlowItem;

class Flow : public QGraphicsView
{
    Q_OBJECT

public:
    enum Pos { Prev = 0, New = 1 };
    
    explicit Flow(QWidget *parent = nullptr);
    ~Flow();

    void setModel(QStandardItemModel *model);
    void setSelectionModel(QItemSelectionModel *model);
    void setCenterIndex(const QModelIndex &index);
    void showCenterIndex(const QModelIndex &index);
    void animateCenterIndex(const QModelIndex &index);
    QModelIndex indexOfItem(FlowItem *item) const;
    bool isAnimating() const;
    float y() const;
    QList<FlowItem *> &items() const;
    QColor &bg() const;

signals:
    void centerIndexChanged(const QModelIndex &centerIndex);
    void opened(const QModelIndex &idx);

public slots:
    void setRootIndex(const QModelIndex &rootIndex);
    void reset();

protected:
    void resizeEvent(QResizeEvent *event) override;
    void showEvent(QShowEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void enterEvent(QEvent *e) override;

private slots:
    void dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight);
    void rowsInserted(const QModelIndex &parent, int start, int end);
    void rowsRemoved(const QModelIndex &parent, int start, int end);
    void clear();
    void animStep(const qreal value);
    void updateItemsPos();
    void scrollBarMoved(const int value);
    void continueIf();
    void updateScene();

private:
    void prepareAnimation();
    void correctItemsPos(const int leftStart, const int rightStart);
    void showPrevious();
    void showNext();

    // Forward declarations for nested classes
    class RootItem;
    class GraphicsScene;
    class ScrollBar;
    class Private;

    Private * const d;
    friend class Private;
    friend class FlowItem;
    friend class RootItem;
    friend class GraphicsScene;
    friend class ScrollBar;
};

/**
 * @brief Custom scrollbar with visual effects
 */
class Flow::ScrollBar : public QScrollBar
{
public:
    QPixmap slider[2], groove;
    explicit ScrollBar(const Qt::Orientation o, QWidget *parent = nullptr) 
        : QScrollBar(o, parent) {}
    void setRange(int min, int max) { clearCache(); QScrollBar::setRange(min, max); }
    void resizeEvent(QResizeEvent *e) { clearCache(); QScrollBar::resizeEvent(e); }
    void clearCache() { for (int i = 0; i < 2; ++i) slider[i] = QPixmap(); groove = QPixmap(); }

protected:
    void paintEvent(QPaintEvent *) override;
};

/**
 * @brief Graphics scene with custom background and foreground
 */
class Flow::GraphicsScene : public QGraphicsScene
{
public:
    Flow *preView;
    QBrush fgBrush, bgBrush;
    inline explicit GraphicsScene(const QRectF &rect = QRectF(), Flow *flow = nullptr)
        : QGraphicsScene(rect, flow)
        , preView(flow) {}
    ~GraphicsScene() {}

protected:
    void drawBackground(QPainter *painter, const QRectF &rect) override { painter->fillRect(rect, bgBrush); }
    void drawForeground(QPainter *painter, const QRectF &rect) override { painter->fillRect(rect, fgBrush); }
};

/**
 * @brief Root item for the graphics scene
 */
class Flow::RootItem : public QGraphicsPixmapItem
{
public:
    explicit RootItem(QGraphicsScene *scene = nullptr) : QGraphicsPixmapItem() { scene->addItem(this); }
    ~RootItem(){}
    QRectF boundingRect() const { return scene()->sceneRect(); }
};

/**
 * @brief Individual album cover item with 3D transformations
 */
class FlowItem : public QGraphicsItem
{
    friend class Flow;
public:
    QPixmap pix[2]; // Main image and reflection
    Flow::GraphicsScene *scene;
    Flow *preView;
    float rotate, savedX;
    bool dirty;
    QPainterPath path;
    
    FlowItem(Flow::GraphicsScene *scene, QGraphicsItem *parent);
    ~FlowItem();
    
    void transform(const float angle, const Qt::Axis axis, const float xscale = 1.0f, const float yscale = 1.0f);
    QRectF boundingRect() const override;
    void saveX();
    QPainterPath shape() const override;
    QModelIndex index();
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    void updateIcon();
    void updateShape();
private:
    static constexpr float SIZE = 258.0f;
    static constexpr float PERSPECTIVE = 0.6f;
    inline static const QRectF RECT = QRectF(0.0f, 0.0f, SIZE, SIZE);
};

#endif // FLOW_H 