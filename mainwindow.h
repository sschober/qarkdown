#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFile>

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
    void fileOpen();
    void fileSave();
    void fileSaveAs();
    void viewSource();
    void viewDirectory();

private:
    Ui::MainWindow *ui;
    QFile *currentFile;
};

#endif // MAINWINDOW_H
