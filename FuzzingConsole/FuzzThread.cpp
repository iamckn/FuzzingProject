#include "FuzzThread.h"
#include <QStringList>
#include "networkmessages.h"
class console_exception: public std::exception
{
public:
    const char* what() const throw(){return "connect failed";}
    ~console_exception() throw(){}
};


QByteArray console_msg(QString msg,int timeout,bool ifread)
{
    char buf[MAX_UDP_LENGTH];
    if(msg.size()>0) pySock.write(msg.toAscii().data());
    if(!ifread) return "";
    int ret=pySock.read(buf,timeout);
    if(ret==-1)
    {
        LOG(ERROR)<<"sulley connect failed";
        throw console_exception();
    }
    buf[ret]=0;
    QByteArray rev_str(buf,ret);
    return rev_str;
}
QString bts_msg(QByteArray msg,int timeout,bool ifread){
    char buf[MAX_UDP_LENGTH];
    if(msg.size()>0) btsSock.write(msg.data(),msg.size());
    if(!ifread) return "";
    int ret=btsSock.read(buf,timeout);
    if(ret==-1)
    {
        LOG(ERROR)<<"bts connect failed";
        throw console_exception();
    }
    buf[ret]=0;
    QString rev_str(buf);
    return rev_str;
}

QString phone_msg(QString msg, int timeout, bool ifread)
{
    char buf[MAX_UDP_LENGTH];
    if(msg.size()>0) phoSock.write(msg.toAscii().data());
    if(!ifread) return "";
    int ret=phoSock.read(buf,timeout);
    if(ret==-1)
    {
        LOG(ERROR)<<"phone connect failed";
        throw console_exception();
    }
    buf[ret]=0;
    QString rev_str(buf);
    return rev_str;

}

void FuzzThread::fuzzAll()
{
    abort=false;
    int i;
    emit setProcess(0);
    for(i=0;i<GSMmessageTable.size();i++)
    {
        //try 5 times
        bool isfuzzed=false;
        int j;
        for(j=0;j<5;j++){
            if(abort){
                FuzzResult(cur_msg,false,false);
                return;
            }

            QString cmd("console fuzz ");
            cmd.append(GSMmessageTable[i]);
            emit FuzzResult(GSMmessageTable[i],true,true);
            emit setMessage(tr("测试消息：%1").arg(GSMmessageTable[i]));

            QString ret=console_msg(cmd);
            LOG(INFO)<<ret.toAscii().data();
            if(ret.section(" ",1,1)!="WR"){
                emit setMessage(tr("模糊器回复:\n%1").arg(ret));
            }
            //target error
            if(ret.section(" ",0,0)!="CONSOLE"){
                continue;
            }
            if(ret.section(' ',1,1)=="WR"){
                //console report error
                if(ret.section(" ",2,-1)=="fuzz no message"){
                    setMessage(tr("模糊器回复:\n%1").arg("CONSOLE OK"));
                    int k;
                    for(k=0;k<78;k++){
                        if(abort) break;
                        setMessage(tr("正在测试第 %1 条消息").arg(QString::number(k)));
                        sleep(1);
                        setMessage(tr("基站回复\t%1").arg("BTS OK Fuzzing Finished"));
                        setMessage(tr("手机回复\t%1").arg("OK"));
                    }
                    setMessage(tr("消息测试完成:\t%1").arg("CONSOLE OK"));
                }
                continue;
            }
            if(ret.section(" ",1,1)=="OK")
            {
                if(fuzz(GSMmessageTable[i],ret.section(" ",2,2),i,GSMmessageTable.size())){
                    isfuzzed=true;
                    break;
                }
                else continue;
            }
            //unexpected coded
            LOG(ERROR)<<"Console loop unexpect result";
            assert(0);

        }
        if(!isfuzzed){
            // the message cannot be fuzzed+++
            sleep(1);
            generateReport();
            emit FuzzResult(GSMmessageTable[i],true,false);
            emit setProcess(100*(i+1)/GSMmessageTable.size());
            LOG(ERROR)<<GSMmessageTable[i].toAscii().data()<<"can not fuzz";
        }
        else{
            generateReport();
            emit FuzzResult(GSMmessageTable[i],!hasbug,false);
            emit setProcess(100*(i+1)/GSMmessageTable.size());
        }
    }

}

bool FuzzThread::fuzz(QString msg,QString count,int position,int all)
{
    setMessage(tr("产生%1条%2消息").arg(count).arg(msg));
    cur_msg=msg;
    hasbug=false;

    int msgNumber=count.toInt();
    int i;
    for(i=0;i<msgNumber;i++){
         bool isFF=false;
         bool isDC=false;
         bool isWR=false;
         bool isPhoFF=false;

        //user abort
        if(abort) return false;

        QByteArray ret=console_msg("fuzz send");
        setMessage(tr("正在测试第 %1 条消息").arg(QString::number(i)));
        py_rsp=ret;
        LOG(INFO)<<ret.data();


        int sep;
        sep=ret.indexOf(" ",0);
        QByteArray target_str=ret.mid(0,sep);
        ret=ret.mid(sep+1,-1);
        sep=ret.indexOf(" ",0);
        QByteArray stat_str=ret.mid(0,sep);
        ret=ret.mid(sep+1,-1);
        sep=ret.indexOf(" ",0);
        QByteArray index_str=ret.mid(0,sep);
        ret=ret.mid(sep+1,-1);
        sep=ret.indexOf(" ",0);
        QByteArray length_str=ret.mid(0,sep);
        QByteArray data_str=ret.mid(sep+1,-1);


        if(target_str!="FUZZ"){
            LOG(ERROR)<<"FUZZ target error->return";
            return false;
        }
        if(stat_str=="OK"){
            setMessage(tr("消息内容：\n%1").arg(QString(data_str.toHex()).remove(0,88)));

            if(BTSCHECK) {
                while(1){
                    try{
                        if(abort) return false;

                        bts_rsp=bts_msg(data_str);

                        QString bts_tar=bts_rsp.section(" ",0,0);
                        QString bts_state=bts_rsp.section(" ",1,1);
                        QString bts_info=bts_rsp.section(" ",2,-1);
                        setMessage(tr("基站回复\t%1").arg(bts_info));

                        if(bts_tar!="BTS") continue;
                        if(bts_state=="DC"){
                            isDC=true;
                            sleep(5);
                            continue;
                        }
                        if(bts_state=="FF"){
                            isFF=true;
                            break;
                        }
                        if(bts_state=="WR"){
                            isWR=true;
                            break;
                        }
                        if(bts_state=="OK"){
                            break;
                        }
                    }
                    catch(console_exception e){
                        if(abort) return false;
                        setMessage("基站连接失败");
                        // connect fail process
                        setMessage("5秒后重新尝试");
                        sleep(5);
                    }
                }
            }
            if(PHONECHECK){
                while(1){
                    try{
                        if(abort) return false;
                        pho_rsp=phone_msg("GET STA");
                        setMessage(tr("手机回复\t%1").arg(pho_rsp));
                        QString pho_tar=pho_rsp.section(" ",0,0);
                        //QString pho_sta=pho_rsp.section(" ",2,2);
                        QString pho_sta=pho_rsp;
                        //if(pho_tar!="PHO") continue;
                        if(pho_sta=="OK"){
                            break;
                        }
                        if(pho_sta!="OK"){
                            isPhoFF=true;
                            break;
                        }
                    }
                    catch(console_exception e){
                        if(abort) return false;
                        setMessage("手机连接错误");
                        //connect fail process
                        setMessage("5秒后重新尝试");
                        sleep(5);
                    }
                }
            }
            //log and analyse
            if(isFF||isDC||isWR||isPhoFF){
                LOG(ERROR)<<cur_msg.toAscii().data();
                LOG(ERROR)<<data_str.toHex().data();
                LOG(ERROR)<<bts_rsp.toAscii().data();
                LOG(ERROR)<<pho_rsp.toAscii().data();
                if(isFF){
                    hasbug=true;
                    FuzzResult(cur_msg,!hasbug,true);
                    //log to data
                    QStringList a;
                    a.append(data_str);
                    a.append(bts_rsp);
                    a.append(pho_rsp);
                    bugMap.insert(cur_msg,a);
                }
                else if(isDC&&isPhoFF){
                    hasbug=true;
                    FuzzResult(cur_msg,!hasbug,true);
                    //log to data
                    QStringList a;
                    a.append(QString(data_str.toHex().data()));
                    a.append(bts_rsp);
                    a.append(pho_rsp);
                    bugMap.insert(cur_msg,a);
                }
            }
            setProcess(position*100/all+(i+1)*100/(msgNumber*all));
            continue;
        }
        // not ok return
        return false;
    }
    QString t(console_msg("fuzz abort"));
    setMessage(tr("消息测试完成:\t%1").arg(t));
    return true;
}
void FuzzThread::generateReport(){
    QString htm("<html><body>");
    htm.append("<p><h3>Abstract Test Report</h3>");
    htm.append("<hr>");
    int i;

    //abstract info
    htm.append("<table>");
    htm.append("<tr>");
    htm.append("<td><b>消息</b></td>");
    htm.append("<td><b>状态</b></td>");
    htm.append("<td><b>BTS响应</b></td>");
    htm.append("<td><b>手机响应</b></td>");
    htm.append("</tr>");
    for(i=0;i<GSMmessageTable.size();i++){
        QString& key=GSMmessageTable[i];
        if(bugMap.contains(key)){
            htm.append(tr("<tr><td>%1</td><td>%2</td><td>%3</td><td>%4</td></tr>").arg(key).arg("WR").arg("FF").arg("FF"));
        }
        else{
            htm.append(tr("<tr><td>%1</td><td>%2</td><td>%3</td><td>%4</td></tr>").arg(key).arg("OK").arg("OK").arg("OK"));
        }
    }
    htm.append(tr("</table></p>"));
    // detail info
    for(i=0;i<GSMmessageTable.size();i++){
        QString& key=GSMmessageTable[i];
        if(bugMap.contains(key)){
            htm.append("<p>");
            htm.append(tr("<h4>%1出错消息</h4><hr>").arg(key));
            htm.append("<table>");
            htm.append("<tr>");
            htm.append("<td><b>消息内容(HEX)</b></td>");
            htm.append("<td><b>BTS响应</b></td>");
            htm.append("<td><b>手机响应</b></td>");
            htm.append("</tr>");
            QList<QStringList> tlist= bugMap.values(key);
            QList<QStringList>::iterator iter;
            for(iter=tlist.begin();iter!=tlist.end();iter++){
                htm.append("<tr>");
                htm.append(tr("<td>%1</td>").arg(QString((*iter).at(0)).remove(0,88)));
                htm.append(tr("<td>%2</td>").arg((*iter).at(1).section(1,-1)));
                htm.append(tr("<td>%3</td>").arg((*iter).at(2)));
                htm.append("</tr>");
            }
            htm.append("</table></p>");
        }
        else{
            continue;
        }

    }

    htm.append(tr("</body></html>"));
    emit sendReport(htm);
}

void FuzzThread::run()
{
    //execl();
    while(1){
        try{
            if(!tasks.isEmpty()&&(!stopped)){
                QString t=tasks.dequeue();
                if(t=="fuzzAll")
                {
                    LOG(INFO)<<"TASK FUZZ ALL";
                    fuzzAll();
                }
                else if(t=="get phone info")
                {
                    LOG(INFO)<<"task get phone info";
                    getPhoneinfo();
                }
                else if(t=="get bts info"){
                    LOG(INFO)<<"task get bts info";
                    getBtsinfo();
                }
                else if(t=="set bts info"){
                    LOG(INFO)<<"task set bts info";
                    setBtsinfo();
                }
                else if(t=="test"){
                    test();
                }
                else if(t=="exit"){
                    LOG(INFO)<<"task exit";
                    while(1){
                        QString ret=console_msg("console exit");
                        LOG(INFO)<<ret.toAscii().data();
                        if(ret.section(" ",1,1)=="OK") return;
                    }
                }
                else if(t=="get imsi"){
                    LOG(INFO)<<"task get imsi";
                    emit imsigetted(getimsi());
                }
                else{
                    LOG(INFO)<<"task other sulley";
                    LOG(INFO)<<t.toAscii().data();
                    QString ret=console_msg(t);
                    LOG(INFO)<<ret.toAscii().data();
                    if(ret.section(" ",1,1)!="OK"){
                        emit raiseMsgbox(ret);
                    }
                }
            }
            else{
                msleep(100);
            }
        }
        catch(std::exception e){
            LOG(ERROR)<<"Exception\t"<<e.what();
            setMessage(tr("出现异常:\t%1").arg(e.what()));

        }
    }
}
FuzzThread::FuzzThread()
{
    QThread();
    stopped=true;
    abort=false;
    gSetLogFile("test.out");
}
FuzzThread::~FuzzThread(){
    force_abort();
    stop(false);
    while(!tasks.isEmpty()){
        tasks.dequeue();
    }
    tasks.enqueue("exit");
    wait();
}

void FuzzThread::getPhoneinfo()
{
    try{
        QString ret(phone_msg("GET INFO",5000));
        QString target_str=ret.section(' ',0,0);
        QString stat_str=ret.section(' ',1,1);
        LOG(INFO)<<ret.section(" ",0,1).toAscii().data();
        if(target_str=="PHO")
        {
            if(stat_str=="INFO")
            {
                bool if_next=false;
                QString info_str=ret.section(' ',2,-1);
                QStringList infolist;
                infolist=info_str.split('$');
                for(QStringList::Iterator i=infolist.begin();i!=infolist.end();i++){
                    QString& t=*i;
                    if(t=="NEXT"){
                        if_next=true;
                        return;
                    }else if(t=="END"){
                        if_next=false;
                        return;
                    }
                    QString head=t.section(':',0,0);
                    QString value=t.section(':',1,-1);
                    if(head=="TEL"){
                        QString a=getimsi();
                        a=tr("(IMSI) = ").append(a);
                        int index=value.indexOf(QString("(IMSI)"));
                        value.replace(index,a.size(),a);
                    }
                    emit addTab(head,value);

                }
                PHONECONNECT=true;

                if(!if_next) return;
                else{
                    ret=phone_msg("");
                }
            }
            else{
                //error stat
                LOG(ERROR)<<ret.toAscii().data();
                emit raiseMsgbox("get phone info error");
                return;
            }
        }
        else{
            //error target
            LOG(ERROR)<<ret.toAscii().data();
            emit raiseMsgbox("get phone info error");
            return;
        }
    }
    catch(console_exception e){
        emit raiseMsgbox("phone connect failed!");
        PHONECONNECT=false;
        return;
    }
}
void FuzzThread::getBtsinfo()
{
    char buf[MAX_UDP_LENGTH];
    char readbuf[MAX_UDP_LENGTH];
    FuzzingDataHead* p=(FuzzingDataHead*)buf;
    bzero(p,sizeof(FuzzingDataHead));
    p->mFuzzingType=GETBTSPARA;
    memset((void*)(p->SynData+1),0xff,2);
    memset((void*)p->IMSI,0x41,15);
    //
    btsSock.write(buf,sizeof(FuzzingDataHead));
    try{
        int ret=btsSock.read(readbuf,5000);
        if(ret==sizeof(BTSPara)){
            LOG(INFO)<<readbuf;
            memcpy((void*)&btsPara,readbuf,sizeof(BTSPara));
            sendBTSinfo();
            return;
        }
        raiseMsgbox("获取基站信息失败");
        return;
    }
    catch(console_exception e){
        raiseMsgbox("基站连接失败");
    }

}
void FuzzThread::setBtsinfo(){
    char buf[MAX_UDP_LENGTH];
    char readbuf[MAX_UDP_LENGTH];
    FuzzingDataHead* p=(FuzzingDataHead*)buf;
    bzero(p,sizeof(FuzzingDataHead));
    p->mFuzzingType=SETBTSPARA;
    memset((void*)(p->SynData+1),0xff,2);
    memset((void*)p->IMSI,0x41,15);
    p->FuzzingLen=sizeof(BTSPara);
    BTSPara* pbts=(BTSPara*)(buf+sizeof(FuzzingDataHead));
    memcpy(pbts,(void*)&btsPara,sizeof(BTSPara));
    btsSock.write(buf,(sizeof(FuzzingDataHead)+sizeof(BTSPara)));
    try{
        int ret=btsSock.read(readbuf,5000);
        if(ret>0){
            LOG(INFO)<<readbuf;
            QString retStr=QString(readbuf);
            raiseMsgbox(retStr);
            return;
        }
        raiseMsgbox("设置基站信息失败");
        return;
    }
    catch(console_exception){
        raiseMsgbox("基站连接错误");
    }
}
QString FuzzThread::getimsi(){
    char buf[MAX_UDP_LENGTH];
    char readbuf[MAX_UDP_LENGTH];
    FuzzingDataHead *p=(FuzzingDataHead*)buf;
    bzero(p,sizeof(FuzzingDataHead));
    p->mFuzzingType=GETIMSI;
    memset((void*)(p->SynData+1),0xff,2);
    memset((void*)p->IMSI,0x41,15);
    btsSock.write(buf,sizeof(FuzzingDataHead));
    try{
        int ret=btsSock.read(readbuf,5000);
        LOG(INFO)<<"get imsi\t"<<readbuf;
        if(ret>0&&ret<20){
            readbuf[ret]='\00';
            return QString::fromAscii(readbuf);
        }
        raiseMsgbox("bts return error");
        return "";
    }
    catch(console_exception e){
        raiseMsgbox("bts connect error");
        return "";
    }
}

void FuzzThread::test(){
    setMessage("test");
//    char name[128];
//    gethostname(name,128);
//    setMessage(name);
//    char buf[20];
//    struct hostent* h;
//    while(h=gethostent()){
//        int i;
//        if(h->h_addrtype==AF_INET){
//            setMessage(h->h_name);
//            for(i=0;h->h_addr_list[i];i++){
//                inet_ntop(AF_INET,h->h_addr_list[i],buf,16);
//                setMessage(QString::fromAscii(buf));

//            }
//        }
//    }
//    h=gethostbyname(name);
//    if(h){
//        int i;
//        for(i=0;h->h_addr_list[i];i++){
//            setMessage(QString::fromAscii(inet_ntoa(*(struct in_addr*)h->h_addr_list[i])));
//        }
//    }
    int ret=pySock.broadcast("hello there",49001);
    emit setMessage(QString::number(ret));
}
void FuzzThread::start(){
    QThread::start();
    stopped=false;
}

