#ifndef HSCROLLWEBVIEW_H
#define HSCROLLWEBVIEW_H

#include <QWebEngineView>

class HScrollWebView : public QWebEngineView
{
    Q_OBJECT

  public:
    explicit HScrollWebView(QWidget *parent = 0);
    
  protected:
    virtual void wheelEvent(QWheelEvent *ev);

  signals:
    void navigationRequest(QString linkTarget);

  public slots:
    void scrollToCursor();
    Q_INVOKABLE void linkClicked(QString linkTarget);
    
};

#endif // HSCROLLWEBVIEW_H
