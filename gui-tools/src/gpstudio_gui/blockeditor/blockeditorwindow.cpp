#include "blockeditorwindow.h"

#include <QLayout>
#include <QSplitter>
#include <QDebug>
#include <QMenu>
#include <QMenuBar>
#include <QToolBar>
#include <QStatusBar>
#include <QMessageBox>
#include <QFileDialog>

#include "lib_parser/lib.h"

#include "codeeditor.h"
#include "viewer/viewerwidgets/pdfviewer.h"

BlockEditorWindow::BlockEditorWindow(QWidget *parent, ModelBlock *block)
    : QMainWindow(parent)
{
    setWindowTitle(tr("gpblock (viewer only)"));
    setWindowIcon(QIcon(":/img/img/gpstudio_block.ico"));

    _filesModel = new QStandardItemModel();
    setupWidgets();
    createToolBarAndMenu();

    setMinimumWidth(800);
    setMinimumHeight(600);

    setBlock(block);
}

BlockEditorWindow::~BlockEditorWindow()
{
    //delete menuBar();
    //delete layout();
}

void BlockEditorWindow::showImplementationsFiles(const QString &driver)
{
    BlockLib *block = Lib::getLib().process(driver);
    if(block)
    {
        BlockEditorWindow *blockEditor = new BlockEditorWindow(NULL, block->modelBlock());
        blockEditor->show();
    }
}

void BlockEditorWindow::openBlock(const QString blockFileName)
{
    ModelBlock *block = ModelBlock::readFromFile(blockFileName);
    if(!block)
        QMessageBox::critical(this, tr("Can not open IP file"), tr("This IP file block does not exist or can not be opened"));

    setBlock(block);
}

void BlockEditorWindow::openBlock()
{
    QString fileName;
    if(fileName.isEmpty())
    {
        fileName = QFileDialog::getOpenFileName(this, tr("Open block IP"), "", tr("All block type (*.io *.proc);;Process block (*.proc);;IO block (*.io))"));
        if(fileName.isEmpty())
            return;
    }
    openBlock(fileName);
}

void BlockEditorWindow::closeEvent(QCloseEvent *event)
{
    //_project->closeProject();
    /*if(!parent())
        deleteLater();*/
    event->accept();
}

void BlockEditorWindow::openFile(const QModelIndex &indexFile)
{
    QStandardItem *item = _filesModel->itemFromIndex(indexFile);
    if(!item)
        return;

    ModelFile *file = _block->getFile(item->text());
    if(file)
    {
        for(int i=0; i<_tabFiles->count(); i++)
        {
            if(_tabFiles->tabText(i)==file->name())
            {
                _tabFiles->setCurrentIndex(i);
                return;
            }
        }

        if(file->path().endsWith(".pdf"))
        {
            PdfViewer::showDocument(_path + "/" + file->path());
        }
        else
        {
            CodeEditor *codeEditor = new CodeEditor(this);
            _tabFiles->addTab(codeEditor, file->name());
            _tabFiles->setCurrentIndex(_tabFiles->count()-1);
            codeEditor->loadFileCode(_path + "/" + file->path());
            qDebug()<<_path + "/" + file->path();
        }
    }
}

void BlockEditorWindow::closeTab(int id)
{
    QWidget *widget = _tabFiles->widget(id);
    _tabFiles->removeTab(id);
    delete widget;
}

void BlockEditorWindow::setupWidgets()
{
    QWidget *centralwidget = new QWidget(this);

    QLayout *layout = new QVBoxLayout(centralwidget);

    QSplitter *splitter = new QSplitter(centralwidget);
    splitter->setOrientation(Qt::Horizontal);
    layout->addWidget(splitter);

    _filesTreeView = new QTreeView(splitter);
    _filesTreeView->setModel(_filesModel);
    splitter->addWidget(_filesTreeView);

    _tabFiles = new QTabWidget(splitter);
    _tabFiles->setTabsClosable(true);
    connect(_tabFiles, SIGNAL(tabCloseRequested(int)), this, SLOT(closeTab(int)));
    splitter->addWidget(_tabFiles);

    centralwidget->setLayout(layout);
    setCentralWidget(centralwidget);

    splitter->setStretchFactor(0, QSizePolicy::Fixed);
    splitter->setStretchFactor(1, QSizePolicy::Maximum);

    QMenuBar *menubar = new QMenuBar(this);
    setMenuBar(menubar);

    QStatusBar *statusBar = new QStatusBar(this);
    setStatusBar(statusBar);

    setGeometry(100, 100, 800, 600);
}

void BlockEditorWindow::createToolBarAndMenu()
{
    _mainToolBar = new QToolBar(this);
    addToolBar(_mainToolBar);

    // ============= File =============
    QMenu *blockMenu = menuBar()->addMenu(tr("&Block"));

    QAction *openDocAction = new QAction(tr("&Open block"),this);
    openDocAction->setIcon(QIcon(":/icons/img/open.png"));
    openDocAction->setShortcut(QKeySequence::Open);
    connect(openDocAction, SIGNAL(triggered()), this, SLOT(openBlock()));
    _mainToolBar->addAction(openDocAction);
    blockMenu->addAction(openDocAction);

    /*QAction *connectAction = new QAction(tr("&Save block"),this);
    connectAction->setIcon(QIcon(":/icons/img/save.png"));
    openDocAction->setShortcut(QKeySequence::Save);
    _mainToolBar->addAction(connectAction);
    blockMenu->addAction(connectAction);*/

    blockMenu->addSeparator();
    QAction *exit = new QAction(tr("E&xit"),this);
    exit->setIcon(QIcon(":/icons/img/exit.png"));
    exit->setShortcut(QKeySequence::Quit);
    blockMenu->addAction(exit);
    connect(exit, SIGNAL(triggered()), this, SLOT(close()));

    // ============= Help =============
    QMenu *helpMenu = menuBar()->addMenu(tr("&Help"));

    QAction *aboutAction = new QAction(tr("&About"), this);
    aboutAction->setIcon(QIcon(":/img/img/gpstudio_block.ico"));
    aboutAction->setStatusTip(tr("Shows informations about block editor"));
    connect(aboutAction, SIGNAL(triggered(bool)), this, SLOT(about()));
    helpMenu->addAction(aboutAction);

    QAction *aboutQtAction = new QAction(tr("About &Qt"), this);
    aboutQtAction->setIcon(QIcon(":/icons/img/qt.png"));
    aboutQtAction->setStatusTip(tr("About Qt version"));
    connect(aboutQtAction, SIGNAL(triggered(bool)), this, SLOT(aboutQt()));
    helpMenu->addAction(aboutQtAction);

    _mainToolBar->addSeparator();
}

void BlockEditorWindow::setBlock(ModelBlock *block)
{
    _block = block;
    if(!block)
        return;

    setWindowTitle(QString(tr("gpblock (viewer only) | %1")).arg(_block->driver()));

    QStandardItem *rootBlockItem = new QStandardItem(_block->driver());
    rootBlockItem->setEditable(false);
    _filesModel->invisibleRootItem()->appendRow(rootBlockItem);

    foreach (ModelFile *file, _block->files())
    {
        QStandardItem *item = new QStandardItem(file->name());
        item->setEditable(false);
        rootBlockItem->appendRow(item);
    }
    connect(_filesTreeView, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(openFile(QModelIndex)));

    _filesTreeView->expandAll();

    BlockLib *blockLib = Lib::getLib().blockLib(_block->driver());
    if(blockLib)
        _path = blockLib->path();
    else
    {
        BlockLib *io = Lib::getLib().io(_block->driver());
        if(io)
            _path = io->path();
    }
}

void BlockEditorWindow::about()
{
    QMessageBox::about(this,"GPStudio: GPBlock 1.21", QString("Copyright (C) 2014-2017 Dream IP (<a href=\"http://dream-lab.fr\">dream-lab.fr</a>)<br>\
<br>\
This sofware is part of GPStudio distribution. To check for new version, please visit <a href=\"http://gpstudio.univ-bpclermont.fr/download\">gpstudio.univ-bpclermont.fr/download</a><br>\
<br>\
GPStudio is a free software: you can redistribute it and/or modify\
it under the terms of the GNU General Public License as published by\
the Free Software Foundation, either version 3 of the License, or\
(at your option) any later version.<br>\
<br>\
This program is distributed in the hope that it will be useful,\
but WITHOUT ANY WARRANTY; without even the implied warranty of\
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\
GNU General Public License for more details.<br>\
<br>\
You should have received a copy of the GNU General Public License\
along with this program.  If not, see <a href=\"http://www.gnu.org/licenses/\">www.gnu.org/licenses</a><br>\
<br>\
Build date: ") + __DATE__ + QString(" time: ")+__TIME__);
}

void BlockEditorWindow::aboutQt()
{
    QMessageBox::aboutQt(this);
}
