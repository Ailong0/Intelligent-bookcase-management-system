/*四个功能
1.书籍信息录入 Information entry
2.书籍信息查询 Information query
3.借阅卡信息绑定 Library card binding
4.借书 Borrow books
5.还书 Return a book
注:借还书时需要有书柜打开功能
*/
#include "stm32f10x.h"
#include "oled.h"
#include "delay.h"
#include "usart.h"
#include "stdio.h"
#include "rc522_config.h"
#include "rc522_function.h"
#include "keypad.h"
#include "menu.h"
RCC_ClocksTypeDef ClockInfo;
void Delay(u32 count)
{
  u32 i = 0;
  for (; i < count; i++)
    ;
}
void Led_init()
{
  GPIO_InitTypeDef GPIO_InitStructure;
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
  GPIO_SetBits(GPIOC, GPIO_Pin_13);
}
int main(void)
{

  delay_init();                                   // 延时函数初始化
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); // 设置NVIC中断分组2:2位抢占优先级，2位响应优先级
  Led_init();
  uart_init(115200);
  OLED_Init();
  OLED_ShowString(0, 0, "asd", 12, 1);
  OLED_Refresh();
  printf("hello");
  RC522_Init();
  KEY_Init();
  printf("WF-RC522 Test\n");
  PcdReset(Book_RFID);
  PcdReset(Borrower_RFID);
  /*设置工作方式*/
  M500PcdConfigISOType('A', Book_RFID);
  M500PcdConfigISOType('A', Borrower_RFID);
  RCC_GetClocksFreq(&ClockInfo);
  while (1)
  {
    Menu_key_set();
    // uint32_t writeValue = 100;
    // uint32_t readValue;
    // char cStr[30];
    // uint8_t ucArray_ID[4];  /*先后存放IC卡的类型和UID(IC卡序列号)*/
    // uint8_t ucStatusReturn; /*返回状态*/

    // /* 按照流程操作 RC522 ,否者会导致操作失败 */
    // while (1)
    // {
    //   keynum = Key_Scan();
    //   /*寻卡*/ printf("%d\n", keynum);
    //   if ((ucStatusReturn = PcdRequest(PICC_REQALL, ucArray_ID, Book_RFID)) != MI_OK)
    //   {
    //     /*若失败再次寻卡*/

    //     ucStatusReturn = PcdRequest(PICC_REQALL, ucArray_ID, Book_RFID); // PICC_REQALL   PICC_REQIDL
    //   }

    //   if (ucStatusReturn == MI_OK)
    //   {
    //     //                sprintf ( cStr, "The Card ID is: %02X%02X%02X%02X",ucArray_ID [0], ucArray_ID [1], ucArray_ID [2],ucArray_ID [3] );
    //     //          printf ( "%s\r\n",cStr );  //打印卡片ID
    //     printf("寻卡成功\n");
    //     /*防冲撞（当有多张卡进入读写器操作范围时，防冲突机制会从其中选择一张进行操作）*/
    //     if (PcdAnticoll(ucArray_ID, Book_RFID) == MI_OK)
    //     {
    //       PcdSelect(ucArray_ID, Book_RFID);

    //       PcdAuthState(PICC_AUTHENT1A, 0x11, KeyValue, ucArray_ID, Book_RFID); // 校验密码
    //       WriteAmount(0x11, writeValue, Book_RFID);                            // 写入金额
    //       if (ReadAmount(0x11, &readValue, Book_RFID) == MI_OK)                // 读取金额
    //       {
    //         //          writeValue +=100;
    //         sprintf(cStr, "The Card ID is: %02X%02X%02X%02X", ucArray_ID[0], ucArray_ID[1], ucArray_ID[2], ucArray_ID[3]);
    //         printf("%s\r\n", cStr); // 打印卡片ID

    //         printf("Book_RFID%d\r\n", readValue);
    //         PcdHalt(Book_RFID);
    //       }
    //     }
    //   }
    //   //**************
    //   if ((ucStatusReturn = PcdRequest(PICC_REQALL, ucArray_ID, Borrower_RFID)) != MI_OK)
    //   {
    //     /*若失败再次寻卡*/

    //     ucStatusReturn = PcdRequest(PICC_REQALL, ucArray_ID, Borrower_RFID); // PICC_REQALL   PICC_REQIDL
    //   }

    //   if (ucStatusReturn == MI_OK)
    //   {
    //     //                sprintf ( cStr, "The Card ID is: %02X%02X%02X%02X",ucArray_ID [0], ucArray_ID [1], ucArray_ID [2],ucArray_ID [3] );
    //     //          printf ( "%s\r\n",cStr );  //打印卡片ID
    //     printf("寻卡成功\n");
    //     /*防冲撞（当有多张卡进入读写器操作范围时，防冲突机制会从其中选择一张进行操作）*/
    //     if (PcdAnticoll(ucArray_ID, Borrower_RFID) == MI_OK)
    //     {
    //       PcdSelect(ucArray_ID, Borrower_RFID);

    //       PcdAuthState(PICC_AUTHENT1A, 0x11, KeyValue, ucArray_ID, Borrower_RFID); // 校验密码
    //       WriteAmount(0x11, writeValue, Borrower_RFID);                            // 写入金额
    //       if (ReadAmount(0x11, &readValue, Borrower_RFID) == MI_OK)                // 读取金额
    //       {
    //         //          writeValue +=100;
    //         sprintf(cStr, "The Card ID is: %02X%02X%02X%02X", ucArray_ID[0], ucArray_ID[1], ucArray_ID[2], ucArray_ID[3]);
    //         printf("%s\r\n", cStr); // 打印卡片ID

    //         printf("Borrower_RFID%d\r\n", readValue);
    //         PcdHalt(Borrower_RFID);
    //       }
    //     }
    //   }
    // }
  }
}
