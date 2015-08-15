#include "splashScreen.h"

splashScreen::splashScreen(const QPixmap &pixmap, Qt::WindowFlags f) : QSplashScreen(pixmap,f)
{
}
//splashScreen::splashScreen(QWidget * parent, const QPixmap & pixmap = QPixmap(), Qt::WindowFlags f = 0)://    QSplashScreen(parent, pixmap, f)//{//}

void splashScreen::setShowSecond(int _second)
{
    timer.setInterval(_second*1000);
    connect(&timer,SIGNAL(timeout()),this,SLOT(stopTimer()));
}
void splashScreen::stopTimer()
{
    timer.stop();
    emit timeOver();
    close();
    deleteLater();//此处是特意添加，只能在一次使用，且只能是在heap区使用
}

