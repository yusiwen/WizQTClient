#include "wizDocumentView.h"

#include <QWebElement>
#include <QWebFrame>
#include <QLineEdit>
#include <QLabel>
#include <QHBoxLayout>

#include <coreplugin/icore.h>

#include "share/wizDatabaseManager.h"
#include "widgets/wizScrollBar.h"
#include "wizDocumentWebView.h"
#include "wiznotestyle.h"
#include "widgets/wizSegmentedButton.h"
#include "wizButton.h"
#include "share/wizsettings.h"
#include "share/wizuihelper.h"

#include "titlebar.h"

using namespace Core;
using namespace Core::Internal;

CWizDocumentView::CWizDocumentView(CWizExplorerApp& app, QWidget* parent)
    : INoteView(parent)
    , m_app(app)
    , m_userSettings(app.userSettings())
    , m_dbMgr(app.databaseManager())
    , m_web(new CWizDocumentWebView(app, this))
    , m_comments(new QWebView(this))
    , m_title(new TitleBar(this))
    , m_viewMode(app.userSettings().noteViewMode())
    , m_bLocked(false)
    , m_bEditingMode(false)
{
    m_title->setEditor(m_web);

    QVBoxLayout* layout = new QVBoxLayout();
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    m_client = new QWidget(this);
    m_client->setLayout(layout);

    m_splitter = new CWizSplitter(this);
    m_splitter->addWidget(m_web);
    m_splitter->addWidget(m_comments);
    m_comments->page()->mainFrame()->setScrollBarPolicy(Qt::Horizontal, Qt::ScrollBarAlwaysOff);
    m_comments->hide();

    layout->addWidget(m_title);
    layout->addWidget(m_splitter);

    layout->setStretchFactor(m_title, 0);
    layout->setStretchFactor(m_splitter, 1);

    QVBoxLayout* layoutMain = new QVBoxLayout();
    layoutMain->setContentsMargins(0, 0, 0, 0);
    setLayout(layoutMain);
    layoutMain->addWidget(m_client);

    connect(&m_dbMgr, SIGNAL(documentModified(const WIZDOCUMENTDATA&, const WIZDOCUMENTDATA&)), \
            SLOT(on_document_modified(const WIZDOCUMENTDATA&, const WIZDOCUMENTDATA&)));

    connect(&m_dbMgr, SIGNAL(documentDataModified(const WIZDOCUMENTDATA&)),
            SLOT(on_document_data_modified(const WIZDOCUMENTDATA&)));

    connect(&m_dbMgr, SIGNAL(attachmentCreated(const WIZDOCUMENTATTACHMENTDATA&)), \
            SLOT(on_attachment_created(const WIZDOCUMENTATTACHMENTDATA&)));

    connect(&m_dbMgr, SIGNAL(attachmentDeleted(const WIZDOCUMENTATTACHMENTDATA&)), \
            SLOT(on_attachment_deleted(const WIZDOCUMENTATTACHMENTDATA&)));

    connect(Core::ICore::instance(), SIGNAL(viewNoteRequested(Core::INoteView*,const WIZDOCUMENTDATA&)),
            SLOT(onViewNoteRequested(Core::INoteView*,const WIZDOCUMENTDATA&)));

    connect(Core::ICore::instance(), SIGNAL(viewNoteLoaded(Core::INoteView*,WIZDOCUMENTDATA,bool)),
            SLOT(onViewNoteLoaded(Core::INoteView*,const WIZDOCUMENTDATA&,bool)));
}

CWizDocumentView::~CWizDocumentView()
{
    m_web->saveDocument(m_note, false);
}

void CWizDocumentView::showEvent(QShowEvent *event)
{
    Q_UNUSED(event);
}

void CWizDocumentView::showClient(bool visible)
{
    m_client->setVisible(visible);
}

void CWizDocumentView::onViewNoteRequested(INoteView* view, const WIZDOCUMENTDATA& doc)
{
    if (view != this)
        return;

    if (doc.tCreated.secsTo(QDateTime::currentDateTime()) == 0) {
        viewNote(doc, true);
    } else {
        viewNote(doc, false);
    }
}

void CWizDocumentView::onViewNoteLoaded(INoteView* view, const WIZDOCUMENTDATA& doc, bool bOk)
{
    if (view != this)
        return;

    showClient(bOk);
}

bool CWizDocumentView::reload()
{
    bool ret = m_dbMgr.db(m_note.strKbGUID).DocumentFromGUID(m_note.strGUID, m_note);
    m_title->updateInfo(note());

    return ret;
}

void CWizDocumentView::reloadNote()
{
    reload();
    m_web->reloadNoteData(note());
}

bool CWizDocumentView::defaultEditingMode()
{
    switch (m_viewMode) {
    case viewmodeAlwaysEditing:
        return true;
    case viewmodeAlwaysReading:
        return false;
    default:
        return  m_bEditingMode; // default is Reading Mode
    }
}

void CWizDocumentView::initStat(const WIZDOCUMENTDATA& data, bool bEditing)
{
    m_bLocked = false;
    int nLockReason = -1;

    if (CWizDatabase::IsInDeletedItems(data.strLocation)) {
        nLockReason = 1;
        m_bLocked = true;
    }

    if (!m_dbMgr.db(data.strKbGUID).CanEditDocument(data)) {
        nLockReason = 2;
        m_bLocked = true;
    }

    if (m_bLocked) {
        m_bEditingMode = false;
    } else {
        m_bEditingMode = bEditing ? true : defaultEditingMode();
    }

    bool bGroup = m_dbMgr.db(data.strKbGUID).IsGroup();
    m_title->setLocked(m_bLocked, nLockReason, bGroup);
}

void CWizDocumentView::viewNote(const WIZDOCUMENTDATA& data, bool forceEdit)
{
    m_web->saveDocument(m_note, false);

    initStat(data, forceEdit);

    m_web->viewDocument(data, m_bEditingMode);
    m_title->setNote(data, m_bEditingMode, m_bLocked);

    // save last
    m_note = data;
}

void CWizDocumentView::setEditNote(bool bEdit)
{
    if (m_bLocked) {
        return;
    }

    m_bEditingMode = bEdit;

    m_title->setEditingDocument(bEdit);
    m_web->setEditingDocument(bEdit);
}

void CWizDocumentView::setViewMode(int mode)
{
    m_viewMode = mode;

    switch (m_viewMode)
    {
    case viewmodeAlwaysEditing:
        setEditNote(true);
        break;
    case viewmodeAlwaysReading:
        setEditNote(false);
        break;
    default:
        Q_ASSERT(0);
        break;
    }
}

void CWizDocumentView::settingsChanged()
{
    setViewMode(m_userSettings.noteViewMode());
}

void CWizDocumentView::setEditorFocus()
{
    m_web->setFocus(Qt::MouseFocusReason);
    m_web->editorFocus();
}

QWebFrame* CWizDocumentView::noteFrame()
{
    return m_web->noteFrame();
}

void CWizDocumentView::on_attachment_created(const WIZDOCUMENTATTACHMENTDATA& attachment)
{
    if (attachment.strDocumentGUID != note().strGUID)
        return;

    reload();
}

void CWizDocumentView::on_attachment_deleted(const WIZDOCUMENTATTACHMENTDATA& attachment)
{
    if (attachment.strDocumentGUID != note().strGUID)
        return;

    reload();
}

void CWizDocumentView::on_document_modified(const WIZDOCUMENTDATA& documentOld, const WIZDOCUMENTDATA& documentNew)
{
    Q_UNUSED(documentOld);

    if (note().strGUID != documentNew.strGUID)
        return;

    reload();
}

void CWizDocumentView::on_document_data_modified(const WIZDOCUMENTDATA& data)
{
    if (note().strGUID != data.strGUID)
        return;

    reloadNote();
}
