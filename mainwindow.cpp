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
#include <QFileSystemModel>
#include <QLabel>
#include <QMessageBox>
#include <QComboBox>
#include <QTextDocument>
#include <QInputDialog>

#include <cmath> // for round

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    currentFile(NULL),
    renderLabel(new QLabel)
{
    ui->setupUi(this);
    statusBar()->addPermanentWidget( renderLabel );

    QComboBox* headerComboBox = new QComboBox( this );

    for ( int i = 1; i <= 6; i++ ) {
        headerComboBox->addItem( QString( "H%1" ).arg( i ), i );
    }

    QAction* separator = ui->toolBar->insertSeparator( ui->actionBold );
    ui->toolBar->insertWidget( separator, headerComboBox );

    // Set font
    QFont font;
    font.setFamily("Courier");
    font.setFixedPitch(true);
    font.setPointSize(font.pointSize() +5);
    ui->plainTextEdit->setFont(font);

    const float targetWidth = 67.0;
    // set plaintextedit to 80 character column width
    int columnWidth = round( QFontMetrics(font).averageCharWidth() * targetWidth)
           // + ui->plainTextEdit->contentOffset().x()
            + ui->plainTextEdit->document()->documentMargin();
    ui->plainTextEdit->setFixedWidth(columnWidth);

    new HGMarkdownHighlighter(ui->plainTextEdit->document(), 1000);

    ui->actionNew->setShortcut(QKeySequence::New);
    ui->actionOpen->setShortcut(QKeySequence::Open);
    ui->actionSave->setShortcut(QKeySequence::Save);
    ui->actionSource->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_U));
    ui->actionDirectory->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_D));
    ui->actionExport_HTML->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_E));

    connect(ui->plainTextEdit, SIGNAL(textChanged()),this, SLOT(textChanged()));
    connect(ui->actionNew, SIGNAL(triggered()), this, SLOT(fileNew()));
    connect(ui->actionOpen, SIGNAL(triggered()), this, SLOT(fileOpen()));
    connect(ui->actionSave, SIGNAL(triggered()), this, SLOT(fileSave()));
    connect(ui->actionExport_HTML, SIGNAL(triggered()), this, SLOT(fileSaveHTML()));
    connect(ui->actionSave_As, SIGNAL(triggered()), this, SLOT(fileSaveAs()));
    connect(ui->actionSource, SIGNAL(triggered()),this,SLOT(viewSource()));
    connect(ui->actionDirectory, SIGNAL(triggered()),this,SLOT(viewDirectory()));
    connect(ui->listView, SIGNAL(clicked(QModelIndex)), this, SLOT(dirViewClicked(QModelIndex)));
    connect( ui->plainTextEdit, SIGNAL( modificationChanged( bool ) ), this, SLOT( setWindowModified( bool ) ) );
    connect( ui->actionAbout, SIGNAL( triggered() ), this, SLOT( about() ) );
    connect( ui->actionAbout_Qt, SIGNAL( triggered() ), this, SLOT( aboutQt() ) );
    connect( headerComboBox, SIGNAL( activated( int ) ), this, SLOT( headerComboBox_activated( int ) ) );

    ui->sourceView->hide();

    ui->listView->hide();

    if(qApp->arguments().size() > 1){
        // a file was given on the command line
        openFile(qApp->arguments().at(1));
    }
    else {
        fileNew();
    }
}

void MainWindow::fileNew(){
    if(NULL != currentFile){
        fileSave();
    }
    delete currentFile;
    ui->plainTextEdit->clear();
    ui->plainTextEdit->document()->setModified( true );
    ui->plainTextEdit->document()->setModified( false );
    setWindowFilePath( tr( "[New File]" ) );
}

void MainWindow::dirViewClicked(QModelIndex idx){
    if(NULL != currentFile){
        fileSave();
    }
    QFileSystemModel *model = (QFileSystemModel*) ui->listView->model();

    openFile(model->filePath(idx));

}

void MainWindow::about()
{
    QMessageBox::information( this, QString::null, tr( "%1 v%2" ).arg( qApp->applicationName() ).arg( qApp->applicationVersion() ) );
}

void MainWindow::aboutQt()
{
    qApp->aboutQt();
}

void MainWindow::closeEvent( QCloseEvent* event )
{
    if ( NULL != currentFile ) {
        fileSave();
    }
    else if ( ui->plainTextEdit->document()->isModified() ) {
        fileSaveAs();
    }

    QMainWindow::closeEvent( event );
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
        delete currentFile;
        currentFile = new QFile(fileName);
        if (!currentFile->open(QIODevice::ReadWrite | QIODevice::Text)) {
            delete currentFile;
            return;
        }
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
        setWindowFilePath(fileName);
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

QString MainWindow::selectedText()
{
    return ui->plainTextEdit->textCursor().selectedText();
}

void MainWindow::replaceSelectedTextBy( const QString& text )
{
    QTextCursor cursor = ui->plainTextEdit->textCursor();
    cursor.insertText( text );
    ui->plainTextEdit->setTextCursor( cursor );
}

void MainWindow::insertTextAtCursorPosition( const QString& text )
{
    replaceSelectedTextBy( text );
}

void MainWindow::fileOpen(){
    openFile(QFileDialog::getOpenFileName(this,tr("Open File"),"","*.md *.mkd"));
}

void MainWindow::fileSave(){
    if(NULL == currentFile){
        QString newFileName = QFileDialog::getSaveFileName();
        currentFile = new QFile(newFileName);
        setWindowFilePath(newFileName);
    }
    if (!currentFile->open(QIODevice::WriteOnly | QIODevice::Text)) {
        delete currentFile;
        return;
    }
    QTextStream out(currentFile);
    out << ui->plainTextEdit->toPlainText();
    currentFile->close();
    ui->plainTextEdit->document()->setModified( false );
    updateListView();
    statusBar()->showMessage( tr( "%1 has been saved" ).arg( currentFile->fileName() ) );
}

static QFile* getHTMLFilename(QFile *file){
    QFileInfo qfi(*file);
    QString htmlFilename = qfi.path() + "/";
    if(qfi.suffix() == "md" || qfi.suffix() == "mkd"){
        htmlFilename +=  qfi.baseName() + ".html";
    }
    else {
        htmlFilename +=  qfi.fileName() + ".html";
    }
    return new QFile(htmlFilename);
}

void MainWindow::fileSaveHTML(){
    if(NULL == currentFile){
        QString newFileName = QFileDialog::getSaveFileName();
        currentFile = new QFile(newFileName);
        setWindowFilePath(newFileName);
     }
    QFile *htmlFile = getHTMLFilename(currentFile);
    if (!htmlFile->open(QIODevice::WriteOnly | QIODevice::Text)) {
        delete currentFile;
        return;
    }

    // TODO: this is not optimal, as the css is referenced via qrc and thus no longer applies in the resulting html document.
    // the API of WebEnginePage is asynchronous, so we have to provide a lamda expression
    ui->hswv->page()->toHtml([htmlFile](const QString &result){
      QTextStream out(htmlFile);
      out << result;
      htmlFile->close();
    });

}

void MainWindow::fileSaveAs(){
    QString fileName = QFileDialog::getSaveFileName(this,tr("Save File As"));
    if(NULL != fileName ){
        delete currentFile;
        currentFile = new QFile(fileName);
        setWindowFilePath(fileName);
        fileSave();
    }
}

static QString markdown(QString in){
    struct buf *ib, *ob;
    struct sd_callbacks cbs;
    struct html_renderopt opts;
    struct sd_markdown *mkd;

    if(in.size() > 0){
        QByteArray qba = in.toUtf8();
        const char *txt = qba.constData();
        if(NULL == txt) qDebug() << "txt was null!";
        if(0 < qba.size()){
            ib = bufnew(qba.size());
            bufputs(ib,txt);
            ob = bufnew(64);
            sdhtml_renderer(&cbs,&opts,0);
            mkd = sd_markdown_new(0,16,&cbs,&opts);
            sd_markdown_render(ob,ib->data,ib->size,mkd);
            sd_markdown_free(mkd);
            return QString::fromUtf8(bufcstr(ob));
        }
        else
            qDebug() <<"qstrlen was null";
    }
    return "";
}

static QString wrapInHTMLDoc(QString in){
    QString result(in
            .prepend("<html>\n <head>\n <meta charset=\"utf-8\">\n  <link type=\"text/css\" rel=\"stylesheet\" href=\"qrc:///css/bootstrap.css\"/>\n </head>\n <body>\n")
            .append("\n </body>\n</html>"));
//    qDebug() << result.toStdString().c_str();
    return result;
}

void MainWindow::textChanged(){
    QTime t;
    t.start();
    QString newText = markdown(ui->plainTextEdit->toPlainText());
    QString newHTML = wrapInHTMLDoc(newText);
    renderLabel->setText(QString("Render time: %1 ms").arg(t.elapsed()));

    // store scrollposition of webengine view before updating contents
    QPointF pos = ui->hswv->page()->scrollPosition();

    ui->hswv->page()->setHtml(newHTML);
    ui->sourceView->setPlainText(newText);

    // restore focus to pte, as of some new version of Qt it seems to be lost during updating the other views
    ui->plainTextEdit->setFocus();

    // restore scrollposition via javascrip
    ui->hswv->page()->runJavaScript(QString("window.scrollTo(%1, %2);").arg(pos.x()).arg(pos.y()));
}

void MainWindow::on_actionBold_triggered()
{
    const QString text = selectedText();

    if ( text.isEmpty() ) {
        return;
    }

    replaceSelectedTextBy( QString( "**%1**" ).arg( text ) );
}

void MainWindow::on_actionItalic_triggered()
{
    const QString text = selectedText();

    if ( text.isEmpty() ) {
        return;
    }

    replaceSelectedTextBy( QString( "_%1_" ).arg( text ) );
}

void MainWindow::on_actionBullet_triggered()
{
    const QString text = selectedText();
    replaceSelectedTextBy( QString( "* %1" ).arg( text ) );
}

void MainWindow::on_actionNumbered_Bullet_triggered()
{
    const QString text = selectedText();
    replaceSelectedTextBy( QString( "1. %1" ).arg( text ) );
}

void MainWindow::on_actionCode_triggered()
{
    const QString language = QInputDialog::getText( this, QString::null, tr( "Enter the language name:" ) );

    if ( language.isEmpty() ) {
        return;
    }

    insertTextAtCursorPosition( QString( ":::%1" ).arg( language ) );
}

void MainWindow::on_actionLink_triggered()
{
    const QString text = QInputDialog::getText( this, QString::null, tr( "Enter the link text:" ) );

    if ( text.isEmpty() ) {
        return;
    }

    const QString url = QInputDialog::getText( this, QString::null, tr( "Enter the link url:" ) );

    if ( url.isEmpty() ) {
        return;
    }

    insertTextAtCursorPosition( QString( "[%1](%2)" ).arg( text ).arg( url ) );
}

void MainWindow::on_actionImage_triggered()
{
    const QString text = QInputDialog::getText( this, QString::null, tr( "Enter the image alt text:" ) );

    if ( text.isEmpty() ) {
        return;
    }

    const QString url = QInputDialog::getText( this, QString::null, tr( "Enter the image url:" ) );

    if ( url.isEmpty() ) {
        return;
    }

    insertTextAtCursorPosition( QString( "[%1](%2)" ).arg( text ).arg( url ) );
}

void MainWindow::headerComboBox_activated( int index )
{
    QString text = selectedText();

    if ( text.isEmpty() ) {
        return;
    }

    const int count = index +1;
    const QString header = QString( count, '#' );

    if ( text.startsWith( "#" ) ) {
        while ( text.startsWith( "#" ) ) {
            text.remove( 0, 1 );
        }

        text.remove( 0, 1 );
    }

    replaceSelectedTextBy( QString( "%1 %2" ).arg( header ).arg( text ) );
}
