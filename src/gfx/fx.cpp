#include "gfx/fx.h"
#include <QPainter>
#include <QtMath>

namespace MS { namespace FX {

// Simple fast blur adapted for UI highlights. Not physically accurate, but good enough for skeuo gloss.
void expblur(QImage &img, int radius, Qt::Orientations o)
{
    if (radius <= 0) return;
    img = img.convertToFormat(QImage::Format_ARGB32_Premultiplied);

    const int w = img.width();
    const int h = img.height();
    const int alphaScale = 1 << 24;

    auto pass = [&](bool horizontal){
        const int len = horizontal ? w : h;
        const int lines = horizontal ? h : w;
        const double decay = std::exp(std::log(0.5) / (radius));
        const double inv = 1.0 - decay;
        for (int y = 0; y < lines; ++y) {
            double r=0,g=0,b=0,a=0;
            for (int i = 0; i < len; ++i) {
                const int x = horizontal ? i : y;
                const int yy = horizontal ? y : i;
                QRgb *px = reinterpret_cast<QRgb*>(img.scanLine(yy));
                QRgb c = horizontal ? px[x] : reinterpret_cast<QRgb*>(img.scanLine(x))[yy];
                double ca = qAlpha(c) / 255.0;
                r = r * decay + qRed(c) * inv * ca;
                g = g * decay + qGreen(c) * inv * ca;
                b = b * decay + qBlue(c) * inv * ca;
                a = a * decay + ca * inv;
                int ir = int(r / (a + 1e-5));
                int ig = int(g / (a + 1e-5));
                int ib = int(b / (a + 1e-5));
                int ia = int(a * 255);
                QRgb out = qRgba(qBound(0, ir, 255), qBound(0, ig, 255), qBound(0, ib, 255), qBound(0, ia, 255));
                if (horizontal)
                    px[x] = out;
                else
                    reinterpret_cast<QRgb*>(img.scanLine(x))[yy] = out;
            }
        }
    };
    if (o & Qt::Horizontal) pass(true);
    if (o & Qt::Vertical) pass(false);
}

QPixmap mid(const QPixmap &p1, const QBrush &b, int a1, int /*a2*/, const QSize &sz)
{
    QPixmap base = sz.isValid() ? p1.scaled(sz, Qt::IgnoreAspectRatio, Qt::SmoothTransformation) : p1;
    QImage img = base.toImage();
    QPainter p(&img);
    p.setCompositionMode(QPainter::CompositionMode_SourceOver);
    QColor c = b.color();
    c.setAlpha(qBound(0, a1 * 32, 255));
    p.fillRect(img.rect(), c);
    p.end();
    return QPixmap::fromImage(img);
}

QPixmap sunkenized(const QRect &r, const QPixmap &source, bool isDark, int shadowOpacity)
{
    QImage img(r.size(), QImage::Format_ARGB32_Premultiplied);
    img.fill(Qt::transparent);
    QPainter p(&img);
    // Drop shadow inside edges
    QColor shadow = QColor(0, 0, 0, shadowOpacity);
    QColor highlight = QColor(255, 255, 255, 90);
    p.fillRect(img.rect(), Qt::transparent);
    p.setPen(Qt::NoPen);
    p.setBrush(shadow);
    p.drawRect(0, 1, r.width(), 1);
    p.drawRect(0, r.height() - 2, r.width(), 1);
    p.drawRect(1, 0, 1, r.height());
    p.drawRect(r.width() - 2, 0, 1, r.height());
    // Inner highlight
    p.setBrush(highlight);
    p.drawRect(0, 0, r.width(), 1);
    p.drawRect(0, r.height() - 1, r.width(), 1);
    p.end();
    QImage s = img;
    expblur(s, 2);
    QImage out = source.toImage().convertToFormat(QImage::Format_ARGB32_Premultiplied);
    QPainter mix(&out);
    mix.setCompositionMode(QPainter::CompositionMode_SourceOver);
    mix.drawImage(0, 0, s);
    mix.end();
    return QPixmap::fromImage(out);
}

int stretch(int v, float n)
{
    return int(std::round(std::pow(std::abs(v), n))) * (v < 0 ? -1 : 1);
}

QImage stretched(QImage img)
{
    // Simple gamma stretch for highlights
    img = img.convertToFormat(QImage::Format_ARGB32_Premultiplied);
    for (int y = 0; y < img.height(); ++y) {
        QRgb *px = reinterpret_cast<QRgb*>(img.scanLine(y));
        for (int x = 0; x < img.width(); ++x) {
            QColor c = QColor::fromRgba(px[x]);
            c.setRed(stretch(c.red(), 1.2f));
            c.setGreen(stretch(c.green(), 1.2f));
            c.setBlue(stretch(c.blue(), 1.2f));
            px[x] = c.rgba();
        }
    }
    return img;
}

} } // namespace

