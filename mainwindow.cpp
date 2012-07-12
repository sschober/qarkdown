#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "highlighter.h"

#include <markdown.h>
#include <html.h>
#include <buffer.h>

#include <QFileDialog>
#include <QTextStream>
#include <QTime>
#include <QDebug>
#include <QWebFrame>
#include <QFileSystemModel>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    hswv = new HScrollWebView(this);
    ui->splitter->addWidget(hswv);

    QFont font;
    font.setFamily("Courier");
    font.setFixedPitch(true);
    font.setPointSize(15);
    ui->plainTextEdit->setFont(font);

    new HGMarkdownHighlighter(ui->plainTextEdit->document(), 1000);

    ui->actionNew->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_N));
    ui->actionOpen->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_O));
    ui->actionSave->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_S));
    ui->actionSource->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_U));
    ui->actionDirectory->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_D));

    connect(ui->plainTextEdit, SIGNAL(textChanged()),this, SLOT(textChanged()));
    connect(ui->actionNew, SIGNAL(triggered()), this, SLOT(fileNew()));
    connect(ui->actionOpen, SIGNAL(triggered()), this, SLOT(fileOpen()));
    connect(ui->actionSave, SIGNAL(triggered()), this, SLOT(fileSave()));
    connect(ui->actionSave_As, SIGNAL(triggered()), this, SLOT(fileSaveAs()));
    connect(ui->actionSource, SIGNAL(triggered()),this,SLOT(viewSource()));
    connect(ui->actionDirectory, SIGNAL(triggered()),this,SLOT(viewDirectory()));
    connect(ui->listView, SIGNAL(clicked(QModelIndex)), this, SLOT(dirViewClicked(QModelIndex)));


    ui->actionSave->setEnabled(false);
    ui->sourceView->hide();

    ui->listView->hide();

    if(qApp->arguments().size() > 1){
        // a file was given on the command line
        openFile(qApp->arguments().at(1));
    }
}

void MainWindow::fileNew(){
    if(NULL != currentFile){
        fileSave();
    }
    currentFile = NULL;
    ui->plainTextEdit->setPlainText("");
}

void MainWindow::dirViewClicked(QModelIndex idx){
    if(NULL != currentFile){
        fileSave();
    }
    QFileSystemModel *model = (QFileSystemModel*) ui->listView->model();

    openFile(model->filePath(idx));

}

void MainWindow::viewSource(){
    if(ui->sourceView->isVisible()){
        ui->sourceView->hide();
    }
    else{
        ui->sourceView->show();
    }
}

void MainWindow::viewDirectory(){
    if(!ui->listView->isVisible()){
        ui->listView->show();
    }
    else{
        ui->listView->hide();
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::openFile(QString fileName){
    if(NULL != fileName){
        currentFile = new QFile(fileName);
        if (!currentFile->open(QIODevice::ReadOnly | QIODevice::Text))
            return;
        QString fileContent;
        QTextStream in(currentFile);
        QString line;
        while(! in.atEnd()){
            line = in.readLine();
            if(! line.isNull() )
                fileContent.append(line).append('\n');
        }
        currentFile->close();
        ui->plainTextEdit->setPlainText(fileContent);
        ui->actionSave->setEnabled(true);
        setWindowTitle(fileName);
        updateListView();
    }

}

void MainWindow::updateListView(){
    QFileSystemModel *model = new QFileSystemModel;
    QString path(QFileInfo(currentFile->fileName()).dir().path());
    model->setFilter(QDir::AllDirs | QDir::AllEntries);
    QStringList flt;
    flt << "*.md" << "*.mkd";
    model->setNameFilters(flt);
    model->setNameFilterDisables(false);
    model->setRootPath(path);
    ui->listView->setModel(model);
    ui->listView->setRootIndex(model->index(path));
}

void MainWindow::fileOpen(){
    openFile(QFileDialog::getOpenFileName(this,tr("Open File"),"","*.md *.mkd"));
}

void MainWindow::fileSave(){
    if(NULL == currentFile){
        QString newFileName = QFileDialog::getSaveFileName();
        currentFile = new QFile(newFileName);
        setWindowTitle(newFileName);
    }
    if (!currentFile->open(QIODevice::WriteOnly | QIODevice::Text))
        return;
    QTextStream out(currentFile);
    out << ui->plainTextEdit->toPlainText();
    currentFile->close();
    updateListView();
}

void MainWindow::fileSaveAs(){
    QString fileName = QFileDialog::getSaveFileName(this,tr("Save File As"));
    if(NULL != fileName ){
        currentFile = new QFile(fileName);
        // this should be coupled to a dirty flag actually...
        ui->actionSave->setEnabled(true);
        setWindowTitle(fileName);
        fileSave();
    }
}

static QString markdown(QString in){
    struct buf *ib, *ob;
    struct sd_callbacks cbs;
    struct html_renderopt opts;
    struct sd_markdown *mkd;

    if(in.size() > 0){
        std::string ss = in.toStdString();
        const char *txt = ss.c_str();
        if(NULL == txt) qDebug() << "txt was null!";
        if(0 < qstrlen(txt)){
            ib = bufnew(qstrlen(txt));
            bufputs(ib,txt);
            ob = bufnew(64);
            sdhtml_renderer(&cbs,&opts,0);
            mkd = sd_markdown_new(0,16,&cbs,&opts);
            sd_markdown_render(ob,ib->data,ib->size,mkd);
            sd_markdown_free(mkd);
            return QString(bufcstr(ob));
        }
        else
            qDebug() <<"qstrlen was null";
    }
    return "";
}

static QString wrapInHTMLDoc(QString in){
    return in
            .prepend("<html>\n <head>\n  <link type=\"text/css\" rel=\"stylesheet\" href=\"qrc:///css/bootstrap.css\"/>\n </head>\n <body>\n")
            .append("\n </body>\n</html>");
}

void MainWindow::textChanged(){
    QTime t;
    t.start();
    QString newText = wrapInHTMLDoc(markdown(ui->plainTextEdit->toPlainText()));
    ui->statusBar->showMessage(QString("Render time: %1 ms").arg(t.elapsed()));
    QPoint pos = hswv->page()->currentFrame()->scrollPosition();
    hswv->setHtml(newText);
    ui->sourceView->setPlainText(newText);
    hswv->page()->currentFrame()->setScrollPosition(pos);
}

