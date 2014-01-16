#include "wizDatabaseManager.h"

#include <QDebug>

#include "wizDatabase.h"
#include "utils/logger.h"

static CWizDatabaseManager* m_instance = 0;

CWizDatabaseManager* CWizDatabaseManager::instance()
{
    return m_instance;
}

CWizDatabaseManager::CWizDatabaseManager(const QString& strUserId)
    : m_strUserId(strUserId)
{
    Q_ASSERT(!m_instance);

    m_instance = this;
}

CWizDatabaseManager::~CWizDatabaseManager()
{
    m_instance = 0;
    closeAll();
}

bool CWizDatabaseManager::open(const QString& strKbGUID)
{
    Q_ASSERT(!m_strUserId.isEmpty());

    if (isOpened(strKbGUID))
        return true;

    CWizDatabase* db = new CWizDatabase();

    if (!db->Open(m_strUserId, strKbGUID)) {
        delete db;
        return false;
    }

    if (strKbGUID.isEmpty()) {
        m_dbPrivate = db;

        // take ownership immediately
        connect(db, SIGNAL(databaseOpened(CWizDatabase*, const QString&)),
                SLOT(on_groupDatabaseOpened(CWizDatabase*, const QString&)),
                Qt::BlockingQueuedConnection);
    } else {
        m_mapGroups[strKbGUID] = db;
    }

    initSignals(db);

    Q_EMIT databaseOpened(strKbGUID);
    return true;
}

bool CWizDatabaseManager::openAll()
{
    // first, open private db
    if (!open()) {
        TOLOG("open user private database failed");
        return false;
    }

    // second, get groups info
    CWizGroupDataArray arrayGroup;
    if (!m_dbPrivate->GetUserGroupInfo(arrayGroup)) {
        TOLOG("Failed to get user group info");
        return true;
    }

    // third, open groups one by one
    CWizGroupDataArray::const_iterator it;
    for (it = arrayGroup.begin(); it != arrayGroup.end(); it++) {
        WIZGROUPDATA group = *it;
        if (!open(group.strGroupGUID)) {
            TOLOG1("failed to open group: %1", group.strGroupName);
        }
    }

    return true;
}

bool CWizDatabaseManager::isOpened(const QString& strKbGUID)
{
    if (m_dbPrivate && (strKbGUID.isEmpty() || m_dbPrivate->kbGUID() == strKbGUID)) {
        return true;
    }

    QMap<QString, CWizDatabase*>::const_iterator it = m_mapGroups.find(strKbGUID);
    if (it != m_mapGroups.end())
        return true;

    return false;
}

CWizDatabase& CWizDatabaseManager::db(const QString& strKbGUID)
{
    Q_ASSERT(m_dbPrivate);

    if (strKbGUID.isEmpty() || m_dbPrivate->kbGUID() == strKbGUID) {
        return *m_dbPrivate;
    }

    QMap<QString, CWizDatabase*>::iterator it = m_mapGroups.find(strKbGUID);
    if (it != m_mapGroups.end()) {
        return *(it.value());
    }

    qDebug() << "[CWizDatabaseManager] request db not exist, create it: " << strKbGUID;

    if (!open(strKbGUID)) {
        qDebug() << "[CWizDatabaseManager] failed to open new datebase: " << strKbGUID;
    }

    return db(strKbGUID);
}

//void CWizDatabaseManager::Guids(QStringList& strings)
//{
//    QList<CWizDatabase*>::const_iterator it;
//    for (it = m_dbGroups.begin(); it != m_dbGroups.end(); it++) {
//        CWizDatabase* db = *it;
//        strings.append(db->kbGUID());
//    }
//}

int CWizDatabaseManager::count()
{
    return m_mapGroups.size();
}

CWizDatabase& CWizDatabaseManager::at(int i)
{
    Q_ASSERT(i < count() && i >= 0);

    CWizDatabase* db = m_mapGroups.values().value(i);
    return *db;
}

//bool CWizDatabaseManager::removeKb(const QString& strKbGUID)
//{
//    Q_UNUSED(strKbGUID);
//    return false;
//}

bool CWizDatabaseManager::close(const QString& strKbGUID, bool bNotify)
{
    // should close all groups db before close user db.
    if (strKbGUID.isEmpty()) {
        Q_ASSERT(m_mapGroups.isEmpty());

        m_dbPrivate->Close();
        m_dbPrivate->deleteLater();
        return true;
    }

    qDebug() << "[CWizDatabaseManager] closed database, "
             << "kb_guid: " << m_mapGroups.value(strKbGUID)->kbGUID()
             << " name: " << m_mapGroups.value(strKbGUID)->name();

    QMap<QString, CWizDatabase*>::const_iterator it = m_mapGroups.find(strKbGUID);
    if (it != m_mapGroups.end()) {
        it.value()->Close();
        it.value()->deleteLater();
        m_mapGroups.remove(strKbGUID);
    } else {
        return false;
    }

    if (bNotify) {
        Q_EMIT databaseClosed(strKbGUID);
    }

    return true;
}

void CWizDatabaseManager::closeAll()
{
    qDebug() << "[CWizDatabaseManager] total " << m_mapGroups.size() << " database needed close";

    QList<CWizDatabase*> dbs = m_mapGroups.values();
    for (int i = 0; i < dbs.size(); i++) {
        CWizDatabase* db = dbs.at(i);

        close(db->kbGUID());
    }

    // close private db at last
    close();
}

void CWizDatabaseManager::initSignals(CWizDatabase* db)
{
    connect(db, SIGNAL(groupsInfoDownloaded(const CWizGroupDataArray&)),
            SLOT(on_groupsInfoDownloaded(const CWizGroupDataArray&)),
            Qt::BlockingQueuedConnection);

    //connect(db, SIGNAL(databaseOpened(const QString&)),
    //        SIGNAL(databaseOpened(const QString&)));

    connect(db, SIGNAL(databaseRename(const QString&)),
            SIGNAL(databaseRename(const QString&)));

    connect(db, SIGNAL(databasePermissionChanged(const QString&)),
            SIGNAL(databasePermissionChanged(const QString&)));

    connect(db, SIGNAL(databaseBizChanged(const QString&)),
            SIGNAL(databaseBizchanged(const QString&)));

    connect(db, SIGNAL(tagCreated(const WIZTAGDATA&)),
            SIGNAL(tagCreated(const WIZTAGDATA&)));
    connect(db, SIGNAL(tagModified(const WIZTAGDATA&, const WIZTAGDATA&)),
            SIGNAL(tagModified(const WIZTAGDATA&, const WIZTAGDATA&)));
    connect(db, SIGNAL(tagDeleted(const WIZTAGDATA&)),
            SIGNAL(tagDeleted(const WIZTAGDATA&)));

    connect(db, SIGNAL(styleCreated(const WIZSTYLEDATA&)),
            SIGNAL(styleCreated(const WIZSTYLEDATA&)));
    connect(db, SIGNAL(styleModified(const WIZSTYLEDATA&, const WIZSTYLEDATA&)),
            SIGNAL(styleModified(const WIZSTYLEDATA&, const WIZSTYLEDATA&)));
    connect(db, SIGNAL(styleDeleted(const WIZSTYLEDATA&)),
            SIGNAL(styleDeleted(const WIZSTYLEDATA&)));

    connect(db, SIGNAL(documentCreated(const WIZDOCUMENTDATA&)),
            SIGNAL(documentCreated(const WIZDOCUMENTDATA&)));
    connect(db, SIGNAL(documentModified(const WIZDOCUMENTDATA&, const WIZDOCUMENTDATA&)),
            SIGNAL(documentModified(const WIZDOCUMENTDATA&, const WIZDOCUMENTDATA&)));
    connect(db, SIGNAL(documentDeleted(const WIZDOCUMENTDATA&)),
            SIGNAL(documentDeleted(const WIZDOCUMENTDATA&)));

    connect(db, SIGNAL(documentTagModified(const WIZDOCUMENTDATA&)),
            SIGNAL(documentTagModified(const WIZDOCUMENTDATA&)));

    connect(db, SIGNAL(documentDataModified(const WIZDOCUMENTDATA&)),
            SIGNAL(documentDataModified(const WIZDOCUMENTDATA&)));
    connect(db, SIGNAL(documentAbstractModified(const WIZDOCUMENTDATA&)),
            SIGNAL(documentAbstractModified(const WIZDOCUMENTDATA&)));

    connect(db, SIGNAL(attachmentCreated(const WIZDOCUMENTATTACHMENTDATA&)),
            SIGNAL(attachmentCreated(const WIZDOCUMENTATTACHMENTDATA&)));
    connect(db, SIGNAL(attachmentModified(const WIZDOCUMENTATTACHMENTDATA&, const WIZDOCUMENTATTACHMENTDATA&)),
            SIGNAL(attachmentModified(const WIZDOCUMENTATTACHMENTDATA&, const WIZDOCUMENTATTACHMENTDATA&)));
    connect(db, SIGNAL(attachmentDeleted(const WIZDOCUMENTATTACHMENTDATA&)),
            SIGNAL(attachmentDeleted(const WIZDOCUMENTATTACHMENTDATA&)));

    connect(db, SIGNAL(folderCreated(const QString&)),
            SIGNAL(folderCreated(const QString&)));
    connect(db, SIGNAL(folderDeleted(const QString&)),
            SIGNAL(folderDeleted(const QString&)));
    connect(db, SIGNAL(folderPositionChanged()),
            SIGNAL(folderPositionChanged()));
}

void CWizDatabaseManager::on_groupDatabaseOpened(CWizDatabase* pDb, const QString& strKbGUID)
{
    // check if this group is already opened, to avoid memory leak!
    if (isOpened(strKbGUID)) {
        close(strKbGUID, false);
    }

    m_mapGroups[strKbGUID] = pDb;
    initSignals(pDb);

    Q_EMIT databaseOpened(strKbGUID);
}

void CWizDatabaseManager::on_groupsInfoDownloaded(const CWizGroupDataArray& arrayGroups)
{
    qDebug() << "[CWizDatabaseManager] Group info downloaded...";

    // set database info
    CWizGroupDataArray::const_iterator it;
    for (it = arrayGroups.begin(); it != arrayGroups.end(); it++) {
        const WIZGROUPDATA& group = *it;

        WIZDATABASEINFO info;
        info.bizGUID = group.bizGUID;
        info.bizName = group.bizName;
        info.name = group.strGroupName;
        info.nPermission = group.nUserGroup;
        db(group.strGroupGUID).SetDatabaseInfo(info);
    }

    // FIXME : close database not inside group list
}
