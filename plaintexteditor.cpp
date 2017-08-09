/*
 * Indent / Unindent code taken from project at: https://code.google.com/p/kodepad
 **/

#include "plaintexteditor.h"

#include <QTextCursor>
#include <QTextBlock>

#define IndentSize 4
#define TabSize 4

PlainTextEditor::PlainTextEditor( QWidget* parent )
  : QPlainTextEdit( parent )
{
  setTabStopWidth( 40 );
  setTabChangesFocus( false );
  setAcceptDrops(true);
}

#include <QDebug>

bool PlainTextEditor::event( QEvent* event )
{
  if ( event->type() == QEvent::KeyPress ) {
    QKeyEvent* ke = static_cast<QKeyEvent*>( event );
    const bool isBackTab =
        ( ke->key() == Qt::Key_Backtab && ( ke->modifiers() == Qt::NoModifier || ke->modifiers() == Qt::ShiftModifier ) ) ||
        ( ke->key() == Qt::Key_Tab && ke->modifiers() == Qt::ShiftModifier )
        ;
    const bool isTab = ke->key() == Qt::Key_Tab && ke->modifiers() == Qt::NoModifier;

    const bool isSideNoteShortCut = Qt::Key_S == ke->key() &&
                                    Qt::ShiftModifier & ke->modifiers() &&
                                    Qt::AltModifier & ke->modifiers();

    if ( isTab ) {
      onTabKey( ke );
      return true;
    }
    else if ( isBackTab ) {
      onShiftTabKey( ke );
      return true;
    }
    else if(isSideNoteShortCut){
      onSideNoteShortCut(ke);
      return true;
    }
  }

  return QPlainTextEdit::event( event );
}

void PlainTextEditor::onSideNoteShortCut(QKeyEvent *event){
  Q_UNUSED(event);

  textCursor().insertText("{@paragraph .sidenote}");
}

void PlainTextEditor::onTabKey( QKeyEvent* event )
{
  Q_UNUSED( event );

  if ( !textCursor().hasSelection() ) {
    textCursor().insertText( QString( IndentSize, QLatin1Char( ' ' ) ) );
  }
  else {
    indentOrUnindent( true );
  }
}

void PlainTextEditor::onShiftTabKey( QKeyEvent* event )
{
  Q_UNUSED( event );

  if ( !textCursor().hasSelection() ) {
    QTextCursor cursor = textCursor();
    int newPos = cursor.position() -IndentSize;

    if ( newPos < 0 ) {
      newPos = 0;
    }

    cursor.setPosition( newPos );
    setTextCursor( cursor );
  }
  else {
    indentOrUnindent( false );
  }
}

void PlainTextEditor::indentOrUnindent( bool doIndent )
{
  QTextCursor cursor = textCursor();
  cursor.beginEditBlock();

  // Indent or unindent the selected lines
  int pos = cursor.position();
  int anchor = cursor.anchor();
  int start = qMin( anchor, pos );
  int end = qMax( anchor, pos );

  QTextDocument* doc = document();
  QTextBlock startBlock = doc->findBlock(start);
  QTextBlock endBlock = doc->findBlock( end -1 ).next();

  for ( QTextBlock block = startBlock; block != endBlock; block = block.next() ) {
    QString text = block.text();

    if ( doIndent ) {
      const int indentPosition = firstNonSpace( text );
      cursor.setPosition( block.position() +indentPosition );
      cursor.insertText( QString( IndentSize, QLatin1Char( ' ' ) ) );
    } else {
      const int indentPosition = firstNonSpace( text );
      const int targetColumn = indentedColumn( columnAt( text, indentPosition ), false );
      cursor.setPosition( block.position() +indentPosition );
      cursor.setPosition( block.position() +targetColumn, QTextCursor::KeepAnchor );
      cursor.removeSelectedText();
    }
  }

  // Reselect the selected lines
  cursor.setPosition( startBlock.position() );
  cursor.setPosition( endBlock.previous().position(), QTextCursor::KeepAnchor );
  cursor.movePosition( QTextCursor::EndOfBlock, QTextCursor::KeepAnchor );

  cursor.endEditBlock();
  setTextCursor( cursor );
}

int PlainTextEditor::firstNonSpace( const QString& text ) const
{
  int i = 0;

  while ( i < text.size() ) {
    if ( !text.at( i ).isSpace() ) {
      return i;
    }

    ++i;
  }

  return i;
}

int PlainTextEditor::columnAt( const QString& text, int position ) const
{
  int column = 0;

  for ( int i = 0; i < position; ++i ) {
    if ( text.at( i ) == QLatin1Char( '\t' ) ) {
      column = column -( column %TabSize ) +TabSize;
    }
    else {
      ++column;
    }
  }

  return column;
}

int PlainTextEditor::indentedColumn( int column, bool doIndent ) const
{
  const int aligned = ( column /IndentSize ) *IndentSize;

  if ( doIndent ) {
    return aligned +IndentSize;
  }

  if ( aligned < column ) {
    return aligned;
  }

  return qMax( 0, aligned -IndentSize );
}

void PlainTextEditor::dragEnterEvent(QDragEnterEvent *event){
  qDebug() << "drop enter event with mime data: " << event->mimeData()->text();
  event->acceptProposedAction();
}

void PlainTextEditor::dragMoveEvent(QDragMoveEvent *event){
  event->acceptProposedAction();
}

void PlainTextEditor::dropEvent(QDropEvent *event){

  const QMimeData *mimeData = event->mimeData();

  qDebug() << "drop event: " << mimeData->formats();

  if(mimeData->hasImage()){
    qDebug() << "drop event containing image (" << mimeData << "). trying to embedd...";
  }
  if(mimeData->hasText()){
    qDebug() << "text: " << mimeData->text();
  }
  if(mimeData->hasHtml()){
    qDebug() << "html: " << mimeData->html();
  }
  if(mimeData->hasUrls()){
    QList<QUrl> urlList = mimeData->urls();

    qDebug() << "urls: " << urlList;

    if(1 < urlList.size()){
      qWarning() << " list size " << urlList.size() << " larger thant 1. using only first URL.";
    }

    emit imageDropped(urlList.at(0).toLocalFile());
  }

  event->acceptProposedAction();
}

void PlainTextEditor::dragLeaveEvent(QDragLeaveEvent *event){
  qDebug() << "drag leave";
  event->accept();
}

