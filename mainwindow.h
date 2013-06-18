#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFile>
#include <QModelIndex>
#include <QPointer>

#include "hscrollwebview.h"

namespace Ui {
class MainWindow;
}

class QLabel;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void textChanged();
    void fileNew();
    void fileOpen();
    void fileSave();
    void fileSaveHTML();
    void fileSaveAs();
    void viewSource();
    void viewDirectory();
    void dirViewClicked(QModelIndex idx);
    void about();
    void aboutQt();

protected:
    void closeEvent( QCloseEvent* event );

private slots:
    void on_actionBold_triggered();
    void on_actionItalic_triggered();
    void on_actionBullet_triggered();
    void on_actionNumbered_Bullet_triggered();
    void on_actionCode_triggered();
    void on_actionLink_triggered();
    void on_actionImage_triggered();
    void headerComboBox_activated( int index );

private:
    void openFile(QString fileName);
    void updateListView();
    QString selectedText();
    void replaceSelectedTextBy( const QString& text );
    void insertTextAtCursorPosition( const QString& text );

    Ui::MainWindow *ui;
    QPointer<QFile> currentFile;
    QLabel* renderLabel;
};

#endif // MAINWINDOW_H
