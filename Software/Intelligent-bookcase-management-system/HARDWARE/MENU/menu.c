#include "oled.h"
#include "menu.h"
#include "rc522_config.h"
#include "rc522_function.h"
#include "keypad.h"
#include "stm32f10x.h"
#include "delay.h"
#include "usart.h"
#include "stdio.h"
typedef struct
{
	uint8_t BookRFID[4]; /*先后存放IC卡的类型和UID(IC卡序列号)*/
	uint16_t ID;
	uint16_t Borrower_ID;
	uint8_t BorrowerRFID[4];
	uint8_t Borrowing_Flag;
	uint8_t Data_Valid_Flag_Bit; // 数据有效标志位 0为无效
} Book_Information;
Book_Information Books[5] = {
	{{0}, 0, 0, {0}, 0, 0},
	{{0}, 0, 0, {0}, 0, 0},
	{{0}, 0, 0, {0}, 0, 0},
	{{0}, 0, 0, {0}, 0, 0},
	{{0}, 0, 0, {0}, 0, 0}};
uint8_t KeyValue[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}; // 卡A密钥
uint8_t rfid_address = 0x11;
//------------------------
static void (*current_operation_index)(void);
typedef struct
{
	uint8_t current;				 // 当前状态索引号
	uint8_t next;					 // 向下一个
	uint8_t last;					 // 向上一个
	uint8_t enter;					 // 确定
	uint8_t back;					 // 返回
	void (*current_operation)(void); // 当前状态应该执行的操作
} Menu_table;

Menu_table table[13] =
	{
		{0, 0, 0, 1, 0, (*fun0)}, // 一级界面  时钟界面

		{1, 2, 5, 6, 0, (*fun1)},
		{2, 3, 1, 7, 0, (*fun2)},
		{3, 4, 2, 8, 0, (*fun3)},
		{4, 5, 3, 9, 0, (*fun4)},
		{5, 1, 4, 10, 0, (*fun5)},

		{6, 6, 6, 6, 6, (*Information_Entry)},
		{7, 7, 7, 7, 7, (*Information_Query)},
		{8, 8, 8, 8, 8, (*Library_Card_Binding)},
		{9, 9, 9, 9, 9, (*Borrow_a_Book)},
		{10, 10, 10, 10, 10, (*Return_a_Book)},
};
uint8_t func_index = 0; // 主程序此时所在界面的索引值
void Menu_key_set(void)
{
	if (next_key == 14)
	{
		// HAL_Delay(15);
		while (next_key == 14)
			;
		func_index = table[func_index].next; // 按键next按下后的索引号
		OLED_Clear();
	}
	if (last_key == 15)
	{
		// HAL_Delay(15);
		while (last_key == 15)
			;
		func_index = table[func_index].last;
		OLED_Clear();
	}
	if (enter_key == 13)
	{
		// HAL_Delay(15);
		while (enter_key == 13)
			;
		func_index = table[func_index].enter;
		OLED_Clear();
	}
	if (back_key == 12)
	{
		// HAL_Delay(15);
		while (back_key == 12)
			;
		func_index = table[func_index].back;
		OLED_Clear();
	}

	current_operation_index = table[func_index].current_operation; // 执行当前索引号所对应的功能函数。
	(*current_operation_index)();								   // 执行当前操作函数
}
void fun0(void)
{
	OLED_ShowString(0, 0, "Intelligent  ", 24, 1);
	OLED_ShowString(0, 30, "   bookcase ", 16, 1);
	OLED_ShowString(0, 48, "    system", 16, 1);
	OLED_Refresh();
}
void fun1()
{
	OLED_ShowString(0, 0, "Information entry      ", 12, 0);
	OLED_ShowString(0, 12, "Information query     ", 12, 1);
	OLED_ShowString(0, 24, "Loan card binding     ", 12, 1);
	OLED_ShowString(0, 36, "Borrow a book         ", 12, 1);
	OLED_ShowString(0, 48, "Return a book         ", 12, 1);
	OLED_Refresh();
}
void fun2()
{
	OLED_ShowString(0, 0, "Information entry      ", 12, 1);
	OLED_ShowString(0, 12, "Information query     ", 12, 0);
	OLED_ShowString(0, 24, "Loan card binding     ", 12, 1);
	OLED_ShowString(0, 36, "Borrow a book         ", 12, 1);
	OLED_ShowString(0, 48, "Return a book         ", 12, 1);
	OLED_Refresh();
}
void fun3()
{
	OLED_ShowString(0, 0, "Information entry      ", 12, 1);
	OLED_ShowString(0, 12, "Information query     ", 12, 1);
	OLED_ShowString(0, 24, "Loan card binding     ", 12, 0);
	OLED_ShowString(0, 36, "Borrow a book         ", 12, 1);
	OLED_ShowString(0, 48, "Return a book         ", 12, 1);
	OLED_Refresh();
}
void fun4()
{
	OLED_ShowString(0, 0, "Information entry      ", 12, 1);
	OLED_ShowString(0, 12, "Information query     ", 12, 1);
	OLED_ShowString(0, 24, "Loan card binding     ", 12, 1);
	OLED_ShowString(0, 36, "Borrow a book         ", 12, 0);
	OLED_ShowString(0, 48, "Return a book         ", 12, 1);
	OLED_Refresh();
}
void fun5()
{
	OLED_ShowString(0, 0, "Information entry      ", 12, 1);
	OLED_ShowString(0, 12, "Information query     ", 12, 1);
	OLED_ShowString(0, 24, "Loan card binding     ", 12, 1);
	OLED_ShowString(0, 36, "Borrow a book         ", 12, 1);
	OLED_ShowString(0, 48, "Return a book         ", 12, 0);
	OLED_Refresh();
}
uint8_t Vacancy_Search()
{
	uint8_t i = 0;
	for (i = 0; i < 5; i++)
	{
		if (Books[i].Data_Valid_Flag_Bit == 0)
			return i;
	}
}
uint8_t Search_Same_Book_RFID(uint8_t *ucArray_ID)
{
	uint8_t i = 0;
	for (i = 0; i < 5; i++)
	{
		if (ucArray_ID[0] == Books[i].BookRFID[0] &&
			ucArray_ID[1] == Books[i].BookRFID[1] &&
			ucArray_ID[1] == Books[i].BookRFID[1] &&
			ucArray_ID[1] == Books[i].BookRFID[1])
			return i;
	}
	return 99;
}
uint16_t Key_Input(uint8_t x, uint8_t y)
{
	uint8_t num[4];
	uint8_t i = 0;
	uint8_t key_num;
	uint16_t return_num;
	while (1)
	{
		if (Key_Scan() != 99)
		{
			key_num = Key_Scan();
			delay_ms(10);
			while (Key_Scan() != 99)
				;
			if (key_num == 10)
			{	LED_OFF();
				func_index = 1;
				return 99;
			}

			num[i] = key_num;
			OLED_ShowNum(x + (i * 10), y, num[i], 1, 12, 1);
			OLED_Refresh();

			i++;
		}
		if (i > 3)
		{
			return_num = num[0] * 1000 + num[1] * 100 + num[2] * 10 + num[3];
			return return_num;
		}
	}
}
void Information_Entry()
{
	uint8_t i;
	uint8_t cStr[20];
	i = Vacancy_Search();
	OLED_ShowString(0, 0, "Put down a book", 12, 1);
	OLED_ShowString(0, 24, "Input ID:", 12, 1);
	OLED_Refresh();
	while (1)
	{
		uint8_t ucStatusReturn;
		if ((ucStatusReturn = PcdRequest(PICC_REQALL, Books[i].BookRFID, Book_RFID)) != MI_OK)
		{
			ucStatusReturn = PcdRequest(PICC_REQALL, Books[i].BookRFID, Book_RFID); // PICC_REQALL   PICC_REQIDL
		}
		if (ucStatusReturn == MI_OK)
		{
			printf("寻卡成功\n");
			if (PcdAnticoll(Books[i].BookRFID, Book_RFID) == MI_OK)
			{
				PcdSelect(Books[i].BookRFID, Book_RFID);
				PcdAuthState(PICC_AUTHENT1A, 0x11, KeyValue, Books[i].BookRFID, Book_RFID); // 校验密码
				sprintf(cStr, "RDID:%02X%02X%02X%02X", Books[i].BookRFID[0], Books[i].BookRFID[1], Books[i].BookRFID[2], Books[i].BookRFID[3]);
				OLED_ShowString(0, 12, cStr, 12, 1);
				OLED_ShowString(0, 24, "Input ID:", 12, 1);
				printf("The Card RFID is: %02X%02X%02X%02X", Books[i].BookRFID[0], Books[i].BookRFID[1], Books[i].BookRFID[2], Books[i].BookRFID[3]); // 打印卡片ID
				PcdHalt(Book_RFID);
				OLED_Refresh();
				Books[i].ID = Key_Input(0, 36);
				Books[i].Data_Valid_Flag_Bit = 1;
				OLED_ShowString(0, 48, "OK", 12, 1);
				OLED_Refresh();
				delay_ms(500);
				func_index = 1;
				break;
			}
		}
		if (Key_Scan() == 10)
		{	LED_OFF();
			func_index = 1;
			break;
		}

		//------------
	}
}
void Information_Query()
{
	uint8_t i;
	uint16_t key_input_num;
	uint8_t show[20];
	OLED_ShowString(0, 0, "Input ID:", 12, 1);
	OLED_Refresh();
	key_input_num = Key_Input(80, 0);
	for (i = 0; i < 6; i++)
	{
		if (i == 5)
		{
			OLED_ShowString(0, 12, "Search fruitless", 12, 1);
			OLED_Refresh();
		}

		if (Books[i].ID == key_input_num && Books[i].Data_Valid_Flag_Bit == 1)
		{
			sprintf(show, "Book_RFID:%02X%02X%02X%02X", Books[i].BookRFID[0], Books[i].BookRFID[1], Books[i].BookRFID[2], Books[i].BookRFID[3]);
			OLED_ShowString(0, 24, show, 12, 1);
			if (Books[i].Borrowing_Flag == 1)
			{

				sprintf(show, "Brrwr_ID:%d", Books[i].Borrower_ID);
				OLED_ShowString(0, 12, show, 12, 1);
				sprintf(show, "Brrwr_RFID:%02X%02X%02X%02X", Books[i].BorrowerRFID[0], Books[i].BorrowerRFID[1], Books[i].BorrowerRFID[2], Books[i].BorrowerRFID[3]);
				OLED_ShowString(0, 36, show, 12, 1);
			}
			else
			{

				OLED_ShowString(0, 12, "Brrwr_ID:None", 12, 1);
				OLED_ShowString(0, 36, "Brrwr_RFID:None", 12, 1);
			}
			if (Books[i].Borrowing_Flag == 1)
			{
				OLED_ShowString(0, 48, "Out on loan", 12, 1);
			}
			else
			{
				OLED_ShowString(0, 48, "Not on loan", 12, 1);
			}
			OLED_Refresh();
			break;
		}
	}
	delay_ms(500);
	while (Key_Scan() != 12)
	{
		if (Key_Scan() == 11)
		{
			Books[i].Data_Valid_Flag_Bit = 0;
			OLED_Clear();
			OLED_ShowString(0, 36, "Delete Successful", 12, 1);
			OLED_Refresh();
		}
		if (Key_Scan() == 10)
		{	LED_OFF();
			func_index = 1;
			break;
		}
	}

	func_index = 2;
}
void Library_Card_Binding()
{
	uint8_t i = 0;
	uint8_t cStr[20];
	uint8_t ucArray_ID[4];
	uint16_t writeValue;
	i = Vacancy_Search();
	OLED_ShowString(0, 0, "Put down Library card", 12, 1);
	OLED_ShowString(0, 24, "Input ID:", 12, 1);
	OLED_Refresh();
	while (1)
	{
		uint8_t ucStatusReturn;
		if ((ucStatusReturn = PcdRequest(PICC_REQALL, ucArray_ID, Borrower_RFID)) != MI_OK)
		{
			ucStatusReturn = PcdRequest(PICC_REQALL, ucArray_ID, Borrower_RFID); // PICC_REQALL   PICC_REQIDL
		}
		if (ucStatusReturn == MI_OK)
		{
			printf("寻卡成功\n");
			if (PcdAnticoll(ucArray_ID, Borrower_RFID) == MI_OK)
			{
				PcdSelect(ucArray_ID, Borrower_RFID);
				PcdAuthState(PICC_AUTHENT1A, 0x11, KeyValue, ucArray_ID, Borrower_RFID); // 校验密码
				sprintf(cStr, "RDID:%02X%02X%02X%02X", ucArray_ID[0], ucArray_ID[1], ucArray_ID[2], ucArray_ID[3]);
				OLED_ShowString(0, 12, cStr, 12, 1);
				OLED_Refresh();
				printf("The Card RFID is: %02X%02X%02X%02X", ucArray_ID[0], ucArray_ID[1], ucArray_ID[2], Books[i].BookRFID[3]); // 打印卡片ID
				writeValue = Key_Input(0, 36);
				WriteAmount(0x11, writeValue, Borrower_RFID);
				PcdHalt(Borrower_RFID);
				OLED_ShowString(0, 48, "OK", 12, 1);
				OLED_Refresh();
				delay_ms(1000);
				func_index = 3;
				break;
			}
		}
		if (Key_Scan() == 10)
		{	LED_OFF();
			func_index = 1;
			break;
		}
		//------------
	}
}
void Borrow_a_Book()
{
	uint8_t i;
	uint8_t cStr[20];
	uint8_t ucArray_ID[4];
	uint8_t ucArray_ID_Borrower[4];
	uint8_t flag = 0;
	uint32_t readValue;
	OLED_ShowString(0, 0, "Put down a book", 12, 1);
	OLED_ShowString(0, 24, "Put down Library card", 12, 1);
	OLED_Refresh();
	while (1)
	{
		uint8_t ucStatusReturn;
		if ((ucStatusReturn = PcdRequest(PICC_REQALL, ucArray_ID, Book_RFID)) != MI_OK)
		{
			ucStatusReturn = PcdRequest(PICC_REQALL, ucArray_ID, Book_RFID); // PICC_REQALL   PICC_REQIDL
		}
		if (ucStatusReturn == MI_OK)
		{
			printf("寻卡成功\n");
			if (PcdAnticoll(ucArray_ID, Book_RFID) == MI_OK)
			{
				PcdSelect(ucArray_ID, Book_RFID);
				PcdAuthState(PICC_AUTHENT1A, 0x11, KeyValue, ucArray_ID, Book_RFID); // 校验密码
				sprintf(cStr, "RDID:%02X%02X%02X%02X", ucArray_ID[0], ucArray_ID[1], ucArray_ID[2], ucArray_ID[3]);
				OLED_ShowString(0, 12, cStr, 12, 1);
				OLED_ShowString(0, 24, "Put down Library card", 12, 1);
				printf("The Card RFID is: %02X%02X%02X%02X", ucArray_ID[0], ucArray_ID[1], ucArray_ID[2], ucArray_ID[3]); // 打印卡片ID
				PcdHalt(Book_RFID);
				OLED_Refresh();
				i = Search_Same_Book_RFID(ucArray_ID);
				flag = 1;
			}
		}
		//-----------
		if ((ucStatusReturn = PcdRequest(PICC_REQALL, ucArray_ID_Borrower, Borrower_RFID)) != MI_OK)
		{
			ucStatusReturn = PcdRequest(PICC_REQALL, ucArray_ID_Borrower, Borrower_RFID); // PICC_REQALL   PICC_REQIDL
		}
		if (ucStatusReturn == MI_OK)
		{
			printf("寻卡成功\n");
			if (PcdAnticoll(ucArray_ID_Borrower, Borrower_RFID) == MI_OK)
			{
				PcdSelect(ucArray_ID_Borrower, Borrower_RFID);
				PcdAuthState(PICC_AUTHENT1A, 0x11, KeyValue, ucArray_ID_Borrower, Borrower_RFID); // 校验密码
				if (ReadAmount(0x11, &readValue, Borrower_RFID) == MI_OK)						  // 读取金额
				{
					printf("read ok %d\r\n", readValue);
				}
				sprintf(cStr, "RDID:%02X%02X%02X%02X", ucArray_ID_Borrower[0], ucArray_ID_Borrower[1], ucArray_ID_Borrower[2], ucArray_ID_Borrower[3]);
				OLED_ShowString(0, 36, cStr, 12, 1);
				printf("The Card RFID is: %02X%02X%02X%02X", ucArray_ID_Borrower[0], ucArray_ID_Borrower[1], ucArray_ID_Borrower[2], ucArray_ID_Borrower[3]); // 打印卡片ID
				PcdHalt(Borrower_RFID);
				OLED_Refresh();				
				if (flag == 1)
					flag = 2;
LED_ON();
			}
		}
		//------------
		if (flag == 2)
		{

			
			if (i == 99)
			{
				OLED_ShowString(0, 48, "fault", 12, 1);
				OLED_Refresh();
				delay_ms(500);
				func_index = 4;
        	LED_OFF();
				break;
			}
			else
			{
				OLED_ShowString(0, 48, "YES OR NO", 12, 1);
				OLED_Refresh();
				while (1)
				{
					if (Key_Scan() == 13)
					{
						Books[i].Borrower_ID = readValue;
						Books[i].BorrowerRFID[0] = ucArray_ID_Borrower[0];
						Books[i].BorrowerRFID[1] = ucArray_ID_Borrower[1];
						Books[i].BorrowerRFID[2] = ucArray_ID_Borrower[2];
						Books[i].BorrowerRFID[3] = ucArray_ID_Borrower[3];
						Books[i].Borrowing_Flag = 1;
						OLED_ShowString(0, 48, "successful", 12, 1);
						OLED_Refresh();
						delay_ms(500);
						func_index = 4;
						break;
					}
					if (Key_Scan() == 10)
					{	LED_OFF();
						func_index = 1;
						break;
					}
				}
        	LED_OFF();
				break;
			}
		}
		if (Key_Scan() == 10)
		{
			func_index = 1;
      	LED_OFF();
			break;
		}
	}
}
void Return_a_Book()
{
	uint8_t i;
	uint8_t cStr[20];
	uint8_t ucArray_ID[4];
	uint8_t ucArray_ID_Borrower[4];
	uint8_t flag = 0;
	OLED_ShowString(0, 0, "Put down a book", 12, 1);
	OLED_ShowString(0, 24, "Put down Library card", 12, 1);
	OLED_Refresh();
	while (1)
	{
		uint8_t ucStatusReturn;
		if ((ucStatusReturn = PcdRequest(PICC_REQALL, ucArray_ID, Book_RFID)) != MI_OK)
		{
			ucStatusReturn = PcdRequest(PICC_REQALL, ucArray_ID, Book_RFID); // PICC_REQALL   PICC_REQIDL
		}
		if (ucStatusReturn == MI_OK)
		{
			printf("寻卡成功\n");
			if (PcdAnticoll(ucArray_ID, Book_RFID) == MI_OK)
			{
				PcdSelect(ucArray_ID, Book_RFID);
				PcdAuthState(PICC_AUTHENT1A, 0x11, KeyValue, ucArray_ID, Book_RFID); // 校验密码
				sprintf(cStr, "RDID:%02X%02X%02X%02X", ucArray_ID[0], ucArray_ID[1], ucArray_ID[2], ucArray_ID[3]);
				OLED_ShowString(0, 12, cStr, 12, 1);
				OLED_ShowString(0, 24, "Put down Library card", 12, 1);
				printf("The Card RFID is: %02X%02X%02X%02X", ucArray_ID[0], ucArray_ID[1], ucArray_ID[2], ucArray_ID[3]); // 打印卡片ID
				PcdHalt(Book_RFID);
				OLED_Refresh();
				i = Search_Same_Book_RFID(ucArray_ID);
				flag = 1;
			}
		}
		//-----------
		if ((ucStatusReturn = PcdRequest(PICC_REQALL, ucArray_ID_Borrower, Borrower_RFID)) != MI_OK)
		{
			ucStatusReturn = PcdRequest(PICC_REQALL, ucArray_ID_Borrower, Borrower_RFID); // PICC_REQALL   PICC_REQIDL
		}
		if (ucStatusReturn == MI_OK)
		{
			printf("寻卡成功\n");
			if (PcdAnticoll(ucArray_ID_Borrower, Borrower_RFID) == MI_OK)
			{
				PcdSelect(ucArray_ID_Borrower, Borrower_RFID);
				PcdAuthState(PICC_AUTHENT1A, 0x11, KeyValue, ucArray_ID_Borrower, Borrower_RFID); // 校验密码
				sprintf(cStr, "RDID:%02X%02X%02X%02X", ucArray_ID_Borrower[0], ucArray_ID_Borrower[1], ucArray_ID_Borrower[2], ucArray_ID_Borrower[3]);
				OLED_ShowString(0, 36, cStr, 12, 1);
				printf("The Card RFID is: %02X%02X%02X%02X", ucArray_ID_Borrower[0], ucArray_ID_Borrower[1], ucArray_ID_Borrower[2], ucArray_ID_Borrower[3]); // 打印卡片ID
				PcdHalt(Borrower_RFID);
				OLED_Refresh();
				if (flag == 1)
					flag = 2;
			}
		}
		//------------
		if (flag == 2)
		{
      LED_ON();
			if (i == 99)
			{
				OLED_ShowString(0, 48, "fault", 12, 1);
				OLED_Refresh();
				delay_ms(500);
				func_index = 5;
        LED_OFF();
				break;
			}
			else
			{
				OLED_ShowString(0, 48, "YES OR NO", 12, 1);
				OLED_Refresh();
				while (1)
				{
					if (Key_Scan() == 13)
					{
						Books[i].Borrowing_Flag = 0;
						OLED_ShowString(0, 48, "successful", 12, 1);
						OLED_Refresh();
						delay_ms(500);
						func_index = 5;

						break;
					}
					if (Key_Scan() == 10)
					{	LED_OFF();
						func_index = 1;
						break;
					}
				}
        LED_OFF();
				break;
			}
		}
		if (Key_Scan() == 10)
		{
			
				LED_OFF();
			func_index = 1;
			break;
		}
	}
}