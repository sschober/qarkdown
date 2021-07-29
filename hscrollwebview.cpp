#include "hscrollwebview.h"
#include <QDebug>
#include <QWheelEvent>
#include <QWebChannel>

HScrollWebView::HScrollWebView(QWidget *parent) :
    QWebEngineView(parent)
{
  connect(this->page(), SIGNAL(loadFinished(bool)), this, SLOT(scrollToCursor()));
  QWebChannel* channel = new QWebChannel(this);
  this->page()->setWebChannel(channel);
  channel->registerObject(QString("app"), this);
}

void HScrollWebView::wheelEvent(QWheelEvent *ev){
  this->page()->runJavaScript(QString("window.scrollBy(%1, 0);").arg( ev->pixelDelta().x()));
}

void HScrollWebView::scrollToCursor(){
  QString jsScrollScript(
        "var element = document.getElementById(\"qdCursor\");"
        "if(typeof element != 'undefined'){"
        "   element.scrollIntoView(); "
        "}"
  );

  this->page()->runJavaScript(jsScrollScript);

  QString jsWebChannelConnect(
                              "var elements = document.getElementsByTagName('a');"
                              "console.log(\"found \", elements.length, \" elements\");"
                              "for(var i = 0, len = elements.length; i < len; i++){"
                              " console.log(\"installing onclick handler on \",i);"
                              " elements[i].onclick = function(e){"
                              "   console.log('onclick handler ', e);"
                              "   if(typeof window.app == 'undefined'){"
                              "     new QWebChannel(qt.webChannelTransport, function(channel){"
                              "       console.log(channel.objects.app);"
                              "       window.app = channel.objects.app;"
                              "       window.app.linkClicked(e.target.href);"
                              "     });"
                              "   }"
                              "   else{"
                              "     window.app.linkClicked(e.target.href);"
                              "   }"
                              "   return false;"
                              " }"
                              "};"
        );
  this->page()->runJavaScript(jsWebChannelConnect);
}

void HScrollWebView::linkClicked(QString linkTarget){
  QString navigationRequestTarget(linkTarget.remove(0, linkTarget.lastIndexOf('/') + 1).replace("html", "mkd"));
  qDebug() << "link clicked: " << navigationRequestTarget;
  emit navigationRequest(navigationRequestTarget);
}
