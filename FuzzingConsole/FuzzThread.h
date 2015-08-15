#ifndef FUZZTHREAD_H
#define FUZZTHREAD_H

#endif // FUZZTHREAD_H
#include <QThread>
#include <QString>
#include <QQueue>
#include "Sockets.h"
#include <vector>
#include "Logger.h"
#include <QMultiMap>
#define BTSCHECK 1


extern bool PHONECHECK;
extern bool PHONECONNECT;
extern UDPSocket pySock;
extern UDPSocket btsSock;
extern UDPSocket phoSock;

extern QString IMSI;
extern QStringList GSMmessageTable;
extern QMultiMap<QString,QStringList> bugMap;
extern struct BTSPara btsPara;

QByteArray console_msg(QString="", int timeout=5000, bool ifread=true);
QString phone_msg(QString msg="",int timeout=20000,bool ifread=true);
QString bts_msg(QByteArray msg="",int timeout=60000,bool ifread=true);

class FuzzThread: public QThread
{
    Q_OBJECT
public:
    FuzzThread();
    ~FuzzThread();
signals:
    void raiseMsgbox(QString);
    void setProcess(int);
    void setMessage(QString);
    void addTab(QString,QString);
    void FuzzResult(QString msg,bool integer,bool state);
    void sendBTSinfo();
    void sendReport(QString htm);
    void imsigetted(QString);
public slots:
    void addTask(QString a){tasks.enqueue(a);}
    void start();
    void stop(bool a){stopped=a;}
    void force_abort(){abort=true;}
    void setBtsinfo();

protected:
    void run();
private:
    QString cur_msg;
    QByteArray py_rsp;
    QString bts_rsp;
    QString pho_rsp;
    bool hasbug;

    bool stopped;
    bool abort;
    void fuzzAll();
    void test();
    bool fuzz(QString, QString count, int position, int all);
    void generateReport();
    void getPhoneinfo();
    void getBtsinfo();
    QString getimsi();
    QQueue<QString> tasks;
};
