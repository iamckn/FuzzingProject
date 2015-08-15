#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "aboutd.h"
#include <QDirModel>
#include <QStandardItemModel>
#include "Sockets.h"
#include "FuzzThread.h"
#include <QFileSystemModel>
#include <QCompleter>

class QListWidget;
class QListWidgetItem;
class QStackedWidget;
class QTreeWidgetItem;

namespace Ui {
    class MainWindow;
}

extern QString conFile;
QStringList getValueFromfile(QString key);
void addKeyVal2file(QString key,QString value);
void initializeGlobal();
extern pid_t sulleyPid;

class sulley_setting{
public:
    sulley_setting(Ui::MainWindow *wui):ui(wui){}
    bool bit_testall;
    QString bit_minEdge;
    QString bit_maxRange;
    bool Lfuzz;
    QString string_value;
    QString string_maxlen;
    QString random_minreps;
    QString random_maxreps;
    QString random_maxmuns;
    QString get_cmd();
private:
    Ui::MainWindow *ui;
    void getUIval(Ui::MainWindow *ui);
};


class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = 0);
    int flag;
    ~MainWindow();
    void showAbout();
    friend class sulley_setting;

signals:


private slots:
    void on_action_A_triggered();

    void show_tabn(int);
    void msgbox(QString);
    //phone-----fumctions
    void add_phoneTab(QString,QString);//react
    void refresh();
    void phonecheck_change(bool);
    //bts--------functions
    void getBTSinfo();
    void onBTSset();
    void onBTSquery();
    //sulley--------functions

    void sulley_add();
    void sulley_remove();
    void select_quick();
    void select_normal();
    void select_deep();

    void MainGetIMSI();
    void start_test();
    //exec----------functions
    void refresh_execTab();
    void set_result(QString, bool, bool);//react to fuzzthread
    void exec_stop();
    //log fuc
    void on_viewrep_clicked();
    void rm();
    void do_file_save();


private:
    Ui::MainWindow *ui;
    FuzzThread fthread;
    QCompleter* imsi;
    void initialize_tab_phone();
    void initialize_tab_BTS();
    void initialize_tab_testconf();
    void initialize_tab_testexec();
    void initialize_tab_testlog();
    void setBTSinfo(BTSPara&);
    void do_file_Open();
    bool do_file_Load(const QString& fileName);
    QString curFile;
    QFileSystemModel *filemodel;

    QStandardItemModel* sulleyMsg;
    QStandardItemModel* sulleySelectedMsg;
    QStandardItemModel* testExecModel;
    sulley_setting* ss;
    int test_type;
};

#endif // MAINWINDOW_H
