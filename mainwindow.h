#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFile>
#include <QModelIndex>

namespace Ui {
class MainWindow;
}

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
    void fileSaveAs();
    void viewSource();
    void viewDirectory();
    void dirViewClicked(QModelIndex idx);


private:
    void openFile(QString fileName);
    void updateListView();

    Ui::MainWindow *ui;
    QFile *currentFile;
};

#endif // MAINWINDOW_H
