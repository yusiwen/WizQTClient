#ifndef WIZSERVICE_INTERNAL_MESSAGELISTVIEW_H
#define WIZSERVICE_INTERNAL_MESSAGELISTVIEW_H

#include <QListWidget>
#include <deque>

class CWizScrollBar;

struct WIZMESSAGEDATA;
typedef std::deque<WIZMESSAGEDATA> CWizMessageDataArray;

namespace WizService {
namespace Internal {

class MessageListViewItem;

class MessageListView : public QListWidget
{
    Q_OBJECT

public:
    explicit MessageListView(QWidget *parent = 0);
    virtual QSize sizeHint() const { return QSize(200, 1); }

    void setMessages(const CWizMessageDataArray& arrayMsg);
    void addMessages(const CWizMessageDataArray& arrayMsg);
    void addMessage(const WIZMESSAGEDATA& msg, bool sort);
    void selectedMessages(QList<WIZMESSAGEDATA>& arrayMsg);

    int rowFromId(qint64 nId) const;
    MessageListViewItem* messageItem(int row) const;
    MessageListViewItem* messageItem(const QModelIndex& index) const;
    const WIZMESSAGEDATA& messageFromIndex(const QModelIndex& index) const;

    void drawItem(QPainter* p, const QStyleOptionViewItemV4* vopt) const;

protected:
    virtual void resizeEvent(QResizeEvent* event);
    virtual void contextMenuEvent(QContextMenuEvent* event);

private:
    CWizScrollBar* m_vScroll;
    QMenu* m_menu;

Q_SIGNALS:
    void sizeChanged(int nCount);

private Q_SLOTS:
    void onAvatarLoaded(const QString& strUserId);

    void on_action_message_mark_read();
    void on_action_message_delete();

    void on_message_created(const WIZMESSAGEDATA& msg);
    void on_message_modified(const WIZMESSAGEDATA& oldMsg,
                             const WIZMESSAGEDATA& newMsg);
    void on_message_deleted(const WIZMESSAGEDATA& msg);
};

} // namespace Internal
} // namespace WizService

#endif // WIZSERVICE_INTERNAL_MESSAGELISTVIEW_H
