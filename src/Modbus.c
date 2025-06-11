#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "Nano100Series.h"
#define _GLOBAL_H
#include "include/global.h"
unsigned char CheckModbusRespond_uart0(unsigned char *pp);
void Program();
void DealWithSingleRegWrite_06(unsigned char *SendTempBuffer);
void DealwithPage(uint16_t page);
void DealWithPageRead_44_1(unsigned char *SendTempBuffer, unsigned char flag);
void DealWithPageRead_44(unsigned char *SendTempBuffer);
void DealWithMoreRegRead_03(unsigned char *SendTempBuffer);
void DealWithMoreDiRead_04(unsigned char *SendTempBuffer);
void DealWithMoreRegWrite_10(unsigned char *SendTempBuffer);
unsigned char CheckModbusRespond_uart1(unsigned char *pp);
void uart1_DealWithSingleRegWrite_06(unsigned char *SendTempBuffer);
void uart1_DealWithMoreRegRead_03(unsigned char *SendTempBuffer);
void uart1_DealWithMoreDiRead_04(unsigned char *SendTempBuffer);
void uart1_DealWithMoreRegWrite_10(unsigned char *SendTempBuffer);

unsigned char CheckModbusRespond_uart3(unsigned char *pp);
void uart3_DealWithSingleRegWrite_06(unsigned char *SendTempBuffer);
void uart3_DealWithMoreRegRead_03(unsigned char *SendTempBuffer);
void uart3_DealWithMoreDiRead_04(unsigned char *SendTempBuffer);
void uart3_DealWithMoreRegWrite_10(unsigned char *SendTempBuffer);
//--------------------------------------------------------
// Function:  unsigned char CheckModbusRespond()
// Input:    None
// Output:   None
// Description: �ȴ�modbus����Э�飬����д��Ԫ�Ͷ���Ԫ
// format:  byte1	byte2	  byte3	   byte4	byte5	  byte6	 byte7	    byte8
//          ��ַ	������	��ַ��	��ַ��	������ ������	CRC�ĸ�λ	CRC�ĵ�λ
//----------------------------------------------------------
// CONSIST
#define cRightStatus 0x01
#define cCrcWrong 0x02
#define cModbusOverTime 0x05 // need modified

// modbus ͨѶЭ�������
#define cComandReadCoil 0x01
#define cComandReadDiStatus 0x02
#define cComandReadHoldRegister 0x03
#define cComandReadDiRegister 0x04

#define cComandWriteSingleCoil 0x05
#define cComandWriteMoreCoil 0x0f

#define cComandWriteSingleUint 0x06
#define cComandWriteMoreUint 0x10

void SendFloodStatus()
{
	unsigned char Sendbuff[100], i = 0, k;
	uint16_t TempCrc;
#if 0
	Sendbuff[i++]=0xaa;
	Sendbuff[i++]=0x55;
	Sendbuff[i++]=0x00;
	Sendbuff[i++]=0x04;
	Sendbuff[i++]=0x01;
	Sendbuff[i++]=0x60;
#endif
	Sendbuff[i++] = DeviceNum;
	Sendbuff[i++] = 0x03;
	Sendbuff[i++] = 0x40;
	for (k = 159; k < 159 + 0x20; k++)
	{

		Sendbuff[i++] = ParaList[k * 2];
		Sendbuff[i++] = ParaList[k * 2 + 1];
	}
	TempCrc = CRC(Sendbuff, i);
	Sendbuff[i++] = TempCrc / 256; // High crc
	Sendbuff[i++] = TempCrc % 256; // Low crc
	SendDataToBus_uart1(Sendbuff, i);
}

unsigned char CheckModbusRespond_uart0(unsigned char *pp)
{
	unsigned char SetT[6];
	WatchdogReset();

	// singleWrite  06-----------------------------------------------------
	if (pp[1] == cComandWriteSingleUint) // д�����Ĵ���06
	{
		DealWithSingleRegWrite_06(pp);
		return (cRightStatus);
	}
	// Read  Register  03-----------------------------------------------------
	else if (pp[1] == cComandReadHoldRegister) // �����ּĴ���03
	{
		DealWithMoreRegRead_03(pp);
		return (cRightStatus);
	}
	// Read  Variable  04-----------------------------------------------------
	else if (pp[1] == cComandReadDiRegister) // �����ݼĴ���04
	{
		DealWithMoreDiRead_04(pp);
		return (cRightStatus);
	}
	// Read  Variable  16-----------------------------------------------------
	else if (pp[1] == 0x10)
	{
		DealWithMoreRegWrite_10(pp);
		return (cRightStatus);
	}
	// Read  History  0x41-----------------------------------------------------
	else if (pp[1] == 0x41)
	{
		DealWithHisRead(pp);
		return (cRightStatus);
	}
	// read page-------------------------------------------------------------
	else if (pp[1] == 0x44) // ����ʷ����0x41
	{
		DealWithPageRead_44(pp);
		return (cRightStatus);
	}
	// FunctionCode Wrong------------------------------------------------------
	else
	{
		pp[0] = pp[1] + 0x80;
		pp[1] = 0x01; // Functioncode wrong
		SendDataToBus1(pp, 2);
		return (cCrcWrong);
	}
}

void Program()
{
	ProgramEepromByte();
	ReadEepromByte();
}
//---------------------------------------------------------------
// Function:  void DealWithSingleRegWrite(uint16_t Addr,unsigned char CommandType)
// Input:    None
// Output:   None
// Description: �ظ�modbus����06д����
//----------------------------------------------------------------
void DealWithSingleRegWrite_06(unsigned char *SendTempBuffer)
{
	uint16_t i, j;
	uint16_t TempCrc = 0;
	int16_t TEmpInter1, TEmpInter2;
	int8_t SetT[7];
	//   unsigned char   tempflash[2];
	for (i = 0; i < 8; i++)
		SendTempBuffer[i] = SendTempBuffer[i];

	TempCrc = SendTempBuffer[2] * 256;
	TempCrc += SendTempBuffer[3]; // modified 2014-02-21 10:29
	i = TempCrc;

	/*-------------ƫ��ֵ�趨������������ڲ���ƫ��ֵ����Ҫ���ű���=78������²���Ч��
				   �¶�ƫ�������ķ�ΧΪ����300��30�ȣ���ʪ��Ϊ����200��20%��
	*/
	// add 2016-5-1 13:48

	if (i < 2010)
	{
		// add 2021-04-18 11:22

		// end of add 2021-04-18 11:22
		if ((i == cTempOffset) || (i == cTempOffset + 1))
		{
#ifdef _Loramain
			if ((SendTempBuffer[0] >= AddrStart) && (SendTempBuffer[0] < AddrStart + AddrLen) && (ParaList[(cLoraNodeAliveSet + SendTempBuffer[0] - AddrStart) * 2 + 1] == 1)

			)
			{
				ParaList[(i - cTempOffset) * 2 + (SendTempBuffer[0] - AddrStart) * 4 + cLoraNodeOffset * 2] = SendTempBuffer[4];
				ParaList[(i - cTempOffset) * 2 + (SendTempBuffer[0] - AddrStart) * 4 + cLoraNodeOffset * 2 + 1] = SendTempBuffer[5];
				Program();
			}
#else
			ParaList[i * 2] = SendTempBuffer[4];
			ParaList[i * 2 + 1] = SendTempBuffer[5];
			Program();
#endif
		}
		else if ((i == cInterTOffet) || (i == cInterHROffet))
		{

			if (i == cInterTOffet)
			{
				if (ParaList[cDoorVar * 2 + 1] == 78)
				{
					j = SendTempBuffer[4] * 256 + SendTempBuffer[5];

					if ((j < 300) || (j > 65236))
					{
						ParaList[i * 2] = SendTempBuffer[4];
						ParaList[i * 2 + 1] = SendTempBuffer[5];
						Program();
					}
				}
			}
			else
			{
				if (ParaList[cDoorVar * 2 + 1] == 78)
				{
					j = SendTempBuffer[4] * 256 + SendTempBuffer[5];

					if ((j < 200) || (j > 65336))
					{
						ParaList[i * 2] = SendTempBuffer[4];
						ParaList[i * 2 + 1] = SendTempBuffer[5];
						Program();
					}
				}
			}
		}
		//-------------------lora  ��д
		else if ((i == cLoraReadConfigPara) && ((SendTempBuffer[5] == 0x01) || (SendTempBuffer[5] == 0x02))) // ��ȡlora����
		{
			ParaList[i * 2] = SendTempBuffer[4];
			ParaList[i * 2 + 1] = SendTempBuffer[5];
			ReadOutLoraPara(SendTempBuffer[5]);
		}

		else if ((i == cLoraReadConfigPara) && (SendTempBuffer[5] == 0x00)) // �˳���ȡ
		{

			ParaList[i * 2] = SendTempBuffer[4];
			ParaList[i * 2 + 1] = SendTempBuffer[5];
			SwtichToWorkmode();
		}
		else if ((i == cLoraWriteConfigPara) && ((SendTempBuffer[5] == 0x01) || (SendTempBuffer[5] == 0x02))) // ����lora����
		{
			ParaList[i * 2] = SendTempBuffer[4];
			ParaList[i * 2 + 1] = SendTempBuffer[5];
			WriteInLoraPara(SendTempBuffer[5]);
		}
		else if ((i == cLoraWriteConfigPara) && (SendTempBuffer[5] == 0x00)) // �˳�����
		{
			ParaList[i * 2] = SendTempBuffer[4];
			ParaList[i * 2 + 1] = SendTempBuffer[5];
			SwtichToWorkmode();
		}

		//-----------�޸��豸��ַ
		else if (i == 0) // �޸��豸��ַ
		{

			ParaList[cDeviceNum * 2] = SendTempBuffer[4];
			ParaList[cDeviceNum * 2 + 1] = SendTempBuffer[5];
			if ((SendTempBuffer[4] == 0) && ((SendTempBuffer[5] != 0) && (SendTempBuffer[5] != 255))) // 2016-01-18 21:19
			{
				ProgramEepromByte();
				ReadEepromByte();
			}
		}
		//--------------------���г�ʼ������-------------------------
		else if (((i == 26) || (i == cInitial)) && (SendTempBuffer[4] == 0x00) && (SendTempBuffer[5] == 26)) // initial
		{
			ResetFlag = 0; // 2021-02-21 17:33
			InitialPara();
			BellOn();
			DelayMicroSeconds(1000);
			BellOff();
		}
		//--------------ͨ�����õ�ǰ��׼ֵ�������ڲ�ƫ�ʹ��ǰ����ʪ��Ϊ��ǰ���¶�-------
		else if (i == cOrigTS) // t  offset  broadcast set   //2015-09-19 14:01
		{
			if (ParaList[cDoorVar * 2 + 1] == 78)
			{
				TEmpInter1 = (int16_t)(VarList[cTemp * 2] * 256 + VarList[cTemp * 2 + 1]);
				TEmpInter2 = (int16_t)(SendTempBuffer[cTempError * 2] * 256 + SendTempBuffer[cTempError * 2 + 1]);
				TEmpInter2 = TEmpInter2 - TEmpInter1;
				TEmpInter1 = (int16_t)(ParaList[cInterTOffet * 2] * 256 + ParaList[cInterTOffet * 2 + 1]);
				TEmpInter2 = TEmpInter2 + TEmpInter1;
				TempCrc = (uint16_t)TEmpInter2;

				ParaList[cInterTOffet * 2] = TempCrc / 256;
				ParaList[cInterTOffet * 2 + 1] = TempCrc % 256;
				ProgramEepromByte();
				ReadEepromByte();
			}
		}
		//-------------------------------------------------------------
		else if (i == cOrigRHS) // hr  offset  broadcast set   //2015-09-19 14:01
		{
			if (ParaList[cDoorVar * 2 + 1] == 78)
			{
				TEmpInter1 = (int16_t)(VarList[cHumi * 2] * 256 + VarList[cHumi * 2 + 1]);
				TEmpInter2 = (int16_t)(SendTempBuffer[cHumiError * 2] * 256 + SendTempBuffer[cHumiError * 2 + 1]);
				TEmpInter2 = TEmpInter2 - TEmpInter1;
				TEmpInter1 = (int16_t)(ParaList[cInterHROffet * 2] * 256 + ParaList[cInterHROffet * 2 + 1]);
				TEmpInter2 = TEmpInter2 + TEmpInter1;
				TempCrc = (uint16_t)TEmpInter2;

				ParaList[cInterHROffet * 2] = TempCrc / 256;
				ParaList[cInterHROffet * 2 + 1] = TempCrc % 256;
				ProgramEepromByte();
				ReadEepromByte();
			}
		}
		//---------------------------�޸�ʱ��--------------------------
		else if ((i == cTimeTrig) && (SendTempBuffer[4] == 0x00) && (SendTempBuffer[5] == 78)) // initial
		{
			SetT[6] = ParaList[cYear * 2 + 1] / 10 * 16 + ParaList[cYear * 2 + 1] % 10;
			SetT[5] = ParaList[cMonth * 2 + 1] / 10 * 16 + ParaList[cMonth * 2 + 1] % 10;
			SetT[3] = ParaList[cDay * 2 + 1] / 10 * 16 + ParaList[cDay * 2 + 1] % 10;
			SetT[2] = ParaList[cHour * 2 + 1] / 10 * 16 + ParaList[cHour * 2 + 1] % 10;
			SetT[1] = ParaList[cMiniter * 2 + 1] / 10 * 16 + ParaList[cMiniter * 2 + 1] % 10;
			SetT[0] = ParaList[cSecond * 2 + 1] / 10 * 16 + ParaList[cSecond * 2 + 1] % 10;
			SetPCF8563(SetT);
		}
		//---------------------------beep control--------------------------
		else if (i == cRemoteControl)
		{
#ifdef _Loramain
			ParaList[cAlarmControlPos * 2 + (SendTempBuffer[0] - 1) * 2 + 1] = SendTempBuffer[5];
#else
			ParaList[cRemoteControl * 2 + 1] = SendTempBuffer[5];
#endif
		}

		//------------------------special adjust------10��error adjust------------------------------
		else if ((i >= cAllowHumi) && (i < cHumiOff1 + 10))
		{
			if (ParaList[23] == 78)
			{
				ParaList[i * 2] = SendTempBuffer[4];
				ParaList[i * 2 + 1] = SendTempBuffer[5];
				ProgramEepromByte();
				ReadEepromByte();
			}
		}
		//------------------------------------------spi flash  initial------------------------------
		else if (i == cSpiFlashInitial)
		{
			LCD4_Clear();
			LCD_string(1, 1, " Flash Initial  ");
			LCD_string(2, 1, "                ");
			W25QXX_Erase_Chip();
			SpiFlash_WaitReady();
			LCD_string(1, 1, " Flash Initial  ");
			LCD_string(2, 1, "    Over        ");
		}
		else
		{
			if ((i != cReordNumIndex) && (i != cCurrentReordNum) && (i != cCurrentReordPage) && (i != cPagePos)) // 2015-09-19 13:59
			{
				ParaList[i * 2] = SendTempBuffer[4];
				ParaList[i * 2 + 1] = SendTempBuffer[5];
				if ((i < cDO1) || (i > cDI4))
				{
					ProgramEepromByte();
					ReadEepromByte();
				}
				else
				{
					if ((i >= cDO1) && (i <= cDO4))
					{
#ifdef __flood
						DigitalOutput(i - cDO1 + 1, SendTempBuffer[5]);
						ParaList[(cAutoStatusDO1 + i - cDO1) * 2] = 0;
						ParaList[(cAutoStatusDO1 + i - cDO1) * 2 + 1] = 0;
						ParaList[cRuntimeDO1 * 2] = 0;
						ParaList[cRuntimeDO1 * 2 + 1] = 0;
						if (SendTempBuffer[5] == 1)
							PumpBeginCount(i - cDO1 + 1);
						SendFloodStatus();

#endif
					}
				}
			}
		}
	}

	/*���õ���ʪ��10��ƫ��У׼ֵ,�趨�ĵ�ַ��ʼֵ,��ַ����(���16���ڵ�),lora���� 3000���ϵ�ַ�Ĵ���*/
	// 3000---3019
	else if (i >= cTempOff)
	{
		if (ParaList[23] == 78)
		{
			ParaList[(i - c02D_ParaActual) * 2] = SendTempBuffer[4];
			ParaList[(i - c02D_ParaActual) * 2 + 1] = SendTempBuffer[5];
			ProgramEepromByte();
			ReadEepromByte();
		}
	}
	//----------- 03D para------------------
	else if ((i >= 2010) && (i < 3000))
	{

		ParaList[(i - cParaActual) * 2] = SendTempBuffer[4];
		ParaList[(i - cParaActual) * 2 + 1] = SendTempBuffer[5];
		if (StartBak != ParaList[(cSaveDataFlag03D - cParaActual) * 2 + 1])
		{
			StartBak = ParaList[(cSaveDataFlag03D - cParaActual) * 2 + 1];
			if (StartBak == 0) // ����--->ֹͣ
			{
				VarList[cRealDataAct * 2 + 1] = 0x0ff;
				VarList[cRealDataAct * 2] = 0x0ff;
			}
			else // ֹͣ--->����
				History.AlarmStatus1 = 0x00;
		}

		//-----------����Ϊ���������ı��,��ʱ������ض�Ӧ�Ĳ���----------------------
		if (i == cAlarmOutControl03D)
		{
			DealAlarm();
			ProgramEepromByte();
		}
		//-------------------------------------------------//	 record status
		else if (i == cBackLightControl)
		{
			DealBacklight();
		}
		else if (i != cTestMode03D)
		{
			ProgramEepromByte();
			ReadEepromByte();
		}
	}

	if (SendTempBuffer[0] != 0)

		SendDataToBus1(SendTempBuffer, 8);
}
//---------------------------------------------------------------
// Function:  void DealwithPage(uint16_t page)
// Input:    None
// Output:   None
// Description: �ظ�0x44 page read ����
//----------------------------------------------------------------
void DealwithPage(uint16_t page)
{
	unsigned char i, TemBuffer2[6];
	TemBuffer2[0] = DeviceNum;
	TemBuffer2[2] = page / 256;
	TemBuffer2[3] = page % 256;
	TemBuffer2[5] = 0x01;
	DealWithPageRead_44(TemBuffer2);
	TemBuffer2[5] = 0x02;
	DealWithPageRead_44(TemBuffer2);
}
//------------------------------------------------------------------------------
void DealWithPageRead_44_1(unsigned char *SendTempBuffer, unsigned char flag)
{
	unsigned char i, TemBuffer[300], TemBufferSend[120];
	uint16_t TempCrc, m;
	uint32_t addr;
	for (i = 0; i < 10; i++)
		TemBuffer[i] = SendTempBuffer[i];
	TemBuffer[1] = 0x45;
	TemBuffer[6] = 255;
	i = SendTempBuffer[5];
	WatchdogReset();

	TempCrc = SendTempBuffer[2] * 256 + SendTempBuffer[3];				  // read page
	addr = ParaList[cReordNumIndex * 2 + 1] * PagesOfOneLoop + TempCrc;	  // act page
	addr *= cPageRecordNum;												  // records of pages
	addr *= cRecordLength;												  // first record address of page
	addr += (SendTempBuffer[5] - 1) * cPageRecordNum * cRecordLength / 2; // up half page  dowm half page offset

	W25QXX_Read(&TemBuffer[7], addr, 255);
	if (flag == 0)
	{
		TempCrc = CRC(TemBuffer, 262);
		TemBuffer[262] = TempCrc / 256; // High crc
		TemBuffer[263] = TempCrc % 256; // Low crc
		WatchdogReset();
		SendDataToBus1(TemBuffer, 264);
		DelaySecond_1(1);
	}
	else
	{
		for (i = 0; i < cPageRecordNum / 2; i++)
		{
			m = 7 + i * cRecordLength;
			sprintf(TemBufferSend, "%d-%d-%d  %d:%d:%d  %d-%d-%d-%d  Temp:%d  Humi:%d \r\n\0\0\0", TemBuffer[m], TemBuffer[m + 1], TemBuffer[m + 2], TemBuffer[m + 3], TemBuffer[m + 4], TemBuffer[m + 5],
					TemBuffer[m + 6], TemBuffer[m + 7], TemBuffer[m + 8], TemBuffer[m + 9],
					TemBuffer[m + 10] * 256 + TemBuffer[m + 11], TemBuffer[m + 12] * 256 + TemBuffer[m + 13]);
			SendDataToBus1(TemBufferSend, strlen(TemBufferSend));
		}
	}
}

void DealWithPageRead_44(unsigned char *SendTempBuffer)
{
	unsigned char TemBuffer[10];
	uint16_t TempCrc, j, TempCrc1, i, k;
	uint32_t addr;
	for (i = 0; i < 10; i++)
		TemBuffer[i] = SendTempBuffer[i];
	TempCrc = TemBuffer[4] * 256 + TemBuffer[5];
	WatchdogReset();
	if ((TempCrc == 1) || (TempCrc == 2)) // half page  01 44 00  00 00 01/02
		DealWithPageRead_44_1(TemBuffer, 0);

	else if (TempCrc == 4000) // whole page    01 44 00  00 0F A0
	{
		TemBuffer[4] = 0x00;
		TemBuffer[5] = 0x01;
		DealWithPageRead_44_1(TemBuffer, 1);
		TemBuffer[5] = 0x02;
		DealWithPageRead_44_1(TemBuffer, 1);
	}
	else if (TempCrc == 4001) // ��ȡ����ǰҳ page    01 44 00  00 0F A1
	{
		for (j = 0; j < CurrentWritePage + 1; j++)
		{
			TemBuffer[2] = j / 256;
			TemBuffer[3] = j % 256;
			TemBuffer[4] = 0x00;
			TemBuffer[5] = 0x01;
			DealWithPageRead_44_1(TemBuffer, 1);
			TemBuffer[5] = 0x02;
			DealWithPageRead_44_1(TemBuffer, 1);
		}
	}
	else if (TempCrc == 4002) // ����ʼҳ03��ȡ����ǰҳ page    01 44 00  03 0F A2
	{
		TempCrc1 = TemBuffer[2] * 256 + TemBuffer[3];
		for (j = TempCrc1; j <= CurrentWritePage; j++)
		{
			TemBuffer[2] = j / 256;
			TemBuffer[3] = j % 256;
			TemBuffer[4] = 0x00;
			TemBuffer[5] = 0x01;
			DealWithPageRead_44_1(TemBuffer, 1);
			TemBuffer[5] = 0x02;
			DealWithPageRead_44_1(TemBuffer, 1);
		}
	}
	else if (TempCrc == 4003) // ��ȡ����loop������    01 44 00  03 0F A3
	{
		for (i = 0; i < ParaList[cReordNumIndex * 2 + 1] + 1; i++)
		{
			if (i = ParaList[cReordNumIndex * 2 + 1])
			{
				for (j = 0; j <= CurrentWritePage; j++)
				{
					TemBuffer[2] = j / 256;
					TemBuffer[3] = j % 256;
					TemBuffer[4] = 0x00;
					TemBuffer[5] = 0x01;
					DealWithPageRead_44_1(TemBuffer, 1);
					TemBuffer[5] = 0x02;
					DealWithPageRead_44_1(TemBuffer, 1);
				}
			}
			else
			{
				for (j = 0; j < 4000; j++)
				{
					TemBuffer[2] = j / 256;
					TemBuffer[3] = j % 256;
					TemBuffer[4] = 0x00;
					TemBuffer[5] = 0x01;
					DealWithPageRead_44_1(TemBuffer, 1);
					TemBuffer[5] = 0x02;
					DealWithPageRead_44_1(TemBuffer, 1);
				}
			}
		}
	}
	// add  2021-05-26 12:23
	else if (TemBuffer[4] == 0xff)
	{
		TempCrc1 = TemBuffer[2] * 256 + TemBuffer[3];
		k = TemBuffer[5];
		for (j = TempCrc1; j <= k; j++)
		{
			TemBuffer[2] = j / 256;
			TemBuffer[3] = j % 256;
			TemBuffer[4] = 0x00;
			TemBuffer[5] = 0x01;
			DealWithPageRead_44_1(TemBuffer, 1);
			TemBuffer[5] = 0x02;
			DealWithPageRead_44_1(TemBuffer, 1);
			;
		}
	}
	// end of add  2021-05-26 12:24

	else if (TempCrc < 4000)
	{
		for (j = TemBuffer[2] * 256 + TemBuffer[3]; j <= TemBuffer[4] * 256 + TemBuffer[5]; j++)
		{
			TemBuffer[2] = j / 256;
			TemBuffer[3] = j % 256;
			TemBuffer[5] = 0x01;
			DealWithPageRead_44_1(TemBuffer, 1);
			TemBuffer[5] = 0x02;
			DealWithPageRead_44_1(TemBuffer, 1);
		}
	}
}

//---------------------------------------------------------------
// Function:  void DealWithMoreRegRead()
// Input:    None
// Output:   None
// Description: �ظ�modbus����03������
//----------------------------------------------------------------
void DealWithMoreRegRead_03(unsigned char *SendTempBuffer)
{
	uint16_t TempCrc = 0;
	uint16_t i;
	uint16_t k, j = 3;
	unsigned char SendTempBuffer1[300];
	for (i = 0; i < 8; i++)
		SendTempBuffer1[i] = SendTempBuffer[i];

	if (SendTempBuffer1[0] == DeviceNum)
	{
		TempCrc = SendTempBuffer[2] * 256;
		TempCrc += SendTempBuffer[3]; // modified 2014-02-21 10:29
		SendTempBuffer1[0] = DeviceNum;
		SendTempBuffer1[1] = 0x03;
		SendTempBuffer1[2] = SendTempBuffer1[5] * 2;
		if (SendTempBuffer1[5] <= ParaNum / 2)
		{

			//------------------<2010
			if (TempCrc < cSaveDataFlag03D)
			{
				i = (unsigned char)TempCrc;
				for (k = i; k < i + SendTempBuffer1[2] / 2; k++)
				{
					if (k == 0)
					{
						SendTempBuffer1[j++] = ParaList[cDeviceNum * 2];
						SendTempBuffer1[j++] = ParaList[cDeviceNum * 2 + 1];
					}

					else
					{
						SendTempBuffer1[j++] = ParaList[k * 2];
						SendTempBuffer1[j++] = ParaList[k * 2 + 1];
					}
				}
			}
			//----------------------->=3000
			else if (TempCrc >= cTempOff)
			{
				TempCrc -= c02D_ParaActual;
				i = (unsigned char)TempCrc;
				for (k = i; k < i + SendTempBuffer1[2] / 2; k++)
				{
					SendTempBuffer1[j++] = ParaList[k * 2];
					SendTempBuffer1[j++] = ParaList[k * 2 + 1];
				}
			}

			else if (TempCrc >= cSaveDataFlag03D)
			{
				TempCrc -= cParaActual;
				i = (unsigned char)TempCrc;
				for (k = i; k < i + SendTempBuffer1[2] / 2; k++)
				{
					SendTempBuffer1[j++] = ParaList[k * 2];
					SendTempBuffer1[j++] = ParaList[k * 2 + 1];
				}
			}

			TempCrc = CRC(SendTempBuffer1, j);
			SendTempBuffer1[j++] = TempCrc / 256; // High crc
			SendTempBuffer1[j++] = TempCrc % 256; // Low crc
			SendDataToBus1(SendTempBuffer1, j);
		}
	}
	else // for read  offset para for lora node
	{
		if ((SendTempBuffer[0] >= AddrStart) && (SendTempBuffer[0] < AddrStart + AddrLen) && (ParaList[(cLoraNodeAliveSet + SendTempBuffer[0] - AddrStart) * 2 + 1] == 1))
		{
			TempCrc = SendTempBuffer[2] * 256;
			TempCrc += SendTempBuffer[3]; // modified 2014-02-21 10:29
			SendTempBuffer1[0] = SendTempBuffer[0];
			SendTempBuffer1[1] = 0x03;
			SendTempBuffer1[2] = SendTempBuffer1[5] * 2;
			i = (unsigned char)TempCrc;
			for (k = i; k < i + SendTempBuffer1[2] / 2; k++)
			{
				SendTempBuffer1[j++] = ParaList[(k - cTempOffset) * 2 + (SendTempBuffer[0] - AddrStart) * 4 + cLoraNodeOffset * 2]; // ��ʪ��λ��+�豸��Ӧλ��+ƫ��baseλ��
				SendTempBuffer1[j++] = ParaList[(k - cTempOffset) * 2 + (SendTempBuffer[0] - AddrStart) * 4 + cLoraNodeOffset * 2 + 1];
			}
			TempCrc = CRC(SendTempBuffer1, j);
			SendTempBuffer1[j++] = TempCrc / 256; // High crc
			SendTempBuffer1[j++] = TempCrc % 256; // Low crc
			SendDataToBus1(SendTempBuffer1, j);
		}
	}
}

//---------------------------------------------------------------
// Function:  void DealWithMoreDiRead()
// Input:    None
// Output:   None
// Description: �ظ�modbus����04������
//----------------------------------------------------------------
void DealWithMoreDiRead_04(unsigned char *SendTempBuffer)
{
	uint16_t TempCrc = 0;
	unsigned char i;
	unsigned char k, j = 3;
	WatchdogReset();

	TempCrc = SendTempBuffer[2] * 256;
	TempCrc += SendTempBuffer[3]; // addr
	SendTempBuffer[0] = SendTempBuffer[0];
	SendTempBuffer[1] = 0x04;
	SendTempBuffer[2] = SendTempBuffer[5] * 2;
	if (TempCrc < cModifiedTime)
	{
		if ((SendTempBuffer[0] != DeviceNum) && ((SendTempBuffer[5] == 3) || (SendTempBuffer[5] == 2)) && (ParaList[(cLoraNodeAliveSet + SendTempBuffer[0] - AddrStart) * 2 + 1] == 1)) // read var len
			TempCrc += cTempStartAddr + (SendTempBuffer[0] - AddrStart) * 2;																											// lora gateway
		else
			TempCrc += cTemp;
	}
	else if (TempCrc >= cRealData) // 03d   rht
	{
		TempCrc = TempCrc - cRealData + cRealDataAct;
	}
	else if ((TempCrc >= cModifiedTime) && (TempCrc < cTempStartAddr))
	{
		TempCrc -= cModifiedTime; // 03d time
		TempCrc += 2;
	}
	i = (unsigned char)TempCrc;

	for (k = i; k < i + SendTempBuffer[2] / 2; k++)
	{
		WatchdogReset();
		SendTempBuffer[j++] = VarList[k * 2];
		SendTempBuffer[j++] = VarList[k * 2 + 1];
	}
	TempCrc = CRC(SendTempBuffer, j);
	SendTempBuffer[j++] = TempCrc / 256; // High crc
	SendTempBuffer[j++] = TempCrc % 256; // Low crc
	SendDataToBus1(SendTempBuffer, j);
}

//---------------------------------------------------------------
// Function:  void DealWithSingleRegWrite(uint16_t Addr,unsigned char CommandType)
// Input:    None
// Output:   None
// Description: �ظ�modbus����0x10д����
//----------------------------------------------------------------
void DealWithMoreRegWrite_10(unsigned char *SendTempBuffer)
{
	unsigned char i;
	unsigned char j, k;
	unsigned char SetT[6]; // 2015-10-21 5:41
	uint16_t TempCrc = 0;
	//  for(i=0;i<40;i++)
	//	  SendTempBuffer[i]=SendTempBufferBak[i];

	TempCrc = SendTempBuffer[2] * 256;
	TempCrc += SendTempBuffer[3]; // base addr
	if (SendTempBuffer[5] < cReceivelength / 2)
	{
		i = (unsigned char)TempCrc;
		j = 7;

		if (TempCrc == cModifiedTime)

		{
			SetT[0] = SendTempBuffer[j + 5 * 2 + 1] / 10 * 16 + SendTempBuffer[j + 5 * 2 + 1] % 10;
			SetT[1] = SendTempBuffer[j + 4 * 2 + 1] / 10 * 16 + SendTempBuffer[j + 4 * 2 + 1] % 10;
			SetT[2] = SendTempBuffer[j + 3 * 2 + 1] / 10 * 16 + SendTempBuffer[j + 3 * 2 + 1] % 10;
			SetT[3] = SendTempBuffer[j + 2 * 2 + 1] / 10 * 16 + SendTempBuffer[j + 2 * 2 + 1] % 10;
			SetT[5] = SendTempBuffer[j + 1 * 2 + 1] / 10 * 16 + SendTempBuffer[j + 1 * 2 + 1] % 10;
			SetT[6] = SendTempBuffer[j + 1] / 10 * 16 + SendTempBuffer[j + 1] % 10;
			SetPCF8563(SetT);
			// add  2020-06-02 8:51
#if 0 
  	      	   RTC_Second=SendTempBuffer[j+5*2+1];
                   RTC_Minite=SendTempBuffer[j+4*2+1];
                   RTC_Hour=SendTempBuffer[j+3*2+1];
                   RTC_Day=SendTempBuffer[j+2*2+1];
                   RTC_Month=SendTempBuffer[j+1*2+1];
                   RTC_Year=SendTempBuffer[j+1];
#endif

			// end of add  2020-06-02 8:52
			// end of add 2015-10-21 5:41
		}
		else if (TempCrc >= cSaveDataFlag03D)
		{
			TempCrc -= cParaActual;
			i = (unsigned char)TempCrc;
			if (i + SendTempBuffer[5] <= cParaEndPos) // add 2016-5-1 15:49
			{
				for (k = i; k < i + SendTempBuffer[5]; k++)
				{
					ParaList[k * 2] = SendTempBuffer[j++];
					ParaList[k * 2 + 1] = SendTempBuffer[j++];
				}
				// if start status change,we should clear the count of all time
				if (StartBak != ParaList[(cSaveDataFlag03D - cParaActual) * 2 + 1])
				{
					StartBak = ParaList[(cSaveDataFlag03D - cParaActual) * 2 + 1];
				}
				//-----------------------------------
				DealAlarm();
				ProgramEepromByte();
				ReadEepromByte();
			}
		}

		TempCrc = CRC(SendTempBuffer, 6);
		SendTempBuffer[6] = TempCrc / 256; // High crc
		SendTempBuffer[7] = TempCrc % 256; // Low crc
		if (SendTempBuffer[0] != 0)

			SendDataToBus1(SendTempBuffer, 8);
	}
}

// add   2021-05-31 13:38
unsigned char CheckModbusRespond_uart1(unsigned char *pp)
{
	unsigned char SetT[6];
	WatchdogReset();

	// singleWrite  06-----------------------------------------------------
	if (pp[1] == cComandWriteSingleUint) // д�����Ĵ���06
	{
		uart1_DealWithSingleRegWrite_06(pp);
		return (cRightStatus);
	}
	// Read  Register  03-----------------------------------------------------
	else if (pp[1] == cComandReadHoldRegister) // �����ּĴ���03
	{
		uart1_DealWithMoreRegRead_03(pp);
		return (cRightStatus);
	}
	// Read  Variable  04-----------------------------------------------------
	else if (pp[1] == cComandReadDiRegister) // �����ݼĴ���04
	{
		uart1_DealWithMoreDiRead_04(pp);
		return (cRightStatus);
	}
	// Read  Variable  16-----------------------------------------------------
	else if (pp[1] == 0x10)
	{
		uart1_DealWithMoreRegWrite_10(pp);
		return (cRightStatus);
	}

	// FunctionCode Wrong------------------------------------------------------
	else
	{
		pp[0] = pp[1] + 0x80;
		pp[1] = 0x01; // Functioncode wrong
		SendDataToBus1(pp, 2);
		return (cCrcWrong);
	}
}

//---------------------------------------------------------------
// Function:  void DealWithSingleRegWrite(uint16_t Addr,unsigned char CommandType)
// Input:    None
// Output:   None
// Description: �ظ�modbus����06д����
//----------------------------------------------------------------
void uart1_DealWithSingleRegWrite_06(unsigned char *SendTempBuffer)
{
	uint16_t i, j;
	uint16_t TempCrc = 0;
	int16_t TEmpInter1, TEmpInter2;
	int8_t SetT[7];
	//   unsigned char   tempflash[2];
	for (i = 0; i < 8; i++)
		SendTempBuffer[i] = SendTempBuffer[i];

	TempCrc = SendTempBuffer[2] * 256;
	TempCrc += SendTempBuffer[3]; // modified 2014-02-21 10:29
	i = TempCrc;

	/*-------------ƫ��ֵ�趨������������ڲ���ƫ��ֵ����Ҫ���ű���=78������²���Ч��
				   �¶�ƫ�������ķ�ΧΪ����300��30�ȣ���ʪ��Ϊ����200��20%��
	*/
	// add 2016-5-1 13:48

	if (i < 2010)
	{
		// add 2021-04-18 11:22

		// end of add 2021-04-18 11:22
		if ((i == cTempOffset) || (i == cTempOffset + 1))
		{
#ifdef _Loramain
			if ((SendTempBuffer[0] >= AddrStart) && (SendTempBuffer[0] < AddrStart + AddrLen) && (ParaList[(cLoraNodeAliveSet + SendTempBuffer[0] - AddrStart) * 2 + 1] == 1)

			)
			{
				ParaList[(i - cTempOffset) * 2 + (SendTempBuffer[0] - AddrStart) * 4 + cLoraNodeOffset * 2] = SendTempBuffer[4];
				ParaList[(i - cTempOffset) * 2 + (SendTempBuffer[0] - AddrStart) * 4 + cLoraNodeOffset * 2 + 1] = SendTempBuffer[5];
				Program();
			}
#else
			ParaList[i * 2] = SendTempBuffer[4];
			ParaList[i * 2 + 1] = SendTempBuffer[5];
			Program();
#endif
		}
		else if ((i == cInterTOffet) || (i == cInterHROffet))
		{

			if (i == cInterTOffet)
			{
				if (ParaList[cDoorVar * 2 + 1] == 78)
				{
					j = SendTempBuffer[4] * 256 + SendTempBuffer[5];

					if ((j < 300) || (j > 65236))
					{
						ParaList[i * 2] = SendTempBuffer[4];
						ParaList[i * 2 + 1] = SendTempBuffer[5];
						Program();
					}
				}
			}
			else
			{
				if (ParaList[cDoorVar * 2 + 1] == 78)
				{
					j = SendTempBuffer[4] * 256 + SendTempBuffer[5];

					if ((j < 200) || (j > 65336))
					{
						ParaList[i * 2] = SendTempBuffer[4];
						ParaList[i * 2 + 1] = SendTempBuffer[5];
						Program();
					}
				}
			}
		}
		//-------------------lora  ��д
		else if ((i == cLoraReadConfigPara) && (SendTempBuffer[5] == 0x01)) // ��ȡlora����
		{
			ParaList[i * 2] = SendTempBuffer[4];
			ParaList[i * 2 + 1] = SendTempBuffer[5];
			ReadOutLoraPara();
		}
		else if ((i == cLoraReadConfigPara) && (SendTempBuffer[5] == 0x00)) // �˳���ȡ
		{

			ParaList[i * 2] = SendTempBuffer[4];
			ParaList[i * 2 + 1] = SendTempBuffer[5];
			SwtichToWorkmode();
		}
		else if ((i == cLoraWriteConfigPara) && (SendTempBuffer[5] == 0x01)) // ����lora����
		{
			ParaList[i * 2] = SendTempBuffer[4];
			ParaList[i * 2 + 1] = SendTempBuffer[5];
			WriteInLoraPara();
		}
		else if ((i == cLoraWriteConfigPara) && (SendTempBuffer[5] == 0x00)) // �˳�����
		{
			ParaList[i * 2] = SendTempBuffer[4];
			ParaList[i * 2 + 1] = SendTempBuffer[5];
			SwtichToWorkmode();
		}

		//-----------�޸��豸��ַ
		else if (i == 0) // �޸��豸��ַ
		{

#ifndef __flood
			ParaList[cDeviceNum * 2] = SendTempBuffer[4];
			ParaList[cDeviceNum * 2 + 1] = SendTempBuffer[5];
			if ((SendTempBuffer[4] == 0) && ((SendTempBuffer[5] != 0) && (SendTempBuffer[5] != 255))) // 2016-01-18 21:19
			{
				ProgramEepromByte();
				ReadEepromByte();
			}
#endif
		}
		//--------------------���г�ʼ������-------------------------
		else if (((i == 26) || (i == cInitial)) && (SendTempBuffer[4] == 0x00) && (SendTempBuffer[5] == 26)) // initial
		{
			ResetFlag = 0; // 2021-02-21 17:33
			InitialPara();
			BellOn();
			DelayMicroSeconds(1000);
			BellOff();
		}
		//--------------ͨ�����õ�ǰ��׼ֵ�������ڲ�ƫ�ʹ��ǰ����ʪ��Ϊ��ǰ���¶�-------
		else if (i == cOrigTS) // t  offset  broadcast set   //2015-09-19 14:01
		{
			if (ParaList[cDoorVar * 2 + 1] == 78)
			{
				TEmpInter1 = (int16_t)(VarList[cTemp * 2] * 256 + VarList[cTemp * 2 + 1]);
				TEmpInter2 = (int16_t)(SendTempBuffer[cTempError * 2] * 256 + SendTempBuffer[cTempError * 2 + 1]);
				TEmpInter2 = TEmpInter2 - TEmpInter1;
				TEmpInter1 = (int16_t)(ParaList[cInterTOffet * 2] * 256 + ParaList[cInterTOffet * 2 + 1]);
				TEmpInter2 = TEmpInter2 + TEmpInter1;
				TempCrc = (uint16_t)TEmpInter2;

				ParaList[cInterTOffet * 2] = TempCrc / 256;
				ParaList[cInterTOffet * 2 + 1] = TempCrc % 256;
				ProgramEepromByte();
				ReadEepromByte();
			}
		}
		//-------------------------------------------------------------
		else if (i == cOrigRHS) // hr  offset  broadcast set   //2015-09-19 14:01
		{
			if (ParaList[cDoorVar * 2 + 1] == 78)
			{
				TEmpInter1 = (int16_t)(VarList[cHumi * 2] * 256 + VarList[cHumi * 2 + 1]);
				TEmpInter2 = (int16_t)(SendTempBuffer[cHumiError * 2] * 256 + SendTempBuffer[cHumiError * 2 + 1]);
				TEmpInter2 = TEmpInter2 - TEmpInter1;
				TEmpInter1 = (int16_t)(ParaList[cInterHROffet * 2] * 256 + ParaList[cInterHROffet * 2 + 1]);
				TEmpInter2 = TEmpInter2 + TEmpInter1;
				TempCrc = (uint16_t)TEmpInter2;

				ParaList[cInterHROffet * 2] = TempCrc / 256;
				ParaList[cInterHROffet * 2 + 1] = TempCrc % 256;
				ProgramEepromByte();
				ReadEepromByte();
			}
		}
		//---------------------------�޸�ʱ��--------------------------
		else if ((i == cTimeTrig) && (SendTempBuffer[4] == 0x00) && (SendTempBuffer[5] == 78)) // initial
		{
			SetT[6] = ParaList[cYear * 2 + 1] / 10 * 16 + ParaList[cYear * 2 + 1] % 10;
			SetT[5] = ParaList[cMonth * 2 + 1] / 10 * 16 + ParaList[cMonth * 2 + 1] % 10;
			SetT[3] = ParaList[cDay * 2 + 1] / 10 * 16 + ParaList[cDay * 2 + 1] % 10;
			SetT[2] = ParaList[cHour * 2 + 1] / 10 * 16 + ParaList[cHour * 2 + 1] % 10;
			SetT[1] = ParaList[cMiniter * 2 + 1] / 10 * 16 + ParaList[cMiniter * 2 + 1] % 10;
			SetT[0] = ParaList[cSecond * 2 + 1] / 10 * 16 + ParaList[cSecond * 2 + 1] % 10;
			SetPCF8563(SetT);
		}
		//---------------------------beep control--------------------------
		else if (i == cRemoteControl)
		{
#ifdef _Loramain
			ParaList[cAlarmControlPos * 2 + (SendTempBuffer[0] - 1) * 2 + 1] = SendTempBuffer[5];
#else
			ParaList[cRemoteControl * 2 + 1] = SendTempBuffer[5];
#endif
		}

		//------------------------special adjust------10��error adjust------------------------------
		else if ((i >= cAllowHumi) && (i < cHumiOff1 + 10))
		{
			if (ParaList[23] == 78)
			{
				ParaList[i * 2] = SendTempBuffer[4];
				ParaList[i * 2 + 1] = SendTempBuffer[5];
				ProgramEepromByte();
				ReadEepromByte();
			}
		}
		//------------------------------------------spi flash  initial------------------------------
		else if (i == cSpiFlashInitial)
		{
			LCD4_Clear();
			LCD_string(1, 1, " Flash Initial  ");
			LCD_string(2, 1, "                ");
			W25QXX_Erase_Chip();
			SpiFlash_WaitReady();
			LCD_string(1, 1, " Flash Initial  ");
			LCD_string(2, 1, "    Over        ");
		}
		else
		{
			if ((i != cReordNumIndex) && (i != cCurrentReordNum) && (i != cCurrentReordPage) && (i != cPagePos)) // 2015-09-19 13:59

			{
				ParaList[i * 2] = SendTempBuffer[4];
				ParaList[i * 2 + 1] = SendTempBuffer[5];
				if ((i < cDO1) && (i > cDI4))
				{
					ProgramEepromByte();
					ReadEepromByte();
				}
				else
				{
					if ((i >= cDO1) && (i <= cDO4))
					{
#ifdef __flood
						DigitalOutput(i - cDO1 + 1, SendTempBuffer[5]);
						ParaList[(cAutoStatusDO1 + i - cDO1) * 2] = 0;
						ParaList[(cAutoStatusDO1 + i - cDO1) * 2 + 1] = 0;
						ParaList[cRuntimeDO1 * 2] = 0;
						ParaList[cRuntimeDO1 * 2 + 1] = 0;
						if (SendTempBuffer[5] == 1)
							PumpBeginCount(i - cDO1 + 1);
						SendFloodStatus();
#endif
					}
				}
			}
		}
	}
	/*���õ���ʪ��10��ƫ��У׼ֵ,�趨�ĵ�ַ��ʼֵ,��ַ����(���16���ڵ�),lora���� 3000���ϵ�ַ�Ĵ���*/
	// 3000---3019
	else if (i >= cTempOff)
	{
		if (ParaList[23] == 78)
		{
			ParaList[(i - c02D_ParaActual) * 2] = SendTempBuffer[4];
			ParaList[(i - c02D_ParaActual) * 2 + 1] = SendTempBuffer[5];
			ProgramEepromByte();
			ReadEepromByte();
		}
	}
	//----------- 03D para------------------
	else if ((i >= 2010) && (i < 3000))
	{

		ParaList[(i - cParaActual) * 2] = SendTempBuffer[4];
		ParaList[(i - cParaActual) * 2 + 1] = SendTempBuffer[5];
		if (StartBak != ParaList[(cSaveDataFlag03D - cParaActual) * 2 + 1])
		{
			StartBak = ParaList[(cSaveDataFlag03D - cParaActual) * 2 + 1];
			if (StartBak == 0) // ����--->ֹͣ
			{
				VarList[cRealDataAct * 2 + 1] = 0x0ff;
				VarList[cRealDataAct * 2] = 0x0ff;
			}
			else // ֹͣ--->����
				History.AlarmStatus1 = 0x00;
		}

		//-----------����Ϊ���������ı��,��ʱ������ض�Ӧ�Ĳ���----------------------
		if (i == cAlarmOutControl03D)
		{
			DealAlarm();
			ProgramEepromByte();
		}
		//-------------------------------------------------//	 record status
		else if (i == cBackLightControl)
		{
			DealBacklight();
		}
		else if (i != cTestMode03D)
		{
			ProgramEepromByte();
			ReadEepromByte();
		}
	}

#ifdef __flood

	SendFloodStatus();
#else
	if (SendTempBuffer[0] != 0)

		SendDataToBus_uart1(SendTempBuffer, 8);
#endif
}

//---------------------------------------------------------------
// Function:  void DealWithMoreRegRead()
// Input:    None
// Output:   None
// Description: �ظ�modbus����03������
//----------------------------------------------------------------
void uart1_DealWithMoreRegRead_03(unsigned char *SendTempBuffer)
{
	uint16_t TempCrc = 0;
	uint16_t i;
	uint16_t k, j = 3;
	unsigned char SendTempBuffer1[300];
	for (i = 0; i < 8; i++)
		SendTempBuffer1[i] = SendTempBuffer[i];

	if (SendTempBuffer1[0] == DeviceNum)
	{
		TempCrc = SendTempBuffer[2] * 256;
		TempCrc += SendTempBuffer[3]; // modified 2014-02-21 10:29
		SendTempBuffer1[0] = DeviceNum;
		SendTempBuffer1[1] = 0x03;
		SendTempBuffer1[2] = SendTempBuffer1[5] * 2;
		if (SendTempBuffer1[5] <= ParaNum / 2)
		{

			//------------------<2010
			if (TempCrc < cSaveDataFlag03D)
			{
				i = (unsigned char)TempCrc;
				for (k = i; k < i + SendTempBuffer1[2] / 2; k++)
				{
					if (k == 0)
					{
						SendTempBuffer1[j++] = ParaList[cDeviceNum * 2];
						SendTempBuffer1[j++] = ParaList[cDeviceNum * 2 + 1];
					}

					else
					{
						SendTempBuffer1[j++] = ParaList[k * 2];
						SendTempBuffer1[j++] = ParaList[k * 2 + 1];
					}
				}
			}
			//----------------------->=3000
			else if (TempCrc >= cTempOff)
			{
				TempCrc -= c02D_ParaActual;
				i = (unsigned char)TempCrc;
				for (k = i; k < i + SendTempBuffer1[2] / 2; k++)
				{
					SendTempBuffer1[j++] = ParaList[k * 2];
					SendTempBuffer1[j++] = ParaList[k * 2 + 1];
				}
			}

			else if (TempCrc >= cSaveDataFlag03D)
			{
				TempCrc -= cParaActual;
				i = (unsigned char)TempCrc;
				for (k = i; k < i + SendTempBuffer1[2] / 2; k++)
				{
					SendTempBuffer1[j++] = ParaList[k * 2];
					SendTempBuffer1[j++] = ParaList[k * 2 + 1];
				}
			}

			TempCrc = CRC(SendTempBuffer1, j);
			SendTempBuffer1[j++] = TempCrc / 256; // High crc
			SendTempBuffer1[j++] = TempCrc % 256; // Low crc
			SendDataToBus_uart1(SendTempBuffer1, j);
		}
	}
	else // for read  offset para for lora node
	{
		if ((SendTempBuffer[0] >= AddrStart) && (SendTempBuffer[0] < AddrStart + AddrLen) && (ParaList[(cLoraNodeAliveSet + SendTempBuffer[0] - AddrStart) * 2 + 1] == 1))
		{
			TempCrc = SendTempBuffer[2] * 256;
			TempCrc += SendTempBuffer[3]; // modified 2014-02-21 10:29
			SendTempBuffer1[0] = SendTempBuffer[0];
			SendTempBuffer1[1] = 0x03;
			SendTempBuffer1[2] = SendTempBuffer1[5] * 2;
			i = (unsigned char)TempCrc;
			for (k = i; k < i + SendTempBuffer1[2] / 2; k++)
			{
				SendTempBuffer1[j++] = ParaList[(k - cTempOffset) * 2 + (SendTempBuffer[0] - AddrStart) * 4 + cLoraNodeOffset * 2]; // ��ʪ��λ��+�豸��Ӧλ��+ƫ��baseλ��
				SendTempBuffer1[j++] = ParaList[(k - cTempOffset) * 2 + (SendTempBuffer[0] - AddrStart) * 4 + cLoraNodeOffset * 2 + 1];
			}
			TempCrc = CRC(SendTempBuffer1, j);
			SendTempBuffer1[j++] = TempCrc / 256; // High crc
			SendTempBuffer1[j++] = TempCrc % 256; // Low crc
			SendDataToBus_uart1(SendTempBuffer1, j);
		}
	}
}

//---------------------------------------------------------------
// Function:  void DealWithMoreDiRead()
// Input:    None
// Output:   None
// Description: �ظ�modbus����04������
//----------------------------------------------------------------
void uart1_DealWithMoreDiRead_04(unsigned char *SendTempBuffer)
{
	uint16_t TempCrc = 0;
	unsigned char i;
	unsigned char k, j = 3;
	WatchdogReset();

	TempCrc = SendTempBuffer[2] * 256;
	TempCrc += SendTempBuffer[3]; // addr
	SendTempBuffer[0] = SendTempBuffer[0];
	SendTempBuffer[1] = 0x04;
	SendTempBuffer[2] = SendTempBuffer[5] * 2;

	if (TempCrc < cModifiedTime)
	{
		if ((SendTempBuffer[0] != DeviceNum) && ((SendTempBuffer[5] == 3) || (SendTempBuffer[5] == 2)) && (ParaList[(cLoraNodeAliveSet + SendTempBuffer[0] - AddrStart) * 2 + 1] == 1)) // read var len
			TempCrc += cTempStartAddr + (SendTempBuffer[0] - AddrStart) * 2;																											// lora gateway
		else
			TempCrc += cTemp;
	}
	else if (TempCrc >= cRealData) // 03d   rht
	{
		TempCrc = TempCrc - cRealData + cRealDataAct;
	}
	else if ((TempCrc >= cModifiedTime) && (TempCrc < cTempStartAddr))
	{
		TempCrc -= cModifiedTime; // 03d time
		TempCrc += 2;
	}
	i = (unsigned char)TempCrc;

	for (k = i; k < i + SendTempBuffer[2] / 2; k++)
	{
		WatchdogReset();
		SendTempBuffer[j++] = VarList[k * 2];
		SendTempBuffer[j++] = VarList[k * 2 + 1];
	}
	TempCrc = CRC(SendTempBuffer, j);
	SendTempBuffer[j++] = TempCrc / 256; // High crc
	SendTempBuffer[j++] = TempCrc % 256; // Low crc
	SendDataToBus_uart1(SendTempBuffer, j);
}

//---------------------------------------------------------------
// Function:  void DealWithSingleRegWrite(uint16_t Addr,unsigned char CommandType)
// Input:    None
// Output:   None
// Description: �ظ�modbus����0x10д����
//----------------------------------------------------------------
void uart1_DealWithMoreRegWrite_10(unsigned char *SendTempBuffer)
{
	unsigned char i;
	unsigned char j, k;
	unsigned char SetT[6]; // 2015-10-21 5:41
	uint16_t TempCrc = 0;
	//  for(i=0;i<40;i++)
	//	  SendTempBuffer[i]=SendTempBufferBak[i];

	TempCrc = SendTempBuffer[2] * 256;
	TempCrc += SendTempBuffer[3]; // base addr
	if (SendTempBuffer[5] < cReceivelength / 2)
	{
		i = (unsigned char)TempCrc;
		j = 7;

		if (TempCrc == cModifiedTime)

		{
			SetT[0] = SendTempBuffer[j + 5 * 2 + 1] / 10 * 16 + SendTempBuffer[j + 5 * 2 + 1] % 10;
			SetT[1] = SendTempBuffer[j + 4 * 2 + 1] / 10 * 16 + SendTempBuffer[j + 4 * 2 + 1] % 10;
			SetT[2] = SendTempBuffer[j + 3 * 2 + 1] / 10 * 16 + SendTempBuffer[j + 3 * 2 + 1] % 10;
			SetT[3] = SendTempBuffer[j + 2 * 2 + 1] / 10 * 16 + SendTempBuffer[j + 2 * 2 + 1] % 10;
			SetT[5] = SendTempBuffer[j + 1 * 2 + 1] / 10 * 16 + SendTempBuffer[j + 1 * 2 + 1] % 10;
			SetT[6] = SendTempBuffer[j + 1] / 10 * 16 + SendTempBuffer[j + 1] % 10;
			SetPCF8563(SetT);
			// add  2020-06-02 8:51
#if 0 
  	      	   RTC_Second=SendTempBuffer[j+5*2+1];
                   RTC_Minite=SendTempBuffer[j+4*2+1];
                   RTC_Hour=SendTempBuffer[j+3*2+1];
                   RTC_Day=SendTempBuffer[j+2*2+1];
                   RTC_Month=SendTempBuffer[j+1*2+1];
                   RTC_Year=SendTempBuffer[j+1];
#endif

			// end of add  2020-06-02 8:52
			// end of add 2015-10-21 5:41
		}
		else if (TempCrc >= cSaveDataFlag03D)
		{
			TempCrc -= cParaActual;
			i = (unsigned char)TempCrc;
			if (i + SendTempBuffer[5] <= cParaEndPos) // add 2016-5-1 15:49
			{
				for (k = i; k < i + SendTempBuffer[5]; k++)
				{
					ParaList[k * 2] = SendTempBuffer[j++];
					ParaList[k * 2 + 1] = SendTempBuffer[j++];
				}
				// if start status change,we should clear the count of all time
				if (StartBak != ParaList[(cSaveDataFlag03D - cParaActual) * 2 + 1])
				{
					StartBak = ParaList[(cSaveDataFlag03D - cParaActual) * 2 + 1];
				}
				//-----------------------------------
				DealAlarm();
				ProgramEepromByte();
				ReadEepromByte();
			}
		}

		TempCrc = CRC(SendTempBuffer, 6);
		SendTempBuffer[6] = TempCrc / 256; // High crc
		SendTempBuffer[7] = TempCrc % 256; // Low crc
		if (SendTempBuffer[0] != 0)

			SendDataToBus_uart1(SendTempBuffer, 8);
	}
}

unsigned char CheckModbusRespond_uart3(unsigned char *pp)
{
	unsigned char SetT[6];
	WatchdogReset();

	// singleWrite  06-----------------------------------------------------
	if (pp[1] == cComandWriteSingleUint) // д�����Ĵ���06
	{
		uart3_DealWithSingleRegWrite_06(pp);
		return (cRightStatus);
	}
	// Read  Register  03-----------------------------------------------------
	else if (pp[1] == cComandReadHoldRegister) // �����ּĴ���03
	{
		uart3_DealWithMoreRegRead_03(pp);
		return (cRightStatus);
	}
	// Read  Variable  04-----------------------------------------------------
	else if (pp[1] == cComandReadDiRegister) // �����ݼĴ���04
	{
		uart3_DealWithMoreDiRead_04(pp);
		return (cRightStatus);
	}
	// Read  Variable  16-----------------------------------------------------
	else if (pp[1] == 0x10)
	{
		uart3_DealWithMoreRegWrite_10(pp);
		return (cRightStatus);
	}

	// FunctionCode Wrong------------------------------------------------------
	else
	{
		pp[0] = pp[1] + 0x80;
		pp[1] = 0x01; // Functioncode wrong
		SendDataToBus1(pp, 2);
		return (cCrcWrong);
	}
}

//---------------------------------------------------------------
// Function:  void DealWithSingleRegWrite(uint16_t Addr,unsigned char CommandType)
// Input:    None
// Output:   None
// Description: �ظ�modbus����06д����
//----------------------------------------------------------------
void uart3_DealWithSingleRegWrite_06(unsigned char *SendTempBuffer)
{
	uint16_t i, j;
	uint16_t TempCrc = 0;
	int16_t TEmpInter1, TEmpInter2;
	int8_t SetT[7];
	//   unsigned char   tempflash[2];
	for (i = 0; i < 8; i++)
		SendTempBuffer[i] = SendTempBuffer[i];

	TempCrc = SendTempBuffer[2] * 256;
	TempCrc += SendTempBuffer[3]; // modified 2014-02-21 10:29
	i = TempCrc;

	/*-------------ƫ��ֵ�趨������������ڲ���ƫ��ֵ����Ҫ���ű���=78������²���Ч��
				   �¶�ƫ�������ķ�ΧΪ����300��30�ȣ���ʪ��Ϊ����200��20%��
	*/
	// add 2016-5-1 13:48

	if (i < 2010)
	{
		// add 2021-04-18 11:22

		// end of add 2021-04-18 11:22
		if ((i == cTempOffset) || (i == cTempOffset + 1))
		{
#ifdef _Loramain
			if ((SendTempBuffer[0] >= AddrStart) && (SendTempBuffer[0] < AddrStart + AddrLen) && (ParaList[(cLoraNodeAliveSet + SendTempBuffer[0] - AddrStart) * 2 + 1] == 1)

			)
			{
				ParaList[(i - cTempOffset) * 2 + (SendTempBuffer[0] - AddrStart) * 4 + cLoraNodeOffset * 2] = SendTempBuffer[4];
				ParaList[(i - cTempOffset) * 2 + (SendTempBuffer[0] - AddrStart) * 4 + cLoraNodeOffset * 2 + 1] = SendTempBuffer[5];
				Program();
			}
#else
			ParaList[i * 2] = SendTempBuffer[4];
			ParaList[i * 2 + 1] = SendTempBuffer[5];
			Program();
#endif
		}
		else if ((i == cInterTOffet) || (i == cInterHROffet))
		{

			if (i == cInterTOffet)
			{
				if (ParaList[cDoorVar * 2 + 1] == 78)
				{
					j = SendTempBuffer[4] * 256 + SendTempBuffer[5];

					if ((j < 300) || (j > 65236))
					{
						ParaList[i * 2] = SendTempBuffer[4];
						ParaList[i * 2 + 1] = SendTempBuffer[5];
						Program();
					}
				}
			}
			else
			{
				if (ParaList[cDoorVar * 2 + 1] == 78)
				{
					j = SendTempBuffer[4] * 256 + SendTempBuffer[5];

					if ((j < 200) || (j > 65336))
					{
						ParaList[i * 2] = SendTempBuffer[4];
						ParaList[i * 2 + 1] = SendTempBuffer[5];
						Program();
					}
				}
			}
		}
		//-------------------lora  ��д
		else if ((i == cLoraReadConfigPara) && (SendTempBuffer[5] == 0x01)) // ��ȡlora����
		{
			ParaList[i * 2] = SendTempBuffer[4];
			ParaList[i * 2 + 1] = SendTempBuffer[5];
			ReadOutLoraPara();
		}
		else if ((i == cLoraReadConfigPara) && (SendTempBuffer[5] == 0x00)) // �˳���ȡ
		{

			ParaList[i * 2] = SendTempBuffer[4];
			ParaList[i * 2 + 1] = SendTempBuffer[5];
			SwtichToWorkmode();
		}
		else if ((i == cLoraWriteConfigPara) && (SendTempBuffer[5] == 0x01)) // ����lora����
		{
			ParaList[i * 2] = SendTempBuffer[4];
			ParaList[i * 2 + 1] = SendTempBuffer[5];
			WriteInLoraPara();
		}
		else if ((i == cLoraWriteConfigPara) && (SendTempBuffer[5] == 0x00)) // �˳�����
		{
			ParaList[i * 2] = SendTempBuffer[4];
			ParaList[i * 2 + 1] = SendTempBuffer[5];
			SwtichToWorkmode();
		}

		//-----------�޸��豸��ַ
		else if (i == 0) // �޸��豸��ַ
		{

#ifndef __flood
			ParaList[cDeviceNum * 2] = SendTempBuffer[4];
			ParaList[cDeviceNum * 2 + 1] = SendTempBuffer[5];
			if ((SendTempBuffer[4] == 0) && ((SendTempBuffer[5] != 0) && (SendTempBuffer[5] != 255))) // 2016-01-18 21:19
			{
				ProgramEepromByte();
				ReadEepromByte();
			}
#endif
		}
		//--------------------���г�ʼ������-------------------------
		else if (((i == 26) || (i == cInitial)) && (SendTempBuffer[4] == 0x00) && (SendTempBuffer[5] == 26)) // initial
		{
			ResetFlag = 0; // 2021-02-21 17:33
			InitialPara();
			BellOn();
			DelayMicroSeconds(1000);
			BellOff();
		}
		//--------------ͨ�����õ�ǰ��׼ֵ�������ڲ�ƫ�ʹ��ǰ����ʪ��Ϊ��ǰ���¶�-------
		else if (i == cOrigTS) // t  offset  broadcast set   //2015-09-19 14:01
		{
			if (ParaList[cDoorVar * 2 + 1] == 78)
			{
				TEmpInter1 = (int16_t)(VarList[cTemp * 2] * 256 + VarList[cTemp * 2 + 1]);
				TEmpInter2 = (int16_t)(SendTempBuffer[cTempError * 2] * 256 + SendTempBuffer[cTempError * 2 + 1]);
				TEmpInter2 = TEmpInter2 - TEmpInter1;
				TEmpInter1 = (int16_t)(ParaList[cInterTOffet * 2] * 256 + ParaList[cInterTOffet * 2 + 1]);
				TEmpInter2 = TEmpInter2 + TEmpInter1;
				TempCrc = (uint16_t)TEmpInter2;

				ParaList[cInterTOffet * 2] = TempCrc / 256;
				ParaList[cInterTOffet * 2 + 1] = TempCrc % 256;
				ProgramEepromByte();
				ReadEepromByte();
			}
		}
		//-------------------------------------------------------------
		else if (i == cOrigRHS) // hr  offset  broadcast set   //2015-09-19 14:01
		{
			if (ParaList[cDoorVar * 2 + 1] == 78)
			{
				TEmpInter1 = (int16_t)(VarList[cHumi * 2] * 256 + VarList[cHumi * 2 + 1]);
				TEmpInter2 = (int16_t)(SendTempBuffer[cHumiError * 2] * 256 + SendTempBuffer[cHumiError * 2 + 1]);
				TEmpInter2 = TEmpInter2 - TEmpInter1;
				TEmpInter1 = (int16_t)(ParaList[cInterHROffet * 2] * 256 + ParaList[cInterHROffet * 2 + 1]);
				TEmpInter2 = TEmpInter2 + TEmpInter1;
				TempCrc = (uint16_t)TEmpInter2;

				ParaList[cInterHROffet * 2] = TempCrc / 256;
				ParaList[cInterHROffet * 2 + 1] = TempCrc % 256;
				ProgramEepromByte();
				ReadEepromByte();
			}
		}
		//---------------------------�޸�ʱ��--------------------------
		else if ((i == cTimeTrig) && (SendTempBuffer[4] == 0x00) && (SendTempBuffer[5] == 78)) // initial
		{
			SetT[6] = ParaList[cYear * 2 + 1] / 10 * 16 + ParaList[cYear * 2 + 1] % 10;
			SetT[5] = ParaList[cMonth * 2 + 1] / 10 * 16 + ParaList[cMonth * 2 + 1] % 10;
			SetT[3] = ParaList[cDay * 2 + 1] / 10 * 16 + ParaList[cDay * 2 + 1] % 10;
			SetT[2] = ParaList[cHour * 2 + 1] / 10 * 16 + ParaList[cHour * 2 + 1] % 10;
			SetT[1] = ParaList[cMiniter * 2 + 1] / 10 * 16 + ParaList[cMiniter * 2 + 1] % 10;
			SetT[0] = ParaList[cSecond * 2 + 1] / 10 * 16 + ParaList[cSecond * 2 + 1] % 10;
			SetPCF8563(SetT);
		}
		//---------------------------beep control--------------------------
		else if (i == cRemoteControl)
		{
#ifdef _Loramain
			ParaList[cAlarmControlPos * 2 + (SendTempBuffer[0] - 1) * 2 + 1] = SendTempBuffer[5];
#else
			ParaList[cRemoteControl * 2 + 1] = SendTempBuffer[5];
#endif
		}

		//------------------------special adjust------10��error adjust------------------------------
		else if ((i >= cAllowHumi) && (i < cHumiOff1 + 10))
		{
			if (ParaList[23] == 78)
			{
				ParaList[i * 2] = SendTempBuffer[4];
				ParaList[i * 2 + 1] = SendTempBuffer[5];
				ProgramEepromByte();
				ReadEepromByte();
			}
		}
		//------------------------------------------spi flash  initial------------------------------
		else if (i == cSpiFlashInitial)
		{
			LCD4_Clear();
			LCD_string(1, 1, " Flash Initial  ");
			LCD_string(2, 1, "                ");
			W25QXX_Erase_Chip();
			SpiFlash_WaitReady();
			LCD_string(1, 1, " Flash Initial  ");
			LCD_string(2, 1, "    Over        ");
		}
		else
		{
			if ((i != cReordNumIndex) && (i != cCurrentReordNum) && (i != cCurrentReordPage) && (i != cPagePos)) // 2015-09-19 13:59

			{
				ParaList[i * 2] = SendTempBuffer[4];
				ParaList[i * 2 + 1] = SendTempBuffer[5];
				if ((i < cDO1) && (i > cDI4))
				{
					ProgramEepromByte();
					ReadEepromByte();
				}
				else
				{
					if ((i >= cDO1) && (i <= cDO4))
					{
#ifdef __flood
						DigitalOutput(i - cDO1 + 1, SendTempBuffer[5]);
						ParaList[(cAutoStatusDO1 + i - cDO1) * 2] = 0;
						ParaList[(cAutoStatusDO1 + i - cDO1) * 2 + 1] = 0;
						ParaList[cRuntimeDO1 * 2] = 0;
						ParaList[cRuntimeDO1 * 2 + 1] = 0;
						if (SendTempBuffer[5] == 1)
							PumpBeginCount(i - cDO1 + 1);
						SendFloodStatus();
#endif
					}
				}
			}
		}
	}
	/*���õ���ʪ��10��ƫ��У׼ֵ,�趨�ĵ�ַ��ʼֵ,��ַ����(���16���ڵ�),lora���� 3000���ϵ�ַ�Ĵ���*/
	// 3000---3019
	else if (i >= cTempOff)
	{
		if (ParaList[23] == 78)
		{
			ParaList[(i - c02D_ParaActual) * 2] = SendTempBuffer[4];
			ParaList[(i - c02D_ParaActual) * 2 + 1] = SendTempBuffer[5];
			ProgramEepromByte();
			ReadEepromByte();
		}
	}
	//----------- 03D para------------------
	else if ((i >= 2010) && (i < 3000))
	{

		ParaList[(i - cParaActual) * 2] = SendTempBuffer[4];
		ParaList[(i - cParaActual) * 2 + 1] = SendTempBuffer[5];
		if (StartBak != ParaList[(cSaveDataFlag03D - cParaActual) * 2 + 1])
		{
			StartBak = ParaList[(cSaveDataFlag03D - cParaActual) * 2 + 1];
			if (StartBak == 0) // ����--->ֹͣ
			{
				VarList[cRealDataAct * 2 + 1] = 0x0ff;
				VarList[cRealDataAct * 2] = 0x0ff;
			}
			else // ֹͣ--->����
				History.AlarmStatus1 = 0x00;
		}

		//-----------����Ϊ���������ı��,��ʱ������ض�Ӧ�Ĳ���----------------------
		if (i == cAlarmOutControl03D)
		{
			DealAlarm();
			ProgramEepromByte();
		}
		//-------------------------------------------------//	 record status
		else if (i == cBackLightControl)
		{
			DealBacklight();
		}
		else if (i != cTestMode03D)
		{
			ProgramEepromByte();
			ReadEepromByte();
		}
	}

#ifdef __flood

	SendFloodStatus();
#else
	if (SendTempBuffer[0] != 0)

		SendDataToBus_uart3(SendTempBuffer, 8);
#endif
}

//---------------------------------------------------------------
// Function:  void DealWithMoreRegRead()
// Input:    None
// Output:   None
// Description: �ظ�modbus����03������
//----------------------------------------------------------------
void uart3_DealWithMoreRegRead_03(unsigned char *SendTempBuffer)
{
	uint16_t TempCrc = 0;
	uint16_t i;
	uint16_t k, j = 3;
	unsigned char SendTempBuffer1[300];
	for (i = 0; i < 8; i++)
		SendTempBuffer1[i] = SendTempBuffer[i];

	if (SendTempBuffer1[0] == DeviceNum)
	{
		TempCrc = SendTempBuffer[2] * 256;
		TempCrc += SendTempBuffer[3]; // modified 2014-02-21 10:29
		SendTempBuffer1[0] = DeviceNum;
		SendTempBuffer1[1] = 0x03;
		SendTempBuffer1[2] = SendTempBuffer1[5] * 2;
		if (SendTempBuffer1[5] <= ParaNum / 2)
		{

			//------------------<2010
			if (TempCrc < cSaveDataFlag03D)
			{
				i = (unsigned char)TempCrc;
				for (k = i; k < i + SendTempBuffer1[2] / 2; k++)
				{
					if (k == 0)
					{
						SendTempBuffer1[j++] = ParaList[cDeviceNum * 2];
						SendTempBuffer1[j++] = ParaList[cDeviceNum * 2 + 1];
					}

					else
					{
						SendTempBuffer1[j++] = ParaList[k * 2];
						SendTempBuffer1[j++] = ParaList[k * 2 + 1];
					}
				}
			}
			//----------------------->=3000
			else if (TempCrc >= cTempOff)
			{
				TempCrc -= c02D_ParaActual;
				i = (unsigned char)TempCrc;
				for (k = i; k < i + SendTempBuffer1[2] / 2; k++)
				{
					SendTempBuffer1[j++] = ParaList[k * 2];
					SendTempBuffer1[j++] = ParaList[k * 2 + 1];
				}
			}

			else if (TempCrc >= cSaveDataFlag03D)
			{
				TempCrc -= cParaActual;
				i = (unsigned char)TempCrc;
				for (k = i; k < i + SendTempBuffer1[2] / 2; k++)
				{
					SendTempBuffer1[j++] = ParaList[k * 2];
					SendTempBuffer1[j++] = ParaList[k * 2 + 1];
				}
			}

			TempCrc = CRC(SendTempBuffer1, j);
			SendTempBuffer1[j++] = TempCrc / 256; // High crc
			SendTempBuffer1[j++] = TempCrc % 256; // Low crc
			SendDataToBus_uart3(SendTempBuffer1, j);
		}
	}
	else // for read  offset para for lora node
	{
		if ((SendTempBuffer[0] >= AddrStart) && (SendTempBuffer[0] < AddrStart + AddrLen) && (ParaList[(cLoraNodeAliveSet + SendTempBuffer[0] - AddrStart) * 2 + 1] == 1))
		{
			TempCrc = SendTempBuffer[2] * 256;
			TempCrc += SendTempBuffer[3]; // modified 2014-02-21 10:29
			SendTempBuffer1[0] = SendTempBuffer[0];
			SendTempBuffer1[1] = 0x03;
			SendTempBuffer1[2] = SendTempBuffer1[5] * 2;
			i = (unsigned char)TempCrc;
			for (k = i; k < i + SendTempBuffer1[2] / 2; k++)
			{
				SendTempBuffer1[j++] = ParaList[(k - cTempOffset) * 2 + (SendTempBuffer[0] - AddrStart) * 4 + cLoraNodeOffset * 2]; // ��ʪ��λ��+�豸��Ӧλ��+ƫ��baseλ��
				SendTempBuffer1[j++] = ParaList[(k - cTempOffset) * 2 + (SendTempBuffer[0] - AddrStart) * 4 + cLoraNodeOffset * 2 + 1];
			}
			TempCrc = CRC(SendTempBuffer1, j);
			SendTempBuffer1[j++] = TempCrc / 256; // High crc
			SendTempBuffer1[j++] = TempCrc % 256; // Low crc
			SendDataToBus_uart3(SendTempBuffer1, j);
		}
	}
}

//---------------------------------------------------------------
// Function:  void DealWithMoreDiRead()
// Input:    None
// Output:   None
// Description: �ظ�modbus����04������
//----------------------------------------------------------------
void uart3_DealWithMoreDiRead_04(unsigned char *SendTempBuffer)
{
	uint16_t TempCrc = 0;
	unsigned char i;
	unsigned char k, j = 3;
	WatchdogReset();

	TempCrc = SendTempBuffer[2] * 256;
	TempCrc += SendTempBuffer[3]; // addr
	SendTempBuffer[0] = SendTempBuffer[0];
	SendTempBuffer[1] = 0x04;
	SendTempBuffer[2] = SendTempBuffer[5] * 2;

	i = (unsigned char)TempCrc;

	for (k = i; k < i + SendTempBuffer[2] / 2; k++)
	{
		WatchdogReset();
		if ((k >= cNodePower) && (k < cNodePower + 16))
		{
			SendTempBuffer[j++] = VarList[(cNodePower + (k - cNodePower) * 2) * 2 + 1];
			SendTempBuffer[j++] = VarList[(cNodePower + (k - cNodePower) * 2) * 2 + 3];
		}
		else

		{
			SendTempBuffer[j++] = VarList[k * 2];
			SendTempBuffer[j++] = VarList[k * 2 + 1];
		}
	}

	TempCrc = CRC(SendTempBuffer, j);
	SendTempBuffer[j++] = TempCrc / 256; // High crc
	SendTempBuffer[j++] = TempCrc % 256; // Low crc
	SendDataToBus_uart3(SendTempBuffer, j);
}

//---------------------------------------------------------------
// Function:  void DealWithSingleRegWrite(uint16_t Addr,unsigned char CommandType)
// Input:    None
// Output:   None
// Description: �ظ�modbus����0x10д����
//----------------------------------------------------------------
void uart3_DealWithMoreRegWrite_10(unsigned char *SendTempBuffer)
{
	unsigned char i;
	unsigned char j, k;
	unsigned char SetT[6]; // 2015-10-21 5:41
	uint16_t TempCrc = 0;
	//  for(i=0;i<40;i++)
	//	  SendTempBuffer[i]=SendTempBufferBak[i];

	TempCrc = SendTempBuffer[2] * 256;
	TempCrc += SendTempBuffer[3]; // base addr
	if (SendTempBuffer[5] < cReceivelength / 2)
	{
		i = (unsigned char)TempCrc;
		j = 7;

		if (TempCrc == cModifiedTime)

		{
			SetT[0] = SendTempBuffer[j + 5 * 2 + 1] / 10 * 16 + SendTempBuffer[j + 5 * 2 + 1] % 10;
			SetT[1] = SendTempBuffer[j + 4 * 2 + 1] / 10 * 16 + SendTempBuffer[j + 4 * 2 + 1] % 10;
			SetT[2] = SendTempBuffer[j + 3 * 2 + 1] / 10 * 16 + SendTempBuffer[j + 3 * 2 + 1] % 10;
			SetT[3] = SendTempBuffer[j + 2 * 2 + 1] / 10 * 16 + SendTempBuffer[j + 2 * 2 + 1] % 10;
			SetT[5] = SendTempBuffer[j + 1 * 2 + 1] / 10 * 16 + SendTempBuffer[j + 1 * 2 + 1] % 10;
			SetT[6] = SendTempBuffer[j + 1] / 10 * 16 + SendTempBuffer[j + 1] % 10;
			SetPCF8563(SetT);
			// add  2020-06-02 8:51
#if 0 
  	      	   RTC_Second=SendTempBuffer[j+5*2+1];
                   RTC_Minite=SendTempBuffer[j+4*2+1];
                   RTC_Hour=SendTempBuffer[j+3*2+1];
                   RTC_Day=SendTempBuffer[j+2*2+1];
                   RTC_Month=SendTempBuffer[j+1*2+1];
                   RTC_Year=SendTempBuffer[j+1];
#endif

			// end of add  2020-06-02 8:52
			// end of add 2015-10-21 5:41
		}
		else if (TempCrc >= cSaveDataFlag03D)
		{
			TempCrc -= cParaActual;
			i = (unsigned char)TempCrc;
			if (i + SendTempBuffer[5] <= cParaEndPos) // add 2016-5-1 15:49
			{
				for (k = i; k < i + SendTempBuffer[5]; k++)
				{
					ParaList[k * 2] = SendTempBuffer[j++];
					ParaList[k * 2 + 1] = SendTempBuffer[j++];
				}
				// if start status change,we should clear the count of all time
				if (StartBak != ParaList[(cSaveDataFlag03D - cParaActual) * 2 + 1])
				{
					StartBak = ParaList[(cSaveDataFlag03D - cParaActual) * 2 + 1];
				}
				//-----------------------------------
				DealAlarm();
				ProgramEepromByte();
				ReadEepromByte();
			}
		}

		TempCrc = CRC(SendTempBuffer, 6);
		SendTempBuffer[6] = TempCrc / 256; // High crc
		SendTempBuffer[7] = TempCrc % 256; // Low crc
		if (SendTempBuffer[0] != 0)

			SendDataToBus_uart3(SendTempBuffer, 8);
	}
}
