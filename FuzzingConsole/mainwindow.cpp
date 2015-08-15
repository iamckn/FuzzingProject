#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QTextCodec>
#include <QtGui>
#include <QTreeWidget>
#include <QTreeView>
#include <QMessageBox>
#include <QStandardItemModel>
#include <QFile>
#include <QTextStream>
#include <unistd.h>
#include <exception>
#include <vector>
#include <sys/types.h>
#include <signal.h>
#include "networkmessages.h"

class QListWidget;
class QListWidgetItem;
class QStackedWidget;

bool PHONECHECK=false;
bool PHONECONNECT=false;
pid_t sulleyPid;
QString IMSI;
QStringList GSMmessageTable;
QMultiMap<QString,QStringList> bugMap;
QString conFile("./config/settings.config");
struct BTSPara btsPara;

char py_ip[64]="localhost";
char phone_ip[64]="121.248.48.123";
char bts_ip[64]="121.248.55.28";
int send_py_port=49101;
int rev_py_port=49001;
int send_bts_port=6001;
int rev_bts_port=6101;
int send_pho_port=46002;
int rev_pho_port=46102;
UDPSocket pySock(rev_py_port,py_ip,send_py_port);
UDPSocket btsSock(rev_bts_port,bts_ip,send_bts_port);
UDPSocket phoSock(rev_pho_port,phone_ip,send_pho_port);

//===============================================global functions=======================================================
QStringList getValueFromfile(QString key){
    QStringList resultlist;
    QFile fid(conFile);
    if(!fid.open(QIODevice::ReadOnly))
        return resultlist;
    while(!fid.atEnd()){
        QString tem(fid.readLine());
        tem.chop(1);
        if(key==tem.section(" ",0,0)){
            resultlist.push_back(tem.section(" ",1,1));
        }
    }
    fid.close();
    return resultlist;
}
void addKeyVal2file(QString key,QString value){
    QStringList tem=getValueFromfile(key);
    if(tem.contains(value)) return;
    QFile fid(conFile);
    fid.open(QIODevice::Append);
    fid.write(key.toAscii());
    fid.write(" ");
    fid.write(value.toAscii());
    fid.write("\n");
    fid.close();
}

void initializeGlobal()
{
    pySock.nonblocking();
    btsSock.nonblocking();
    phoSock.nonblocking();
    GSMmessageTable.push_back(QString("pagingRequest1"));
    GSMmessageTable.push_back(QString("MMInfo"));
    GSMmessageTable.push_back(QString("setUp"));
    GSMmessageTable.push_back(QString("locationUpdateAccept"));
    QStringList tem=getValueFromfile("phoneIP");
    if(!tem.isEmpty()) strcpy(phone_ip,tem.takeFirst().toAscii().data());
    tem=getValueFromfile("btsIP");
    if(!tem.isEmpty()) strcpy(bts_ip,tem.takeFirst().toAscii().data());
    tem=getValueFromfile("btsPort");
    if(!tem.isEmpty()) send_bts_port=tem.takeFirst().toInt();
    tem=getValueFromfile("phonePort");
    if(!tem.isEmpty()) send_pho_port=tem.takeFirst().toInt();
    btsSock.destination(send_bts_port,bts_ip);
    phoSock.destination(send_pho_port,phone_ip);

    PHONECHECK=false;
}

void start_sulley(){
    // This is harmless - if someone is running OpenBTS they WANT no transceiver
    // instance at the start anyway.
    if (sulleyPid > 0) {
        LOG(INFO) << "RESTARTING sulley";
        kill(sulleyPid,SIGKILL); // TODO - call on ctrl-c (put in signal?)
    }

    // Start the transceiver binary, if the path is defined.
    // If the path is not defined, the transceiver must be started by some other process.
    sulleyPid = vfork();
    assert(sulleyPid>=0);
    if (sulleyPid==0) {
            // Pid==0 means this is the process that starts the transceiver.
        execl("./fuzz_pro/openbtsControl.py","openbtsControl.py",NULL);
        LOG(ERROR) << "cannot start python";
        _exit(0);
    }
}
unsigned uplinkFreqKHz(GSMBand band, unsigned ARFCN)
{
    switch (band) {
        case GSM850:
            assert((ARFCN<252)&&(ARFCN>129));
            return 824200+200*(ARFCN-128);
        case EGSM900:
            if (ARFCN<=124) return 890000+200*ARFCN;
            assert((ARFCN>974)&&(ARFCN<1024));
            return 890000+200*(ARFCN-1024);
        case DCS1800:
            assert((ARFCN>511)&&(ARFCN<886));
            return 1710200+200*(ARFCN-512);
        case PCS1900:
            assert((ARFCN>511)&&(ARFCN<811));
            return 1850200+200*(ARFCN-512);
        default:
            assert(0);
    }
}
unsigned uplinkOffsetKHz(GSMBand band)
{
    switch (band) {
        case GSM850: return 45000;
        case EGSM900: return 45000;
        case DCS1800: return 95000;
        case PCS1900: return 80000;
        default: assert(0);
    }
}


unsigned downlinkFreqKHz(GSMBand band, unsigned ARFCN)
{
    return uplinkFreqKHz(band,ARFCN) + uplinkOffsetKHz(band);
}

//======================================================mainwindow fucs=======================================================
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("GB2312"));
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("GB2312"));
    QTextCodec::setCodecForTr(QTextCodec::codecForName("GB2312"));
    connect(&fthread,SIGNAL(raiseMsgbox(QString)),this,SLOT(msgbox(QString)));
    connect(ui->action_A,SIGNAL(triggered()),this,SLOT(on_action_A_triggered()));


    initializeGlobal();
    start_sulley();
    initialize_tab_phone();
    initialize_tab_BTS();
    initialize_tab_testconf();
    ss=new sulley_setting(ui);
    initialize_tab_testexec();
    initialize_tab_testlog();


    show_tabn(0);
    //fthread.addTask("get bts info");
    fthread.addTask("get phone info");
    fthread.addTask("get bts info");
    fthread.start();
}

MainWindow::~MainWindow()
{
    delete ui;
    delete ss;
    delete imsi;
    fthread.force_abort();
    delete &fthread;
    pySock.close();
    btsSock.close();
    phoSock.close();
    kill(sulleyPid,SIGKILL);
}
void MainWindow::msgbox(QString msg){
    QMessageBox a(this);
    a.setText(msg);
    a.exec();
}

void MainWindow::showAbout()
{
    aboutD *w=new aboutD(this);
    w->exec();

}

void MainWindow::on_action_A_triggered()
{
    showAbout();
}

//???tab n
void MainWindow::show_tabn(int n)
{
    ui->tabWidget->setCurrentIndex(n);
}




//========================================phone fucs================================================================
void MainWindow::add_phoneTab(QString head,QString value)
{
    QTextEdit* a=new QTextEdit;
    a->append(value);
    QString thead;
    if(head=="VER"){
        thead="Linux版本";
    }
    if(head=="SYS"){
        thead="系统信息";
    }
    if(head=="TEL"){
        thead="基带信息";
    }
    if(head=="CPU"){
        thead="处理器信息";
    }
    if(head=="MEM"){
        thead="内存信息";
    }
    if(head=="DIS"){
        thead="闪存信息";
    }

    ui->tabWidget_2->addTab(a,thead);

}
void MainWindow::phonecheck_change(bool ischecked){
    if(ischecked){
        if(!PHONECONNECT){
            QMessageBox::warning(this,"",tr("Phone not connect,set failed!"));
            ui->phoneConnect->setChecked(false);
            PHONECHECK=false;
            return;
        }
        if(PHONECONNECT){
            PHONECHECK=true;
            return;
        }
    }
    else{
        PHONECHECK=false;
    }
}

void MainWindow::initialize_tab_phone()
{
    connect(ui->pushButton_3,SIGNAL(clicked()),this,SLOT(refresh()));
    connect(ui->phoneConnect,SIGNAL(toggled(bool)),this,SLOT(phonecheck_change(bool)));
    connect(&fthread,SIGNAL(addTab(QString,QString)),this,SLOT(add_phoneTab(QString,QString)));
    ui->tabWidget_2->clear();
    PHONECONNECT=false;
    PHONECHECK=false;

}
// button ???????????
void MainWindow::refresh()
{
    ui->tabWidget_2->clear();
    fthread.addTask("get phone info");
}


//===========================================bts funcs====================================================================
void MainWindow::initialize_tab_BTS()
{
    connect(&fthread,SIGNAL(sendBTSinfo()),this,SLOT(getBTSinfo()));
    connect(ui->pushButton_BTSOK,SIGNAL(clicked()),this,SLOT(onBTSset()));
    connect(ui->pushButton,SIGNAL(clicked()),this,SLOT(onBTSquery()));
}
void MainWindow::onBTSquery(){
    fthread.addTask("get bts info");
}
void MainWindow::onBTSset(){
    setBTSinfo(btsPara);
    fthread.addTask("set bts info");
}

void MainWindow::getBTSinfo(){
    //base info
    BTSPara &para=btsPara;
    ui->lineEdit_name->setText(QString::fromAscii(para.mBasePara.mName));
    ui->lineEdit_CI->setText(QString::number(para.mBasePara.mCI));
    ui->lineEdit_LAC->setText(QString::number(para.mBasePara.mLAI.mLAC));
    QString mcc,mnc;
    int i;
    for(i=0;i<3;i++){
        mcc.append(QString(para.mBasePara.mLAI.mMCC[i]+'0'));
    }
    for(i=0;i<2;i++){
        mnc.append(QString(para.mBasePara.mLAI.mMNC[i]+'0'));
    }
    ui->lineEdit_MNC->setText(mnc);
    ui->lineEdit_MCC->setText(mcc);

    ui->lineEdit_brand->setText(QString::number(para.mBasePara.mBTSARFCN.mBand));
    ui->lineEdit_ARFCN->setText(QString::number(para.mBasePara.mBTSARFCN.arfcn));
    unsigned uplink=uplinkFreqKHz((GSMBand)para.mBasePara.mBTSARFCN.mBand,para.mBasePara.mBTSARFCN.arfcn);
    unsigned downlink=downlinkFreqKHz((GSMBand)para.mBasePara.mBTSARFCN.mBand,para.mBasePara.mBTSARFCN.arfcn);

    ui->lineEdit_downward->setText(QString::number(uplink));
    ui->lineEdit_upward->setText(QString::number(downlink));
    //reselect info
    ui->lineEdit_MS_TXPWR_MAX_CCH->setText(QString::number(para.mReselectPara.mMS_TXPWR_MAX_CCH));
    ui->lineEdit_RXLEV_ACCESS_MIN->setText(QString::number(para.mReselectPara.mRXLEV_ACCESS_MIN));
    ui->lineEdit_CELL_SELECTION->setText(QString::number(para.mReselectPara.mCELL_RESELECT_HYSTERESIS));
    //ctrl info
    ui->lineEdit_ATT->setText(QString::number(para.mCtrlPara.mATT));
    ui->lineEdit_CCCH_CONF->setText(QString::number(para.mCtrlPara.mCCCH_CONF));
    ui->lineEdit_BS_AG_BLKS_RES->setText(QString::number(para.mCtrlPara.mBS_AG_BLKS_RES));
    ui->lineEdit_BS_PA_MFTMS->setText(QString::number(para.mCtrlPara.mBS_PA_MFRMS));
    ui->lineEdit_T3212->setText(QString::number(para.mCtrlPara.mT3212));
    //neighbour
    ui->lineEdit_NeighbourCellDes->setText(QString::fromAscii(para.mCtrlPara.mNeighbour));
    ui->lineEdit_MaxRetrans->setText(QString::number(para.mCtrlPara.mMaxRetrans));
    ui->lineEdit_TX_integer->setText(QString::number(para.mCtrlPara.mTxInteger));
    ui->lineEdit_CELL_BAR_ACCESS->setText(QString::number(para.mCtrlPara.mCellBarAccess));
    ui->lineEdit_AC->setText(QString::number(para.mCtrlPara.mAC));
    ui->lineEdit_NCC_Permitted->setText(QString::number(para.mCtrlPara.mNCCsPermitted));
}
void MainWindow::setBTSinfo(BTSPara& para){
    //base
    strcpy(para.mBasePara.mName,ui->lineEdit_name->text().toAscii().data());
    para.mBasePara.BTSID=ui->lineEdit_CI->text().toInt();
    para.mBasePara.mLAI.mLAC=ui->lineEdit_LAC->text().toInt();
    para.mBasePara.mLAI.mMCC[0]=ui->lineEdit_MCC->text().toAscii().at(0)-'0';
    para.mBasePara.mLAI.mMCC[1]=ui->lineEdit_MCC->text().toAscii().at(1)-'0';
    para.mBasePara.mLAI.mMCC[2]=ui->lineEdit_MCC->text().toAscii().at(2)-'0';
    para.mBasePara.mLAI.mMNC[0]=ui->lineEdit_MNC->text().toAscii().at(0)-'0';
    para.mBasePara.mLAI.mMNC[1]=ui->lineEdit_MNC->text().toAscii().at(1)-'0';


    para.mBasePara.mBTSARFCN.mBand=ui->lineEdit_brand->text().toInt();
    para.mBasePara.mBTSARFCN.arfcn=ui->lineEdit_ARFCN->text().toInt();
    //para.mBasePara.mBTSARFCN.DownlinkFreq=ui->lineEdit_downward->text().toInt();
    //para.mBasePara.mBTSARFCN.UplinkFreq=ui->lineEdit_upward->text().toInt();
    //reselect info
    para.mReselectPara.mMS_TXPWR_MAX_CCH=ui->lineEdit_MS_TXPWR_MAX_CCH->text().toInt();
    para.mReselectPara.mRXLEV_ACCESS_MIN=ui->lineEdit_RXLEV_ACCESS_MIN->text().toInt();
    para.mReselectPara.mCELL_RESELECT_HYSTERESIS=ui->lineEdit_CELL_SELECTION->text().toInt();
    //ctrl info
    para.mCtrlPara.mATT=ui->lineEdit_ATT->text().toInt();
    para.mCtrlPara.mCCCH_CONF=ui->lineEdit_CCCH_CONF->text().toInt();
    para.mCtrlPara.mBS_AG_BLKS_RES=ui->lineEdit_BS_AG_BLKS_RES->text().toInt();
    para.mCtrlPara.mBS_PA_MFRMS=ui->lineEdit_BS_PA_MFTMS->text().toInt();
    para.mCtrlPara.mT3212=ui->lineEdit_T3212->text().toInt();
    strcpy(para.mCtrlPara.mNeighbour,ui->lineEdit_NeighbourCellDes->text().toAscii().data());

    para.mCtrlPara.mMaxRetrans=ui->lineEdit_MaxRetrans->text().toInt();
    para.mCtrlPara.mTxInteger=ui->lineEdit_TX_integer->text().toInt();
    para.mCtrlPara.mCellBarAccess=ui->lineEdit_CELL_BAR_ACCESS->text().toInt();
    para.mCtrlPara.mAC=ui->lineEdit_AC->text().toInt();
    para.mCtrlPara.mNCCsPermitted=ui->lineEdit_NCC_Permitted->text().toInt();
    //to do send
}

//===============================================sulley fucs=========================================================
void MainWindow::select_quick()
{
    ui->bitfield_minEdge->setCurrentIndex(0);
    ui->bitfield_maxRange->setValue(1);
    ui->string_maxlen->setCurrentIndex(0);
    ui->random_maxmuns->setText("50");
    ui->bitfield_testall->setChecked(false);
    ui->string_fuzzLength->setChecked(false);
}
void MainWindow::select_normal()
{
    ui->bitfield_minEdge->setCurrentIndex(1);
    ui->bitfield_maxRange->setValue(5);
    ui->string_maxlen->setCurrentIndex(1);
    ui->random_maxmuns->setText("100");
    ui->string_fuzzLength->setChecked(true);
    ui->bitfield_testall->setChecked(false);
}
void MainWindow::select_deep()
{
    ui->bitfield_minEdge->setCurrentIndex(2);
    ui->bitfield_maxRange->setValue(10);
    ui->string_maxlen->setCurrentIndex(2);
    ui->random_maxmuns->setText("200");
    ui->bitfield_testall->setChecked(true);
    ui->string_fuzzLength->setChecked(true);
}

void MainWindow::start_test()
{
    fthread.force_abort();
    if(ui->IMSI->text().size()!=15){
        QMessageBox a;
        a.setText("IMSI长度错误");
        a.exec();
        return;
    }
    addKeyVal2file("IMSI",ui->IMSI->text());
    fthread.addTask(QString("console IMSI %1").arg(ui->IMSI->text()));
    fthread.addTask(ss->get_cmd());
    refresh_execTab();
    show_tabn(3);
    fthread.addTask("fuzzAll");
    if(ui->button_stop->text()=="start"){
        ui->button_stop->setText("stop");
        ui->button_stop->setIcon(QIcon(":/img/stop.png"));
    }
}

void MainWindow::initialize_tab_testconf()
{
    //settings
    QStringList tem=getValueFromfile("bitfield_minEdge");
    while(!tem.isEmpty()){
        ui->bitfield_minEdge->addItem(tem.takeFirst());
    }
    tem=getValueFromfile("bitfield_maxRange");
    while(!tem.isEmpty()){
        ui->bitfield_maxRange->setMaximum(tem.takeFirst().toInt());
    }
    tem=getValueFromfile("string_maxlen");
    while(!tem.isEmpty()){
        ui->string_maxlen->addItem(tem.takeFirst());
    }
    tem=getValueFromfile("random_minreps");
    if(!tem.isEmpty()){
        ui->random_minreps->setText(tem.takeFirst());
    }
    tem=getValueFromfile("random_maxreps");
    if(!tem.isEmpty()){
        ui->random_maxreps->setText(tem.takeFirst());
    }
    tem=getValueFromfile("random_maxmuns");
    if(!tem.isEmpty()){
        ui->random_maxmuns->setText(tem.takeFirst());
    }
    tem=getValueFromfile("IMSI");
    if(!tem.isEmpty()){
        imsi=new QCompleter(tem);
        ui->IMSI->setCompleter(imsi);
    }

    select_quick();
    //============================================================
    connect(ui->radioButton_quick,SIGNAL(clicked()),this,SLOT(select_quick()));
    connect(ui->radioButton_deep,SIGNAL(clicked()),this,SLOT(select_deep()));
    connect(ui->radioButton_norm,SIGNAL(clicked()),this,SLOT(select_normal()));

    QTreeView* qtree=ui->treeView2;

    // model\view objects
    sulleyMsg=new QStandardItemModel;
    sulleySelectedMsg=new QStandardItemModel;
    ui->listView->setModel(sulleySelectedMsg);

    QStandardItem* root=sulleyMsg->invisibleRootItem();
    QStandardItem *rr=new QStandardItem(QString("RR层消息"));
    QStandardItem *mm=new QStandardItem(QString("MM层消息"));
    QStandardItem *cc=new QStandardItem(QString("CC层消息"));
    QStandardItem *sms=new QStandardItem(QString("SMS"));
    root->appendRow(rr);
    root->appendRow(mm);
    root->appendRow(cc);
    root->appendRow(sms);


    QList<QStandardItem* > rrLayer;
    QList<QStandardItem* > mmLayer;
    QList<QStandardItem* > ccLayer;
    QList<QStandardItem* >* tem_pointer=&rrLayer;


    QFile file("./config/messages.config");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;
    QTextStream in(&file);
    QString line = in.readLine();
    while (!line.isNull()) {
        if(line.compare("RR")==0)
        {
           tem_pointer=&rrLayer;
        }else if(line.compare("MM")==0)
        {
            tem_pointer=&mmLayer;
        }else if(line.compare("CC")==0)
        {
            tem_pointer=&ccLayer;
        }
        else if(!line.isEmpty())
        {
            QStandardItem* tem=new QStandardItem(line);
            tem_pointer->append(tem);
        }
        line=in.readLine();
    }
    file.close();
    rr->appendRows(rrLayer);
    mm->appendRows(mmLayer);
    cc->appendRows(ccLayer);
    sulleyMsg->setHeaderData(0,Qt::Horizontal,"GSM messages");
    qtree->setModel(sulleyMsg);
    qtree->expandAll();
    connect(ui->pushButton_5,SIGNAL(clicked()),this,SLOT(sulley_add()));
    connect(ui->pushButton_6,SIGNAL(clicked()),this,SLOT(sulley_remove()));
    connect(ui->button_getimsi,SIGNAL(clicked()),this,SLOT(MainGetIMSI()));
    connect(&fthread,SIGNAL(imsigetted(QString)),ui->IMSI,SLOT(setText(QString)));
    connect(ui->button_startTest,SIGNAL(clicked()),this,SLOT(start_test()));
}
void MainWindow::MainGetIMSI(){
    fthread.addTask("get imsi");
}

void MainWindow::sulley_add(){
    QTreeView*qt=ui->treeView2;
    QModelIndex sel=qt->selectionModel()->currentIndex();
    if(sel.isValid()){
        QStandardItem* t_item=sulleyMsg->itemFromIndex(sel);
        if(t_item->hasChildren()){
            int i;
            for(i=0;i<t_item->rowCount();i++){
                QStandardItem* t_child=t_item->child(i);
                if(sulleySelectedMsg->findItems(t_child->data(Qt::DisplayRole).toString()).isEmpty()){
                    sulleySelectedMsg->appendRow(new QStandardItem(t_child->data(Qt::DisplayRole).toString()));
                }
            }
        }
        else{
            QString t=sulleyMsg->data(sel).toString();
            QList<QStandardItem*> t_list=sulleySelectedMsg->findItems(t);
            if(t_list.isEmpty()){
                sulleySelectedMsg->appendRow(new QStandardItem(t));
            }
        }
    }
}

void MainWindow::sulley_remove(){
    QItemSelectionModel *sel_model=ui->listView->selectionModel();
    int row=sel_model->currentIndex().row();
    sulleySelectedMsg->removeRow(row);

}


//==============================================exec fucs=================================================================
void MainWindow::initialize_tab_testexec()
{
      testExecModel =  new QStandardItemModel(5, 3);
      testExecModel->setHeaderData(1, Qt::Horizontal, "测试消息");
      testExecModel->setHeaderData(2, Qt::Horizontal, "状态");
      testExecModel->setHeaderData(0, Qt::Horizontal, " ");



      QStandardItem *item1 = new QStandardItem();
      item1->setIcon(QIcon(":/img/start.png"));
      QStandardItem *item2 = new QStandardItem();
      item2->setIcon(QIcon(":/img/start.png"));
      QStandardItem *item3 = new QStandardItem();
      item3->setIcon(QIcon(":/img/start.png"));
      QStandardItem *item4 = new QStandardItem();
      item4->setIcon(QIcon(":/img/start.png"));
      QStandardItem *item5 = new QStandardItem();
      item5->setIcon(QIcon(":/img/stop.png"));

      QStandardItem *item6 = new QStandardItem("aaaaaa");
      QStandardItem *item7 = new QStandardItem("bbbbbb");
      QStandardItem *item8 = new QStandardItem("cccccc");
      QStandardItem *item9 = new QStandardItem("dddddd");
      QStandardItem *item10 = new QStandardItem("eeeeee");

      testExecModel->setRowCount(3);
      testExecModel->setItem(0, 0, item1);
      testExecModel->setItem(1, 0, item2);
      testExecModel->setItem(2, 0, item3);
      testExecModel->setItem(3, 0, item4);
      testExecModel->setItem(4, 0, item5);

      testExecModel->setItem(0, 1, item6);
      testExecModel->setItem(1, 1, item7);
      testExecModel->setItem(2, 1, item8);
      testExecModel->setItem(3, 1, item9);
      testExecModel->setItem(4, 1, item10);

      QStandardItem *item11 = new QStandardItem();
      item11->setIcon(QIcon(":/img/correct.png"));

      QStandardItem *item12 = new QStandardItem();
      item12->setIcon(QIcon(":/img/error.png"));
      testExecModel->setItem(0, 2, item11);

      ui->treeView_testexec->setModel(testExecModel);
      ui->treeView_testexec->setColumnWidth(1,410);


      connect(&fthread,SIGNAL(setProcess(int)),ui->progressBar,SLOT(setValue(int)));
      connect(&fthread,SIGNAL(setMessage(QString)),ui->text_status,SLOT(append(QString)));
      connect(&fthread,SIGNAL(FuzzResult(QString,bool,bool)),this,SLOT(set_result(QString,bool,bool)));
      connect(ui->button_stop,SIGNAL(clicked()),this,SLOT(exec_stop()));

}
void MainWindow::exec_stop(){
    if(ui->button_stop->text()=="stop"){
        ui->button_stop->setIcon(QIcon(":/img/start.png"));
        ui->button_stop->setText(QString::fromAscii("start"));
        fthread.force_abort();
    }else{
        ui->button_stop->setIcon(QIcon(":/img/stop.png"));
        ui->button_stop->setText(QString::fromAscii("stop"));
        start_test();
    }
}

void MainWindow::refresh_execTab(){
    GSMmessageTable.clear();
    testExecModel->removeRows(0,testExecModel->rowCount());
    int i;
    for(i=0;i<sulleySelectedMsg->rowCount();i++){
        QString t=sulleySelectedMsg->item(i)->data(Qt::DisplayRole).toString();
        GSMmessageTable.push_back(t);
        QList<QStandardItem*> tem_list;
        tem_list.append(new QStandardItem(QIcon(":/img/stop.png"),""));
        tem_list.append(new QStandardItem(t));
        tem_list.append(new QStandardItem(QIcon(":/img/correct.png"),""));
        testExecModel->appendRow(tem_list);
    }
}

void MainWindow::set_result(QString msg,bool integer,bool state){
    if(!(testExecModel->findItems(msg,Qt::MatchExactly,1).isEmpty())){
        QStandardItem* t_item=testExecModel->findItems(msg,Qt::MatchExactly,1).at(0);
        int item_row=t_item->row();
        if(state) testExecModel->item(item_row,0)->setIcon(QIcon(":/img/start.png"));
        else testExecModel->item(item_row,0)->setIcon(QIcon(":/img/stop.png"));
        if(integer) testExecModel->item(item_row,2)->setIcon(QIcon(":/img/correct.png"));
        else testExecModel->item(item_row,2)->setIcon(QIcon(":/img/error.png"));

    }
    else LOG(ERROR)<<"no item found";
}







//=====================================others =============================================================================
void MainWindow::do_file_Open()
{
    QString fileName;
    QModelIndex a=ui->history_tree->currentIndex();
    if(a.isValid()){
        fileName=filemodel->fileName(a);
    }else{
        fileName = QFileDialog::getOpenFileName(this);
    }
     if(!fileName.isEmpty())//
     {
      do_file_Load(fileName);
     }
      ui->text_report->setVisible(true);
}

bool MainWindow::do_file_Load(const QString &fileName)
{
    QFile file(fileName);
      if(!file.open(QFile::ReadOnly | QFile::Text))
     {
       QMessageBox::warning(this,tr(""),tr("打开文件错误 %1:\n%2.").arg(fileName).arg(file.errorString()));
       return false; //
      }
      QTextStream in(&file);
      ui->text_report->setText(in.readAll()); //
      curFile = QFileInfo(fileName).canonicalFilePath();
      setWindowTitle(curFile);

      return true;
}
void MainWindow::do_file_save(){
    QString htm=ui->text_report->toHtml();
    QString fileName=QFileDialog::getSaveFileName(this);
    QFile file(fileName);
    if(file.open(QIODevice::WriteOnly | QIODevice::Text)){
        file.write(htm.toAscii());
        file.close();
        return;
    }
    QMessageBox::warning(this,"","文件保存失败!");
}

void MainWindow::on_viewrep_clicked()
{
    do_file_Open();
}

void MainWindow::rm()//
{
    QModelIndex index = ui->history_tree->currentIndex();
    if (!index.isValid())
    {
           return;
    }
     bool ok;
      if (filemodel->fileInfo(index).isDir())
      {
            ok = filemodel->rmdir(index);
      }
      else
      {
            ok = filemodel->remove(index);
      }
      if (!ok)
      {
            QMessageBox::information(this, tr(""),
                            tr("文件删除失败%1").arg(filemodel->fileName(index)));
      }
}


void MainWindow::initialize_tab_testlog()
{
       filemodel = new QFileSystemModel;
       QString reports=QDir::currentPath().append("/reports");
       filemodel->setRootPath(reports);
       ui->history_tree->setModel(filemodel);
       ui->history_tree->setRootIndex(filemodel->index(reports));

       ui->history_tree->header()->setStretchLastSection(true);
       ui->history_tree->header()->setSortIndicator(0, Qt::AscendingOrder);
       ui->history_tree->header()->setSortIndicatorShown(true);
       ui->history_tree->header()->setClickable(true);

       QModelIndex index = filemodel->index(QDir::currentPath());
       ui->history_tree->expand(index);
       ui->history_tree->scrollTo(index);
       ui->history_tree->resizeColumnToContents(0);

       connect(ui->del_log, SIGNAL(clicked()), this, SLOT(rm()));
       connect(ui->view_log,SIGNAL(clicked()),this,SLOT(on_viewrep_clicked()));
       connect(ui->pushButton_2,SIGNAL(clicked()),this,SLOT(do_file_save()));
       connect(&fthread,SIGNAL(sendReport(QString)),ui->text_report,SLOT(setHtml(QString)));
}

QString sulley_setting::get_cmd(){
    getUIval(ui);
    QString cmd("console sulley bit_testall=%1&bit_minEdge=%2&bit_maxRange=%3&Lfuzz=%4&str_value=%5&str_maxlen=%6&rand_minreps=%7&rand_maxreps=%8&rand_maxmuns=%9");
    if(bit_testall) cmd=cmd.arg("1");
    else cmd=cmd.arg("0");
    cmd=cmd.arg(bit_minEdge).arg(bit_maxRange);
    if(Lfuzz) cmd=cmd.arg("1");
    else cmd=cmd.arg("0");
    cmd=cmd.arg(string_value).arg(string_maxlen).arg(random_minreps).arg(random_maxreps).arg(random_maxmuns);
    ui->text_status->append(QString("模糊器回复\n%1").arg(cmd));
    return cmd;
}

void sulley_setting::getUIval(Ui::MainWindow *ui){
    //bit_field
    bit_maxRange=ui->bitfield_maxRange->text();
    bit_minEdge=ui->bitfield_minEdge->currentText();
    bit_testall=ui->bitfield_testall->isChecked();
    //Lfuzz
    Lfuzz=ui->string_fuzzLength->isChecked();
    //str
    string_maxlen=ui->string_maxlen->currentText();
    string_value=ui->string_value->text();
    //random
    random_minreps=ui->random_minreps->text();
    random_maxreps=ui->random_maxreps->text();
    random_maxmuns=ui->random_maxmuns->text();

}
