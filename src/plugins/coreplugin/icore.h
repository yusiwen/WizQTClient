#ifndef CORE_ICORE_H
#define CORE_ICORE_H

#include <QObject>

#include "core_global.h"

struct WIZDOCUMENTDATA;

namespace Core {
class INoteView;

namespace Internal {
class MainWindow;
}

class CORE_EXPORT ICore : public QObject
{
    Q_OBJECT

    friend class Internal::MainWindow;
    explicit ICore(Internal::MainWindow* mw);
    ~ICore();

public:
    static ICore* instance();
    static Internal::MainWindow* mainWindow();

    static void emitViewNoteRequested(INoteView* view, const WIZDOCUMENTDATA& doc);
    static void emitViewNoteLoaded(INoteView* view, const WIZDOCUMENTDATA& doc, bool bOk);

Q_SIGNALS:
    void viewNoteRequested(Core::INoteView* view, const WIZDOCUMENTDATA& doc);
    void viewNoteLoaded(Core::INoteView* view, const WIZDOCUMENTDATA& doc, bool bOk);
    void closeNoteRequested(Core::INoteView* view);
};

} // namespace Core

#endif // CORE_ICORE_H
