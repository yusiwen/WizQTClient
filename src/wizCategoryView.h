#ifndef WIZCATEGORYCTRL_H
#define WIZCATEGORYCTRL_H

#include <QTreeWidget>
#include <QPointer>

#include "wizCategoryViewItem.h"

class CWizFolder;
class CWizScrollBar;
class CWizDatabaseManager;
class CWizExplorerApp;
class CWizWebSettingsDialog;
class QSettings;

#define CATEGORY_MESSAGES_ALL               QObject::tr("Message Center")
#define CATEGORY_MESSAGES_SEND_TO_ME        QObject::tr("Send to me")
#define CATEGORY_MESSAGES_MODIFY            QObject::tr("Note modified")
#define CATEGORY_MESSAGES_COMMENTS          QObject::tr("Comments")
#define CATEGORY_MESSAGES_SEND_FROM_ME      QObject::tr("Send from me")

class CWizCategoryBaseView : public QTreeWidget
{
    Q_OBJECT

public:
    CWizCategoryBaseView(CWizExplorerApp& app, QWidget *parent = 0);
    virtual ~CWizCategoryBaseView();

    QString selectedItemKbGUID();
    void getDocuments(CWizDocumentDataArray& arrayDocument);
    bool acceptDocument(const WIZDOCUMENTDATA& document);
    void addSeparator();
    CWizCategoryViewSpacerItem* addSpacer();

    void saveSelection();
    void restoreSelection();

    template <class T> T* currentCategoryItem() const;
    CWizCategoryViewItemBase* categoryItemFromIndex(const QModelIndex &index) const;
    bool isHelperItemByIndex(const QModelIndex &index) const;
    bool isSeparatorItemByPosition(const QPoint& pt) const;

    bool isDragHovered() const { return m_bDragHovered; }
    QPoint dragHoveredPos() const { return m_dragHoveredPos; }
    bool validateDropDestination(const QPoint& p) const;

    void drawItem(QPainter* p, const QStyleOptionViewItemV4 *vopt) const;

protected:
    virtual void startDrag(Qt::DropActions supportedActions);
    virtual void dragEnterEvent(QDragEnterEvent* event);
    virtual void dragMoveEvent(QDragMoveEvent* event);
    virtual void dragLeaveEvent(QDragLeaveEvent* event);
    virtual void dropEvent(QDropEvent* event);

    virtual void resizeEvent(QResizeEvent* event);
    virtual void contextMenuEvent(QContextMenuEvent* e);

    virtual QModelIndex moveCursor(CursorAction cursorAction, Qt::KeyboardModifiers modifiers);

protected:
    CWizExplorerApp& m_app;
    CWizDatabaseManager& m_dbMgr;
    QTreeWidgetItem* m_selectedItem;

private:
    bool m_bDragHovered;
    QPoint m_dragHoveredPos;
    CWizScrollBar* m_vScroll;

protected Q_SLOTS:
    virtual void on_document_created(const WIZDOCUMENTDATA& doc) { Q_UNUSED(doc); }
    virtual void on_document_modified(const WIZDOCUMENTDATA& documentOld,
                                      const WIZDOCUMENTDATA& documentNew)
    { Q_UNUSED(documentOld); Q_UNUSED(documentNew); }
    virtual void on_document_deleted(const WIZDOCUMENTDATA& doc) { Q_UNUSED(doc); }
    virtual void on_document_tag_modified(const WIZDOCUMENTDATA& doc) { Q_UNUSED(doc); }

    virtual void on_folder_created(const QString& strLocation) { Q_UNUSED(strLocation); }
    virtual void on_folder_deleted(const QString& strLocation) { Q_UNUSED(strLocation); }
    virtual void on_folder_positionChanged() {}

    virtual void on_tag_created(const WIZTAGDATA& tag) { Q_UNUSED(tag); }
    virtual void on_tag_modified(const WIZTAGDATA& tagOld, const WIZTAGDATA& tagNew) { Q_UNUSED(tagOld); Q_UNUSED(tagNew); }
    virtual void on_tag_deleted(const WIZTAGDATA& tag) { Q_UNUSED(tag); }

    virtual void on_group_opened(const QString& strKbGUID) { Q_UNUSED(strKbGUID); }
    virtual void on_group_closed(const QString& strKbGUID) { Q_UNUSED(strKbGUID); }
    virtual void on_group_renamed(const QString& strKbGUID) { Q_UNUSED(strKbGUID); }
    virtual void on_group_permissionChanged(const QString& strKbGUID) { Q_UNUSED(strKbGUID); }
    virtual void on_group_bizChanged(const QString& strKbGUID) { Q_UNUSED(strKbGUID); }
};


class CWizCategoryView : public CWizCategoryBaseView
{
    Q_OBJECT

public:
    CWizCategoryView(CWizExplorerApp& app, QWidget *parent = 0);
    void init();

    QString m_strSelectedId;
    QString selectedId(QSettings* settings);

    void loadState();
    void loadChildState(QTreeWidgetItem* pi, QSettings* settings);
    void loadItemState(QTreeWidgetItem* pi, QSettings* settings);
    void saveState();
    void saveChildState(QTreeWidgetItem* pi, QSettings* settings);
    void saveItemState(QTreeWidgetItem* pi, QSettings* settings);
    void saveSelected(QSettings* settings);

    // action user data
    enum CategoryActions
    {
        ActionNewDocument,
        ActionNewItem,
        ActionMoveItem,
        ActionRenameItem,
        ActionDeleteItem,
        ActionItemAttribute,
        ActionEmptyTrash
    };

    enum CategoryMenuType
    {
        FolderRootItem,
        FolderItem,
        TagRootItem,
        TagItem,
        GroupRootItem,
        GroupItem,
        TrashItem
    };

    void initMenus();

    // becase Qt can't invoke two action with one shortcut keys, we can only use
    // one QAction and for different type of usages, and delegate task, so reset
    // menu text is necessary.
    void resetMenu(CategoryMenuType type);
    void setActionsEnabled(bool enable);

    void showFolderRootContextMenu(QPoint pos);
    void showFolderContextMenu(QPoint pos);
    void showTagRootContextMenu(QPoint pos);
    void showTagContextMenu(QPoint pos);
    void showGroupRootContextMenu(QPoint pos);
    void showGroupContextMenu(QPoint pos);
    void showTrashContextMenu(QPoint pos);

private:
    void initGeneral();
    void initFolders();
    void initFolders(QTreeWidgetItem* pParent, const QString& strParentLocation, \
                     const CWizStdStringArray& arrayAllLocation);//, const QMap<QString, int> &mfpos);
    void initTags();
    void initTags(QTreeWidgetItem* pParent, const QString& strParentTagGUID);
    void initStyles();
    void initGroups();
    void initGroup(CWizDatabase& db);
    void initGroup(CWizDatabase& db, QTreeWidgetItem* pParent,
                   const QString& strParentTagGUID);

    void doLocationSanityCheck(CWizStdStringArray& arrayLocation);

public:
    // folders
    CWizCategoryViewFolderItem* findFolder(const QString& strLocation, bool create, bool sort);
    CWizCategoryViewFolderItem* addFolder(const QString& strLocation, bool sort);
    void addAndSelectFolder(const CString& strLocation);

    void sortFolders();
    void sortFolders(CWizCategoryViewFolderItem* pItem);

    // tags
    CWizCategoryViewTagItem* findTag(const WIZTAGDATA& tag, bool create, bool sort);
    CWizCategoryViewTagItem* addTagWithChildren(const WIZTAGDATA& tag);
    CWizCategoryViewTagItem* addTag(const WIZTAGDATA& tag, bool sort);
    CWizCategoryViewTagItem* addAndSelectTag(const WIZTAGDATA& tag);
    CWizCategoryViewTagItem* findTagInTree(const WIZTAGDATA& tag);
    CWizCategoryViewTagItem* findTagInTree(const WIZTAGDATA& tag,
                                           QTreeWidgetItem* itemParent);
    void removeTag(const WIZTAGDATA& tag);

    // groups
    CWizCategoryViewGroupItem* findGroupFolder(const WIZTAGDATA& tag, bool create, bool sort);
    CWizCategoryViewGroupItem* addGroupFolderWithChildren(const WIZTAGDATA& tag);
    CWizCategoryViewGroupRootItem* findGroup(const QString& strKbGUID);
    CWizCategoryViewGroupItem* findGroupFolderInTree(const WIZTAGDATA& tag);
    CWizCategoryViewGroupItem* findGroupFolderInTree(const WIZTAGDATA& tag,
                                                     QTreeWidgetItem* itemParent);
    void removeGroupFolder(const WIZTAGDATA& tag);

    // helper
    QAction* findAction(CategoryActions type);
    CWizCategoryViewItemBase* findCategory(const QString& strName, bool bCreate = true);
    CWizCategoryViewTrashItem* findTrash(const QString& strKbGUID = NULL);

    // document count update
    void updateFolderDocumentCount();
    void updateFolderDocumentCount_impl();
    int updateFolderDocumentCount_impl(CWizCategoryViewItemBase* pItem,
                                       const std::map<CString, int>& mapDocumentCount);

    void updateTagDocumentCount(const QString& strKbGUID = NULL);
    void updateTagDocumentCount_impl(const QString& strKbGUID = NULL);
    int updateTagDocumentCount_impl(CWizCategoryViewItemBase* pItem,
                                    const std::map<CString, int>& mapDocumentCount);

    void updatePrivateTagDocumentCount();
    void updateGroupTagDocumentCount(const QString &strKbGUID);

    void createDocument(WIZDOCUMENTDATA& data);

private:
    QPointer<QMenu> m_menuFolderRoot;
    QPointer<QMenu> m_menuFolder;
    QPointer<QMenu> m_menuTagRoot;
    QPointer<QMenu> m_menuTag;
    QPointer<QMenu> m_menuGroupRoot;
    QPointer<QMenu> m_menuGroup;
    QPointer<QMenu> m_menuTrash;
    QPointer<CWizWebSettingsDialog> m_groupSettings;
    QPointer<QTimer> m_timerUpdateFolderCount;
    QPointer<QTimer> m_timerUpdateTagCount;
    QMap<QString, QTimer*> m_mapTimerUpdateGroupCount;

    QString m_strRequestedGroupKbGUID;

private Q_SLOTS:
    void on_updateFolderDocumentCount_timeout();
    void on_updateTagDocumentCount_timeout();
    void on_updateTagDocumentCount_mapped_timeout(const QString& strKbGUID);

protected Q_SLOTS:
    virtual void on_document_created(const WIZDOCUMENTDATA& doc);
    virtual void on_document_modified(const WIZDOCUMENTDATA& docOld,
                                      const WIZDOCUMENTDATA& docNew);
    virtual void on_document_deleted(const WIZDOCUMENTDATA& doc);
    virtual void on_document_tag_modified(const WIZDOCUMENTDATA& doc);

    virtual void on_folder_created(const QString& strLocation);
    virtual void on_folder_deleted(const QString& strLocation);
    virtual void on_folder_positionChanged();

    virtual void on_tag_created(const WIZTAGDATA& tag);
    virtual void on_tag_modified(const WIZTAGDATA& tagOld, const WIZTAGDATA& tagNew);
    virtual void on_tag_deleted(const WIZTAGDATA& tag);

    virtual void on_group_opened(const QString& strKbGUID);
    virtual void on_group_closed(const QString& strKbGUID);
    virtual void on_group_renamed(const QString& strKbGUID);
    virtual void on_group_permissionChanged(const QString& strKbGUID);
    virtual void on_group_bizChanged(const QString& strKbGUID);

public Q_SLOTS:
    void on_action_newDocument();

    void on_action_newItem();
    void on_action_user_newFolder();
    void on_action_user_newFolder_confirmed(int result);
    void on_action_user_newTag();
    void on_action_user_newTag_confirmed(int result);
    void on_action_group_newFolder();
    void on_action_group_newFolder_confirmed(int result);

    void on_action_moveItem();
    void on_action_user_moveFolder();
    void on_action_user_moveFolder_confirmed(int result);
    void on_action_user_moveFolder_confirmed_progress(int nMax, int nValue,
                                                      const QString& strOldLocation,
                                                      const QString& strNewLocation,
                                                      const WIZDOCUMENTDATA& data);

    void on_action_renameItem();
    void on_action_user_renameFolder();
    void on_action_user_renameFolder_confirmed(int result);
    void on_action_user_renameFolder_confirmed_progress(int nMax, int nValue,
                                                        const QString& strOldLocation,
                                                        const QString& strNewLocation,
                                                        const WIZDOCUMENTDATA& data);
    void on_action_user_renameTag();
    void on_action_user_renameTag_confirmed(int result);
    void on_action_group_renameFolder();
    void on_action_group_renameFolder_confirmed(int result);

    void on_action_deleteItem();
    void on_action_user_deleteFolder();
    void on_action_user_deleteFolder_confirmed(int result);
    void on_action_user_deleteTag();
    void on_action_user_deleteTag_confirmed(int result);
    void on_action_group_deleteFolder();
    void on_action_group_deleteFolder_confirmed(int result);

    void on_action_itemAttribute();
    void on_action_group_attribute();
    void on_action_group_attribute_showProgress();
    void on_action_group_attribute_requested(const QString& strToken);


    void on_action_emptyTrash();

    void on_itemSelectionChanged();

Q_SIGNALS:
    void newDocument();
    void documentsHint(const QString& strHint);

public:
    // Public API:
    Q_INVOKABLE CWizFolder* SelectedFolder();
};


#endif // WIZCATEGORYCTRL_H
