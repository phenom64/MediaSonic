/*
 * Simple image FX helpers, loosely inspired by DocSurf gfx/fx.h
 */
#ifndef MEDIASONIC_GFX_FX_H
#define MEDIASONIC_GFX_FX_H

#include <QPixmap>
#include <QImage>
#include <QBrush>
#include <QRect>
#include <QColor>

namespace MS { namespace FX {

// Fast separable exponential blur
void expblur(QImage &img, int radius, Qt::Orientations o = Qt::Horizontal | Qt::Vertical);

// Compose two pixmaps with a brush tint
QPixmap mid(const QPixmap &p1, const QBrush &b, int a1 = 1, int a2 = 1, const QSize &sz = QSize());

// Create a subtle sunken/embossed appearance around a pixmap
QPixmap sunkenized(const QRect &r, const QPixmap &source, bool isDark = false, int shadowOpacity = 127);

// Utility transforms
int stretch(int v, float n = 1.5f);
QImage stretched(QImage img);

} } // namespace MS::FX

#endif // MEDIASONIC_GFX_FX_H

