#ifndef __RC522_FUNCTION_H
#define	__RC522_FUNCTION_H


#include "stm32f10x_it.h"


#define          macDummy_Data              0x00


void             PcdReset                   ( uint8_t WITCH );                       //复位
void             M500PcdConfigISOType       ( u8 type , uint8_t WITCH);                    //工作方式
char             PcdRequest                 ( u8 req_code, u8 * pTagType, uint8_t WITCH ); //寻卡
char             PcdAnticoll                ( u8 * pSnr, uint8_t WITCH);                   //读卡号
char             PcdHalt                    (  uint8_t WITCH );
char             PcdSelect                  ( uint8_t * pSnr, uint8_t WITCH );
char             PcdAuthState               ( uint8_t ucAuth_mode, uint8_t ucAddr, uint8_t * pKey, uint8_t * pSnr , uint8_t WITCH);
char             WriteAmount                ( uint8_t ucAddr, uint32_t pData , uint8_t WITCH);
char             ReadAmount                 ( uint8_t ucAddr, uint32_t *pData, uint8_t WITCH );


char IsDataBlock( uint8_t ucAddr , uint8_t WITCH);
char PcdWriteString( uint8_t ucAddr, uint8_t *pData, uint8_t WITCH);
char PcdReadString( uint8_t ucAddr, uint8_t *pData, uint8_t WITCH);

char ChangeKeyA( uint8_t ucAddr, uint8_t *pKeyA , uint8_t WITCH);
char WriteDataBlock( uint8_t ucAddr, uint8_t *pData, uint8_t Len, uint8_t WITCH);
char ReadDataBlock( uint8_t ucAddr, uint8_t *pData, uint8_t WITCH);


#endif /* __RC522_FUNCTION_H */

