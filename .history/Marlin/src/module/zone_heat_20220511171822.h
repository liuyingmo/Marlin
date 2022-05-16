 #pragma once

 #ifdef EXTRN
	#undef EXTRN
#endif

#ifdef here_to_exec
	#define	EXTRN
	#define	INIT_END	1
#else
	#define	EXTRN extern
	#define	INIT_END	0
#endif

#include "../inc/MarlinConfig.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "Arduino.h"
#include <SoftwareSerial.h>

EXTRN  const uint32_t MaskBits[32]
#if INIT_END
= {
	0x1, 0x2, 0x4, 0x8, 0x10, 0x20, 0x40, 0x80,
	(0x1UL << 8), (0x1UL << 9), (0x1UL << 10), (0x1UL << 11),
	(0x1UL << 12), (0x1UL << 13), (0x1UL << 14), (0x1UL << 15),
	(0x1UL << 16), (0x1UL << 17), (0x1UL << 18), (0x1UL << 19),
	(0x1UL << 20), (0x1UL << 21), (0x1UL << 22), (0x1UL << 23),
	(0x1UL << 24), (0x1UL << 25), (0x1UL << 26), (0x1UL << 27),
	(0x1UL << 28), (0x1UL << 29), (0x1UL << 30), (0x1UL << 31),
}
#endif
;

typedef void (*fun_)(uint8_t,int32_t);
typedef struct
{
	fun_     p_fun;
  uint32_t nLine;
	int32_t  nStep;
	uint32_t bTick;
  bool     lock;
}type_fun;

#define MAXFUN 15
EXTRN type_fun SuperFun[MAXFUN];

#define crBegin(_id,_Step) {SuperFun[_id].nStep -=_Step;\
        if(SuperFun[_id].nStep>0)return; switch(SuperFun[_id].nLine){ case 0:

#define crReturn(_id) do{ SuperFun[_id].nLine=__LINE__;return; case __LINE__:;}while(0)
#define crWait(_id,nTick) SuperFun[_id].nStep=nTick;crReturn(_id)
#define crFinish(_id)  SuperFun[_id].nLine=0;}}
#define crDel(_id) do{SuperFun[_id].nLine=0;SuperFun[_id].nStep=0;SuperFun[_id].p_fun=NULL;}while(0)
#define crAgain(_id) do { SuperFun[_id].nLine=0; return;}while(0)
#define crEnd()  }}
#define crLock(_id) SuperFun[_id].lock=1
#define crUnlock(_id) SuperFun[_id].lock=0

void task_init(void);
void task_get(fun_ p_fun_name);
void task_del(fun_ p_fun_name);
void task_exec(void);
void task_get_array(const fun_ arr[],int sum);
void task_del_array(const fun_ arr[],int sum);
void task_del_other(fun_ p_fun_name);

#define COMM_FRAME  128
typedef struct 
{
  uint8_t buf[COMM_FRAME];
  int16_t index;
  uint8_t end_flag;
}sw_uart_rece_buf_t;

#define GetItemSum( XX ) (sizeof(XX)/sizeof(XX[0]))

extern SoftwareSerial sw_uart;
