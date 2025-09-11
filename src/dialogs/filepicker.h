/*
 * FilePickerDialog - native-feel, skeuomorphic file chooser with Places sidebar
 * and optional Cover Flow view. Accepts both files and folders, multi-select.
 */
#ifndef MEDIASONIC_DIALOGS_FILEPICKER_H
#define MEDIASONIC_DIALOGS_FILEPICKER_H

#include <QDialog>
#include <QFileSystemModel>
#include <QSortFilterProxyModel>
#include <QUrl>

class QListView;
class QTreeView;
class QToolButton;
class QLineEdit;
class QComboBox;
class QDialogButtonBox;
class QStackedWidget;
class QStandardItemModel;
class Flow;
class QLabel;
class QSplitter;
class QHBoxLayout;

class FilePickerDialog : public QDialog
{
    Q_OBJECT
public:
    enum ViewMode { ViewList = 0, ViewFlow = 1, ViewColumn = 2 };

    explicit FilePickerDialog(QWidget *parent = nullptr);
    QStringList selectedPaths() const;
    void setStartUrl(const QUrl &url);
    void setNameFilters(const QStringList &filters); // Qt-style patterns: *.mp3;*.flac

private slots:
    void goBack();
    void goForward();
    void goUp();
    void onPlaceActivated(const QModelIndex &idx);
    void onEntryActivated(const QModelIndex &idx);
    void onSelectionChanged();
    void onSearchText(const QString &text);
    void switchToList();
    void switchToFlow();
    void newFolder();

private:
    void setCurrentPath(const QString &path, bool addToHistory = true);
    void refreshFlow();
    bool isAudioFile(const QString &fileName) const;

    // Models and proxies
    QFileSystemModel *m_dirModel;    // directories for Places
    QFileSystemModel *m_entryModel;  // files+dirs for main view
    QSortFilterProxyModel *m_entryProxy;

    // Widgets
    QTreeView *m_places;
    QTreeView *m_list;
    QStackedWidget *m_stack;
    QListView *m_flowList; // icon mode grid
    Flow *m_flow;          // 3D flow for docs/media
    QStandardItemModel *m_flowModel;
    QToolButton *m_back;
    QToolButton *m_fwd;
    QToolButton *m_up;
    QToolButton *m_viewList;
    QToolButton *m_viewFlow;
    QToolButton *m_viewColumn;
    QLineEdit *m_search;
    QComboBox *m_filterBox;
    QDialogButtonBox *m_buttons;
    QWidget *m_toolbar;
    QWidget *m_breadcrumb;
    QHBoxLayout *m_breadcrumbLayout;

    // State
    QString m_currentPath;
    QStringList m_history; int m_histIndex = -1;
    QStringList m_filters; // name filters

    // Column view + preview
    QWidget *m_columnPage;
    QWidget *m_columnContainer;
    QHBoxLayout *m_columnsLayout;
    QSplitter *m_columnSplit;
    QWidget *m_previewPane;
    QLabel *m_previewArt;
    QLabel *m_previewTitle;
    QLabel *m_previewSub;
    QLabel *m_previewMeta;

    void buildColumnsForPath(const QString &path);
    void updatePreview(const QFileInfo &fi);
    void updateBreadcrumb();
};

#endif // MEDIASONIC_DIALOGS_FILEPICKER_H
