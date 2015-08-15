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
     void setShowSecond(int _second);//�����������������ʱ��
     void startTimer()
     {
         show();
         timer.start();
     }//��ʱ����ʼ����
 signals:
     void timeOver();//�ﵽ�涨����ʾʱ���������ź�
 public slots:
 protected slots:
     void stopTimer();//�رն�ʱ��
 private:
      ~splashScreen(){}
      QTimer timer;//��ʾʱ����ʱ��
};

#endif // SPLASHSCREEN_H
