#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <markdown.h>
#include <html.h>
#include <buffer.h>

#include <QFileDialog>
#include <QTextStream>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->plainTextEdit, SIGNAL(textChanged()),this, SLOT(textChanged()));
    connect(ui->actionOpen, SIGNAL(triggered()), this, SLOT(fileOpen()));
    connect(ui->actionSave, SIGNAL(triggered()), this, SLOT(fileSave()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::fileOpen(){
    fileName = QFileDialog::getOpenFileName(this,tr("Open File"),"","*.md");
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;
    QString fileContent(file.readAll());
    ui->plainTextEdit->setPlainText(fileContent);
}

void MainWindow::fileSave(){
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return;
    QTextStream out(&file);
    out << ui->plainTextEdit->toPlainText();
}

static QString markdown(QString in){
    struct buf *ib, *ob;
    struct sd_callbacks cbs;
    struct html_renderopt opts;
    struct sd_markdown *mkd;

    if(in.size() > 0){
        char *txt = (char*) in.toStdString().c_str();
        ib = bufnew(strlen(txt));
        bufputs(ib,txt);
        ob = bufnew(64);
        sdhtml_renderer(&cbs,&opts,0);
        mkd = sd_markdown_new(0,16,&cbs,&opts);
        sd_markdown_render(ob,ib->data,ib->size,mkd);
        sd_markdown_free(mkd);
        return QString(bufcstr(ob));
    }
    return "";
}

static QString wrapInHTMLDoc(QString in){
    return in
            .prepend("<html>\n <head>\n  <link type=\"text/css\" rel=\"stylesheet\" href=\"qrc:///css/bootstrap.css\"/>\n </head>\n <body>\n")
            .append("\n </body>\n</html>");
}

void MainWindow::textChanged(){
    QString newText = wrapInHTMLDoc(markdown(ui->plainTextEdit->toPlainText()));
    ui->webView->setHtml(newText);
    ui->sourceView->setPlainText(newText);
}

