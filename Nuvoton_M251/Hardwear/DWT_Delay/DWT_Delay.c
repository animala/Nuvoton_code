#include "DWT_Delay.h"
 
// 0xE000EDFC DEMCR RW Debug Exception and Monitor Control Register.
#define DEMCR           ( *(unsigned int *)0xE000EDFC )
#define TRCENA          ( 0x01 << 24) // DEMCR的DWT使能位
 
// 0xE0001000 DWT_CTRL RW The Debug Watchpoint and Trace (DWT) unit
#define DWT_CTRL        ( *(unsigned int *)0xE0001000 )
#define CYCCNTENA       ( 0x01 << 0 ) // DWT的SYCCNT使能位
// 0xE0001004 DWT_CYCCNT RW Cycle Count register, 
#define DWT_CYCCNT      ( *(unsigned int *)0xE0001004) // 显示或设置处理器的周期计数值
 
//#define DWT_DELAY_mS(mSec)    DWT_DELAY_uS(mSec*1000)
 
static int SYSCLK = 0;;
 
void DWT_INIT(int sys_clk)
{
  DEMCR |= TRCENA;
  DWT_CTRL |= CYCCNTENA;
  
  SYSCLK = sys_clk; // 保存当前系统的时钟周期，eg. 72,000,000(72MHz). 
}
 
// 微秒延时
void DWT_DELAY_uS(unsigned int uSec)
{
  int ticks_start, ticks_end, ticks_delay;
  
  ticks_start = DWT_CYCCNT;
  
  if ( !SYSCLK )
    DWT_INIT( MY_MCU_SYSCLK );
  
  ticks_delay = ( uSec * ( SYSCLK / (1000*1000) ) ); // 将微秒数换算成滴答数          
  
  ticks_end = ticks_start + ticks_delay;
  
  if ( ticks_end > ticks_start )
  {
    while( DWT_CYCCNT < ticks_end );
  }
  else // 计数溢出，翻转
  {
    while( DWT_CYCCNT >= ticks_end ); // 翻转后的值不会比ticks_end小
    while( DWT_CYCCNT < ticks_end );
  }
}
 


