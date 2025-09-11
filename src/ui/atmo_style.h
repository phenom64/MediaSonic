/*
 * AtmoStyle - central palette and gradient derivation for skeuomorphic UI
 */
#ifndef MEDIASONIC_UI_ATMO_STYLE_H
#define MEDIASONIC_UI_ATMO_STYLE_H

#include <QLinearGradient>
#include <QRadialGradient>
#include <QColor>
#include <QPalette>

namespace MS {

struct AtmoStyle
{
    QColor baseBg;
    QColor chromeLight;
    QColor chromeDark;
    QColor accent;
    QColor text;
    QColor lcdKhakiLight;
    QColor lcdKhakiDark;
    QColor lcdInk;

    static AtmoStyle fromPalette(const QPalette &pal);

    QLinearGradient topbarBackground(int height) const;
    QLinearGradient buttonMetal(int height) const;
    QLinearGradient buttonPressed(int height) const;
    QLinearGradient lcdBackground(int height) const;
    QRadialGradient overlayGlow(int w, int h) const;
};

}

#endif // MEDIASONIC_UI_ATMO_STYLE_H

