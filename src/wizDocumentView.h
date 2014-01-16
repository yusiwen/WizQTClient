#ifndef CORE_WIZDOCUMENTVIEW_H
#define CORE_WIZDOCUMENTVIEW_H

#include <coreplugin/inoteview.h>

#include "share/wizobject.h"

class QWebView;
class QScrollArea;
class QLineEdit;

struct WIZDOCUMENTDATA;
struct WIZDOCUMENTATTACHMENTDATA;
class CWizExplorerApp;
class CWizDatabaseManager;
class CWizUserSettings;
class CWizScrollBar;
class CWizDocumentWebView;
class CWizDatabase;
class CWizSplitter;

class QWebFrame;


namespace Core {
namespace Internal {
class TitleBar;
class EditorToolBar;
} // namespace Internal

class CWizDocumentView : public INoteView
{
    Q_OBJECT

public:
    CWizDocumentView(CWizExplorerApp& app, QWidget* parent = 0);
    ~CWizDocumentView();
    virtual QSize sizeHint() const { return QSize(200, 1); }

    QWidget* client() const { return m_client; }
    CWizDocumentWebView* web() const { return m_web; }
    QWebView* commentView() const { return m_comments; }

protected:
    CWizExplorerApp& m_app;
    CWizDatabaseManager& m_dbMgr;
    CWizUserSettings& m_userSettings;
    CWizDocumentWebView* m_web;
    QWebView* m_comments;
    CWizSplitter* m_splitter;
    Core::Internal::TitleBar* m_title;
    QWidget* m_client;

    virtual void showEvent(QShowEvent *event);

private:
    WIZDOCUMENTDATA m_note;
    bool m_bLocked; // note is force locked as readonly status
    bool m_bEditingMode; // true: editing mode, false: reading mode
    int m_viewMode; // user defined editing mode

public:
    const WIZDOCUMENTDATA& note() const { return m_note; }
    bool isLocked() const { return m_bLocked; }
    bool isEditing() const { return m_bEditingMode; }
    bool defaultEditingMode();
    bool reload();
    void reloadNote();
    void setEditorFocus();

    void initStat(const WIZDOCUMENTDATA& data, bool bEditing);
    void viewNote(const WIZDOCUMENTDATA& data, bool forceEdit);
    void showClient(bool visible);
    void setEditNote(bool bEdit);
    void setViewMode(int mode);
    void setModified(bool modified);
    void settingsChanged();

    QWebFrame* noteFrame();

public Q_SLOTS:
    void onViewNoteRequested(Core::INoteView* view, const WIZDOCUMENTDATA& doc);
    void onViewNoteLoaded(Core::INoteView*,const WIZDOCUMENTDATA&,bool);

    void on_document_modified(const WIZDOCUMENTDATA& documentOld,
                              const WIZDOCUMENTDATA& documentNew);
    void on_document_data_modified(const WIZDOCUMENTDATA& data);

    void on_attachment_created(const WIZDOCUMENTATTACHMENTDATA& attachment);
    void on_attachment_deleted(const WIZDOCUMENTATTACHMENTDATA& attachment);
};

} // namespace Core

#endif // CORE_WIZDOCUMENTVIEW_H
