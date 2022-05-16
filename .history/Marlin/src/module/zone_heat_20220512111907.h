#pragma once
#define GetItemSum( XX ) (sizeof(XX)/sizeof(XX[0]))

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

extern SoftwareSerial sw_uart;

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

#define _YEAR ((((__DATE__[7]-'0')*10+(__DATE__[8]-'0'))*10 \
+(__DATE__[9]-'0'))*10+(__DATE__[10]-'0'))
#define _MONTH (__DATE__[2]=='n'?1 \
:__DATE__[2]=='b'?2 \
:__DATE__[2]=='r'?(__DATE__[0]=='M'?3:4) \
:__DATE__[2]=='y'?5 \
:__DATE__[2]=='n'?6 \
:__DATE__[2]=='l'?7 \
:__DATE__[2]=='g'?8 \
:__DATE__[2]=='p'?9 \
:__DATE__[2]=='t'?10 \
:__DATE__[2]=='v'?11:12)
#define _DAY ((__DATE__[4]==' '?0:__DATE__[4]-'0')*10 \
+(__DATE__[5]-'0'))
#define DATE_AS_INT (((_YEAR - 2000) * 12 + _MONTH) * 31 + _DAY)
#define _HOUR ((__TIME__[0]-'0')*10+(__TIME__[1]-'0'))
#define _MINUTE ((__TIME__[3]-'0')*10+(__TIME__[4]-'0'))
#define _SECOND ((__TIME__[6]-'0')*10+(__TIME__[7]-'0'))
#define TIME_AS_INT ((((((_YEAR - 2000) * 12 + _MONTH) * 31 + _DAY)*12+_HOUR)*60+_MINUTE)*60+_SECOND)

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
#define MSG_SIZE    20
#define MSG_USE     1
#define MSG_UNUSE   0

typedef void (*MSG_CMD_FUNC) (uint8_t *);
typedef struct
{
   char const *cmd;
   MSG_CMD_FUNC func;
}msg_cmd_t;

typedef struct 
{
  uint8_t buf[COMM_FRAME];
  int16_t index;
  uint8_t end_flag;
}sw_uart_rece_buf_t;
EXTRN sw_uart_rece_buf_t rece_buf;

typedef struct
{
   uint8_t used;
   uint8_t buf[COMM_FRAME];
}msg_buf_t;
EXTRN msg_buf_t msg_buf[MSG_SIZE];

enum
{
    bit_request_temp =0,
    bit_start_heating,
    bit_turnoff_heat,
    bit_flash_init,
    bit_requset_pwm,
    bit_auto_pid,
    bit_pid_state,
    bit_set_pid,
    bit_setup_finish,
};

EXTRN uint32_t comm_bit_updata
#if INIT_END
=0
#endif
;
#define COMM_BIT_SET(XXX) comm_bit_updata |=MaskBits[XXX]
#define COMM_BIT_CLS(XXX) comm_bit_updata &=~MaskBits[XXX]
#define COMM_BIT_IS0(XXX) (comm_bit_updata&MaskBits[XXX])==0
#define COMM_BIT_IS1(XXX) (comm_bit_updata&MaskBits[XXX])!=0

typedef struct
{
  uint32_t compile_time;
  double   pid1[3]; 
  double   pid2[3]; 
  double   pid3[3]; 
  double   pid4[3]; 
  double   pid5[3]; 
  double   pid6[3]; 
  double   pid7[3]; 
  double   pid8[3]; 
  double   pid9[3]; 
  uint32_t keepstep,slowstep;
  uint8_t  used[9];
  uint32_t min_temp,max_temp; 
}par_t;
EXTRN par_t par_config,par_temp;
#define COMPILE_ADD  900
EXTRN const par_t const_par
#if INIT_END
={
	.compile_time =TIME_AS_INT+COMPILE_ADD,
	.pid1 ={1.60,0.30,5.64},
	.pid2 ={1.60,0.30,5.64},
	.pid3 ={1.60,0.30,5.64},
	.pid4 ={1.60,0.30,5.64},
	.pid5 ={1.60,0.30,5.64},
	.pid6 ={1.60,0.30,5.64},
	.pid7 ={1.60,0.30,5.64},
	.pid8 ={1.60,0.30,5.64},
	.pid9 ={1.60,0.30,5.64},
	.keepstep =30,//constant temperature
	.slowstep =80,//slow temperature rise
	.used ={1,1,0,0,0,0,0,0,0},
	.min_temp =BED_MINTEMP,
	.max_temp =BED_MAXTEMP,
}
#endif
;

void init_msg_buf(void);
void sw_uart_recv(sw_uart_rece_buf_t *p_rece_buf,char c);
void msg_deal(void);
int spit_string(char *str,const char *c,char *p[]);
void cmd_string(const char*cmd,char const *format,...);
void printf_string(char const *format,...);
void printf_nothing(char const *format,...)

#if DEGUG
#define debug_string  printf_string
#else
#define debug_string printf_nothing
#endif

