/*
 * NSE UNO helpers for MediaSonic: thin wrappers to make Atmo usage
 * feel native and consistent across the codebase.
 */
#ifndef MEDIASONIC_UI_NSE_UNO_H
#define MEDIASONIC_UI_NSE_UNO_H

#include <QToolBar>
#include <QLabel>

// Forward-declare Atmo UNO manager and include when available.
#ifdef __has_include
#  if __has_include("atmolib/titlewidget.h")
#    include "atmolib/titlewidget.h"
#    define MS_HAVE_ATMO_UNO 1
#  endif
#endif

namespace NSEUI {

// A lightweight expanding spacer widget for toolbars
class NSESpacer : public QWidget {
    Q_OBJECT
public:
    explicit NSESpacer(QWidget *parent = nullptr) : QWidget(parent) {
        setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        setMinimumWidth(0);
        setAttribute(Qt::WA_TransparentForMouseEvents);
    }
};

// Unified toolbar with optional Atmo UNO management and a centered title label
class NSEUnoToolBar : public QToolBar {
    Q_OBJECT
public:
    explicit NSEUnoToolBar(QWidget *window, QWidget *parent = nullptr);
    QLabel *titleLabel() const { return m_titleLabel; }
    void setTitleText(const QString &text);

private:
    void initCommon();
    void installCenterTitle(QWidget *window);
    QLabel *m_titleLabel;
};

} // namespace NSEUI

#endif // MEDIASONIC_UI_NSE_UNO_H

