#ifndef HSCROLLWEBVIEW_H
#define HSCROLLWEBVIEW_H

#include <QWebView>

class HScrollWebView : public QWebView
{
    Q_OBJECT
public:
    explicit HScrollWebView(QWidget *parent = 0);
    
protected:
    virtual void wheelEvent(QWheelEvent *ev);

signals:
    
public slots:
    
};

#endif // HSCROLLWEBVIEW_H
