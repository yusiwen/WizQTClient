#include "wizAboutDialog.h"

#include <QApplication>
#include <QLabel>
#include <QFileInfo>
#include <QTextBrowser>
#include <QVBoxLayout>
#include <QIcon>
#include <QDateTime>

#include "wizdef.h"
#include "share/wizmisc.h"


CWizAboutDialog::CWizAboutDialog(QWidget *parent)
    : QDialog(parent)
{
    QLabel* labelIcon = new QLabel(this);
    labelIcon->setPixmap(qApp->windowIcon().pixmap(QSize(58, 58)));

#if defined Q_OS_MAC
    QString strProduct("<span style=\"font-weight:bold;font-size:14px\">WizNote for Mac</span>");
#elif defined Q_OS_LINUX
    QString strProduct("<span style=\"font-weight:bold;font-size:14px\">WizNote for Linux</span>");
#else
    QString strProduct("<span style=\"font-weight:bold;font-size:14px\">WizNote for Windows</span>");
#endif

    QLabel* labelProduct = new QLabel(this);
    labelProduct->setText(strProduct);

    QFileInfo fi(::WizGetAppFileName());
    QDateTime t = fi.lastModified();
    QString strBuildNumber("(%1.%2.%3 %4:%5)");
    strBuildNumber = strBuildNumber.\
            arg(t.date().year()).\
            arg(t.date().month()).\
            arg(t.date().day()).\
            arg(t.time().hour()).\
            arg(t.time().minute());

    QString strInfo = QString(tr("<span style=\"font-size:11px\">Version %2 %3</span>")).arg(WIZ_CLIENT_VERSION, strBuildNumber);
    QLabel* labelBuild = new QLabel(this);
    labelBuild->setText(strInfo);

    QTextBrowser* textCredits = new QTextBrowser(this);
    textCredits->setOpenExternalLinks(true);

    QString strHtml;
    ::WizLoadUnicodeTextFromFile(":/credits.html", strHtml);
    textCredits->setHtml(strHtml);

    QLabel* labelCopyright = new QLabel(this);
    labelCopyright->setText(tr("<span style=\"font-size:10px\">Copy Right 2013 Wiz inc. All rights reserved</span>"));

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 10, 0, 10);

    layout->addWidget(labelIcon);
    layout->addWidget(labelProduct);
    layout->addWidget(labelBuild);
    layout->addWidget(textCredits);
    layout->addWidget(labelCopyright);
    layout->setAlignment(labelIcon, Qt::AlignCenter);
    layout->setAlignment(labelProduct, Qt::AlignCenter);
    layout->setAlignment(labelBuild, Qt::AlignCenter);
    layout->setAlignment(textCredits, Qt::AlignCenter);
    layout->setAlignment(labelCopyright, Qt::AlignCenter);

    setWindowTitle(tr("About WizNote"));
}
