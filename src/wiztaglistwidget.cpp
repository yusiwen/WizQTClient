#include "wiztaglistwidget.h"

#include <QLineEdit>
#include <QListWidget>
#include <QListWidgetItem>
#include <QVBoxLayout>
#include <QLabel>

#include "share/wizDatabaseManager.h"
#include "share/wizuihelper.h"
#include "share/wizkmcore.h"
#include "share/wizobject.h"
#include "share/wizDatabase.h"


class CWizTagListWidgetItem: public QListWidgetItem
{
public:
    CWizTagListWidgetItem(const WIZTAGDATA& tag, QListWidget* parent = 0)
        : QListWidgetItem(parent)
        , m_tag(tag)
    {
        setText(CWizDatabase::TagNameToDisplayName(tag.strName));
        setFlags(flags() | Qt::ItemIsUserCheckable);
        setCheckState(Qt::Unchecked);
    }

    const WIZTAGDATA& tag() const { return m_tag; }

private:
    WIZTAGDATA m_tag;
};


CWizTagListWidget::CWizTagListWidget(QWidget* parent)
    : CWizPopupWidget(parent)
    , m_dbMgr(*CWizDatabaseManager::instance())
    , m_bUpdating(false)
{
    setContentsMargins(0, 20, 0, 0);

    m_tagsEdit = new QLineEdit(this);
    m_tagsEdit->setPlaceholderText(tr("Use semicolon to seperate tags..."));
    connect(m_tagsEdit, SIGNAL(returnPressed()), SLOT(on_tagsEdit_returnPressed()));

    m_list = new QListWidget(this);
    m_list->setAttribute(Qt::WA_MacShowFocusRect, false);
    connect(m_list, SIGNAL(itemChanged(QListWidgetItem*)),
            SLOT(on_list_itemChanged(QListWidgetItem*)));

    QPalette pal;
#ifdef Q_OS_LINUX
    pal.setBrush(QPalette::Base, QBrush("#D7D7D7"));
#elif defined(Q_OS_MAC)
    pal.setBrush(QPalette::Base, QBrush("#F7F7F7"));
#endif
    m_list->setPalette(pal);

    QVBoxLayout* layout = new QVBoxLayout();
    layout->setContentsMargins(0, 0, 0, 0);
    setLayout(layout);

    QHBoxLayout* layoutTitle = new QHBoxLayout();

    layoutTitle->addWidget(new QLabel(tr("Tags:"), this));
    layoutTitle->addWidget(m_tagsEdit);
    layoutTitle->setSpacing(8);
    layoutTitle->setMargin(4);

    layout->addLayout(layoutTitle);
    layout->addWidget(m_list);

    //setGeometry(0, 0, sizeHint().width(), sizeHint().height());
}

void CWizTagListWidget::showEvent(QShowEvent* event)
{
    Q_UNUSED(event);

    m_tagsEdit->clear();
    m_tagsEdit->clearFocus();
}

void CWizTagListWidget::reloadTags()
{
    m_list->clear();

    CWizTagDataArray arrayTag;
    m_dbMgr.db().GetAllTags(arrayTag);

    foreach (const WIZTAGDATA& tag, arrayTag) {
        CWizTagListWidgetItem* item = new CWizTagListWidgetItem(tag, m_list);
        m_list->addItem(item);
    }
}

void CWizTagListWidget::setDocument(const WIZDOCUMENTDATAEX& doc)
{
    Q_ASSERT(doc.strKbGUID == m_dbMgr.db().kbGUID());

    m_bUpdating = true;

    reloadTags();

    m_arrayDocuments.clear();
    m_arrayDocuments.push_back(doc);
    QString strGUIDs = m_dbMgr.db().GetDocumentTagGUIDsString(doc.strGUID);

    for (int i = 0; i < m_list->count(); i++) {
        CWizTagListWidgetItem* pItem = dynamic_cast<CWizTagListWidgetItem*>(m_list->item(i));

        if (-1 != strGUIDs.indexOf(pItem->tag().strGUID)) {
            pItem->setCheckState(Qt::Checked);
        }
    }

    m_bUpdating = false;
}

void CWizTagListWidget::setDocuments(const CWizDocumentDataArray& arrayDocument)
{
    m_bUpdating = true;

    reloadTags();
    m_arrayDocuments.clear();

    if (arrayDocument.size() == 0)
        return;

    QString strAllGUIDs;
    for (CWizDocumentDataArray::const_iterator it = arrayDocument.begin();
         it != arrayDocument.end(); it++) {
        const WIZDOCUMENTDATAEX&  doc = *it;
        Q_ASSERT(doc.strKbGUID == m_dbMgr.db().kbGUID());

        m_arrayDocuments.push_back(doc);

        strAllGUIDs += (m_dbMgr.db().GetDocumentTagGUIDsString(doc.strGUID) + ";");
    }

    QStringList listGUIDs = strAllGUIDs.split(";");

    for (int i = 0; i < m_list->count(); i++) {
        CWizTagListWidgetItem* pItem = dynamic_cast<CWizTagListWidgetItem*>(m_list->item(i));

        int n  = listGUIDs.count(pItem->tag().strGUID);
        if (n  && n < arrayDocument.size()) {
            pItem->setCheckState(Qt::PartiallyChecked);
        } else if (n == arrayDocument.size()) {
            pItem->setCheckState(Qt::Checked);
        }
    }

    m_bUpdating = false;
}

void CWizTagListWidget::on_list_itemChanged(QListWidgetItem* pItem)
{
    if (m_bUpdating)
        return;

    CWizTagListWidgetItem* pItemTag = dynamic_cast<CWizTagListWidgetItem *>(pItem);
    if (!pItemTag)
        return;

    for (CWizDocumentDataArray::const_iterator it = m_arrayDocuments.begin();
         it != m_arrayDocuments.end(); it++) {
        CWizDocument doc(m_dbMgr.db(), *it);

        if (pItemTag->checkState() == Qt::Checked) {
            doc.AddTag(pItemTag->tag());
        } else if (pItemTag->checkState() == Qt::Unchecked) {
            doc.RemoveTag(pItemTag->tag());
        } else {
            Q_ASSERT(0);
        }
    }
}

void CWizTagListWidget::on_tagsEdit_returnPressed()
{
    QString tagsText = m_tagsEdit->text();

    CWizTagDataArray arrayTagNew;
    m_dbMgr.db().TagsTextToTagArray(tagsText, arrayTagNew);

    for (CWizDocumentDataArray::iterator it = m_arrayDocuments.begin();
         it != m_arrayDocuments.end(); it++) {
        WIZDOCUMENTDATAEX& doc = *it;

        for (CWizTagDataArray::const_iterator it = arrayTagNew.begin();
             it != arrayTagNew.end(); it++) {
            const WIZTAGDATA& tag = *it;
            m_dbMgr.db().InsertDocumentTag(doc, tag.strGUID);
        }
    }

    CWizDocumentDataArray arrayDocument(m_arrayDocuments);
    setDocuments(arrayDocument);    //refresh tags
}
