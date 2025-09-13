#include "ui/nse_uno.h"
#include <QAction>
#include <QWidgetAction>
#include <QFont>

using namespace NSEUI;

NSEUnoToolBar::NSEUnoToolBar(QWidget *window, QWidget *parent)
    : QToolBar(parent)
    , m_titleLabel(nullptr)
{
    initCommon();
    Q_UNUSED(window); // Title centering enabled explicitly later
}

void NSEUnoToolBar::initCommon()
{
    setObjectName(QStringLiteral("NSEUnoToolBar"));
    setMovable(false);
    setFloatable(false);
    setIconSize(QSize(16, 16));
    setContentsMargins(0, 0, 0, 0);
}

void NSEUnoToolBar::enableCenteredTitle(QWidget *window)
{
    // Append spacers and title after any existing content (e.g., TopBar),
    // so that expanding spacers center the title and keep content on edges.
    installCenterTitle(window);
}

void NSEUnoToolBar::installCenterTitle(QWidget *window)
{
    // Left spacer to push title to center; added after current content
    QWidget *left = new NSESpacer(this);
    addWidget(left);

    // Title label
    m_titleLabel = new QLabel(window->windowTitle(), this);
    QFont f = m_titleLabel->font();
    f.setBold(true);
    m_titleLabel->setFont(f);
    m_titleLabel->setAlignment(Qt::AlignCenter);
    m_titleLabel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    m_titleLabel->setAttribute(Qt::WA_TransparentForMouseEvents);
    addWidget(m_titleLabel);

    // Right spacer to truly center the title
    QWidget *right = new NSESpacer(this);
    addWidget(right);

    QObject::connect(window, &QWidget::windowTitleChanged, m_titleLabel, &QLabel::setText);
}

void NSEUnoToolBar::setTitleText(const QString &text)
{
    if (m_titleLabel) m_titleLabel->setText(text);
}
