#ifndef __SEQUETASK_H__
#define __SEQUETASK_H__

#ifdef EXTRN
	#undef EXTRN
#endif

#ifdef EXEC_SEQUETASK
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

#endif// __SEQUETASK_H__
