/*********************************************************
     +---------------------------------------------+
     |��Ȥ����       ԭ������    �๦����תPOV LED |
     +---------------------------------------------+	   

 ��Դ���Ȩ�� ��Ȥ���� ȫȨ���У��������ã�������������֪
        �Ͻ�����������ҵĿ�ģ�Υ�߱ؾ�������Ը�
        ��Ȥ���� 
         ->��Ʒ��ѯ QQ:1327074668 MSN:zhiqudz@163.com
		 ->��Ʒ���� http://58788771.taobao.com/
	                Tel:18106718021
	 /*********************************************/
	 /*	�� Ŀ:	ƽ��/����/��ɫ/�๦����תPOV LED  */
	 /*	ģ �飺	EEPROMģ��						  */
	 /*	�� �ܣ�	EEPROM����              		  */
	 /*	�� ����	V3.0							  */
	 /*	�� �ƣ�	ZhiQu Studio					  */
	 /*	ʱ �䣺	2013��7��5��					  */
	 /*	�� ��:	2013.7.5.19:00 ������Ŀ			  */
	 /*	˵ ����	����MCU STC115F08S2  			  */		 
	 /*********************************************/
#include "EEPROM.H"
#include <string.h>

/*----------------------------
������ʱ
----------------------------*/
void Delay1() //��ʱ����
{
    uint i;
    uint d=10;
    while (d--)
    {
        i=1000;
        while (i--);
    }
}

/*----------------------------
�ر�IAP
----------------------------*/
void IapIdle()
{
    IAP_CONTR = 0;                  //�ر�IAP����
    IAP_CMD = 0;                    //�������Ĵ���
    IAP_TRIG = 0;                   //��������Ĵ���
    IAP_ADDRH = 0x80;               //����ַ���õ���IAP����
    IAP_ADDRL = 0;
}

/*----------------------------
��ISP/IAP/EEPROM�����ȡһ�ֽ�
----------------------------*/
uchar IapReadByte(uint addr)
{
    uchar dat;                       //���ݻ�����

    IAP_CONTR = ENABLE_IAP;         //ʹ��IAP
    IAP_CMD = CMD_READ;             //����IAP����
    IAP_ADDRL = addr;               //����IAP�͵�ַ
    IAP_ADDRH = addr >> 8;          //����IAP�ߵ�ַ
    IAP_TRIG = 0x5a;                //д��������(0x5a)
    IAP_TRIG = 0xa5;                //д��������(0xa5)
    _nop_();                        //�ȴ�ISP/IAP/EEPROM�������
    dat = IAP_DATA;                 //��ISP/IAP/EEPROM����
    IapIdle();                      //�ر�IAP����

    return dat;                     //����
}

/*----------------------------
дһ�ֽ����ݵ�ISP/IAP/EEPROM����
----------------------------*/
void IapProgramByte(uint addr, uchar dat)
{
    IAP_CONTR = ENABLE_IAP;         //ʹ��IAP
    IAP_CMD = CMD_PROGRAM;          //����IAP����
    IAP_ADDRL = addr;               //����IAP�͵�ַ
    IAP_ADDRH = addr >> 8;          //����IAP�ߵ�ַ
    IAP_DATA = dat;                 //дISP/IAP/EEPROM����
    IAP_TRIG = 0x5a;                //д��������(0x5a)
    IAP_TRIG = 0xa5;                //д��������(0xa5)
    _nop_();                        //�ȴ�ISP/IAP/EEPROM�������
    IapIdle();
}


void IapProgramBytes(uint addr,uchar *dat,unsigned int length)
{
    unsigned char i=0;
	for(i=0;i<length;i++)
	{
		IapProgramByte(addr+i,dat[i]);
	}
}
void IapReadBytes(uint addr,unsigned char *dat,unsigned int length)
{
	unsigned char i=0;
	for(i=0;i<length;i++)
	{
		dat[i]=IapReadByte(addr+i);
	}
} 


/*----------------------------
��������
----------------------------*/
void IapEraseSector(uint addr)
{
    IAP_CONTR = ENABLE_IAP;         //ʹ��IAP
    IAP_CMD = CMD_ERASE;            //����IAP����
    IAP_ADDRL = addr;               //����IAP�͵�ַ
    IAP_ADDRH = addr >> 8;          //����IAP�ߵ�ַ
    IAP_TRIG = 0x5a;                //д��������(0x5a)
    IAP_TRIG = 0xa5;                //д��������(0xa5)
    _nop_();                        //�ȴ�ISP/IAP/EEPROM�������
    IapIdle();
}


unsigned long ReadAgentID(void)
{
	unsigned long lAgentID = 0;
	unsigned char datas[4]= {0xff, 0xff, 0xff, 0xff};
	IapReadBytes(AGENT_DATA_ADDR, datas, 4);

	if(datas[0] == 0xFF 
		&& datas[1] == 0xFF 
		&& datas[2] == 0xFF 
		&& datas[3] == 0xFF)
	{
		lAgentID = 0xFFFFFFFF;
	}
	else
	{
		lAgentID = (datas[0] << 24) + (datas[1] << 16) + (datas[2] << 8) + datas[3];
	}
	return lAgentID;
}


//uchar WriteAgentID(uchar datas[])
//{	
//	//unsigned long ulData = 0;
//	uchar temp[16] = {0};
//	IapReadBytes(DEVICE_STATUS_ADDR, temp, 16);

//	temp[2] = datas[0];
//	temp[3] = datas[1];
//	temp[4] = datas[2];
//	temp[5] = datas[3];
//	IapEraseSector(DEVICE_STATUS_ADDR);
//	
//	IapProgramBytes(DEVICE_STATUS_ADDR, temp, 16);

//	IapReadBytes(AGENT_ADDR, temp, 4);

//	if(temp[0] == datas[0] 
//		&& temp[1] == datas[1] 
//		&& temp[2] == datas[2] 
//		&& temp[3] == datas[3])
//		
//		return Do_True;
//	
//	return Do_False;
//}

uchar ReadVerifyProduceFlag(void)
{
	uchar temp = 0;
	temp = IapReadByte(VERIFY_PRODUCE_ADDR);
	if(0x01 == temp)
		return DO_VERIFY;
	else
		return DIS_VERIFY;
}
uchar WriteVerifyProduceFlag(uchar ucData)
{
	uchar ucFlag;
	uchar xdata temp[16] = {0};
	IapReadBytes(DEVICE_STATUS_ADDR, temp, 16);

	temp[VERIFY_PRODUCE_ADDR] = ucData;
	IapEraseSector(DEVICE_STATUS_ADDR);
	
	IapProgramBytes(DEVICE_STATUS_ADDR, temp, 16);
	
	ucFlag = IapReadByte(VERIFY_PRODUCE_ADDR);

	if(ucFlag == ucData )		
		return Do_True;
	
	return Do_False;
}



uchar ReadOffset()
{
	uchar ucData = 0;
	ucData = IapReadByte(OFFSET_ADDR);
 	if(ucData == 0xff)
		return 0;
 	return ucData;
}

void ReadSectionPsw(uchar* pdatas)
{	
	IapReadBytes(PSW_DATA_ADDR, pdatas, 4);
}

uchar ReadPasswordFlag(void)
{
	uchar chFlag = 0;
	chFlag = IapReadByte(OFFSET_FLAG_ADDR);
	if(chFlag  == 1)
		return DO_PASSWORD;
	return DIS_PASSWORD;
}

uchar WriteFlagDatas(uchar* pdatas, uint uLen)
{
	uchar temp[16] = {0};
	
	IapReadBytes(DEVICE_STATUS_ADDR, temp, 16);

	memcpy(temp + 1,pdatas, uLen);
		
	IapEraseSector(DEVICE_STATUS_ADDR);
	
	IapProgramBytes(DEVICE_STATUS_ADDR, temp, 16);

	memcpy(pdatas, temp + 1, uLen);
	
	return Do_True;
}

uchar ReadFlagDatas(uchar* ucDatas)
{
	IapReadBytes(DEVICE_STATUS_ADDR, ucDatas, 16);
	
	return Do_True;
}

uchar ReadReverseCheckFlag(void)
{
	uchar chFlag = 0;
	chFlag = IapReadByte(REVERSE_FLAG_ADDR);
	if(chFlag == 1)
		return DO_REVERSE;
	return DIS_REVERSE;
}

//uchar ReadVerifyAgentFlag(void)
//{
//	char temp = 0;
//	temp = IapReadByte(VERIFY_AGENT_FLAG_ADDR);
//	if(temp == 0xff)
//		return DIS_VERIFY_AGENT;
//	else
//		return DO_VERIFY_AGENT;
//}
//uchar WriteVerifyAgentFlag(uchar ucData)
//{
//	unsigned char ucFlag;
//	uchar temp[16] = {0};
//	IapReadBytes(DEVICE_STATUS_ADDR, temp, 16);

//	temp[VERIFY_AGENT_FLAG_ADDR] = ucData;
//	IapEraseSector(DEVICE_STATUS_ADDR);
//	
//	ucFlag = IapReadByte(VERIFY_AGENT_FLAG_ADDR);

//	if(ucFlag == ucData )
//		
//		return Do_True;
//	
//	return Do_False;
//}


