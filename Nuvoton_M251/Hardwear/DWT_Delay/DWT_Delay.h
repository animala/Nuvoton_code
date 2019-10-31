#ifndef _DWTDELAY_H_
 
#define _DWTDELAY_H_
 
// ����MCU���޸�
#define MY_MCU_SYSCLK           (32000000)
 
 
 
void DWT_INIT(int sys_clk);
 
// ΢����ʱ
void DWT_DELAY_uS(unsigned int uSec);
 
#define DWT_DELAY_mS(mSec)    DWT_DELAY_uS( mSec*1000 )
 
#endif // _DWTDELAY_H_

