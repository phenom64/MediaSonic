#include "ui/atmo_style.h"
#include "gfx/color.h"

namespace MS {

AtmoStyle AtmoStyle::fromPalette(const QPalette &pal)
{
    AtmoStyle s;
    QColor base = pal.color(QPalette::Base);
    QColor window = pal.color(QPalette::Window);
    QColor hl = pal.color(QPalette::Highlight);
    s.baseBg = MS::Color::mid(window, base, 2, 1);
    s.chromeLight = MS::Color::mid(window, QColor(255,255,255), 3, 1);
    s.chromeDark = MS::Color::mid(window, QColor(0,0,0), 3, 1);
    s.accent = hl;
    s.text = pal.color(QPalette::Text);

    // iTunes 9 khaki LCD palette derived from base
    QColor khaki1(215, 220, 200);
    QColor khaki2(200, 205, 185);
    s.lcdKhakiLight = MS::Color::mid(khaki1, base, 3, 2);
    s.lcdKhakiDark = MS::Color::mid(khaki2, base.darker(105), 3, 2);
    s.lcdInk = QColor(50, 59, 40);
    return s;
}

QLinearGradient AtmoStyle::topbarBackground(int height) const
{
    QLinearGradient g(0, 0, 0, height);
    g.setColorAt(0, chromeLight.lighter(104));
    g.setColorAt(0.3, chromeLight);
    g.setColorAt(0.7, baseBg);
    g.setColorAt(1, chromeDark);
    return g;
}

QLinearGradient AtmoStyle::buttonMetal(int height) const
{
    QLinearGradient g(0, 0, 0, height);
    g.setColorAt(0, chromeLight);
    g.setColorAt(0.5, baseBg);
    g.setColorAt(1, chromeDark);
    return g;
}

QLinearGradient AtmoStyle::buttonPressed(int height) const
{
    QLinearGradient g(0, 0, 0, height);
    g.setColorAt(0, chromeDark);
    g.setColorAt(0.5, baseBg.darker(110));
    g.setColorAt(1, chromeDark.darker(120));
    return g;
}

QLinearGradient AtmoStyle::lcdBackground(int height) const
{
    QLinearGradient g(0, 0, 0, height);
    g.setColorAt(0, lcdKhakiLight);
    g.setColorAt(0.5, MS::Color::mid(lcdKhakiLight, lcdKhakiDark, 2, 3));
    g.setColorAt(1, lcdKhakiDark);
    return g;
}

QRadialGradient AtmoStyle::overlayGlow(int w, int h) const
{
    QRadialGradient rg(w/2.0, h/2.0, w/2.0);
    QColor glow = accent; glow.setAlpha(24);
    rg.setColorAt(0, glow);
    QColor trans = glow; trans.setAlpha(0);
    rg.setColorAt(1, trans);
    return rg;
}

}

