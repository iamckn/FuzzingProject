/*
* Copyright 2008, 2010 Free Software Foundation, Inc.
*
* This software is distributed under the terms of the GNU Affero Public License.
* See the COPYING file in the main directory for details.
*
* This use of this software may be subject to additional restrictions.
* See the LEGAL file in the main directory for details.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/




#include "TRXManager.h"
#include "GSMCommon.h"
#include "GSMTransfer.h"
#include "GSMLogicalChannel.h"
#include <GSML3MMMessages.h>
#include <GSML3RRMessages.h>
#include <GSML3CCMessages.h>
#include "GSMConfig.h"
#include "GSML1FEC.h"
#include <string.h>

#include <Logger.h>


using namespace GSM;
using namespace std;
extern char gFuzzingIMSI[16];


int GetBtsInfo(char * buffer)
{
	BTSPara* btsPara=(BTSPara* )buffer;

	
	//strcpy(btsPara->mBasePara.BTSID,gConfig.getStr("GSM.BTSID"));
	btsPara->mBasePara.BTSID=gConfig.getNum("GSM.BTSID");
	strcpy(btsPara->mBasePara.mName,gConfig.getStr("GSM.ShortName"));
	
	
	const char* wMCC = gConfig.getStr("GSM.MCC");
	const char* wMNC = gConfig.getStr("GSM.MNC");

	btsPara->mBasePara.mLAI.mMCC[0] = wMCC[0]-'0';
	btsPara->mBasePara.mLAI.mMCC[1] = wMCC[1]-'0';
	btsPara->mBasePara.mLAI.mMCC[2] = wMCC[2]-'0';

	btsPara->mBasePara.mLAI.mMNC[0] = wMNC[0]-'0';
	btsPara->mBasePara.mLAI.mMNC[1] = wMNC[1]-'0';
	if (wMNC[2]!='\0') btsPara->mBasePara.mLAI.mMNC[2]=wMNC[2]-'0';
	else btsPara->mBasePara.mLAI.mMNC[2]=0x0f;

	btsPara->mBasePara.mLAI.mLAC=gConfig.getNum("GSM.LAC");

	btsPara->mBasePara.mCI=gConfig.getNum("GSM.CI");
	btsPara->mBasePara.mBTSARFCN.mBand=gConfig.getNum("GSM.Band");
	
	btsPara->mBasePara.mBTSARFCN.arfcn=gConfig.getNum("GSM.ARFCN");

	btsPara->mCtrlPara.mATT=gConfig.getNum("GSM.CCD.ATT");
	btsPara->mCtrlPara.mCCCH_CONF=gConfig.getNum("GSM.CCD.CCCH_CONF");
	btsPara->mCtrlPara.mBS_AG_BLKS_RES=2;
	btsPara->mCtrlPara.mBS_PA_MFRMS=0;
	btsPara->mCtrlPara.mT3212=gConfig.getNum("GSM.T3212");

	strcpy(btsPara->mCtrlPara.mNeighbour,gConfig.getStr("GSM.Neighbors"));

	btsPara->mCtrlPara.mMaxRetrans=gConfig.getNum("GSM.RACH.MaxRetrans");
	btsPara->mCtrlPara.mTxInteger=gConfig.getNum("GSM.RACH.TxInteger");

	btsPara->mCtrlPara.mCellBarAccess=0;
	btsPara->mCtrlPara.mAC=gConfig.getNum("GSM.RACH.AC");
	btsPara->mCtrlPara.mNCCsPermitted=gConfig.getNum("GSM.NCCsPermitted");

	btsPara->mReselectPara.mMS_TXPWR_MAX_CCH=gConfig.getNum("GSM.CS.MS_TXPWR_MAX_CCH");
	btsPara->mReselectPara.mRXLEV_ACCESS_MIN=gConfig.getNum("GSM.CS.RXLEV_ACCESS_MIN");
	btsPara->mReselectPara.mCELL_RESELECT_HYSTERESIS=gConfig.getNum("GSM.CS.CELL_RESELECT_HYSTERESIS");

	int slotCount=0;
	btsPara->mChannelCombination[slotCount++]=5;

	for(int i=0;i<gConfig.getNum("GSM.NumC7s");i++)
	{
		btsPara->mChannelCombination[slotCount++]=7;
	}
	for(int i=0;i<gConfig.getNum("GSM.NumC1s");i++)
	{
		btsPara->mChannelCombination[slotCount++]=1;
	}
	
}


int SetBtsInfo(char * buffer)
{
	BTSPara* btsPara=(BTSPara* )buffer;


	gConfig.set("GSM.BTSID",btsPara->mBasePara.BTSID);
	gConfig.set("GSM.ShortName",btsPara->mBasePara.mName);
	
	char MCC[4]={0};
	char MNC[4]={0};
	MCC[0]=btsPara->mBasePara.mLAI.mMCC[0]+'0';
	MCC[1]=btsPara->mBasePara.mLAI.mMCC[1]+'0';
	MCC[2]=btsPara->mBasePara.mLAI.mMCC[2]+'0';
	MNC[0]=btsPara->mBasePara.mLAI.mMNC[0]+'0';
	MNC[1]=btsPara->mBasePara.mLAI.mMNC[1]+'0';
	if (btsPara->mBasePara.mLAI.mMNC[2]!=0x0f) MNC[2]=btsPara->mBasePara.mLAI.mMNC[2]+'0';
		
	gConfig.set("GSM.MCC", MCC);
	gConfig.set("GSM.MNC", MNC);
	gConfig.set("GSM.LAC",btsPara->mBasePara.mLAI.mLAC);
	gConfig.set("GSM.CI",btsPara->mBasePara.mCI);
	gConfig.set("GSM.ARFCN",btsPara->mBasePara.mBTSARFCN.arfcn);

	gConfig.set("GSM.CCD.ATT",btsPara->mCtrlPara.mATT);
	gConfig.set("GSM.CCD.CCCH_CONF",btsPara->mCtrlPara.mCCCH_CONF);
	/*btsPara->mCtrlPara.mBS_AG_BLKS_RES=2;
	btsPara->mCtrlPara.mBS_PA_MFRMS=0;*/
	gConfig.set("GSM.T3212",btsPara->mCtrlPara.mT3212);
	
	gConfig.set("GSM.Neighbors",btsPara->mCtrlPara.mNeighbour);

	gConfig.set("GSM.RACH.MaxRetrans",btsPara->mCtrlPara.mMaxRetrans);
	gConfig.set("GSM.RACH.TxInteger",btsPara->mCtrlPara.mTxInteger);

	//btsPara->mCtrlPara.mCellBarAccess=0;
	gConfig.set("GSM.RACH.AC",btsPara->mCtrlPara.mAC);
	gConfig.set("GSM.NCCsPermitted",btsPara->mCtrlPara.mNCCsPermitted);

	gConfig.set("GSM.CS.MS_TXPWR_MAX_CCH",btsPara->mReselectPara.mMS_TXPWR_MAX_CCH);
	gConfig.set("GSM.CS.RXLEV_ACCESS_MIN",btsPara->mReselectPara.mRXLEV_ACCESS_MIN);
	gConfig.set("GSM.CS.CELL_RESELECT_HYSTERESIS",btsPara->mReselectPara.mCELL_RESELECT_HYSTERESIS);

	//cerr<<"before dumpfile";
	gConfig.dumpFile("OpenBTS.config");
	gBTS.regenerateBeacon();
	
}





TransceiverManager::TransceiverManager(int numARFCNs,
		const char* wTRXAddress, int wBasePort)
	:mHaveClock(false),
	mClockSocket(wBasePort+100)
{
	// set up the ARFCN managers
	for (int i=0; i<numARFCNs; i++) {
		int thisBasePort = wBasePort + 1 + 2*i;
		mARFCNs.push_back(new ::ARFCNManager(wTRXAddress,thisBasePort,*this));
	}
}



void TransceiverManager::start()
{
	mClockThread.start((void*(*)(void*))ClockLoopAdapter,this);
	for (unsigned i=0; i<mARFCNs.size(); i++) {
		mARFCNs[i]->start();
	}
}





void* ClockLoopAdapter(TransceiverManager *transceiver)
{
	while (1) {
		transceiver->clockHandler();
		pthread_testcancel();
	}
	return NULL;
}



void TransceiverManager::clockHandler()
{
	char buffer[MAX_UDP_LENGTH];
	int msgLen = mClockSocket.read(buffer,3000);

	// Did the transceiver die??
	if (msgLen<0) {
		LOG(ALARM) << "TRX clock interface timed out, assuming TRX is dead.";
		abort();
	}

	if (msgLen==0) {
		LOG(ALARM) << "read error on TRX clock interface, return " << msgLen;
		return;
	}

	if (strncmp(buffer,"IND CLOCK",9)==0) {
		uint32_t FN;
		sscanf(buffer,"IND CLOCK %u", &FN);
		LOG(DEBUG) << "CLOCK indication, clock="<<FN;
		gBTS.clock().set(FN);
		mHaveClock = true;
		return;
	}

	buffer[msgLen]='\0';
	LOG(ALARM) << "bogus message " << buffer << " on clock interface";
}










::ARFCNManager::ARFCNManager(const char* wTRXAddress, int wBasePort, TransceiverManager &wTransceiver)
	:mTransceiver(wTransceiver),
	mDataSocket(wBasePort+100+1,wTRXAddress,wBasePort+1),
	mControlSocket(wBasePort+100,wTRXAddress,wBasePort)
{
	// The default demux table is full of NULL pointers.
	for (int i=0; i<8; i++) {
		for (unsigned j=0; j<maxModulus; j++) {
			mDemuxTable[i][j] = NULL;
		}
	}
}




void ::ARFCNManager::start()
{
	mRxThread.start((void*(*)(void*))ReceiveLoopAdapter,this);
}


void ::ARFCNManager::installDecoder(GSM::L1Decoder *wL1d)
{
	unsigned TN = wL1d->TN();
	const TDMAMapping& mapping = wL1d->mapping();

	// Is this mapping a valid uplink on this slot?
	assert(mapping.uplink());
	assert(mapping.allowedSlot(TN));

	LOG(DEBUG) << "ARFCNManager::installDecoder TN: " << TN << " repeatLength: " << mapping.repeatLength();

	mTableLock.lock();
	for (unsigned i=0; i<mapping.numFrames(); i++) {
		unsigned FN = mapping.frameMapping(i);
		while (FN<maxModulus) {
			// Don't overwrite existing entries.
			assert(mDemuxTable[TN][FN]==NULL);
			mDemuxTable[TN][FN] = wL1d;
			FN += mapping.repeatLength();
		}
	}
	mTableLock.unlock();
}




void ::ARFCNManager::writeHighSide(const GSM::TxBurst& burst)
{
	LOG(DEEPDEBUG) << "transmit at time " << gBTS.clock().get() << ": " << burst;
	// format the transmission request message
	static const int bufferSize = gSlotLen+1+4+1;
	char buffer[bufferSize];
	unsigned char *wp = (unsigned char*)buffer;
	// slot
	*wp++ = burst.time().TN();
	// frame number
	uint32_t FN = burst.time().FN();
	*wp++ = (FN>>24) & 0x0ff;
	*wp++ = (FN>>16) & 0x0ff;
	*wp++ = (FN>>8) & 0x0ff;
	*wp++ = (FN) & 0x0ff;
	// power level
	/// FIXME -- We hard-code gain to 0 dB for now.
	*wp++ = 0;
	// copy data
	const char *dp = burst.begin();
	for (unsigned i=0; i<gSlotLen; i++) {
		*wp++ = (unsigned char)((*dp++) & 0x01);
	}
	// write to the socket
	mDataSocketLock.lock();
	mDataSocket.write(buffer,bufferSize);
	mDataSocketLock.unlock();
}




void ::ARFCNManager::driveRx()
{
	// read the message
	char buffer[MAX_UDP_LENGTH];
	int msgLen = mDataSocket.read(buffer);
	if (msgLen<=0) SOCKET_ERROR;
	// decode
	unsigned char *rp = (unsigned char*)buffer;
	// timeslot number
	unsigned TN = *rp++;
	// frame number
	int32_t FN = *rp++;
	FN = (FN<<8) + (*rp++);
	FN = (FN<<8) + (*rp++);
	FN = (FN<<8) + (*rp++);
	// physcial header data
	signed char* srp = (signed char*)rp++;
	// reported RSSI is negated dB wrt full scale
	int RSSI = *srp;
	srp = (signed char*)rp++;
	// timing error comes in 1/256 symbol steps
	// because that fits nicely in 2 bytes
	int timingError = *srp;
	timingError = (timingError<<8) | (*rp++);
	// soft symbols
	float data[gSlotLen];
	for (unsigned i=0; i<gSlotLen; i++) data[i] = (*rp++) / 256.0F;
	// demux
	receiveBurst(RxBurst(data,GSM::Time(FN,TN),timingError/256.0F,-RSSI));
}


void* ReceiveLoopAdapter(::ARFCNManager* manager){
	while (true) {
		manager->driveRx();
		pthread_testcancel();
	}
	return NULL;
}





int ::ARFCNManager::sendCommandPacket(const char* command, char* response)
{
	int msgLen = 0;
	response[0] = '\0';

	LOG(INFO) << "command " << command;
	mControlLock.lock();

	for (int retry=0; retry<10; retry++) {
		mControlSocket.write(command);
		msgLen = mControlSocket.read(response,3000);
		if (msgLen>0) {
			response[msgLen] = '\0';
			break;
		}
		LOG(WARN) << "TRX link timeout on attempt " << retry+1;
	}

	mControlLock.unlock();
	LOG(DEBUG) << "response " << response;

	if ((msgLen>4) && (strncmp(response,"RSP ",4)==0)) {
		return msgLen;
	}

	LOG(ALARM) << "lost control link to transceiver";
	SOCKET_ERROR;
}




int ::ARFCNManager::sendCommand(const char*command, int param, int *responseParam)
{
	// Send command and get response.
	char cmdBuf[MAX_UDP_LENGTH];
	char response[MAX_UDP_LENGTH];
	sprintf(cmdBuf,"CMD %s %d", command, param);
	int rspLen = sendCommandPacket(cmdBuf,response);
	if (rspLen<=0) return -1;
	// Parse and check status.
	char cmdNameTest[10];
	int status;
	cmdNameTest[0]='\0';
        if (!responseParam)
	  sscanf(response,"RSP %10s %d", cmdNameTest, &status);
        else
          sscanf(response,"RSP %10s %d %d", cmdNameTest, &status, responseParam);
	if (strcmp(cmdNameTest,command)!=0) return -1;
	return status;
}


int ::ARFCNManager::sendCommand(const char*command, const char* param)
{
	// Send command and get response.
	char cmdBuf[MAX_UDP_LENGTH];
	char response[MAX_UDP_LENGTH];
	sprintf(cmdBuf,"CMD %s %s", command, param);
	int rspLen = sendCommandPacket(cmdBuf,response);
	if (rspLen<=0) return -1;
	// Parse and check status.
	char cmdNameTest[10];
	int status;
	cmdNameTest[0]='\0';
	sscanf(response,"RSP %10s %d", cmdNameTest, &status);
	if (strcmp(cmdNameTest,command)!=0) return -1;
	return status;
}



int ::ARFCNManager::sendCommand(const char*command)
{
	// Send command and get response.
	char cmdBuf[MAX_UDP_LENGTH];
	char response[MAX_UDP_LENGTH];
	sprintf(cmdBuf,"CMD %s", command);
	int rspLen = sendCommandPacket(cmdBuf,response);
	if (rspLen<=0) return -1;
	// Parse and check status.
	char cmdNameTest[10];
	int status;
	cmdNameTest[0]='\0';
	sscanf(response,"RSP %10s %d", cmdNameTest, &status);
	if (strcmp(cmdNameTest,command)!=0) return -1;
	return status;
}




bool ::ARFCNManager::tune(int wARFCN)
{
	// convert ARFCN number to a frequency
	unsigned rxFreq = uplinkFreqKHz(gBTS.band(),wARFCN);
	unsigned txFreq = downlinkFreqKHz(gBTS.band(),wARFCN);
	// tune rx
	int status = sendCommand("RXTUNE",rxFreq);
	if (status!=0) {
		LOG(ALARM) << "RXTUNE failed with status " << status;
		return false;
	}
	// tune tx
	status = sendCommand("TXTUNE",txFreq);
	if (status!=0) {
		LOG(ALARM) << "TXTUNE failed with status " << status;
		return false;
	}
	// done
	mARFCN=wARFCN;
	return true;
}



bool ::ARFCNManager::tuneLoopback(int wARFCN)
{
	// convert ARFCN number to a frequency
	unsigned txFreq = downlinkFreqKHz(gBTS.band(),wARFCN);
	// tune rx
	int status = sendCommand("RXTUNE",txFreq);
	if (status!=0) {
		LOG(ALARM) << "RXTUNE failed with status " << status;
		return false;
	}
	// tune tx
	status = sendCommand("TXTUNE",txFreq);
	if (status!=0) {
		LOG(ALARM) << "TXTUNE failed with status " << status;
		return false;
	}
	// done
	mARFCN=wARFCN;
	return true;
}


bool ::ARFCNManager::powerOff()
{
	int status = sendCommand("POWEROFF");
	if (status!=0) {
		LOG(ALARM) << "POWEROFF failed with status " << status;
		return false;
	}
	return true;
}


bool ::ARFCNManager::powerOn()
{
	int status = sendCommand("POWERON");
	if (status!=0) {
		LOG(ALARM) << "POWERON failed with status " << status;
		return false;
	}
	return true;
}





bool ::ARFCNManager::setPower(int dB)
{
	int status = sendCommand("SETPOWER",dB);
	if (status!=0) {
		LOG(ALARM) << "SETPOWER failed with status " << status;
		return false;
	}
	return true;
}


bool ::ARFCNManager::setTSC(unsigned TSC) 
{
	assert(TSC<8);
	int status = sendCommand("SETTSC",TSC);
	if (status!=0) {
		LOG(ALARM) << "SETTSC failed with status " << status;
		return false;
	}
	return true;
}


bool ::ARFCNManager::setSlot(unsigned TN, unsigned combination)
{
	assert(TN<8);
	assert(combination<8);
	char paramBuf[MAX_UDP_LENGTH];
	sprintf(paramBuf,"%d %d", TN, combination);
	int status = sendCommand("SETSLOT",paramBuf);
	if (status!=0) {
		LOG(ALARM) << "SETSLOT failed with status " << status;
		return false;
	}
	return true;
}

bool ::ARFCNManager::setMaxDelay(unsigned km)
{
        int status = sendCommand("SETMAXDLY",km);
        if (status!=0) {
                LOG(ALARM) << "SETMAXDLY failed with status " << status;
                return false;
        }
        return true;
}

signed ::ARFCNManager::setRxGain(signed rxGain)
{
        signed newRxGain;
        int status = sendCommand("SETRXGAIN",rxGain,&newRxGain);
        if (status!=0) {
                LOG(ALARM) << "SETRXGAIN failed with status " << status;
                return false;
        }
        return newRxGain;
}

signed ::ARFCNManager::getNoiseLevel(void)
{
	signed noiselevel;
	int status = sendCommand("NOISELEV",0,&noiselevel);
        if (status!=0) {
                LOG(ALARM) << "NOISELEV failed with status " << status;
                return false;
        }
        return noiselevel;
}

void ::ARFCNManager::receiveBurst(const RxBurst& inBurst)
{
	LOG(DEEPDEBUG) << "receiveBurst: " << inBurst;
	uint32_t FN = inBurst.time().FN() % maxModulus;
	unsigned TN = inBurst.time().TN();

	mTableLock.lock();
	L1Decoder *proc = mDemuxTable[TN][FN];
	if (proc==NULL) {
		LOG(DEBUG) << "ARFNManager::receiveBurst in unconfigured TDMA position TN: " << TN << " FN: " << FN << ".";
		mTableLock.unlock();
		return;
	}
	proc->writeLowSide(inBurst);
	mTableLock.unlock();
}

FuzzingControl::FuzzingControl(int wBasePort,const char * FuzzingPlatformAddress)
	:mFuzzingSocket(wBasePort, FuzzingPlatformAddress, wBasePort+100),
	mState(NonFuzzing), mPD(-1), mMTI(-1),
	mFuzzingData(NULL), mFuzzingFrame(NULL)
{
	memset(mIMSI,0,sizeof(mIMSI));
	memset(response, 0, sizeof(response));
}

void FuzzingControl::start()
{
	mControlLoopThread.start((void*(*)(void*))ControlLoopAdapter,this);

}

void FuzzingControl::ControlHandle()
{
	// check control socket
	char buffer[MAX_PACKET_LENGTH];
	
	int msgLen = -1;
	buffer[0] = '\0';
	 
	msgLen = mFuzzingSocket.read(buffer,1000);
	
	if (msgLen < 1) {
		//if(mState==NonFuzzing)	return;
		return;
		//Check the timer, Check the Fuzzing State

	}
	
	FuzzingDataHead *DataHead=(FuzzingDataHead *)buffer;

	//Head Valid Check
	unsigned char SyncValue[4]={0x00,0xff,0xff,0x00};
	if(memcmp(SyncValue, DataHead->SynData,sizeof(SyncValue))!=0){
		LOG(WARN)<<"The Sync Value is invalid!";
		sprintf(response, "BTS WR wrong Sync Value: 0x%02x,0x%02x,0x%02x,0x%02x", \
			DataHead->SynData[0],DataHead->SynData[1],DataHead->SynData[2],DataHead->SynData[3]);
		mFuzzingSocket.write(response, strlen(response)+1);
		return;
		}
	if(DataHead->FuzzingLen+sizeof(FuzzingDataHead)!=msgLen){
		LOG(WARN)<<"The length of the fuzzing Data received is invalid!";
		sprintf(response, "BTS WR Invalid Data Length %d, the fuzzing frame len is %d", msgLen, DataHead->FuzzingLen);
		mFuzzingSocket.write(response, strlen(response)+1);
		return;
		}


	if(DataHead->FuzzingType==GETBTSPARA){
		LOG(INFO)<<"Trying to get the BTS parameter";
		char *BTSParaData;
		BTSParaData=(char *)malloc(sizeof(BTSPara));
		GetBtsInfo(BTSParaData);
		mFuzzingSocket.write(BTSParaData,sizeof(BTSPara));
		LOG(INFO)<<"Upload the BTS Parameter success";
		free(BTSParaData);
		return;
	}

	if(DataHead->FuzzingType==SETBTSPARA){
		LOG(INFO)<<"Trying to set the BTS parameter";
		SetBtsInfo(buffer+sizeof(FuzzingDataHead));
		sprintf(response, "BTS OK Finished the setting");
		mFuzzingSocket.write(response, strlen(response)+1);
		LOG(INFO)<<"Finished Setting the BTS Parameter";
		return;
		}

	if(DataHead->FuzzingType==GETIMSI){
		LOG(INFO)<<"Trying to get the Fuzzing IMSI";
		sprintf(response, "%s", gFuzzingIMSI);
		mFuzzingSocket.write(response, strlen(response)+1);
		LOG(INFO)<<"Upload the Fuzzing IMSI "<<gFuzzingIMSI;
		return;
	}


	if(strlen(DataHead->IMSI)!=15){
		LOG(WARN)<<"The length of the IMSI is invalid!";
		sprintf(response, "BTS WR Invalid IMSI Length %d", strlen(DataHead->IMSI));
		mFuzzingSocket.write(response, strlen(response)+1);
		return;
		}


		
	Control::TransactionEntry dummy;
	switch(mState){
		case L3Fuzzing:
		case SMSFuzzing:    //class FuzzingControl :FuzzingState mState :NonFuzzing=0x00,SMSFuzzing=0x01,L3Fuzzing=0x02,
			LOG(WARN)<<"The state of fuzzing is running!";
			sprintf(response, "BTS WR The fuzzing state is running");
			mFuzzingSocket.write(response, strlen(response)+1);
			return;
			break;
		case NonFuzzing:          //first time!!!!!!!!!!!!!!!!!!!!!!!!!!
			//Get the information of the fuzzing phone
			memcpy(mIMSI, DataHead->IMSI, sizeof(mIMSI));
			LOG(INFO)<<"Get the Fuzzing Phone IMSI: "<<mIMSI;
			COUT("get the Fuzzing Phone IMSI: "<<mIMSI);
			mMobileIdentity=GSM::L3MobileIdentity(mIMSI);
			mFuzzingData=new BitVector(DataHead->FuzzingLen*8);
//???????????????????????			

			//test the phone is connect with Fuzzing BTS
			
			gBTS.pager().addID(GSM::L3MobileIdentity(mIMSI),GSM::SDCCHType,dummy,1000);
			mState=TestPhoneConnect;
			mLock.lock();
			mFuzzingSignal.wait(mLock,3000);
			mLock.unlock();
			if(mState!=GetPagingResponse){
				mState=NonFuzzing;
				LOG(INFO)<<"The Phone didn't connect with Fuzzing BTS";
				sprintf(response, "BTS DC The Fuzzing Phone did not connect with the BTS");
				mFuzzingSocket.write(response, strlen(response)+1);
				break;
			}
			
			
			switch((FuzzingType)DataHead->FuzzingType){
				case L3Type:
					LOG(INFO)<<"Start the L3Type Message Fuzzing";
					mPD=(GSM::L3PD)DataHead->L3ProtocolDiscriminator;
					mMTI=DataHead->L3MessageType;
					mElementNumber=DataHead->ElementNumber;
					mL2FrameType=DataHead->L2FrameType;
					//mFuzzingData=new BitVector(DataHead->FuzzingLen*8);
					mFuzzingData->unpack((unsigned char *)buffer+sizeof(FuzzingDataHead));
					COUT("The L3 Fuzzing Data is: "<<mPD<<", "<<mMTI<<", "<<mElementNumber<<", "<<DataHead->FuzzingLen<<", "<<*mFuzzingData);
					LOG(INFO)<<"The L3 Fuzzing Data is: "<<mPD<<", "<<mMTI<<", "<<mElementNumber<<", "<<DataHead->FuzzingLen<<", "<<*mFuzzingData;
					FuzzingDispatcher(mPD, mMTI, mElementNumber,mFuzzingData);
					break;
				case SMSType:
					LOG(INFO)<<"Start the SMS Message Fuzzing";
					//mState=SMSFuzzing;
					mPD=-1;
					mMTI=-1;
					mElementNumber=-1;
					mL2FrameType=-1;
					
					mFuzzingData->unpack((unsigned char *)buffer+sizeof(FuzzingDataHead));
					COUT("The SMS Fuzzing Data is: "<<mPD<<", "<<mMTI<<", "<<mElementNumber<<", "<<DataHead->FuzzingLen<<", "<<*mFuzzingData);
					LOG(INFO)<<"The SMS Fuzzing Data is: "<<mPD<<", "<<mMTI<<", "<<mElementNumber<<", "<<DataHead->FuzzingLen<<", "<<*mFuzzingData;
					FuzzingSMS(mFuzzingData);
					//mFuzzingData=new BitVector(DataHead->FuzzingLen*8);
					//mFuzzingData->unpack((unsigned char *)buffer+sizeof(FuzzingDataHead));
					//mFuzzingData=new BitVector((char *)((unsigned char *)buffer+sizeof(FuzzingDataHead)), DataHead->FuzzingLen);
					break;
				default:
					LOG(WARN)<<"unhandle Fuzzing Type!";
					sprintf(response, "BTS WR unhandle Fuzzing Type %d", DataHead->FuzzingType);
					mFuzzingSocket.write(response, strlen(response)+1);
					mState=NonFuzzing;
					return;
				}
			delete mFuzzingData;
			break;
		default:
			LOG(WARN)<<"invalid state of fuzzing control"<<mState;
			mState=NonFuzzing;
				
		}
	
}

void FuzzingControl::FuzzingDispatcher(int wPD, int wMTI, int wElementNumber, BitVector * wFuzzingData)
{
	switch((GSM::L3PD)wPD){
		case L3MobilityManagementPD:
			LOG(INFO)<<"Enter L3MobilityManagementPD Fuzzing";
			FuzzingDispatchMM(wMTI,wElementNumber,wFuzzingData);
			break;
		case L3RadioResourcePD:
			LOG(INFO)<<"Enter L3RadioResourcePD Fuzzing";
			FuzzingDispatchRR(wMTI, wElementNumber,wFuzzingData);
			break;
		case L3CallControlPD:
			LOG(INFO)<<"Enter L3CallControlPD Fuzzing";
			FuzzingDispatchCC(wMTI, wElementNumber,wFuzzingData);
			break;
		case L3SMSPD:
			LOG(INFO)<<"Enter L3SMSPD Fuzzing";						
			FuzzingDispatchSMS(wMTI, wElementNumber,wFuzzingData);
			break;
		default:
			LOG(WARN)<<"unhandle Protocol!"<<wPD;
			sprintf(response, "BTS WR unhandle Protocol %d", wPD);
			mFuzzingSocket.write(response, strlen(response)+1);
			mState=NonFuzzing;
		}

}

void FuzzingControl::FuzzingDispatchRR(int wMTI,int wElementNumber, BitVector * wFuzzingData)
{
	
	//char FuzzingIMSI[256];
	Control::TransactionEntry dummy;

	switch((L3RRMessage::MessageType)wMTI){
		case L3RRMessage::PagingRequestType1:
			LOG(INFO)<<"Enter L3RRMessage::PagingRequestType1 Fuzzing";

			
			if(mElementNumber!=7){
				LOG(WARN)<<"unhandle Element in PagingRequestType1 message!"<<mElementNumber;
				sprintf(response, "BTS WR unhandle Element in PagingRequestType1 message %d", mElementNumber);
				mFuzzingSocket.write(response, strlen(response)+1);
				mState=NonFuzzing;
				break;
				}
			//if(wFuzzingData->size()>256*8)wFuzzingData->head(256*8).pack((unsigned char *)FuzzingIMSI);
			//else	wFuzzingData->pack((unsigned char *)FuzzingIMSI);
			
			//gBTS.pager().addID(GSM::FuzzingL3MobileIdentity(FuzzingIMSI,wFuzzingData->size()/8),GSM::SDCCHType,dummy,3000);
			//gBTS.pager().addID(GSM::FuzzingL3MobileIdentity(wFuzzingData),GSM::SDCCHType,dummy,3000);

			mFuzzingPageRequest = FuzzingL3PagingRequestType1(mFuzzingData,SDCCHType);
			mState=L3Fuzzing;
			gBTS.pager().sendpage();
			
			mLock.lock();
			mFuzzingSignal.wait(mLock,5000);
			mLock.unlock();


		
			if(mState==FuzzingFailed){
				mState=NonFuzzing;
				LOG(INFO)<<"Get the paging response correctly";
				sprintf(response, "BTS FF Get the paging response correctly");
				mFuzzingSocket.write(response, strlen(response)+1);
				}
			else if(mState==L3Fuzzing){
				mState=NonFuzzing;
				LOG(INFO)<<"Fuzzing Finished with no dectect bug";
				sprintf(response, "BTS OK Fuzzing Finished");
				mFuzzingSocket.write(response, strlen(response)+1);
				}

						
			break;

		
		
		default:
			LOG(WARN)<<"unhandle RR message!"<<wMTI;
			sprintf(response, "BTS WR unhandle RR message %d", wMTI);
			mFuzzingSocket.write(response, strlen(response)+1);
			mState=NonFuzzing;
	}

}

void FuzzingControl::FuzzingDispatchMM(int wMTI, int wElementNumber, BitVector *wFuzzingData)
{
	
	switch((L3MMMessage::MessageType)wMTI){
		case L3MMMessage::LocationUpdatingAccept:{
			LOG(INFO)<<"Enter L3MMMessage::LocationUpdatingAccept Fuzzing";

			if(mElementNumber!=4&&mElementNumber!=5){
				LOG(WARN)<<"unhandle Element in LocationUpdatingAccept message!"<<mElementNumber;
				sprintf(response, "BTS WR unhandle Element in LocationUpdatingAccept message %d", mElementNumber);
				mFuzzingSocket.write(response, strlen(response)+1);
				mState=NonFuzzing;
				break;
				}

			if(mElementNumber==4){
				srand (time(NULL));
				unsigned FuzzingTMSI=(unsigned int)rand();
				mFuzzingLAILUA=FuzzingLAIL3LocationUpdatingAccept(mFuzzingData, FuzzingTMSI);
				mState=L3Fuzzing;
				}

			if(mElementNumber==5){
				mFuzzingIDLUA=FuzzingIDL3LocationUpdatingAccept(gBTS.LAI(),mFuzzingData);
				mState=L3Fuzzing;
				}
			
		
			
			//trigger the location updating procedure
			int lac = rand() % 9999;
			gConfig.set("GSM.LAC", lac);
			int ci = rand() % 999;
			gConfig.set("GSM.CI", ci);
			gBTS.regenerateBeacon();

			//Waiting for location updating procedure finished
			mLock.lock();
			mFuzzingSignal.wait(mLock,30000);
			mLock.unlock();

			sleep(1);
			if(mState==L3Fuzzing){
				mState=NonFuzzing;
				LOG(INFO)<<"Fuzzing Finished with no location updating request";
				sprintf(response, "BTS DC No Location Updating request!");
				mFuzzingSocket.write(response, strlen(response)+1);
				break;
			}

			Control::TransactionEntry dummy;
			gBTS.pager().addID(GSM::L3MobileIdentity(mIMSI),GSM::SDCCHType,dummy,1000);

			mLock.lock();
			mFuzzingSignal.wait(mLock,3000);
			mLock.unlock();
			
			if(mState==NonFuzzing){
				mState=NonFuzzing;
				LOG(INFO)<<"Get the paging response,";
				sprintf(response, "BTS OK Get the paging response, it means the phone is connected with BTS after receving a LUA with fuzzing element %d", mElementNumber);
				mFuzzingSocket.write(response, strlen(response)+1);
			}
			else if(mState==GetFuzzingResponse){
				mState=NonFuzzing;
				LOG(INFO)<<"Fuzzing Finished with no paging response";
				sprintf(response, "BTS FF No paging response got, the phone may be failed with fuzzing element %d!", mElementNumber);
				mFuzzingSocket.write(response, strlen(response)+1);
				
			}
			/*
			if(mState==FuzzingFailed){
				mState=NonFuzzing;
				LOG(INFO)<<"Get the TMSI assignment response.";
				sprintf(response, "BTS FF Get the TMSI assignment response, this should not happen!");
				mFuzzingSocket.write(response, strlen(response)+1);
				}
			else if(mState==L3Fuzzing){
				LOG(INFO)<<"Didn't Get the TMSI assignment response, should test further to make sure whether the mobile phone connect.";

				Control::TransactionEntry dummy;
				gBTS.pager().addID(GSM::L3MobileIdentity(mIMSI),GSM::SDCCHType,dummy,1000);
				
				mLock.lock();
				mFuzzingSignal.wait(mLock,3000);
				mLock.unlock();
				if(mState==FuzzingFailed){
					mState=NonFuzzing;
					LOG(INFO)<<"Get the paging response,";
					sprintf(response, "BTS FF Get the paging response, it means the phone is connected with BTS after receving a fuzzing location updating accept message.");
					mFuzzingSocket.write(response, strlen(response)+1);
					}
				else if(mState==L3Fuzzing){
					mState=NonFuzzing;
					LOG(INFO)<<"Fuzzing Finished with no dectect bug";
					sprintf(response, "BTS OK Fuzzing Finished");
					mFuzzingSocket.write(response, strlen(response)+1);
					}
				}*/
			break;
		}
		case L3MMMessage::MMInformation:{
			LOG(INFO)<<"Enter L3MMMessage::MMInformation Fuzzing";

			if(mElementNumber!=5 && mElementNumber!=6){
				LOG(WARN)<<"unhandle Element in MMInformation message!"<<mElementNumber;
				sprintf(response, "BTS WR unhandle Element in MMInformation message %d", mElementNumber);
				mFuzzingSocket.write(response, strlen(response)+1);
				mState=NonFuzzing;
				break;
				}
			
			if(mElementNumber==5){
				mFuzzingShortName=FuzzingShortNameL3MMInformation(mFuzzingData);
				mState=L3Fuzzing;
				}

			if(mElementNumber==6){
				if (gConfig.defines("GSM.ShortName")){
					mFuzzingTime=FuzzingTimeL3MMInformation(gConfig.getStr("GSM.ShortName"),mFuzzingData);
					mState=L3Fuzzing;
				}
			}

			//trigger the location updating procedure
			int lac = rand() % 9999;
			gConfig.set("GSM.LAC", lac);
			int ci = rand() % 999;
			gConfig.set("GSM.CI", ci);
			gBTS.regenerateBeacon();

			//Waiting for location updating procedure finished
			mLock.lock();
			mFuzzingSignal.wait(mLock,30000);
			mLock.unlock();

			sleep(1);

			if(mState==L3Fuzzing){
				mState=NonFuzzing;
				LOG(INFO)<<"Fuzzing Finished with no location updating request";
				sprintf(response, "BTS DC No Location Updating request!");
				mFuzzingSocket.write(response, strlen(response)+1);
				break;
			}

			Control::TransactionEntry dummy;
			gBTS.pager().addID(GSM::L3MobileIdentity(mIMSI),GSM::SDCCHType,dummy,1000);

			mLock.lock();
			mFuzzingSignal.wait(mLock,3000);
			mLock.unlock();
			
			if(mState==NonFuzzing){
				mState=NonFuzzing;
				LOG(INFO)<<"Get the paging response,";
				sprintf(response, "BTS OK Get the paging response, it means the phone is connected with BTS after receving a MMInformation with fuzzing element %d", mElementNumber);
				mFuzzingSocket.write(response, strlen(response)+1);
			}
			else if(mState==GetFuzzingResponse){
				mState=NonFuzzing;
				LOG(INFO)<<"Fuzzing Finished with no paging response";
				sprintf(response, "BTS FF No paging response got, the phone may be failed with fuzzing element %d!", mElementNumber);
				mFuzzingSocket.write(response, strlen(response)+1);
				
			}
			
			/*if(mState==FuzzingFailed){
				mState=NonFuzzing;
				LOG(INFO)<<"Not get the MMStatus response.";
				sprintf(response, "BTS FF Not Get the MMStatus reponse, this should not happen!");
				mFuzzingSocket.write(response, strlen(response)+1);
				}
			else if(mState==L3Fuzzing){
				LOG(INFO)<<"Get the MMStatus response.";
				mState=NonFuzzing;
				sprintf(response, "BTS OK Get the MMStatus response.");
				mFuzzingSocket.write(response, strlen(response)+1);
				}*/

			break;			
		}
		default:
			LOG(WARN)<<"unhandle MM message!"<<wMTI;
			sprintf(response, "BTS WR unhandle MM message %d", wMTI);
			mFuzzingSocket.write(response, strlen(response)+1);
			mState=NonFuzzing;
			break;
	}
				
				
}
void FuzzingControl::FuzzingDispatchCC(int wMTI, int wElementNumber, BitVector *wFuzzingData)
{
	switch((L3CCMessage::MessageType)wMTI){
		
		case L3CCMessage::Setup:{
			
			LOG(INFO)<<"Enter L3CCMessage::Setup Fuzzing";
			
			if(mElementNumber!=10){
				LOG(WARN)<<"unhandle Element in Setup message!"<<mElementNumber;
				sprintf(response, "BTS WR unhandle Element in Setup message %d", mElementNumber);
				mFuzzingSocket.write(response, strlen(response)+1);
				mState=NonFuzzing;
				break;
				}


			//mFuzzingSetup=FuzzingL3Setup();
			mState=L3Fuzzing;
			Control::TransactionEntry transaction(
				GSM::L3MobileIdentity(mIMSI),
				GSM::L3CMServiceType::MobileTerminatedCall, 0,
				mFuzzingData);
			transaction.Q931State(Control::TransactionEntry::Paging);
			gTransactionTable.add(transaction);
			gBTS.pager().addID(GSM::L3MobileIdentity(mIMSI),GSM::SDCCHType,transaction,3000);
			
			
			//Waiting for  procedure finished
			mLock.lock();
			mFuzzingSignal.wait(mLock,30000);
			mLock.unlock();

	
	
			if(mState==FuzzingFailed){
				mState=TestCCFail;
				LOG(INFO)<<"Not get the Setup response.";
				COUT("Not Get the Setup reponse!");
				//sprintf(response, "BTS FF Not Get the Setup reponse, this should not happen!");
				//mFuzzingSocket.write(response, strlen(response)+1);
				//break;
				}
			if(mState==GetFuzzingResponse){
				mState=TestCCFail;
				LOG(INFO)<<"Get the Alarm in Fuzzing Setup Message";
				COUT("Get the Alarm in Fuzzing Setup Message!");
			}
				mState=TestCCFail;
				sleep(2);
				Control::TransactionEntry dummy;
				gBTS.pager().addID(GSM::L3MobileIdentity(mIMSI),GSM::SDCCHType,dummy,1000);
			
				mLock.lock();
				mFuzzingSignal.wait(mLock,3000);
				mLock.unlock();
				
				if(mState==GetPagingResponse){
					mState=NonFuzzing;
					LOG(INFO)<<"Get the paging response in Fuzzing Setup Message";
					sprintf(response, "BTS OK Get the paging response after get the CC setup response");
					mFuzzingSocket.write(response, strlen(response)+1);
				}
				else {
					mState=NonFuzzing;
					LOG(INFO)<<"Fuzzing Finished with no paging response in Fuzzing Setup Message";
					sprintf(response, "BTS FF No paging response in Fuzzing CC Setup Message");
					mFuzzingSocket.write(response, strlen(response)+1);
				}
			break;
		}
		default:
			LOG(WARN)<<"unhandle CC message!"<<wMTI;
			sprintf(response, "BTS WR unhandle CC message %d", wMTI);
			mFuzzingSocket.write(response, strlen(response)+1);
			mState=NonFuzzing;
			break;
	}


}

void FuzzingControl::FuzzingDispatchSMS(int wMTI, int wElementNumber, BitVector *wFuzzingData)
{

	mState=NonFuzzing;

}


void FuzzingControl::FuzzingSMS(BitVector * wFuzzingData)
{
	LOG(INFO)<<"Enter SMS Fuzzing";
	
	
	
	Control::TransactionEntry transaction(
		GSM::L3MobileIdentity(mIMSI),
		GSM::L3CMServiceType::MobileTerminatedShortMessage,
		GSM::L3CallingPartyBCDNumber("10000000")
		//txtBuf);
		);

	mState=SMSFuzzing;
	gTransactionTable.add(transaction);
	gBTS.pager().addID(transaction.subscriber(),GSM::SDCCHType,transaction,30000);
	
	//Waiting for location updating procedure finished
	mLock.lock();
	mFuzzingSignal.wait(mLock,30000);
	mLock.unlock();
	
	sleep(1);

	
    gTransactionTable.remove(transaction.ID());
    

	if(mState==SMSFuzzing){
		mState=NonFuzzing;
		LOG(INFO)<<"Fuzzing Finished with No SMS Paging Response";
		sprintf(response, "BTS DC Sending SMS Failed with no Raging Response!");
		return;
		}

	if(mState==SMSFailed){
		mState=TestSMSFail;
		LOG(INFO)<<"Fuzzing Finished with sending SMS Failed";
		//sprintf(response, "BTS FF Sending SMS Failed!");
		//mFuzzingSocket.write(response, strlen(response)+1);
		
	}

	if(mState==NoResponse){
		mState=TestSMSFail;
		LOG(INFO)<<"Fuzzing SMS Finished with error";
		//sprintf(response, "BTS FF Sending SMS Failed!");
		//mFuzzingSocket.write(response, strlen(response)+1);
		
	}

	if(mState==SMSReponse){
		mState=TestSMSFail;
		LOG(INFO)<<"Fuzzing SMS Finished with success";
		//sprintf(response, "BTS FF Sending SMS Failed!");
		//mFuzzingSocket.write(response, strlen(response)+1);
		
	}

	//if(mState==SMSReponse){
		Control::TransactionEntry dummy;
		gBTS.pager().addID(GSM::L3MobileIdentity(mIMSI),GSM::SDCCHType,dummy,1000);
	
		mLock.lock();
		mFuzzingSignal.wait(mLock,3000);
		mLock.unlock();
	
		if(mState==GetPagingResponse){
			mState=NonFuzzing;
			LOG(INFO)<<"Get the paging response,";
			sprintf(response, "BTS OK Get the paging response, it means the phone is connected with BTS after receving a Fuzzing SMS");
			mFuzzingSocket.write(response, strlen(response)+1);
		}
		//else if(mState==GetFuzzingResponse){
		else{
			mState=NonFuzzing;
			LOG(INFO)<<"Fuzzing Finished with no paging response";
			sprintf(response, "BTS FF No paging response got, the phone may be failed with fuzzing SMS!");
			mFuzzingSocket.write(response, strlen(response)+1);
			}
	//}
	
	mState=NonFuzzing;
}

void *ControlLoopAdapter(FuzzingControl *wFuzzingControl)
{
	while(1){
		wFuzzingControl->ControlHandle();
		pthread_testcancel();
	}
	return NULL;		
}

// vim: ts=4 sw=4
