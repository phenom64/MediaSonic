#include "gfx/color.h"
#include <algorithm>

namespace MS {

QColor Color::mid(const QColor &c1, const QColor &c2, int i1, int i2)
{
    const int t = std::max(1, i1) + std::max(1, i2);
    const double f1 = double(std::max(1, i1)) / t;
    const double f2 = 1.0 - f1;
    QColor a = c1.convertTo(QColor::Hsv);
    QColor b = c2.convertTo(QColor::Hsv);

    // Hue wrap-around aware interpolation
    int h1 = a.hsvHue();
    int h2 = b.hsvHue();
    if (h1 < 0) h1 = 0; if (h2 < 0) h2 = 0;
    int dh = h2 - h1;
    if (std::abs(dh) > 180) {
        if (dh > 0) h1 += 360; else h2 += 360;
    }
    int h = int(h1 * f1 + h2 * f2) % 360;
    int s = int(a.hsvSaturation() * f1 + b.hsvSaturation() * f2);
    int v = int(a.value() * f1 + b.value() * f2);
    int alpha = int(a.alpha() * f1 + b.alpha() * f2);
    QColor out; out.setHsv(h, s, v, alpha);
    return out;
}

bool Color::contrast(const QColor &c1, const QColor &c2)
{
    // WCAG-like luminance contrast check
    auto l1 = lum(c1) / 255.0;
    auto l2 = lum(c2) / 255.0;
    double hi = std::max(l1, l2) + 0.05;
    double lo = std::min(l1, l2) + 0.05;
    return (hi / lo) > 1.6; // slightly relaxed threshold for skeuo surfaces
}

void Color::ensureContrast(QColor &c1, QColor &c2)
{
    int guard = 0;
    while (!contrast(c1, c2) && guard++ < 20) {
        if (lum(c1) > lum(c2)) setValue(c2.value() - 8, c2);
        else setValue(c1.value() + 8, c1);
    }
}

void Color::setValue(int value, QColor &c)
{
    QColor hsv = c.convertTo(QColor::Hsv);
    hsv.setHsv(hsv.hsvHue(), hsv.hsvSaturation(), std::clamp(value, 0, 255), c.alpha());
    c = hsv.convertTo(QColor::Rgb);
}

int Color::lum(const QColor &c)
{
    // Perceived luminance
    return int(0.2126 * c.red() + 0.7152 * c.green() + 0.0722 * c.blue());
}

void Color::shiftHue(QColor &c, int amount)
{
    QColor hsv = c.convertTo(QColor::Hsv);
    int h = hsv.hsvHue(); if (h < 0) h = 0;
    h = (h + amount) % 360; if (h < 0) h += 360;
    hsv.setHsv(h, hsv.hsvSaturation(), hsv.value(), hsv.alpha());
    c = hsv.convertTo(QColor::Rgb);
}

QColor Color::complementary(QColor c)
{
    shiftHue(c, 180);
    return c;
}

}

