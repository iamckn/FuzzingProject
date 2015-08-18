/*
* Copyright 2008 Free Software Foundation, Inc.
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



#ifndef TRXMANAGER_H
#define TRXMANAGER_H

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "Threads.h"
#include "Sockets.h"
#include "Interthread.h"

#include "GSMCommon.h"
#include "GSML3CommonElements.h"
#include "GSML3MMMessages.h"
#include "GSML3RRMessages.h"
#include "GSML3CCMessages.h"
#include "GSMTransfer.h"

#include <list>

using namespace GSM;

/* Forward refs into the GSM namespace. */
namespace GSM {

class L1Decoder;

};


class ARFCNManager;


/**
	The TransceiverManager processes the complete transcevier interface.
	There is one of these for each access point.
 */
class TransceiverManager {

	private:

	/// the ARFCN manangers under this TRX
	std::vector<ARFCNManager*> mARFCNs;		

	/// set true when the first CLOCK packet is received
	volatile bool mHaveClock;
	/// socket for clock management messages
	UDPSocket mClockSocket;		
	/// a thread to monitor the global clock socket
	Thread mClockThread;	


	public:

	/**
		Construct a TransceiverManager.
		@param numARFCNs Number of ARFCNs supported by the transceiver.
		@param wTRXAddress IP address of the transceiver.
		@param wBasePort The base port for the interface, as defined in README.TRX.
	*/
	TransceiverManager(int numARFCNs,
		const char* wTRXAddress, int wBasePort);

	/**@name Accessors. */
	//@{
	ARFCNManager* ARFCN(unsigned i) { assert(i<mARFCNs.size()); return mARFCNs.at(i); }
	//@}

	/** Start the clock management thread and all ARFCN managers. */
	void start();

	/** Clock service loop. */
	friend void* ClockLoopAdapter(TransceiverManager*);

	private:

	/** Handler for messages on the clock interface. */
	void clockHandler();
};




void* ClockLoopAdapter(TransceiverManager *TRXm);




/**
	The ARFCN Manager processes transceiver functions for a single ARFCN.
	When we do frequency hopping, this will manage a full rate radio channel.
*/
class ARFCNManager {

	private:

	TransceiverManager &mTransceiver;

	Mutex mDataSocketLock;			///< lock to prevent contentional for the socket
	UDPSocket mDataSocket;			///< socket for data transfer
	Mutex mControlLock;				///< lock to prevent overlapping transactions
	UDPSocket mControlSocket;		///< socket for radio control

	Thread mRxThread;				///< thread to receive data from rx

	/**@name The demux table. */
	//@{
	Mutex mTableLock;
	static const unsigned maxModulus=51*26*4;	///< maximum unified repeat period
	GSM::L1Decoder* mDemuxTable[8][maxModulus];		///< the demultiplexing table for received bursts
	//@}

	unsigned mARFCN;						///< the current ARFCN


	public:

	ARFCNManager(const char* wTRXAddress, int wBasePort, TransceiverManager &wTRX);

	/** Start the uplink thread. */
	void start();

	unsigned ARFCN() const { return mARFCN; }

	void writeHighSide(const GSM::TxBurst& burst);


	/**@name Transceiver controls. */
	//@{

	/**
		Tune to a given ARFCN.
		@param wARFCN Target for tuning.
		@return true on success.
	*/
	bool tune(int wARFCN);

	/**
		Tune to a given ARFCN, but with rx and tx on the same (downlink) frequency.
		@param wARFCN Target for tuning, using downlink frequeny.
		@return true on success.
	*/
	bool tuneLoopback(int wARFCN);

	/** Turn off the transceiver. */
	bool powerOff();

	/** Turn on the transceiver. */
	bool powerOn();

        /**     
		Set maximum expected delay spread.
		@param km Max network range in kilometers.
		@return true on success.
        */
        bool setMaxDelay(unsigned km);

        /**     
                Set radio receive gain.
                @param new desired gain in dB.
                @return new gain in dB.
        */
        signed setRxGain(signed dB);

        /**
                Get noise level as RSSI.
                @return current noise level.
        */
        signed getNoiseLevel(void);

	/**
		Set power wrt full scale.
		@param dB Power level wrt full power.
		@return true on success.
	*/
	bool setPower(int dB);

	/**
		Set TSC for all slots on the ARFCN.
		@param TSC TSC to use.
		@return true on success.
	*/
	bool setTSC(unsigned TSC);

	/**
		Describe the channel combination on a given slot.
		@param TN The timeslot number 0..7.
		@param combo Channel combination, GSM 05.02 6.4.1, 0 for inactive.
		@return true on success.
	*/
	bool setSlot(unsigned TN, unsigned combo);

	//@}


	/** Install a decoder on this ARFCN. */
	void installDecoder(GSM::L1Decoder* wL1);



	private:

	/** Action for reception. */
	void driveRx();

	/** Demultiplex and process a received burst. */
	void receiveBurst(const GSM::RxBurst&);

	/** Receiver loop. */
	friend void* ReceiveLoopAdapter(ARFCNManager*);

	/**
		Send a command packet and get the response packet.
		@param command The NULL-terminated command string to send.
		@param response A buffer for the response packet, assumed to be char[MAX_PACKET_LENGTH].
		@return Length of the response or -1 on failure.
	*/
	int sendCommandPacket(const char* command, char* response);

	/**
		Send a command with a parameter.
		@param command The command name.
		@param param The parameter for the command.
                @param responseParam Optional parameter returned
		@return The status code, 0 on success, -1 on local failure.
	*/
	int sendCommand(const char* command, int param, int *responseParam=NULL);

	/**
		Send a command with a string parameter.
		@param command The command name.
		@param param The string parameter(s).
		@return The status code, 0 on success, -1 on local failure.
	*/
	int sendCommand(const char* command, const char* param);


	/**
		Send a command with no parameter.
		@param command The command name.
		@return The status code, 0 on success, -1 on local failure.
	*/
	int sendCommand(const char* command);


};


/** C interface for ARFCNManager threads. */
void* ReceiveLoopAdapter(ARFCNManager*);


enum FuzzingType{
	L3Type=0x00,
	SMSType=0x01,
	GETBTSPARA=0X02,
	SETBTSPARA=0X03,
	GETIMSI=0x04,
};

enum FuzzingState{
	NonFuzzing=0x00,
	SMSFuzzing=0x01,
	L3Fuzzing=0x02,
	FuzzingFailed=0x03,
	GetFuzzingResponse=0x04,
	TestPhoneConnect=0x05,
	GetPagingResponse=0x06,
	SMSFailed=0x07,
	SMSReponse=0x08,
	NoResponse=0x09,
	TestSMSFail=0x10,
	TestCCFail=0x11,
	TestMMFail=0x12,
	TestRRFail=0x13,
	
};

#define MAX_PACKET_LENGTH 65535

typedef struct FuzzingData_Head
{
	unsigned char SynData[4];       //Sync Frame Head, the value is 0x00, 0xff, 0xff, 0x00;
	int FuzzingType;		//0 means L3 Fuzzing, 1 means SMS Fuzzing; if the value is SMS, the value of L3ProtocolDiscriminator and L3MessageType is invalid
	char IMSI[16];			//The IMSI of the tested phone;
	int L3ProtocolDiscriminator;	//L3 Protocol Discriminator, GSM 04.08 10.2, gsm 04.07 11.2.3.1.1
	int L3MessageType;	//Message MTIs; GSM 04.08 Table 10.1/3, 10.2, 10.3
	int ElementNumber;	//The number of element in the L3 Frame
	int L2FrameType;   //0 means UNIT_DATA,  1 means DATA
	unsigned int FuzzingLen;		//The Fuzzing Data length, not include the head length
}__attribute__ ((packed)) FuzzingDataHead;


struct BTSLAI{
	unsigned mMCC[3];		///< mobile country code digits
	unsigned mMNC[3];		///< mobile network code digits
	unsigned mLAC;			///< location area code
}__attribute__((packed));

struct BTSARFCN{    
	unsigned mBand;         ///< ¹€×÷Æµ¶Î£¬Ä¿Ç°»ùÕŸÖ»Ö§³Ö¹€×÷ÔÚ900MHz£¬Valid band values are 850, 900, 1800, 1900    
	unsigned arfcn;		    ///< ÆµµãÖµ£¬Valid values are 1-128;    
}__attribute__((packed));


struct BTSBasePara{
	int BTSID;
	char mName[20];    //åºç«åç§°
	struct BTSLAI mLAI;
	int mCI;
	struct BTSARFCN mBTSARFCN;
}__attribute__((packed));


struct BTSCtrlPara{
	int mATT;		// IMSIç»ååçŠ»åè®žïŒATTïŒ,1äžºæåçŠ»åè®žïŒ0äžºæ 
	unsigned mCCCH_CONF; //å¬å±æ§å¶ä¿¡ééçœ®,validåŒ0-7
	unsigned mBS_AG_BLKS_RES; //æ¥å¥åè®žä¿çæš¡åïŒç®åäžºå®åŒïŒ2
	unsigned mBS_PA_MFRMS; //å¯»åŒä¿¡éå€åž§æ°ïŒç®åäžºå®åŒïŒ0
	unsigned mT3212;	//åšææ§äœçœ®æŽæ°å®æ¶åš
	char	 mNeighbour[32];  //é»å°åºARFCNåŒïŒæå€8äžª
	unsigned mMaxRetrans;	//æå€§éåæ¬¡æ°, validåŒ0-3ïŒ
	unsigned mTxInteger;		//åéååžæ¶éæ°ïŒvalidåŒ0-15ïŒ
	unsigned mCellBarAccess; //å°åºçŠæ­¢æ¥å¥,0åè®žæ¥å¥ïŒ1çŠæ­¢æ¥å¥
	unsigned mAC;		//æ¥å¥çç­çº§æ§å¶
	unsigned mNCCsPermitted ;	//åè®žççœç»è²ç , validåŒ0-8ïŒçŒºçäžº1.
}__attribute__((packed));

struct BTSReslectPara{
	unsigned mMS_TXPWR_MAX_CCH; 	//æå€§åŒ63ïŒ
	unsigned mRXLEV_ACCESS_MIN;		//åäžïŒ
	unsigned mCELL_RESELECT_HYSTERESIS;	//å°åºééæ»å, validåŒ0-7
}__attribute__((packed));


struct BTSPara{
	struct BTSBasePara mBasePara;		//åºç«åºæ¬åæ°
	struct BTSCtrlPara mCtrlPara;			//åºç«ç³»ç»æ§å¶åæ°
	struct BTSReslectPara mReselectPara;	//åºç«å°åºéæ©åééåæ°
	int mChannelCombination[8];
}__attribute__((packed));



//Using for fuzzing control
class FuzzingControl{
private:
	UDPSocket mFuzzingSocket;
	Thread mControlLoopThread;
	char mIMSI[16]; //The IMSI of fuzzing phone
	GSM::L3MobileIdentity mMobileIdentity;
	int mPD;
	int mMTI;
	//int mElementNumber;
	FuzzingState mState;
	
	GSM::L3Frame *mFuzzingFrame;
	char response[MAX_PACKET_LENGTH];
	//Z100Timer mFuzzingTimer;
	Mutex mLock;
	Signal mFuzzingSignal;		///< signal used to wake a thread waiting for an ack
public:
	int mElementNumber;
       int mL2FrameType;
	BitVector *mFuzzingData;	
	FuzzingL3PagingRequestType1 mFuzzingPageRequest;
	FuzzingL3Setup mFuzzingSetup;
	FuzzingTimeL3MMInformation mFuzzingTime;
	FuzzingShortNameL3MMInformation mFuzzingShortName;
	FuzzingIDL3LocationUpdatingAccept mFuzzingIDLUA;
	FuzzingLAIL3LocationUpdatingAccept mFuzzingLAILUA;
public:
	FuzzingControl(int wBasePort, const char *FuzzingPlatformAddress);
	void start();
	void ControlHandle();
	void FuzzingDispatcher(int wPD, int wMTI, int wElementNumber, BitVector * wFuzzingData);
	void FuzzingDispatchRR(int wMTI, int wElementNumber, BitVector * wFuzzingData);
	void FuzzingDispatchMM(int wMTI, int wElementNumber, BitVector * wFuzzingData);
	void FuzzingDispatchCC(int wMTI, int wElementNumber, BitVector * wFuzzingData);
	void FuzzingDispatchSMS(int wMTI, int wElementNumber, BitVector * wFuzzingData);
	void FuzzingSMS(BitVector *wFuzzingData);

	int PD() {return mPD;}
	void PD(int wPD) {mPD=wPD;}
	int MTI() {return mMTI;}
	void MTI(int wMTI) {mMTI=wMTI;}
	void mobileID(char *wIMSI) {memcpy(mIMSI, wIMSI, sizeof(mIMSI)-1); mMobileIdentity=GSM::L3MobileIdentity(mIMSI);}
	int state(){return mState;}
	void state(int wState) {mState=(FuzzingState)wState;}
	const GSM::L3MobileIdentity& mobileIdentity() const {return mMobileIdentity;}
	//void reset() {mFuzzingTimer.reset();}
	void signal() {mLock.lock();mFuzzingSignal.signal();mLock.unlock();}
	friend void *ControlLoopAdapter(FuzzingControl *);

	void wait(int wtime=5000) {mLock.lock();mFuzzingSignal.wait(mLock, wtime);mLock.unlock();}

	
};	

void *ControlLoopAdapter(FuzzingControl *);



#endif
// vim: ts=4 sw=4
