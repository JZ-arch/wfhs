#include "widget.h"
#include <QApplication>
#include <QTextCodec>
#include <QFont>
#include <QStyleFactory>
#include <QWSServer>
#include <QMessageBox>
#include <QStandardItem>
#include <QCloseEvent>
#include <QObject>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

   // QWSServer::setCursorVisible(false);

     QTextCodec::setCodecForTr(QTextCodec::codecForName("utf-8"));
     QTextCodec::setCodecForCStrings(QTextCodec::codecForName("utf-8"));
   //QTextCodec::setCodecForTr(QTextCodec::codecForName("GB2312"));

    a.setFont(QFont("unifont", 16));
   // a.setFont(QFont("wenquanyi", 16));
    //a.setInputContext(new MyInputPanelContext);

    Widget w;
   // w.setCursor(QCursor(Qt::BlankCursor)); // for pad use ,
    w.show();
    QMessageBox::information(0,QObject::tr("提醒"),QObject::tr("请登录！"));


  //  QStringList keyList = QStyleFactory::keys();

  //  for(int i = 0; i < keyList.length(); i++)
   // {
  //      qDebug(keyList.at(i).toUtf8());
 // //  }

    a.setStyle(QStyleFactory::create("Plastique"));//设置界面风格  "macintosh"));//

    return a.exec();
}
