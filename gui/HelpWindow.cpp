#include "HelpWindow.h"
#include "ui_HelpWindow.h"
#include "QtHelpSchemeHandler.h"

#include <QBuffer>
#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 0)
#include <QDirIterator>
#include <QHelpLink>
#include <QStandardPaths>
#endif

#include <QTemporaryDir>
#include <QFile>
#include <QWebEngineHistory>
#include <QWebEngineHistoryItem>
#include <QWebEnginePage>
#include <QWebEngineProfile>
#include <QWebEngineUrlRequestJob>
#include <QWebEngineUrlSchemeHandler>
#include <QWebEngineView>
#include <QDesktopServices>
#include <QHBoxLayout>
#include <QSplitter>
#include <QTabWidget>
#include <QTextBrowser>
#include <QtHelp/QHelpContentWidget>
#include <QtHelp/QHelpIndexWidget>
#include <QDebug>
#include <QMessageBox>

class HelpBrowser : public QTextBrowser
{
public:
    explicit HelpBrowser(QHelpEngine* helpEngine, QWidget* parent = 0):QTextBrowser(parent), helpEngine(helpEngine)
    {}

    QVariant loadResource (int type, const QUrl& name)
    {
        qDebug() << name;
        if (name.scheme() == "qthelp")
        {
            return QVariant(helpEngine->fileData(name));
        }
        else
        {
            return QTextBrowser::loadResource(type, name);
        }
    }

private:
    QHelpEngine* helpEngine;
};

class MyPage : public QWebEnginePage
{
public:
    bool acceptNavigationRequest(const QUrl &url, NavigationType type, bool isMainFrame) override
    {
        qDebug() << url;

        if (type == QWebEnginePage::NavigationTypeLinkClicked)
        {            
            QDesktopServices::openUrl(url);
            return false;
        }
        return true;
    }
};

#define xstr(s) str(s)
#define str(s) #s

HelpWindow::HelpWindow(QWidget *parent) : QWidget(parent), ui(new Ui::HelpWindow), m_page(new MyPage)
{

#ifdef Q_OS_LINUX
//    QDirIterator it(":", QDirIterator::Subdirectories);
//    while (it.hasNext())
//    {
//        qDebug() << it.next();
//    }

    const char* location = ":/help/funkplot.qhc";
    const char* location2 = ":/help/funkplot.qch";

    ui->setupUi(this);

    /*if(!QFile::exists(location))
    {
        qCritical() << "Cannot load help file from:" << location;
    }*/

    QString helpFinalDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/help";


    QDir tmpDir(helpFinalDir);

    if(!tmpDir.exists())
    {
        tmpDir.mkpath(helpFinalDir);
    }

    if (tmpDir.exists())
    {
        QFileInfo fiTmp(tmpDir.path() );
        if(fiTmp.isDir())
        {
            if(!fiTmp.isWritable())
            {
                qCritical() << "Not writeable directory:" << tmpDir.path() ;
            }
        }

        //tmpDir.setAutoRemove(false);
        QString finalHelp = tmpDir.path() + "/funkplot.qch";
        if(QFile::exists(finalHelp))
        {
            qDebug() << "Removing older version of" << finalHelp;
            QFile::remove(finalHelp);
        }

        QFile f2(location2);
        if(!f2.copy(finalHelp))
        {
            qCritical() << "Cannot copy" << location2 << "to" << finalHelp << ":" << f2.errorString();
        }

        if(!f2.setPermissions(f2.permissions() | QFileDevice::WriteUser))
        {
            qWarning() << "Cannot set permission" << f2.errorString();
        }

        finalHelp = tmpDir.path() + "/funkplot.qhc";
        if(QFile::exists(finalHelp))
        {
            QFile::remove(finalHelp);
        }

        if(!QFile(location).copy(finalHelp))
        {
            qCritical() << "Cannot copy" << location << "to" << finalHelp;
        }
        helpEngine = new QHelpEngine(finalHelp);
        qInfo() << "Help at:" << finalHelp;
    }
    else
    {
        qInfo() << helpFinalDir << "still does not exist, there will be no help";
    }

#endif

#ifdef Q_OS_WIN32
    helpEngine = new QHelpEngine(QApplication::applicationDirPath() + "/help/funkplot.qhc");
#endif

    if (!helpEngine)
    {
        return;
    }

    helpEngine->setProperty("_q_readonly", QVariant::fromValue<bool>(true));


    if(!helpEngine->setupData())
    {
        qCritical() << "Could not set up the help engine:" << helpEngine->error();
        QMessageBox::critical(this, "Error", "Could not set up the help engine: " + helpEngine->error());
    }

    QTabWidget* tWidget = new QTabWidget;
    tWidget->addTab(helpEngine->contentWidget(), "Contents");
    tWidget->addTab(helpEngine->indexWidget(), "Index");

    tWidget->setMaximumSize(250, 16777215);

    view = new QWebEngineView(this);
    view->setPage(m_page);
    view->page()->profile()->installUrlSchemeHandler("qthelp", new QtHelpSchemeHandler(helpEngine));

    view->load(QUrl("qthelp://unauthorized.frog.funkplot/doc/html/intro.html"));
    view->show();


#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 0)
    connect(helpEngine->indexWidget(), &QHelpIndexWidget::documentActivated, this, [this](QHelpLink url, QString)
            {
                qDebug() << "Loading:" << url.url;
                this->view->load(url.url);
            }
    );
#else
    connect(helpEngine->indexWidget(), &QHelpIndexWidget::linkActivated, this, [this](QUrl url, QString)
            {
                qDebug() << "Loading:" << url;

                this->view->load(url);
            }
    );
#endif

    connect(helpEngine->contentWidget(), &QHelpContentWidget::linkActivated, this, [this](QUrl url)
            {
                qDebug() << "Loading 1:" << url;
                this->view->load(url);
            }
    );


    auto verticalLayout = new QVBoxLayout(this);

    QSplitter *horizSplitter = new QSplitter(Qt::Horizontal);
    horizSplitter->insertWidget(0, tWidget);
    horizSplitter->insertWidget(1, view);

    verticalLayout->addWidget(horizSplitter);

}

HelpWindow::~HelpWindow()
{
    delete ui;
    delete m_page;
}
