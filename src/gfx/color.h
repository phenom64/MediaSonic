/*
 * Color helpers inspired by DocSurf's color.h
 */
#ifndef MEDIASONIC_GFX_COLOR_H
#define MEDIASONIC_GFX_COLOR_H

#include <QColor>

namespace MS {

class Color
{
public:
    // Weighted mid color between c1 and c2
    static QColor mid(const QColor &c1, const QColor &c2, int i1 = 1, int i2 = 1);

    // Ensure there is contrast between foreground/background
    static bool contrast(const QColor &c1, const QColor &c2);
    static void ensureContrast(QColor &c1, QColor &c2);

    static void setValue(int value, QColor &c);
    static int lum(const QColor &c);
    static void shiftHue(QColor &c, int amount);
    static QColor complementary(QColor c);
};

}

#endif // MEDIASONIC_GFX_COLOR_H

