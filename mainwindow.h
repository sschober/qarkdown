#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

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
private:
    Ui::MainWindow *ui;
    QString fileName;
};

#endif // MAINWINDOW_H
