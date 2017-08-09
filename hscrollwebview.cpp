#include "hscrollwebview.h"
#include <QDebug>
#include <QWheelEvent>

HScrollWebView::HScrollWebView(QWidget *parent) :
    QWebEngineView(parent)
{
  connect(this->page(), SIGNAL(loadFinished(bool)), this, SLOT(scrollToCursor()));
}

void HScrollWebView::wheelEvent(QWheelEvent *ev){
  this->page()->runJavaScript(QString("window.scrollBy(%1, 0);").arg( ev->delta()));
}

void HScrollWebView::scrollToCursor(){
  QString jsScrollScript(
        "   var element = document.getElementById(\"qdCursor\");"
        "   element.scrollIntoView(); "
  );

  this->page()->runJavaScript(jsScrollScript);
}
