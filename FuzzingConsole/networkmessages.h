enum FuzzingType{
    L3Type=0x00,
    SMSType=0x01,
    GETBTSPARA=0x02,
    SETBTSPARA=0x03,
    GETIMSI=0x04
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
    TestRRFail=0x13

};
enum GSMBand{
    GSM850=850,			///< US cellular
    EGSM900=900,		///< extended GSM
    DCS1800=1800,		///< worldwide DCS band
    PCS1900=1900		///< US PCS band
};

#define MAX_PACKET_LENGTH 65535

typedef struct FuzzingData_Head
{
    unsigned char SynData[4];       //Sync Frame Head, the value is 0x00, 0xff, 0xff, 0x00;
    FuzzingType mFuzzingType;		//0 means L3 Fuzzing, 1 means SMS Fuzzing; if the value is SMS, the value of L3ProtocolDiscriminator and L3MessageType is invalid
    char IMSI[16];			//The IMSI of the tested phone;
    int L3ProtocolDiscriminator;	//L3 Protocol Discriminator, GSM 04.08 10.2, gsm 04.07 11.2.3.1.1
    int L3MessageType;	//Message MTIs; GSM 04.08 Table 10.1/3, 10.2, 10.3
    int ElementNumber;	//The number of element in the L3 Frame
    int L2FrameType;   //0 means UNIT_DATA,  1 means DATA
    unsigned int FuzzingLen;		//The Fuzzing Data length, not include the head length
}__attribute__ ((packed)) FuzzingDataHead;


struct BTSLAI{
    unsigned mMCC[3];		//< mobile country code digits
    unsigned mMNC[3];		//< mobile network code digits
    unsigned mLAC;			//< location area code
}__attribute__((packed));

struct BTSARFCN{
    unsigned mBand;         //< ¹€×÷Æµ¶Î£¬Ä¿Ç°»ùÕŸÖ»Ö§³Ö¹€×÷ÔÚ900MHz£¬Valid band values are 850, 900, 1800, 1900
    unsigned arfcn;		    //< ÆµµãÖµ£¬Valid values are 1-128;
}__attribute__((packed));


struct BTSBasePara{
    int BTSID;//
    char mName[20];    //
    struct BTSLAI mLAI;//
    int mCI;//
    struct BTSARFCN mBTSARFCN;//
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
    unsigned mNCCsPermitted ;	//åè®žççœç»è²ç , validåŒ0-8ïŒçŒºçäžº1.
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
