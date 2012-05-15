#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <markdown.h>
#include <html.h>
#include <buffer.h>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->plainTextEdit, SIGNAL(textChanged()),this, SLOT(textChanged()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::textChanged(){
    QString text = ui->plainTextEdit->toPlainText();
    struct buf *ib, *ob;
    struct sd_callbacks cbs;
    struct html_renderopt opts;
    struct sd_markdown *mkd;

    if(text.size() > 0){
        char *txt = (char*) text.toStdString().c_str();
        ib = bufnew(strlen(txt));
        bufputs(ib,txt);
        ob = bufnew(64);
        sdhtml_renderer(&cbs,&opts,0);
        mkd = sd_markdown_new(0,16,&cbs,&opts);
        sd_markdown_render(ob,ib->data,ib->size,mkd);
        sd_markdown_free(mkd);
        ui->webView->setHtml(QString(bufcstr(ob)));
    }
}
