#include "MainWindow.h"
#include "BuiltinDictionary.h"
#include "ui_MainWindow.h"
#include "DrawingForm.h"
#include "CodeEditorTabPage.h"
#include "PaletteListForm.h"
#include "Program.h"
#include "AboutDialog.h"
#include "Builtin.h"
#include "BuiltinFunctionDetailer.h"

#include <RuntimeProvider.h>
#include <Set.h>

#include <TabToolbar/TabToolbar.h>
#include <TabToolbar/Page.h>
#include <TabToolbar/Group.h>
#include <TabToolbar/SubGroup.h>
#include <TabToolbar/StyleTools.h>
#include <TabToolbar/Builder.h>

#include <QDebug>
#include <QSharedPointer>
#include <QPen>
#include <QDockWidget>
#include <QResizeEvent>
#include <QColorDialog>
#include <QLabel>
#include <QMenu>
#include <QCheckBox>
#include <QComboBox>
#include <QSpinBox>
#include <QLineEdit>
#include <QTextEdit>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QDateTime>
#include <QTextDocumentFragment>
#include <QPainter>
#include <QShortcut>
#include <QJsonDocument>
#include <DockWidget.h>

#include <QtConcurrent/QtConcurrent>

#include <QFileDialog>
#include <QStandardItemModel>
#include <QStyledItemDelegate>
#include <functional>

MainWindow::MainWindow(RuntimeProvider *rp, DrawingForm* df, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_df(df), m_rp(rp)
{

    ui->setupUi(this);

    createDefaultProgram();

    m_ttb.SetCustomWidgetCreator("textEdit", []() { return new QTextEdit(); });
    m_ttb.SetCustomWidgetCreator("checkBox", []() { return new QCheckBox(); });
    m_ttb.SetCustomWidgetCreator("pushButton", []() { return new QPushButton(); });

    tt::TabToolbar* tabToolbar = m_ttb.CreateTabToolbar(":/tt/tabtoolbar.json");
    addToolBar(Qt::TopToolBarArea, tabToolbar);
    tabToolbar->SetStyle("Vienna");

//    ttb["customTextEdit"]->setMaximumWidth(100);

//    ((QCheckBox*)ttb["customCheckBox"])->setText("Check 1");
//    QPushButton* btn = (QPushButton*)ttb["customEditButton"];
//    btn->setText("Edit");
//    static bool kek = true;
//    tt::Page* editPage = (tt::Page*)ttb["Edit"];
//    QObject::connect(btn, &QPushButton::clicked, [editPage]()
//                     {
//                         if(kek)
//                             editPage->hide();
//                         else
//                             editPage->show();
//                         kek = !kek;
//                     });

//    QObject::connect(tabToolbar, &tt::TabToolbar::SpecialTabClicked, this, [this]()
//                     {
//                         QMessageBox::information(this, "Kek", "Cheburek");
//                     });

//    //create buttons for each style
//    tt::Group* stylesGroup = (tt::Group*)ttb["Styles"];
//    stylesGroup->AddSeparator();
//    QStringList styles = tt::GetRegisteredStyles();
//    for(int i=0; i<styles.size(); i++)
//    {
//        const QString styleName = styles.at(i);
//        QPushButton* btn = new QPushButton(styleName, this);
//        QObject::connect(btn, &QPushButton::clicked, [styleName, tabToolbar]() { tabToolbar->SetStyle(styleName); });
//        stylesGroup->AddWidget(btn);
//    }

//    tt::RegisterStyle("NoStyle", []()
//                      {
//                          tt::StyleParams* params = new tt::StyleParams();
//                          params->UseTemplateSheet = false;
//                          params->AdditionalStyleSheet = "";
//                          return params;
//                      });
//    btn = (QPushButton*)ttb["nativeStyleButton"];
//    btn->setText("No Style");
//    QObject::connect(btn, &QPushButton::clicked, [tabToolbar]() { tabToolbar->SetStyle("NoStyle"); });
//    btn = (QPushButton*)ttb["defaultStyleButton"];
//    btn->setText("Default");
//    QObject::connect(btn, &QPushButton::clicked, [tabToolbar]() { tabToolbar->SetStyle(tt::GetDefaultStyle()); });


    connect(ui->actionRun, &QAction::triggered, this, [this]() {runCurrentCode();});
    connect(ui->actionStop, &QAction::triggered, this, [this]() {stopRunningCurrentCode();});

    QObject::connect(this, SIGNAL(onError(int,int,QString)), this, SLOT(onErrorReceived(int,int,QString)), Qt::QueuedConnection);
    QObject::connect(this, SIGNAL(onString(QString)), this, SLOT(onStringReceived(QString)), Qt::QueuedConnection);

    m_plf = new PaletteListForm();

    connect(m_plf, SIGNAL(paletteChosen(QString)), m_currentProgram, SLOT(paletteChosenFromMenu(QString)));
    ui->frmProject->hide();


    connect(ui->action2D_View, &QAction::triggered, this, [this]() {
        setView(ViewType::VIEW_2D_QWIDGET);
        ui->action2D_View->setChecked(true);
        ui->action3D_View->setChecked(false);
    });

#if QT_VERSION > QT_VERSION_CHECK(6, 0, 0)

    connect(ui->action3D_View, &QAction::triggered, this, [this]() {
        setView(ViewType::VIEW_3D);
        ui->action2D_View->setChecked(false);
        ui->action3D_View->setChecked(true);
    });
#endif

    setView(ViewType::VIEW_2D_QWIDGET);
    ui->action2D_View->setChecked(true);

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    // in Qt5 hide the view group
    tt::Group* viewGroup = (tt::Group*)m_ttb["View"];
    viewGroup->hide();
#endif
    auto grp = m_ttb["Plot"];
    auto btnStop = (*dynamic_cast<tt::Group*>(grp))["actionStop"];
    btnStop->setEnabled(false);

    m_currentProgram->m_tabPage->getTextEdit()->setFocus();

    // generate a shortcut to run the curent code
    auto shortcut = new QShortcut(QKeySequence("Ctrl+R"), this);
    connect(shortcut, SIGNAL(activated()), this, SLOT(runCurrentCode()));
    connect(shortcut, SIGNAL(activatedAmbiguously()), this, SLOT(runCurrentCode()));

    // functions database
    buildFunctionDatabase();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::printString(QString s)
{
    emit onString(s);
}

void MainWindow::reportError(int l, int c, QString err)
{
    qWarning() << err << "@" << l;
    emit onError(l, c, err);
    if(!m_sessionErrors.contains( {l, c, err} ))
    {
        m_sessionErrors.append({l, c, err});
    }
}


void MainWindow::runCurrentCode()
{
    ui->splitter->setEnabled(false);
    ui->splitter_2->setEnabled(false);

    ui->actionExport->setEnabled(false);

    m_currentProgram->highlightLine(-1, "");
    ui->statusbar->clearMessage();
    m_sessionErrors.clear();

    ui->txtOutput->clear();

    connect(&m_runningWatcher, &QFutureWatcher<bool>::finished, this, &MainWindow::runningFinished);

    m_rp->parse(m_currentProgram->codeLines());
    // see if we have coordinate system modifiers in there
    auto sts = m_rp->getStatements();
    for(auto& st : sts)
    {
        auto s = dynamic_cast<Set*>(st.get());
        if(s)
        {
            if(s->what ==  SetTargets::TARGET_COORDINATES)
            {
                s->execute(m_rp);
            }
        }
    }

    m_df->reset();

    auto grp = m_ttb["Plot"];
    auto btnRun = (*dynamic_cast<tt::Group*>(grp))["actionRun"];
    btnRun->setEnabled(false);
    auto btnStop = (*dynamic_cast<tt::Group*>(grp))["actionStop"];
    btnStop->setEnabled(true);

    m_runningFuture = QtConcurrent::run([&]() -> bool
                                        {
                                            return m_currentProgram->run();
                                        });

    m_runningWatcher.setFuture(m_runningFuture);

    QEventLoop loop;
    connect(this, SIGNAL(runningDone()), &loop, SLOT(quit()));
    loop.exec();

    emit m_df->contentChanged();
    m_df->repaint();

    // and put up all the errors
    if(!m_sessionErrors.isEmpty())
    {
        for(const auto&t : qAsConst(m_sessionErrors))
        {
            QString err = std::get<2>(t);
            int l = std::get<0>(t);
            int c = std::get<1>(t);

            QString errStr = "<font color=\"red\">Error " + QString::number(c) + "</font> at line " + QString::number(l) + ": " + err + "";

            ui->txtOutput->append(errStr);
        }
    }

    btnRun->setEnabled(true);
    btnStop->setEnabled(false);
    ui->actionExport->setEnabled(true);
    ui->splitter->setEnabled(true);
    ui->splitter_2->setEnabled(true);
}

void MainWindow::stopRunningCurrentCode()
{
    m_currentProgram->stop();

    auto grp = m_ttb["Plot"];

    auto btnRun = (*dynamic_cast<tt::Group*>(grp))["actionRun"];
    auto btnStop = (*dynamic_cast<tt::Group*>(grp))["actionStop"];

    btnRun->setEnabled(true);
    btnStop->setEnabled(false);
}

void MainWindow::setView(ViewType v)
{
    m_df->setView(v);
}

std::tuple<int, QString> MainWindow::errorAndLineFromErrorText(const QString &s)
{
    qDebug() << s;

    QStringList ps = s.split("]");
    if(ps.length() != 2)
    {
        return {-1, ""};
    }

    QStringList lns = ps[0].split(":");
    if(lns.length() != 2)
    {
        return {-1, ""};
    }

    int ln = lns[1].toInt();

    return {ln, ps[1].trimmed()};
}

void MainWindow::createDefaultProgram(const QString& prgName)
{
    m_currentProgram = new Program(prgName, ui->tabWidget->currentWidget(), m_rp, m_programs.size());
    m_programs.append(m_currentProgram);

    connect(m_currentProgram, &Program::textChanged, this, [this]()
            {
                updateCurrentTabTextToReflectSavestate();
            }
            );

    ui->tabWidget->addTab(m_currentProgram->m_tabPage.get(), prgName);
}

void MainWindow::set_HelpWindow(KDDockWidgets::DockWidget *newHelpWindow)
{
    m_helpWindow = newHelpWindow;
}

void MainWindow::updateCurrentTabTextToReflectSavestate()
{
    QString current = ui->tabWidget->tabText(m_currentProgram->index());
    QString newText = current.startsWith("*") ? current :  "* " + current;
    ui->tabWidget->setTabText(m_currentProgram->index(), newText);
}

void MainWindow::setCurrentStatement(const QString &newCurrentStatement)
{
    m_currentProgram->setCurrentStatement(newCurrentStatement);
}


void MainWindow::on_actionPalettes_triggered()
{
    m_plf->show();
}

void MainWindow::runningFinished()
{
    bool result = m_runningWatcher.result();
    Q_UNUSED(result);

    emit runningDone();
}

void MainWindow::onErrorReceived(int l, int c, QString err)
{

}

void MainWindow::onStringReceived(QString s)
{
    QString ts = ui->txtOutput->toPlainText();
    ts += s;
    ui->txtOutput->setPlainText(ts);
}

void MainWindow::onFunctionsMenuEntryTriggered()
{
    QAction* action = qobject_cast<QAction*>(sender());

    QString key = action->data().toString();
    qInfo() << "Sender:" << key;

    if(m_builtins.contains(key))
    {

        BuiltinFunctionDetailer* dlg = new BuiltinFunctionDetailer(m_builtins[key], this);

        if(dlg->exec() == QDialog::Accepted)
        {

        }
    }
}

void MainWindow::MainWindow::closeEvent(QCloseEvent *event)
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Quit the IDE", "Are you sure you want to exit the fũnkplot IDE?", QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::Yes)
    {
        event->accept();
        m_helpWindow->deleteLater(); // I try this , solved the problem
        m_df->deleteLater();
        deleteLater();
        QApplication::quit();
    }
    else
        event->ignore();

//    QSettings settings;
//    auto parentr = dynamic_cast<KDDockWidgets::DockWidget*>( parent() );
//    QRect r = parentr->geometry();

//    settings.setValue("geometry", r);
//    settings.setValue("position", parentr->mapToGlobal(r.topLeft()));

//    QMainWindow::closeEvent(event);
//    deleteLater();

//    settings.sync();
//    QWidget::closeEvent(event);
}

void MainWindow::showEvent(QShowEvent *event)
{
    QWidget::showEvent( event );
    if(m_currentProgram)
    {
        m_currentProgram->m_tabPage->getTextEdit()->setFocus();
    }
}

void MainWindow::buildFunctionDatabase()
{
    // create the 2d curves
    QString val;
    QFile file;
    file.setFileName(":/curves/2dcurves.json");
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qWarning() << "Cannot open curve database";
        return;
    }
    val = file.readAll();
    file.close();
    QJsonParseError err;
    QJsonDocument jd = QJsonDocument::fromJson(val.toLatin1(), &err);

    if(jd.isNull())
    {
        qWarning() << "Cannot get curve database:" << err.errorString();

        return;
    }

    // The dictionary
    for(const auto& de : jd.object()["dictionary"].toArray())
    {
        for(const auto&w : de.toObject()["words"].toArray())
        {
            BuiltinDictionary::addEntry(w.toObject()["word"].toString(),de.toObject()["description"].toString() );
        }
    }

    // Then gather the classes
    for(const auto& c : jd.object()["classes"].toArray())
    {
        QMenu* subMenu = new QMenu(c.toObject()["description"].toString(), this);
        m_functionsMenu->addMenu(subMenu);
        m_classMenus[c.toObject()["name"].toString()] = subMenu;
    }

    // Then the categories
    for(const auto& c : jd.object()["categories"].toArray())
    {
        QMenu* subMenu = new QMenu(c.toObject()["description"].toString(), this);
        m_classMenus[c.toObject()["class"].toString()]->addMenu(subMenu);
        m_categoryMenus[c.toObject()["class"].toString() + "-" + c.toObject()["name"].toString()] = subMenu;
    }

    for(const auto& c : jd.object()["curves"].toArray())
    {
        QAction* a = new QAction(c.toObject()["name"].toString(), this);
        a->setData(c.toObject()["key"].toVariant());
        connect(a, &QAction::triggered, this, &MainWindow::onFunctionsMenuEntryTriggered);
        QIcon icon(":/icons/icons/function.png");
        a->setIcon(icon);

        QMenu* mnu = nullptr;
        QString key = c.toObject()["class"].toString() + "-" + c.toObject()["category"].toString();
        if(m_categoryMenus.contains(key))
        {
            m_categoryMenus[key]->addAction(a);
        }

        m_builtins[c.toObject()["key"].toString()] = QSharedPointer<Builtin>(new Builtin(c.toObject()));
        qDebug() << "Loaded:" << c.toObject()["key"].toString();
    }


}


void MainWindow::on_actionHelp_triggered()
{
#ifdef SNAP_BUILD
    QString helpFinalDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/help/funkplot.qhc";;
    QProcess *subprocess = new QProcess(this);
    subprocess->start("assistant", QStringList()  << "-collectionFile" << helpFinalDir);
    subprocess->waitForStarted();
    qInfo() << "Started help via qtassistant";
#else
    m_helpWindow->show();
#endif
}

void MainWindow::on_actionSyntax_triggered()
{
    on_actionHelp_triggered();
}

void MainWindow::on_actionNew_triggered()
{
    QSet<int> usedNrs;
    int number = 0;
    int max = 0;
    for(const auto& p : qAsConst(m_programs))
    {
        QRegularExpression rx("noname (?<nr>\\d+)");
        auto match = rx.match(p->m_programName);
        if (match.hasMatch())
        {
            int tmp = match.captured("nr").toInt();
            usedNrs.insert(tmp);
            if(tmp > max)
            {
                max = tmp;
            }
        }
    }
    QVector<bool> useds;
    for(int i=0; i<max; i++)
    {
        useds.append(usedNrs.contains(i + 1));
    }
    int broke = 1;
    for(int i=0; i<useds.size(); i++)
    {
        number = i + 1;
        if(!useds[i])
        {
            broke = 0;
            break;
        }
    }
    // the next is the right one
    number += broke;

    QString name = "noname " + QString::number(number);
    createDefaultProgram(name);


    ui->tabWidget->setCurrentWidget(m_currentProgram->m_tabPage.get());
    m_currentProgram->m_tabPage->getTextEdit()->setFocus();
}

void MainWindow::on_tabWidget_currentChanged(int index)
{
    if(index >= 0)
    {
        m_currentProgram = m_programs.at(index);
    }
    else
    {
        m_currentProgram = nullptr;
    }
}

void MainWindow::on_actionSave_triggered()
{
    QString fileName = m_currentProgram->m_saveName;
    if(!m_currentProgram->alreadySaved())
    {
        fileName = QFileDialog::getSaveFileName(this, tr("Save program"),  QDir::homePath(),
                                                tr("fũnkplot programs (*.fnk);;All files (*.*)"));

        if(!fileName.isEmpty()&& !fileName.isNull())
        {
            if(!fileName.endsWith("fnk", Qt::CaseInsensitive))
            {
                if(fileName.endsWith("."))
                {
                    fileName += "fnk";
                }
                else
                {
                    fileName += ".fnk";
                }
            }
        }
    }

    if(!fileName.isEmpty()&& !fileName.isNull())
    {
        if(m_currentProgram->saveToFile(fileName))
        {
            QFileInfo fi(fileName);
            ui->tabWidget->setTabText(m_currentProgram->index(), fi.fileName());
        }
    }
}

void MainWindow::on_actionSaveAs_triggered()
{
    m_currentProgram->m_saveName.clear();
    on_actionSave_triggered();
}


void MainWindow::on_actionOpen_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Save program"),  QDir::homePath(),
                                            tr("fũnkplot programs (*.fnk);;All files (*.*)"));

    if(!fileName.isEmpty()&& !fileName.isNull())
    {
        QFileInfo fi(fileName);

        QStringList l2;
        QFile fIn(fileName);
        if (fIn.open(QFile::ReadOnly | QFile::Text))
        {
            QTextStream sIn(&fIn);
            while (!sIn.atEnd())
            {
                l2 += sIn.readLine();
            }
            int cps = m_programs.size();
            m_currentProgram = new Program(fi.fileName(),
                                               ui->tabWidget->currentWidget(),
                                               RuntimeProvider::get(),
                                               cps
                                   );

            connect(m_currentProgram, &Program::textChanged, this, [this]()
                    {
                        updateCurrentTabTextToReflectSavestate();
                    }
                    );

            m_programs.append(m_currentProgram);
            int c = ui->tabWidget->addTab(m_currentProgram->m_tabPage.get(), fi.fileName());
            m_currentProgram->setCodelines(l2);
            m_currentProgram->m_saveName = fileName;
            qDebug() << m_currentProgram;

            ui->tabWidget->setCurrentIndex(c);
        }
        else
        {
            QMessageBox::critical(ui->tabWidget, tr("Cannot open file"), tr("Cannot open file: ") + fIn.errorString(), QMessageBox::Ok);
        }
    }
}


void MainWindow::on_tabWidget_tabCloseRequested(int index)
{
    auto page = ui->tabWidget->widget(index);

    auto i = std::find_if(m_programs.begin(), m_programs.end(), [page](Program* p) -> bool {
        return p->m_tabPage == page;
    });

    if(i != m_programs.end())
    {
        QMessageBox::StandardButton r = QMessageBox::Ok;
        if(!(*i)->m_justSaved)
        {
            r = QMessageBox::question(this,
                                      "fũnkplot",
                                      QString("The program <b>") + (*i)->m_programName + "</b> is not saved.<p>Would you like to save it?",
                                      QMessageBox::Cancel | QMessageBox::Yes | QMessageBox::No);
            if(r == QMessageBox::Yes)
            {
                (*i)->saveToFile((*i)->m_saveName);
            }
        }


        if(r != QMessageBox::Cancel)
        {
            m_programs.removeAll(*i);

            ui->tabWidget->removeTab(index);

            int total = m_programs.size();
            int newIndex = index < total - 1 ? index: total - 1;
            if(newIndex >= 0)
            {
                m_currentProgram = m_programs.at(newIndex);
            }
            else
            {
                m_currentProgram = nullptr;
            }
        }
    }
}


void MainWindow::on_actionExport_triggered()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"),
                                                    QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation),
                                                    tr("Images (*.png *.xpm *.jpg)"));


    QPixmap pm = m_df->screenshot();
    pm.toImage().save(fileName);
    return;
#if 0

    ExportImageDialog* expimgD = new ExportImageDialog(m_df);

    if(expimgD->exec() == QDialog::Accepted)
    {
        ImageDrawer* imgDrawer = new ImageDrawer(expimgD->width(), expimgD->height());
        m_df->populateDrawerData(imgDrawer);
        imgDrawer->redrawEverything();
        imgDrawer->save(QFileDialog::getSaveFileName(this, tr("Save File"),
                                                    QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation),
                                                    tr("Images (*.png *.xpm *.jpg)")));
    }
#endif
}


void MainWindow::on_actionAbout_triggered()
{
    AboutDialog* abt = new AboutDialog(this);
    abt->setWindowFlags(Qt::SplashScreen);
    abt->exec();
}

void MainWindow::on_actionFunctions_triggered()
{
    auto p = m_ttb["Functions"];
    QPoint pp = p->pos();
    m_functionsMenu->exec( mapToGlobal( QPoint{pp.x(), pp.y() + p->height()} ) );
}

