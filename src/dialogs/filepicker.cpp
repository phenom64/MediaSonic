#include "dialogs/filepicker.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSplitter>
#include <QToolButton>
#include <QIcon>
#include <QLabel>
#include <QLineEdit>
#include <QTreeView>
#include <QListView>
#include <QComboBox>
#include <QFileIconProvider>
#include <QDialogButtonBox>
#include <QStandardPaths>
#include <QDir>
#include <QStackedWidget>
#include <QHeaderView>
#include <QInputDialog>
#include <QRegularExpression>
#include <QItemSelectionModel>
#include <QStandardItemModel>
#include <QPushButton>
#include "flow.h"
#ifdef HAVE_TAGLIB
#include <taglib/fileref.h>
#include <taglib/tag.h>
#include <taglib/audioproperties.h>
#endif

namespace {
static QStringList defaultPlaces()
{
    QStringList out;
    const auto locs = {
        QStandardPaths::PicturesLocation,
        QStandardPaths::MusicLocation,
        QStandardPaths::MoviesLocation,
        QStandardPaths::DocumentsLocation,
        QStandardPaths::DownloadLocation,
        QStandardPaths::HomeLocation
    };
    for (auto loc : locs) {
        const auto dirs = QStandardPaths::standardLocations(loc);
        if (!dirs.isEmpty()) out << dirs.first();
    }
    out.removeDuplicates();
    return out;
}
}

class NameFilterProxy : public QSortFilterProxyModel
{
public:
    void setFilters(const QStringList &filters) { m_filters = filters; invalidateFilter(); }
    void setSearch(const QString &s) { m_search = s; invalidateFilter(); }
protected:
    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override
    {
        auto *fs = qobject_cast<QFileSystemModel*>(sourceModel());
        if (!fs) return true;
        QModelIndex idx = fs->index(source_row, 0, source_parent);
        if (!idx.isValid()) return false;
        QFileInfo fi = fs->fileInfo(idx);
        if (fi.isDir()) return true; // always allow directories
        const QString name = fi.fileName();
        bool ok = m_filters.isEmpty();
        for (const QString &pat : m_filters) {
            QRegularExpression rx(QRegularExpression::wildcardToRegularExpression(pat), QRegularExpression::CaseInsensitiveOption);
            if (rx.match(name).hasMatch()) { ok = true; break; }
        }
        if (!m_search.isEmpty()) ok = ok && name.contains(m_search, Qt::CaseInsensitive);
        return ok;
    }
private:
    QStringList m_filters;
    QString m_search;
};

FilePickerDialog::FilePickerDialog(QWidget *parent)
    : QDialog(parent)
    , m_dirModel(new QFileSystemModel(this))
    , m_entryModel(new QFileSystemModel(this))
    , m_entryProxy(new NameFilterProxy)
{
    setWindowTitle(tr("Add to Library"));
    resize(980, 600);
    setSizeGripEnabled(true);

    // Sidebar directories only
    m_dirModel->setFilter(QDir::AllDirs | QDir::NoDotAndDotDot | QDir::Drives);
    m_dirModel->setRootPath("/");

    // Entries model: files + directories for current path
    m_entryModel->setFilter(QDir::AllEntries | QDir::NoDotAndDotDot | QDir::Readable);
    m_entryModel->setRootPath(QDir::homePath());

    m_entryProxy->setSourceModel(m_entryModel);
    m_entryProxy->setDynamicSortFilter(true);

    // Toolbar widget (for skeuo background)
    m_toolbar = new QWidget(this);
    auto *toolbar = new QHBoxLayout(m_toolbar);
    m_back = new QToolButton(this); m_back->setIcon(QIcon::fromTheme("go-previous"));
    m_fwd  = new QToolButton(this); m_fwd->setIcon(QIcon::fromTheme("go-next"));
    m_up   = new QToolButton(this); m_up->setIcon(QIcon::fromTheme("go-up"));
    m_viewList = new QToolButton(this); m_viewList->setIcon(QIcon::fromTheme("view-list-details"));
    m_viewFlow = new QToolButton(this); m_viewFlow->setIcon(QIcon::fromTheme("view-carousel"));
    m_viewColumn = new QToolButton(this); m_viewColumn->setIcon(QIcon::fromTheme("view-column"));
    m_search = new QLineEdit(this); m_search->setPlaceholderText(tr("search"));
    m_filterBox = new QComboBox(this);
    m_filterBox->addItem(tr("Audio Files"), "*.mp3;*.flac;*.m4a;*.wav;*.ogg;*.aac;*.opus;*.aiff;*.wma");
    m_filterBox->addItem(tr("All Files"), "*");
    toolbar->addWidget(m_back); toolbar->addWidget(m_fwd); toolbar->addWidget(m_up);
    toolbar->addStretch();
    toolbar->addWidget(m_viewList); toolbar->addWidget(m_viewFlow); toolbar->addWidget(m_viewColumn);
    toolbar->addSpacing(6);
    toolbar->addWidget(new QLabel(tr("Filter:"), this));
    toolbar->addWidget(m_filterBox);
    toolbar->addSpacing(6);
    toolbar->addWidget(m_search, 1);

    // Breadcrumb path bar
    m_breadcrumb = new QWidget(this);
    m_breadcrumbLayout = new QHBoxLayout(m_breadcrumb);
    m_breadcrumbLayout->setContentsMargins(6, 2, 6, 2);
    m_breadcrumbLayout->setSpacing(4);

    // Body: splitter with Places and main view (stacked)
    auto *split = new QSplitter(this);
    m_places = new QTreeView(split);
    m_places->setModel(m_dirModel);
    m_places->setHeaderHidden(true);
    m_places->setRootIndex(m_dirModel->index("/"));
    // Seed places list: expand Home/Music etc.
    for (const QString &p : defaultPlaces()) m_places->expand(m_dirModel->index(p));

    m_stack = new QStackedWidget(split);
    m_list = new QTreeView(m_stack);
    m_list->setModel(m_entryProxy);
    m_list->setRootIsDecorated(false);
    m_list->setSortingEnabled(true);
    m_list->header()->setStretchLastSection(true);
    m_list->setSelectionMode(QAbstractItemView::ExtendedSelection);
    m_stack->addWidget(m_list);

    m_flowList = new QListView(m_stack);
    m_flowList->setViewMode(QListView::IconMode);
    m_flowList->setResizeMode(QListView::Adjust);
    m_flowList->setMovement(QListView::Static);
    m_flowList->setIconSize(QSize(96,96));
    m_flowList->setModel(m_entryProxy);
    m_flowList->setSelectionMode(QAbstractItemView::ExtendedSelection);
    m_stack->addWidget(m_flowList);

    m_flowModel = new QStandardItemModel(this);
    m_flow = new Flow(m_stack);
    m_flow->setModel(m_flowModel);
    m_stack->addWidget(m_flow);

    // Column view + preview panel
    m_columnPage = new QWidget(m_stack);
    m_columnSplit = new QSplitter(Qt::Horizontal, m_columnPage);
    m_columnContainer = new QWidget(m_columnSplit);
    m_columnsLayout = new QHBoxLayout(m_columnContainer);
    m_columnsLayout->setContentsMargins(0,0,0,0);
    m_columnsLayout->setSpacing(0);
    m_previewPane = new QWidget(m_columnSplit);
    auto *prevLayout = new QVBoxLayout(m_previewPane);
    prevLayout->setContentsMargins(10,10,10,10);
    prevLayout->setSpacing(6);
    m_previewArt = new QLabel(m_previewPane); m_previewArt->setMinimumSize(128,128); m_previewArt->setAlignment(Qt::AlignCenter);
    m_previewTitle = new QLabel(m_previewPane); m_previewTitle->setStyleSheet("font-weight: bold; font-size: 14px");
    m_previewSub = new QLabel(m_previewPane); m_previewSub->setStyleSheet("color: gray");
    m_previewMeta = new QLabel(m_previewPane); m_previewMeta->setStyleSheet("font-family: Monospace"); m_previewMeta->setWordWrap(true);
    prevLayout->addWidget(m_previewArt);
    prevLayout->addWidget(m_previewTitle);
    prevLayout->addWidget(m_previewSub);
    prevLayout->addWidget(m_previewMeta, 1);
    m_columnSplit->addWidget(m_columnContainer);
    m_columnSplit->addWidget(m_previewPane);
    m_columnSplit->setStretchFactor(0, 3);
    m_columnSplit->setStretchFactor(1, 2);
    auto *colLayout = new QVBoxLayout(m_columnPage);
    colLayout->setContentsMargins(0,0,0,0);
    colLayout->addWidget(m_columnSplit);
    m_stack->addWidget(m_columnPage);

    split->setStretchFactor(0, 0);
    split->setStretchFactor(1, 1);

    // Buttons
    m_buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    auto *newFolderBtn = m_buttons->addButton(tr("New Folder"), QDialogButtonBox::ActionRole);

    // Layout
    auto *outer = new QVBoxLayout(this);
    // Skeuo metal gradient for toolbar using CSS (quick)
    m_toolbar->setStyleSheet("background: qlineargradient(x1:0,y1:0,x2:0,y2:1, stop:0 #f7f7f7, stop:0.5 #e8e8e8, stop:1 #dcdcdc); border-bottom: 1px solid #b0b0b0;");
    m_breadcrumb->setStyleSheet("background: #f3f3f3; border-bottom: 1px solid #c8c8c8;");

    outer->addWidget(m_toolbar);
    outer->addWidget(m_breadcrumb);
    outer->addWidget(split, 1);
    outer->addWidget(m_buttons);

    // Signals
    connect(m_back, &QToolButton::clicked, this, &FilePickerDialog::goBack);
    connect(m_fwd,  &QToolButton::clicked, this, &FilePickerDialog::goForward);
    connect(m_up,   &QToolButton::clicked, this, &FilePickerDialog::goUp);
    connect(m_places, &QTreeView::activated, this, &FilePickerDialog::onPlaceActivated);
    connect(m_list, &QTreeView::activated, this, &FilePickerDialog::onEntryActivated);
    connect(m_flowList, &QListView::activated, this, &FilePickerDialog::onEntryActivated);
    connect(m_list->selectionModel(), &QItemSelectionModel::selectionChanged, this, &FilePickerDialog::onSelectionChanged);
    connect(m_flowList->selectionModel(), &QItemSelectionModel::selectionChanged, this, &FilePickerDialog::onSelectionChanged);
    connect(m_search, &QLineEdit::textChanged, this, &FilePickerDialog::onSearchText);
    connect(m_filterBox, &QComboBox::currentTextChanged, this, [this]{
        const auto pat = m_filterBox->currentData().toString().split(';');
        m_filters = pat; static_cast<NameFilterProxy*>(m_entryProxy)->setFilters(pat); refreshFlow();
    });
    connect(m_viewList, &QToolButton::clicked, this, &FilePickerDialog::switchToList);
    connect(m_viewFlow, &QToolButton::clicked, this, &FilePickerDialog::switchToFlow);
    connect(m_viewColumn, &QToolButton::clicked, this, [this]{ m_stack->setCurrentWidget(m_columnPage); });
    connect(m_buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(m_buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
    connect(newFolderBtn, &QPushButton::clicked, this, &FilePickerDialog::newFolder);
    connect(m_flow, &Flow::opened, this, [this](const QModelIndex &idx){
        QString path = m_flowModel->itemFromIndex(idx)->data(Qt::UserRole+1).toString();
        if (QFileInfo(path).isDir()) setCurrentPath(path); else {/*select in list*/}
    });

    // Initial path
    setCurrentPath(QStandardPaths::writableLocation(QStandardPaths::MusicLocation));
    // Apply initial filter
    const auto pat = m_filterBox->currentData().toString().split(';');
    static_cast<NameFilterProxy*>(m_entryProxy)->setFilters(pat);
    refreshFlow();
    updateBreadcrumb();
}

void FilePickerDialog::setStartUrl(const QUrl &url)
{
    if (url.isLocalFile()) setCurrentPath(url.toLocalFile());
}

void FilePickerDialog::setNameFilters(const QStringList &filters)
{
    m_filters = filters;
    static_cast<NameFilterProxy*>(m_entryProxy)->setFilters(filters);
}

QStringList FilePickerDialog::selectedPaths() const
{
    QStringList out;
    const auto addFromSel = [&](QItemSelectionModel *sel){
        for (const QModelIndex &pidx : sel->selectedRows()) {
            QModelIndex idx = m_entryProxy->mapToSource(pidx);
            QFileInfo fi = m_entryModel->fileInfo(idx);
            out << fi.absoluteFilePath();
        }
    };
    addFromSel(m_list->selectionModel());
    addFromSel(m_flowList->selectionModel());
    // Collect from column lists
    if (m_columnsLayout) {
        for (int i=0;i<m_columnsLayout->count();++i) {
            QWidget *w = m_columnsLayout->itemAt(i)->widget();
            auto *lv = qobject_cast<QListView*>(w);
            if (!lv) continue;
            for (const QModelIndex &sidx : lv->selectionModel()->selectedRows()) {
                QFileInfo fi = m_entryModel->fileInfo(sidx);
                out << fi.absoluteFilePath();
            }
        }
    }
    out.removeDuplicates();
    return out;
}

void FilePickerDialog::goBack()
{
    if (m_histIndex > 0) {
        --m_histIndex; setCurrentPath(m_history[m_histIndex], false);
    }
}

void FilePickerDialog::goForward()
{
    if (m_histIndex >= 0 && m_histIndex < m_history.size()-1) {
        ++m_histIndex; setCurrentPath(m_history[m_histIndex], false);
    }
}

void FilePickerDialog::goUp()
{
    QDir d(m_currentPath); d.cdUp(); setCurrentPath(d.absolutePath());
}

void FilePickerDialog::onPlaceActivated(const QModelIndex &idx)
{
    QString p = m_dirModel->fileInfo(idx).absoluteFilePath();
    setCurrentPath(p);
}

void FilePickerDialog::onEntryActivated(const QModelIndex &pidx)
{
    QModelIndex idx = m_entryProxy->mapToSource(pidx);
    QFileInfo fi = m_entryModel->fileInfo(idx);
    if (fi.isDir()) setCurrentPath(fi.absoluteFilePath());
}

void FilePickerDialog::onSelectionChanged()
{
    // Could update preview footer later
}

void FilePickerDialog::onSearchText(const QString &text)
{
    static_cast<NameFilterProxy*>(m_entryProxy)->setSearch(text);
}

void FilePickerDialog::switchToList()
{
    m_stack->setCurrentWidget(m_list);
}

void FilePickerDialog::switchToFlow()
{
    m_stack->setCurrentWidget(m_flow);
}

void FilePickerDialog::newFolder()
{
    bool ok=false; QString name = QInputDialog::getText(this, tr("New Folder"), tr("Folder name:"), QLineEdit::Normal, QString(), &ok);
    if (!ok || name.trimmed().isEmpty()) return;
    QDir dir(m_currentPath); dir.mkdir(name.trimmed());
}

void FilePickerDialog::setCurrentPath(const QString &path, bool addToHistory)
{
    if (path.isEmpty()) return;
    m_currentPath = path;
    QModelIndex root = m_entryModel->index(path);
    if (!root.isValid()) {
        // Fallback to home if the path model didn't load (e.g., missing Music folder)
        root = m_entryModel->index(QDir::homePath());
        m_currentPath = QDir::homePath();
    }
    m_list->setRootIndex(m_entryProxy->mapFromSource(root));
    m_flowList->setRootIndex(m_entryProxy->mapFromSource(root));
    refreshFlow();
    buildColumnsForPath(m_currentPath);
    updateBreadcrumb();
    if (addToHistory) {
        // Truncate forward history and append
        if (m_histIndex >= 0 && m_histIndex < m_history.size()-1)
            m_history = m_history.mid(0, m_histIndex+1);
        m_history << path; m_histIndex = m_history.size()-1;
    }
}

void FilePickerDialog::refreshFlow()
{
    m_flowModel->clear();
    QDir dir(m_currentPath);
    QFileIconProvider prov;
    const auto entries = dir.entryInfoList(QDir::AllEntries | QDir::NoDotAndDotDot, QDir::DirsFirst | QDir::Name);
    for (const QFileInfo &fi : entries) {
        // Filter files by current patterns
        if (fi.isFile()) {
            bool ok = m_filters.isEmpty();
            for (const QString &pat : m_filters) {
                QRegularExpression rx(QRegularExpression::wildcardToRegularExpression(pat), QRegularExpression::CaseInsensitiveOption);
                if (rx.match(fi.fileName()).hasMatch()) { ok = true; break; }
            }
            if (!ok) continue;
        }
        QStandardItem *it = new QStandardItem(fi.fileName());
        it->setData(fi.absoluteFilePath(), Qt::UserRole+1);
        it->setIcon(prov.icon(fi));
        m_flowModel->appendRow(it);
    }
}

void FilePickerDialog::buildColumnsForPath(const QString &path)
{
    // Clear old columns
    if (!m_columnsLayout) return;
    QLayoutItem *child;
    while ((child = m_columnsLayout->takeAt(0)) != nullptr) {
        if (child->widget()) child->widget()->deleteLater();
        delete child;
    }
    // Build a column per path segment
    QStringList parts = QDir::cleanPath(path).split('/', Qt::SkipEmptyParts);
    QString walk = "/";
    if (parts.isEmpty()) parts << QString();
    for (int i=0;i<parts.size();++i) {
        if (i==0 && path.startsWith('/')) walk = "/"; else walk = QDir(walk).filePath(parts[i]);
        QModelIndex idx = m_entryModel->index(walk);
        if (!idx.isValid()) continue;
        QListView *col = new QListView(m_columnContainer);
        col->setModel(m_entryModel);
        col->setRootIndex(idx);
        col->setSelectionMode(QAbstractItemView::SingleSelection);
        col->setUniformItemSizes(true);
        col->setMinimumWidth(220);
        m_columnsLayout->addWidget(col);
        connect(col, &QListView::activated, this, [this](const QModelIndex &sidx){
            QFileInfo fi = m_entryModel->fileInfo(sidx);
            if (fi.isDir()) setCurrentPath(fi.absoluteFilePath());
            else updatePreview(fi);
        });
        connect(col->selectionModel(), &QItemSelectionModel::currentChanged, this, [this](const QModelIndex &curr){
            QFileInfo fi = m_entryModel->fileInfo(curr);
            updatePreview(fi);
        });
    }
}

void FilePickerDialog::updatePreview(const QFileInfo &fi)
{
    if (!m_previewPane) return;
    if (!fi.exists()) { m_previewArt->clear(); m_previewTitle->clear(); m_previewSub->clear(); m_previewMeta->clear(); return; }
    QIcon ic = QFileIconProvider().icon(fi);
    m_previewArt->setPixmap(ic.pixmap(196,196));
    m_previewTitle->setText(fi.fileName());
    if (fi.isDir()) {
        m_previewSub->setText(tr("Folder"));
        m_previewMeta->setText(tr("%1 items").arg(QDir(fi.absoluteFilePath()).entryList(QDir::Files|QDir::NoDotAndDotDot).size()));
        return;
    }
    // File: attempt to show basic audio metadata (TagLib if available)
#ifdef HAVE_TAGLIB
    try {
        TagLib::FileRef f(fi.absoluteFilePath().toUtf8().constData());
        if (!f.isNull() && f.tag()) {
            QString artist = QString::fromUtf8(f.tag()->artist().toCString(true));
            QString album = QString::fromUtf8(f.tag()->album().toCString(true));
            QString title = QString::fromUtf8(f.tag()->title().toCString(true));
            if (!title.isEmpty()) m_previewTitle->setText(title);
            m_previewSub->setText(QString("%1 — %2").arg(artist, album));
        }
        if (!f.isNull() && f.audioProperties()) {
            int dur = f.audioProperties()->length();
            int br = f.audioProperties()->bitrate();
            int sr = f.audioProperties()->sampleRate();
            m_previewMeta->setText(tr("%1:%2  %3 kbps  %4 Hz")
                .arg(dur/60,2,10,QChar('0')).arg(dur%60,2,10,QChar('0')).arg(br).arg(sr));
        }
    } catch (...) {}
#else
    m_previewSub->setText(QString());
    m_previewMeta->setText(QString());
#endif
}

void FilePickerDialog::updateBreadcrumb()
{
    if (!m_breadcrumbLayout) return;
    QLayoutItem *child;
    while ((child = m_breadcrumbLayout->takeAt(0)) != nullptr) {
        if (child->widget()) child->widget()->deleteLater();
        delete child;
    }
    QStringList parts = QDir::cleanPath(m_currentPath).split('/', Qt::SkipEmptyParts);
    QString walk = "/";
    auto addSeg = [&](const QString &label, const QString &path){
        QToolButton *seg = new QToolButton(m_breadcrumb);
        seg->setText(label.isEmpty()?"/":label);
        seg->setToolButtonStyle(Qt::ToolButtonTextOnly);
        seg->setStyleSheet("QToolButton { border: 1px solid #c0c0c0; background: qlineargradient(x1:0,y1:0,x2:0,y2:1, stop:0 #ffffff, stop:1 #e6e6e6); padding: 2px 6px; }");
        connect(seg, &QToolButton::clicked, this, [this,path]{ setCurrentPath(path); });
        m_breadcrumbLayout->addWidget(seg);
    };
    addSeg("", "/");
    for (int i=0;i<parts.size();++i) {
        walk = QDir(walk).filePath(parts[i]);
        addSeg(parts[i], walk);
    }
    m_breadcrumbLayout->addStretch();
}
