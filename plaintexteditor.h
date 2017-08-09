#ifndef PLAINTEXTEDITOR_H
#define PLAINTEXTEDITOR_H

#include <QPlainTextEdit>

/**
 * @brief The PlainTextEditor class
 *
 * A custom editor, that adds indentation to the standad Qt widget.
 */
class PlainTextEditor : public QPlainTextEdit
{
    Q_OBJECT

public:
    PlainTextEditor( QWidget* parent = 0 );

    virtual bool event( QEvent* event );

public slots:
    void indentOrUnindent( bool doIndent );

signals:
    void imageDropped(QString path);

protected:
    void dragEnterEvent(QDragEnterEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dragLeaveEvent(QDragLeaveEvent *event);
    void dropEvent(QDropEvent *event);

private:
    void onTabKey( QKeyEvent* event );
    void onShiftTabKey(QKeyEvent* event );
    void onSideNoteShortCut(QKeyEvent* event);
    int firstNonSpace( const QString& text ) const;
    int columnAt( const QString& text, int position ) const;
    int indentedColumn( int column, bool doIndent ) const;
};

#endif // PLAINTEXTEDITOR_H
