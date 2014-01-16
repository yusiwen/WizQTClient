#ifndef WIZDEF_H
#define WIZDEF_H

#include <QtGlobal>

#define WIZ_CLIENT_VERSION  "2.1.0"
#define WIZNOTE_FTS_VERSION "5"
#define WIZNOTE_THUMB_VERSION "3"

#if defined Q_OS_MAC
#define WIZ_CLIENT_TYPE     "QTMAC"
#elif defined Q_OS_LINUX
#define WIZ_CLIENT_TYPE     "QTLINUX"
#elif defined Q_OS_WIN
#define WIZ_CLIENT_TYPE     "QTWIN"
#endif

class QWidget;
class QObject;
class CWizDatabaseManager;
class CWizCategoryBaseView;
class CWizUserSettings;

class CWizExplorerApp
{
public:
    virtual QWidget* mainWindow() = 0;
    virtual QObject* object() = 0;
    virtual CWizDatabaseManager& databaseManager() = 0;
    virtual CWizCategoryBaseView& category() = 0;
    virtual CWizUserSettings& userSettings() = 0;
};

#endif // WIZDEF_H
