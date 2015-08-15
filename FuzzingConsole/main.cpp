#include <QtGui/QApplication>
#include "mainwindow.h"
#include <splashScreen.h>
#include <QTimer>
#include <QtGui/QApplication>  //启动画面

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    //w.show();
    splashScreen *screen=new splashScreen(QPixmap(":/img1/start.png"),Qt::WindowStaysOnTopHint);
    screen->setWindowOpacity(1);
    screen->setDisabled(true);

    screen->startTimer();
    screen->setShowSecond(1);
    screen->showMessage(QObject::tr("加载模块......"),Qt::AlignLeft,Qt::yellow);
    //loadModules()加载模块函数
    screen->showMessage(QObject::tr("建立连接......"),Qt::AlignRight|Qt::AlignBottom,Qt::yellow);
    //establishingConnections()建立连接函数
   // Widget w;
    QObject::connect(screen,SIGNAL(timeOver()),&w,SLOT(show()));

    return a.exec();
}
