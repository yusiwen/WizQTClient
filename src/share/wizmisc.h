#ifndef WIZMISC_H
#define WIZMISC_H

#include <stdint.h>

#include <QIcon>
#include <QBuffer>
#include <QByteArray>

#include "wizobject.h"
#include "wizmd5.h"

#define WIZNOTE_OBSOLETE

QString WizGetEmailPrefix(const QString& strMail);

QString WizGetTimeStamp();
COleDateTime WizGetCurrentTime();
bool WizStringToDateTime(const QString& str, COleDateTime& t, QString& strError);
COleDateTime WizStringToDateTime(const CString& str);
COLORREF WizStringToColor(const CString& str);
QColor WizStringToColor2(const CString& str);

std::string WizBSTR2UTF8(const CString& str);

CString WizFormatString0(const CString& str);
CString WizFormatString1(const CString& strFormat, int n);
CString WizFormatString1(const CString& strFormat, const CString& strParam1);
CString WizFormatString2(const CString& strFormat, const CString& strParam1, const CString& strParam2);
CString WizFormatString3(const CString& strFormat, const CString& strParam1, const CString& strParam2, const CString& strParam3);
CString WizFormatString4(const CString& strFormat, const CString& strParam1, const CString& strParam2, const CString& strParam3, const CString& strParam4);
CString WizFormatString5(const CString& strFormat, const CString& strParam1, const CString& strParam2, const CString& strParam3, const CString& strParam4, const CString& strParam5);
CString WizFormatString6(const CString& strFormat, const CString& strParam1, const CString& strParam2, const CString& strParam3, const CString& strParam4, const CString& strParam5, const CString& strParam6);
CString WizFormatString7(const CString& strFormat, const CString& strParam1, const CString& strParam2, const CString& strParam3, const CString& strParam4, const CString& strParam5, const CString& strParam6, const CString& strParam7);
CString WizFormatString8(const CString& strFormat, const CString& strParam1, const CString& strParam2, const CString& strParam3, const CString& strParam4, const CString& strParam5, const CString& strParam6, const CString& strParam7, const CString& strParam8);
CString WizFormatInt(__int64 n);

time_t WizTimeGetTimeT(const COleDateTime& t);

CString WizIntToStr(int n);

BOOL WizSplitTextToArray(const CString& strText, QChar ch, CWizStdStringArray& arrayResult);
BOOL WizSplitTextToArray(CString strText, const CString& strSplitterText, BOOL bMatchCase, CWizStdStringArray& arrayResult);
void WizStringArrayToText(const CWizStdStringArray& arrayText, CString& strText, const CString& strSplitter);
int WizFindInArray(const CWizStdStringArray& arrayText, const CString& strFind);
int WizFindInArrayNoCase(const CWizStdStringArray& arrayText, const CString& strFind);
void WizStringArrayEraseEmptyLine(CWizStdStringArray& arrayText);
void WizStringArrayRemoveMultiElement(CWizStdStringArray& arrayText);
void WizStringArrayRemoveMultiElementNoCase(CWizStdStringArray& arrayText);


CString WizStringArrayGetValue(const CWizStdStringArray& arrayText, const CString& valueName);
void WizCommandLineToStringArray(const CString& commandLine, CWizStdStringArray& arrayLine);
CString WizGetCommandLineValue(const CString& strCommandLine, const CString& strKey);

BOOL WizStringSimpleSplit(const CString& str, char ch, CString& strLeft, CString& strRight);

CString WizDateToLocalString(const COleDateTime& t);

QString WizGetAppPath();
QString WizGetAppFileName();
QString WizGetResourcesPath();
QString WizGetDataStorePath();
QString WizGetUpgradePath();
CString WizGetSettingsFileName();
QString WizGetLogFileName();
QString WizGetLocaleFileName(const QString& strLocale);
QString WizGetQtLocaleFileName(const QString& strLocale);
void WizGetTranslatedLocales(QStringList& locales);
QString WizGetTranslatedLocaleDisplayName(int index);

bool WizIsPredefinedLocation(const QString& strLocation);
QString WizLocation2Display(const QString& strLocation);

qint64 WizGetFileSize(const CString& strFileName);
QString WizGetFileSizeHumanReadalbe(const QString& strFileName);

void WizPathAddBackslash(QString& strPath);
void WizPathRemoveBackslash(CString& strPath);
CString WizPathAddBackslash2(const CString& strPath);
CString WizPathRemoveBackslash2(const CString& strPath);
void WizEnsurePathExists(const CString& strPath);
void WizEnsureFileExists(const QString& strFileName);

CString WizExtractFilePath(const CString& strFileName);
QString WizExtractFileName(const QString& strFileName);
QString WizExtractFileTitle(const QString& strFileName);
CString WizExtractTitleTemplate(const CString& strFileName);
CString WizExtractFileExt(const CString& strFileName);

#define EF_INCLUDEHIDDEN			0x01
#define EF_INCLUDESUBDIR			0x02

void WizEnumFiles(const QString& strPath, const QString& strExts, CWizStdStringArray& arrayFiles, UINT uFlags);
void WizEnumFolders(const QString& strPath, CWizStdStringArray& arrayFolders, UINT uFlags);
QString WizFolderNameByPath(const QString& strPath);

BOOL WizCopyFile(const CString& strSrcFileName, const CString& strDestFileName, BOOL bFailIfExists);
void WizGetNextFileName(CString& strFileName);

QString WizEncryptPassword(const QString& strPassword);
QString WizDecryptPassword(const QString& strEncryptedText);


bool WizLoadUnicodeTextFromFile(const QString& strFileName, QString& steText);
bool WizSaveUnicodeTextToUtf16File(const QString& strFileName, const QString& strText);
bool WizSaveUnicodeTextToUtf8File(const QString& strFileName, const QString& strText);
bool WizSaveUnicodeTextToUtf8File(const QString& strFileName, const QByteArray& strText);

CString WizDateTimeToIso8601String(const COleDateTime& t);
BOOL WizIso8601StringToDateTime(CString str, COleDateTime& t, CString& strError);
CString WizDateTimeToString(const COleDateTime& t);
CString WizStringToSQL(const CString& str);
CString WizTimeToSQL(const COleDateTime& t);
CString WizColorToString(COLORREF cr);
CString WizColorToString(const QColor& cr);
CString WizColorToSQL(COLORREF cr);
CString WizColorToSQL(const QColor& cr);

intptr_t WizStrStrI_Pos(const CString& str, const CString& Find, int nStart = 0);

CString WizInt64ToStr(__int64 n);

CString WizGenGUIDLowerCaseLetterOnly();

BOOL WizBase64Encode(const QByteArray& arrayData, QString& str);
BOOL WizBase64Decode(const QString& str, QByteArray& arrayData);

CString WizStringToBase64(const CString& strSource);
CString WizStringFromBase64(const CString& strBase64);

// skin related
QString WizGetDefaultSkinName();
void WizGetSkins(QStringList& skins);
QString WizGetSkinResourcePath(const QString& strSkinName);
QString WizGetSkinDisplayName(const QString& strSkinName, const QString& strLocale);
QString WizGetSkinResourceFileName(const QString& strSkinName, const QString& strName);
QIcon WizLoadSkinIcon(const QString& strSkinName, const QString& strIconName, QIcon::Mode mode = QIcon::Normal, QIcon::State state = QIcon::Off);
QIcon WizLoadSkinIcon(const QString& strSkinName, QColor forceground, const QString& strIconName);
QIcon WizLoadSkinIcon2(const QString& strSkinName, const QColor& blendColor, const QString& strIconName);

bool WizImageBlending(QImage& img, const QColor& blendColor, QIcon::Mode mode = QIcon::Normal);
void WizLoadSkinIcon3(QIcon& icon, const QString& strSkinName, const QString& strIconName,
                      QIcon::Mode mode, QIcon::State state, const QColor& blendColor);
QIcon WizLoadSkinIcon3(const QString& strIconName, QIcon::Mode mode);

void WizHtml2Text(const QString& strHtml, QString& strText);
void WizDeleteFolder(const CString& strPath);
void WizDeleteFile(const CString& strFileName);
BOOL WizDeleteAllFilesInFolder(const CString& strPath);

BOOL WizIsValidFileNameNoPath(const CString& strFileName);
void WizMakeValidFileNameNoPath(CString& strFileName);
void WizMakeValidFileNameNoPathLimitLength(CString& strFileName, int nMaxTitleLength);
void WizMakeValidFileNameNoPathLimitFullNameLength(CString& strFileName, int nMaxFullNameLength);
CString WizMakeValidFileNameNoPathReturn(const CString& strFileName);

bool WizSaveDataToFile(const QString& strFileName, const QByteArray& arrayData);
bool WizLoadDataFromFile(const QString& strFileName, QByteArray& arrayData);

class CWizBufferAlloc
{
public:
    CWizBufferAlloc(int nInitSize = 0);
    ~CWizBufferAlloc();
private:
    BYTE* m_pBuffer;
    int m_nSize;
public:
    BYTE* GetBuffer();
    BOOL SetNewSize(int nNewSize);
};


class CWaitCursor
{
public:
    CWaitCursor();
    ~CWaitCursor();
};

#endif // WIZMISC_H
