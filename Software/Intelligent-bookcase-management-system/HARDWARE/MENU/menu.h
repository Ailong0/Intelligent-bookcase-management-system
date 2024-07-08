#ifndef __menu_h
#define __menu_h
#include "sys.h"
#define back_key Key_Scan()
#define enter_key Key_Scan()
#define last_key Key_Scan()
#define next_key Key_Scan()

void Menu_key_set(void);
void fun0(void);
void fun1(void);
void fun2(void);
void fun3(void);
void fun4(void);
void fun5(void);
void Information_Entry();
void Information_Query();
void Library_Card_Binding();
void Borrow_a_Book();
void Return_a_Book();
#define LED_ON() GPIO_ResetBits(GPIOC,GPIO_Pin_13)
#define LED_OFF() GPIO_SetBits(GPIOC,GPIO_Pin_13)
#endif
