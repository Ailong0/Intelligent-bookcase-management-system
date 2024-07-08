/*�ĸ�����
1.�鼮��Ϣ¼�� Information entry
2.�鼮��Ϣ��ѯ Information query
3.���Ŀ���Ϣ�� Library card binding
4.���� Borrow books
5.���� Return a book
ע:�軹��ʱ��Ҫ�����򿪹���
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

  delay_init();                                   // ��ʱ������ʼ��
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); // ����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
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
  /*���ù�����ʽ*/
  M500PcdConfigISOType('A', Book_RFID);
  M500PcdConfigISOType('A', Borrower_RFID);
  RCC_GetClocksFreq(&ClockInfo);
  while (1)
  {
    Menu_key_set();
    // uint32_t writeValue = 100;
    // uint32_t readValue;
    // char cStr[30];
    // uint8_t ucArray_ID[4];  /*�Ⱥ���IC�������ͺ�UID(IC�����к�)*/
    // uint8_t ucStatusReturn; /*����״̬*/

    // /* �������̲��� RC522 ,���߻ᵼ�²���ʧ�� */
    // while (1)
    // {
    //   keynum = Key_Scan();
    //   /*Ѱ��*/ printf("%d\n", keynum);
    //   if ((ucStatusReturn = PcdRequest(PICC_REQALL, ucArray_ID, Book_RFID)) != MI_OK)
    //   {
    //     /*��ʧ���ٴ�Ѱ��*/

    //     ucStatusReturn = PcdRequest(PICC_REQALL, ucArray_ID, Book_RFID); // PICC_REQALL   PICC_REQIDL
    //   }

    //   if (ucStatusReturn == MI_OK)
    //   {
    //     //                sprintf ( cStr, "The Card ID is: %02X%02X%02X%02X",ucArray_ID [0], ucArray_ID [1], ucArray_ID [2],ucArray_ID [3] );
    //     //          printf ( "%s\r\n",cStr );  //��ӡ��ƬID
    //     printf("Ѱ���ɹ�\n");
    //     /*����ײ�����ж��ſ������д��������Χʱ������ͻ���ƻ������ѡ��һ�Ž��в�����*/
    //     if (PcdAnticoll(ucArray_ID, Book_RFID) == MI_OK)
    //     {
    //       PcdSelect(ucArray_ID, Book_RFID);

    //       PcdAuthState(PICC_AUTHENT1A, 0x11, KeyValue, ucArray_ID, Book_RFID); // У������
    //       WriteAmount(0x11, writeValue, Book_RFID);                            // д����
    //       if (ReadAmount(0x11, &readValue, Book_RFID) == MI_OK)                // ��ȡ���
    //       {
    //         //          writeValue +=100;
    //         sprintf(cStr, "The Card ID is: %02X%02X%02X%02X", ucArray_ID[0], ucArray_ID[1], ucArray_ID[2], ucArray_ID[3]);
    //         printf("%s\r\n", cStr); // ��ӡ��ƬID

    //         printf("Book_RFID%d\r\n", readValue);
    //         PcdHalt(Book_RFID);
    //       }
    //     }
    //   }
    //   //**************
    //   if ((ucStatusReturn = PcdRequest(PICC_REQALL, ucArray_ID, Borrower_RFID)) != MI_OK)
    //   {
    //     /*��ʧ���ٴ�Ѱ��*/

    //     ucStatusReturn = PcdRequest(PICC_REQALL, ucArray_ID, Borrower_RFID); // PICC_REQALL   PICC_REQIDL
    //   }

    //   if (ucStatusReturn == MI_OK)
    //   {
    //     //                sprintf ( cStr, "The Card ID is: %02X%02X%02X%02X",ucArray_ID [0], ucArray_ID [1], ucArray_ID [2],ucArray_ID [3] );
    //     //          printf ( "%s\r\n",cStr );  //��ӡ��ƬID
    //     printf("Ѱ���ɹ�\n");
    //     /*����ײ�����ж��ſ������д��������Χʱ������ͻ���ƻ������ѡ��һ�Ž��в�����*/
    //     if (PcdAnticoll(ucArray_ID, Borrower_RFID) == MI_OK)
    //     {
    //       PcdSelect(ucArray_ID, Borrower_RFID);

    //       PcdAuthState(PICC_AUTHENT1A, 0x11, KeyValue, ucArray_ID, Borrower_RFID); // У������
    //       WriteAmount(0x11, writeValue, Borrower_RFID);                            // д����
    //       if (ReadAmount(0x11, &readValue, Borrower_RFID) == MI_OK)                // ��ȡ���
    //       {
    //         //          writeValue +=100;
    //         sprintf(cStr, "The Card ID is: %02X%02X%02X%02X", ucArray_ID[0], ucArray_ID[1], ucArray_ID[2], ucArray_ID[3]);
    //         printf("%s\r\n", cStr); // ��ӡ��ƬID

    //         printf("Borrower_RFID%d\r\n", readValue);
    //         PcdHalt(Borrower_RFID);
    //       }
    //     }
    //   }
    // }
  }
}
