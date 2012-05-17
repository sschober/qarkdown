#include "mainwindow.h"
#include "ui_mainwindow.h"

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

    QFont font;
    font.setFamily("Courier");
    font.setFixedPitch(true);
    font.setPointSize(10);
    ui->plainTextEdit->setFont(font);

    ui->actionOpen->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_O));
    ui->actionSave->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_S));
    ui->actionSource->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_U));
    ui->actionDirectory->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_D));

    connect(ui->plainTextEdit, SIGNAL(textChanged()),this, SLOT(textChanged()));
    connect(ui->actionOpen, SIGNAL(triggered()), this, SLOT(fileOpen()));
    connect(ui->actionSave, SIGNAL(triggered()), this, SLOT(fileSave()));
    connect(ui->actionSave_As, SIGNAL(triggered()), this, SLOT(fileSaveAs()));
    connect(ui->actionSource, SIGNAL(triggered()),this,SLOT(viewSource()));
    connect(ui->actionDirectory, SIGNAL(triggered()),this,SLOT(viewDirectory()));

    ui->actionSave->setEnabled(false);
    ui->sourceView->hide();

    ui->listView->hide();
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
        QFileSystemModel *model = new QFileSystemModel;
        // if file is opened open parent dir
        // else open cwd
        model->setRootPath(QDir::currentPath());
        ui->listView->setModel(model);
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

void MainWindow::fileOpen(){
    QString fileName = QFileDialog::getOpenFileName(this,tr("Open File"),"","*.md *.mkd");
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

        // tend list view
        // show parent dir
        // and select current file
    }
}

void MainWindow::fileSave(){
    if (!currentFile->open(QIODevice::WriteOnly | QIODevice::Text))
        return;
    QTextStream out(currentFile);
    out << ui->plainTextEdit->toPlainText();
    currentFile->close();
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
    QPoint pos = ui->webView->page()->currentFrame()->scrollPosition();
    ui->webView->setHtml(newText);
    ui->webView->page()->currentFrame()->setScrollPosition(pos);
    ui->sourceView->setPlainText(newText);
}

