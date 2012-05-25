#include "hscrollwebview.h"
#include <QDebug>
#include <QWheelEvent>
#include <QWebPage>
#include <QWebFrame>

HScrollWebView::HScrollWebView(QWidget *parent) :
    QWebView(parent)
{
}

void HScrollWebView::wheelEvent(QWheelEvent *ev){
    page()->currentFrame()->scroll(-1 * ev->delta(),0);
}
