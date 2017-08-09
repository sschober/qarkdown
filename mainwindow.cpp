#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ui_settings.h"
#include "highlighter.h"

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
#include <QWebEngineSettings>

#include <cmath> // for round

MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow),
  currentFile(NULL),
  renderLabel(new QLabel),
  projectSettings(NULL)
{
  ui->setupUi(this);
  statusBar()->addPermanentWidget( renderLabel );

  readUserSettings();
  readProjectSettings();

  QComboBox* headerComboBox = new QComboBox( this );

  for ( int i = 1; i <= 6; i++ ) {
    headerComboBox->addItem( QString( "H%1" ).arg( i ), i );
  }

  QAction* separator = ui->toolBar->insertSeparator( ui->actionBold );
  ui->toolBar->insertWidget( separator, headerComboBox );

  // Set font
  currentFont = new QFont();
  currentFont->setFamily("Courier");
  currentFont->setFixedPitch(true);
  currentFont->setPointSize(currentFont->pointSize() +5);
  qDebug() << "editor font size: " << currentFont->pointSize();
  ui->plainTextEdit->setFont(*currentFont);

  const float targetWidth = 67.0;
  // set plaintextedit to 80 character column width
  int columnWidth = round( QFontMetrics(*currentFont).averageCharWidth() * targetWidth)
                    + ui->plainTextEdit->document()->documentMargin();
  ui->plainTextEdit->setFixedWidth(columnWidth);

  new HGMarkdownHighlighter(ui->plainTextEdit->document(), 1000);

  ui->actionNew->setShortcut(QKeySequence::New);
  ui->actionOpen->setShortcut(QKeySequence::Open);
  ui->actionSave->setShortcut(QKeySequence::Save);
  ui->actionSource->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_U));
  ui->actionDirectory->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_D));
  ui->actionExport_HTML->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_E));
  ui->actionSettings->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Comma));
  ui->actionProjectSettings->setEnabled(false);

  // when text changes in the plaintexteditor, we update the web view
  connect(ui->plainTextEdit, SIGNAL(textChanged()),this, SLOT(textChanged()));
  // when the cursor position changes, we update the web view
  connect(ui->plainTextEdit, SIGNAL(cursorPositionChanged()), this, SLOT(textChanged()));

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

  connect( ui->plainTextEdit, SIGNAL(imageDropped(QString)), this, SLOT(imageDropped(QString)));
  ui->sourceView->hide();

  ui->listView->hide();

  if(qApp->arguments().size() > 1){
    // a file was given on the command line
    openFile(qApp->arguments().at(1));
  }
  else if(NULL != currentFile){
    openFile(currentFile->fileName());
  }
  else {
    fileNew();
  }

  QWebEngineSettings::defaultSettings()->setAttribute(QWebEngineSettings::LocalContentCanAccessFileUrls, true);
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
  writeUserSettings();

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
    currentFileLastRead = QDateTime::currentDateTime();
    qDebug() << "read " << fileName << " at " << currentFileLastRead.toString();
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
    currentFileLastRead = QDateTime::currentDateTime();
    qDebug() << "read " << newFileName << " at " << currentFileLastRead.toString();
    setWindowFilePath(newFileName);
  }
  QFileInfo qfi(*currentFile);
  QMessageBox::StandardButton reply = QMessageBox::Yes;
  if(qfi.lastModified() > currentFileLastRead){
    qDebug() << "emitting warning: " << qfi.lastModified().toString() << " > " << currentFileLastRead.toString();
    reply = QMessageBox::question(this, "File changed on disk", "The file which is currently displayed in the editor has changed on disk, since it was last read. Do you want to save the contents of the editor, thus overwriting the changes on disk?", QMessageBox::Yes|QMessageBox::No);
  }
  if( QMessageBox::Yes == reply){
    if (!currentFile->open(QIODevice::WriteOnly | QIODevice::Text)) {
      delete currentFile;
      return;
    }
    QTextStream out(currentFile);
    out << ui->plainTextEdit->toPlainText();
    currentFile->close();
    currentFileLastRead = QDateTime::currentDateTime();
  }
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

void MainWindow::on_actionSettings_triggered(){
  QDialog *settingsDialog = new QDialog();
  Ui::SettingsDialog settingsUi;
  settingsUi.setupUi(settingsDialog);
  settingsUi.fontComboBox->setFontFilters(QFontComboBox::MonospacedFonts);
  settingsUi.fontComboBox->setCurrentFont(*currentFont);
  settingsUi.fontSize->setValue(currentFont->pointSize());
  settingsDialog->exec();
  reopenLastFile = settingsUi.reopenLastFile->isEnabled();
}

static QString markdown(QString in){

  if(in.size() > 0){
    QByteArray qba = in.toUtf8();
    const char *txt = qba.constData();
    if(NULL == txt) qDebug() << "txt was null!";
    if(0 < qba.size()){
      hoedown_renderer *renderer = hoedown_html_renderer_new((hoedown_html_flags)NULL,16);
      // TODO: investigate further extensions
      unsigned int extensions = HOEDOWN_EXT_FOOTNOTES | HOEDOWN_EXT_SPECIAL_ATTRIBUTE;
      hoedown_document *document = hoedown_document_new(renderer, (hoedown_extensions)extensions, 16, 0, NULL, NULL);
      hoedown_buffer *in_buf = hoedown_buffer_new(qba.size());
      hoedown_buffer_puts(in_buf, txt);
      hoedown_buffer *html = hoedown_buffer_new(64);
      hoedown_document_render(document, html, in_buf->data,in_buf->size);
      QString result(QString::fromUtf8(hoedown_buffer_cstr(html)));
      hoedown_buffer_free(html);
      hoedown_buffer_free(in_buf);
      hoedown_document_free(document);
      hoedown_html_renderer_free(renderer);
      return result;
    }
    else
      qDebug() <<"qstrlen was null";
  }
  return "";
}

QString MainWindow::wrapInHTML(QString in){
  QString charset("<meta charset=\"utf-8\">");

  QString stylesheetPath("qrc:///css/bootstrap.css");
  QString stylesheet;
  if(NULL != projectSettings && NULL != currentFile && projectSettings->contains("stylesheet")){
    // project path + configured css path
    stylesheetPath = QFileInfo(currentFile->fileName()).dir().path() + "/" + projectSettings->value("stylesheet").toString();
    QFile stylesheetFile(stylesheetPath);
    stylesheetFile.open(QFile::ReadOnly);
    stylesheet = QString("<style>\n%1\n</style>").arg(QString(stylesheetFile.readAll()));
  }
  else {
    stylesheet = QString("<link type=\"text/css\" rel=\"stylesheet\" href=\"%1\"/>").arg(stylesheetPath);
  }

  QString header = QString("%1\n\t\t\t%2").arg(charset).arg(stylesheet);

  QString body = in;

  QString result = QString("<html>\n\t<head>\n\t\t%1\n\t</head>\n\t<body><div class=\"content\">\n\t\t%2\n\t</div></body>\n</html>").arg(header).arg(body);

  //qDebug() << result.toStdString().c_str();

  return result;
}

void MainWindow::textChanged(){
  QTime t;
  t.start();
  QString editorText(ui->plainTextEdit->toPlainText());
  editorText.insert(ui->plainTextEdit->textCursor().position(), QString::fromUtf8("<span id=\"qdCursor\">\u2663</span>"));
  QString newText = markdown(editorText);
  QString newHTML = wrapInHTML(newText);
  renderLabel->setText(QString("Render time: %1 ms").arg(t.elapsed()));

  ui->hswv->page()->setHtml(newHTML);
  ui->sourceView->setPlainText(newText);

  // restore focus to pte, as of some new version of Qt it seems to be lost during updating the other views
  ui->plainTextEdit->setFocus();
  ui->hswv->page()->settings()->setAttribute(QWebEngineSettings::LocalContentCanAccessFileUrls,true);
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

void MainWindow::readUserSettings(){
  userSettings = new QSettings("SSSM", "QarkDown");
  userSettings->beginGroup("MainWindow");
  if(userSettings->value("saveGeometry", false).toBool()){
    resize(userSettings->value("size",QSize(400,400)).toSize());
    move(userSettings->value("pos", QPoint(200,200)).toPoint());
  }
  if(userSettings->value("reopenLastFile", false).toBool()){
    reopenLastFile = true;
    currentFile = new QFile(userSettings->value("lastFile").toString());
  }
  userSettings->endGroup();
}

void MainWindow::writeUserSettings(){
  userSettings->beginGroup("MainWindow");

  if(reopenLastFile){
    if(NULL != currentFile){
      userSettings->setValue("reopenLastFile", true);
      userSettings->setValue("lastFile",windowFilePath());
    }
  }
  userSettings->endGroup();
  userSettings->sync();
}

void MainWindow::readProjectSettings(){
  if(NULL != currentFile){
    QString path(QFileInfo(currentFile->fileName()).dir().path());
    if(QFileInfo(path + "/.qarkdownrc").exists()){
      qDebug() << "found project file under path: " << path;
      ui->actionMake_directory_a_project->setEnabled(false);
      ui->actionProjectSettings->setEnabled(true);
      // TODO: load project specific settings
      projectSettings = new QSettings(path + "/.qarkdownrc", QSettings::IniFormat);
    }
    else{
      qDebug() << "found no project file under path: " << path;
      ui->actionMake_directory_a_project->setEnabled(true);
      ui->actionProjectSettings->setEnabled(false);
      projectSettings = NULL;
    }
  }
}

void MainWindow::imageDropped(QString path){
  qDebug() << "image dropped:  " << path;

  QFile imageFile(path);

  QString imageDirPath(currentFile->fileName() + ".img");

  QDir imageDir(imageDirPath);

  if(!imageDir.exists()){
    QDir projectDir(QFileInfo(currentFile->fileName()).dir());
    if(!projectDir.mkdir(currentFile->fileName() + ".img")){
      qWarning() << " error creating dir: " << currentFile->fileName() << ".img";
      return;
    }
  }

  QString targetPath(imageDirPath + "/" + QFileInfo(imageFile).fileName());
  qDebug() << "copy image to:  " << targetPath;
  if(! QFile::copy(path, targetPath)){
    qWarning() << " error copying file: ";
    return;
  }

  ui->plainTextEdit->insertPlainText(QString("![](file://%1)").arg(targetPath));
}
