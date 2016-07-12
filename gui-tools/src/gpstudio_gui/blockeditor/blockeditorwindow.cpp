#include "blockeditorwindow.h"

#include <QLayout>
#include <QSplitter>
#include <QDebug>
#include <QMenu>
#include <QMenuBar>
#include <QToolBar>
#include <QStatusBar>
#include <QMessageBox>

#include "lib_parser/lib.h"

BlockEditorWindow::BlockEditorWindow(QWidget *parent, ModelBlock *block)
    : QMainWindow(parent)
{
    _filesModel = new QStandardItemModel();
    setupWidgets();
    createToolBarAndMenu();

    setMinimumWidth(800);
    setMinimumHeight(600);

    setBlock(block);

    ProcessLib *process = Lib::getLib().process(_block->driver());
    if(process)
        _path = process->path();
    else
    {
        IOLib *io = Lib::getLib().io(_block->driver());
        if(io)
            _path = io->path();
    }
}

BlockEditorWindow::~BlockEditorWindow()
{
    //delete menuBar();
    //delete layout();
}

void BlockEditorWindow::closeEvent(QCloseEvent *event)
{
    //_project->closeProject();
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

        CodeEditor *codeEditor = new CodeEditor(this);
        _tabFiles->addTab(codeEditor, file->name());
        _tabFiles->setCurrentIndex(_tabFiles->count()-1);
        codeEditor->loadFileCode(_path + "/" + file->path());
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
    QMenu *nodeMenu = menuBar()->addMenu("&Node");

    QAction *openDocAction = new QAction("&Open block",this);
    openDocAction->setIcon(QIcon(":/icons/img/open.png"));
    openDocAction->setShortcut(QKeySequence::Open);
    _mainToolBar->addAction(openDocAction);
    nodeMenu->addAction(openDocAction);

    QAction *connectAction = new QAction("&Save block",this);
    connectAction->setIcon(QIcon(":/icons/img/save.png"));
    openDocAction->setShortcut(QKeySequence::Save);
    _mainToolBar->addAction(connectAction);
    nodeMenu->addAction(connectAction);

    nodeMenu->addSeparator();
    QAction *exit = new QAction("E&xit",this);
    exit->setIcon(QIcon(":/icons/img/exit.png"));
    exit->setShortcut(QKeySequence::Quit);
    nodeMenu->addAction(exit);
    connect(exit, SIGNAL(triggered()), this, SLOT(close()));

    // ============= Help =============
    QMenu *helpMenu = menuBar()->addMenu("&Help");

    QAction *aboutAction = new QAction("&About", this);
    connect(aboutAction, SIGNAL(triggered(bool)), this, SLOT(about()));
    helpMenu->addAction(aboutAction);

    QAction *aboutQtAction = new QAction("About &Qt", this);
    connect(aboutQtAction, SIGNAL(triggered(bool)), this, SLOT(aboutQt()));
    helpMenu->addAction(aboutQtAction);

    _mainToolBar->addSeparator();
}

void BlockEditorWindow::setBlock(ModelBlock *block)
{
    if(!block)
        return;
    _block = block;

    setWindowTitle(QString("gpblock | %1").arg(_block->driver()));

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
}

void BlockEditorWindow::about()
{
    QMessageBox::about(this,"GPStudio: GPBlock 1.01","Copyright (C) 2016 Dream IP\n\
\n\
This sofware is part of GPStudio.\n\
\n\
GPStudio is a free software: you can redistribute it and/or modify\n\
it under the terms of the GNU General Public License as published by\n\
the Free Software Foundation, either version 3 of the License, or\n\
(at your option) any later version.\n\
\n\
This program is distributed in the hope that it will be useful,\n\
but WITHOUT ANY WARRANTY; without even the implied warranty of\n\
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n\
GNU General Public License for more details.\n\
\n\
You should have received a copy of the GNU General Public License\n\
along with this program.  If not, see <http://www.gnu.org/licenses/>\n.");
}

void BlockEditorWindow::aboutQt()
{
    QMessageBox::aboutQt(this);
}
