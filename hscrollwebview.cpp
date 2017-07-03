#include "hscrollwebview.h"
#include <QDebug>
#include <QWheelEvent>

HScrollWebView::HScrollWebView(QWidget *parent) :
    QWebEngineView(parent)
{
}

void HScrollWebView::wheelEvent(QWheelEvent *ev){
  this->page()->runJavaScript(QString("window.scrollBy(%1, 0);").arg( ev->delta()));
}
