#include "sys_timer.h"

uint8_t g_ubCount1ms = 0;
uint8_t g_ubCount10ms = 0;
uint8_t g_ubCount20ms = 0;
uint8_t g_ubCount50ms = 0;
uint8_t g_ubCount100ms = 0;
uint8_t g_ubCount500ms = 0;
uint8_t g_ubCount1000ms = 0;

void SysTimerLoop(void)
{
	if(g_ubCount1ms >= 1)
	{
		SysTimer1msEvent();
		g_ubCount1ms = 0;
	}
	if(g_ubCount10ms == 1)
	{
		g_ubCount10ms = 0;
		/*--- Create 10ms event ---*/
		SysTimer10msEvent();
		/*--- Create 20ms event ---*/
		g_ubCount20ms++;
		if(g_ubCount20ms == 2)
		{
			SysTimer20msEvent();
			g_ubCount20ms = 0;
		}
		/*-------------------------*/
		g_ubCount50ms++;
		if(g_ubCount50ms == 5)
		{
			g_ubCount50ms = 0;
			/*--- Create 50ms event ---*/
			SysTimer50msEvent();
			/*-------------------------*/
			g_ubCount1000ms++;
			if((g_ubCount1000ms & 0x01) == 0x00)
			{
				/*--- Create 100ms event ---*/
				SysTimer100msEvent();
				/*--------------------------*/
				if(g_ubCount1000ms == 10 || g_ubCount1000ms >= 20)
				{
					/*--- Create 500ms event ---*/
					SysTimer500msEvent();
					/*--------------------------*/
					if(g_ubCount1000ms >= 20)
					{
						SysTimer1000msEvent();
						g_ubCount1000ms = 0;
					}
				}
			}
		}
	}
}
__weak void SysTimer1msEvent(void)
{

}
__weak void SysTimer10msEvent(void)
{

}
__weak void SysTimer20msEvent(void)
{

}
__weak void SysTimer50msEvent(void)
{

}
__weak void SysTimer100msEvent(void)
{

}
__weak void SysTimer500msEvent(void)
{

}
__weak void SysTimer1000msEvent(void)
{

}
void SysTimerHandle(void)
{
	static uint8_t s_ubCout10ms = 0;
	g_ubCount1ms++;
	s_ubCout10ms++;
	if(s_ubCout10ms >= 10)
	{
		g_ubCount10ms = 1;
		s_ubCout10ms = 0;
	}
}

