/*****************************************************************************
  文件: main.c
  作者: Zhengyu https://gzwelink.taobao.com
  版本: V1.0.0
  时间: 20200401
	平台:MINI-STM32F103C8T6

*******************************************************************************/
#include "delay.h"
#include "sys.h"
#include "oled.h"
#include "rc522.h"

//SPI引脚初始化
void GPIO_Config_Init(void)
{
///////////////SPI1  CS引脚初始化////////////////////////////////////////////
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	 //使能PA端口时钟使能
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;				 //PA4 端口配置
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//IO口速度为50MHz
	GPIO_Init(GPIOA, &GPIO_InitStructure);					 //根据设定参数初始化GPIOA4
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);	 //AF时钟使能

///////////////SPI1引脚初始化/////////////////////////////////////////////////
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);


	
}
void RCC_Configuration(void)
{
  /* PCLK2 = HCLK/2 */
  RCC_PCLK2Config(RCC_HCLK_Div2); 
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1 | RCC_APB2Periph_GPIOA , ENABLE);
}
//SPI1配置初始化
void SPI_Config_init(void)
{
	SPI_InitTypeDef   SPI_InitStructure;
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;
  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
  SPI_InitStructure.SPI_CRCPolynomial = 7;
  SPI_Init(SPI1, &SPI_InitStructure);
	SPI_Cmd(SPI1, ENABLE);

}

uint8_t 	k;
uint8_t 	i;
uint8_t 	j;
uint8_t 	b;
uint8_t 	comand;
uint8_t		text2[9] = "Card ID: ";
uint8_t		end[1] = "\r";
uint8_t		txBuffer[18] = "00000000";
uint8_t 	retstr[10];
uint8_t 	rxBuffer[8];
uint8_t		lastID[4];
uint8_t		memID[8] = "9C55A1B5";
uint8_t		str[MFRC522_MAX_LEN];																						// MFRC522_MAX_LEN = 16
uint8_t MFRC522_Check(uint8_t* id);
uint8_t MFRC522_Compare(uint8_t* CardID, uint8_t* CompareID);
void MFRC522_WriteRegister(uint8_t addr, uint8_t val);
uint8_t MFRC522_ReadRegister(uint8_t addr);
void MFRC522_SetBitMask(uint8_t reg, uint8_t mask);
void MFRC522_ClearBitMask(uint8_t reg, uint8_t mask);
uint8_t MFRC522_Request(uint8_t reqMode, uint8_t* TagType);
uint8_t MFRC522_ToCard(uint8_t command, uint8_t* sendData, uint8_t sendLen, uint8_t* backData, uint16_t* backLen);
uint8_t MFRC522_Anticoll(uint8_t* serNum);
void MFRC522_CalulateCRC(uint8_t* pIndata, uint8_t len, uint8_t* pOutData);
uint8_t MFRC522_SelectTag(uint8_t* serNum);
uint8_t MFRC522_Auth(uint8_t authMode, uint8_t BlockAddr, uint8_t* Sectorkey, uint8_t* serNum);
uint8_t MFRC522_Read(uint8_t blockAddr, uint8_t* recvData);
uint8_t MFRC522_Write(uint8_t blockAddr, uint8_t* writeData);
void MFRC522_Init(void);
void MFRC522_Reset(void);
void MFRC522_AntennaOn(void);
void MFRC522_AntennaOff(void);
void MFRC522_Halt(void);
/* USER CODE END PFP */

/* USER CODE BEGIN 0 */

// char number to string hex (FF) (Only big letters!)
void char_to_hex(uint8_t data) {
	uint8_t digits[] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
	
	if (data < 16) {
		retstr[0] = '0';
		retstr[1] = digits[data];
	} else {
		retstr[0] = digits[(data & 0xF0)>>4];
		retstr[1] = digits[(data & 0x0F)];
	}
}

//主函数，采用外部8M晶振，72M系统主频，可以在void SetSysClock(void)函数中选择主频率设置
int main(void)
 {	
	
	delay_init();	    	//延时函数初始化,通过Systick中断实现1ms延时功能  
	GPIO_Config_Init();	//初始化GPIO
	RCC_Configuration();//配置SPI时钟
	SPI_Config_init();//SPI1
	OLED_Init();//OLED初始化  
	OLED_Clear();//清屏
	MFRC522_Init();//RC522初始化

  while (1)
  {
		OLED_Clear();//清屏
    OLED_ShowCHinese(18,0,0);//光
		OLED_ShowCHinese(36,0,1);//子
		OLED_ShowCHinese(54,0,2);//物
		OLED_ShowCHinese(72,0,3);//联
		OLED_ShowCHinese(90,0,4);//网
		OLED_ShowString(6,3,"NFC RC522 TEST");//显示字符串
    OLED_ShowString(3,6,"ID:");//显示字符串
		OLED_ShowString(30,6,txBuffer);//显示读到的ID值

	if (!MFRC522_Request(PICC_REQIDL, str)) {//寻卡
			if (!MFRC522_Anticoll(str)) {//获得卡序列号
				j = 0;
				b = 0;
				for (i=0; i<4; i++) if (lastID[i] != str[i]) j = 1;								// 如果读到ID不同，需要显示更新
				if (j) {
				
					for (i=0; i<4; i++) lastID[i] = str[i];
					for (i=0; i<4; i++) {
						char_to_hex(str[i]);
						txBuffer[b] = retstr[0];//序列号存在txBuffer中
						b++;
						txBuffer[b] = retstr[1];
						b++;
					}										
				
				}
			}
		}
  }
 }

