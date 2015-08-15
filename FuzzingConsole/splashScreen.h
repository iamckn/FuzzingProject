#ifndef SPLASHSCREEN_H
#define SPLASHSCREEN_H

#include <QSplashScreen>
#include <QTimer>

class splashScreen : public QSplashScreen
{
    Q_OBJECT
 public:
     explicit splashScreen(const QPixmap &pixmap = QPixmap(), Qt::WindowFlags f = 0);
     // splashScreen(QWidget * parent, const QPixmap & pixmap = QPixmap(), Qt::WindowFlags f = 0);
     void setShowSecond(int _second);//设置启动画面的显现时长
     void startTimer()
     {
         show();
         timer.start();
     }//定时器开始工作
 signals:
     void timeOver();//达到规定的显示时长发出此信号
 public slots:
 protected slots:
     void stopTimer();//关闭定时器
 private:
      ~splashScreen(){}
      QTimer timer;//显示时长定时器
};

#endif // SPLASHSCREEN_H
