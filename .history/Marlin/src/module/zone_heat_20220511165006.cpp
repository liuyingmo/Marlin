#include "zone_heat.h"
SoftwareSerial sw_uart(PB7,PB8);

void task_init(void)
{
  LOOP_L_N(i,MAXFUN){
        SuperFun[i].p_fun = NULL;
        SuperFun[i].nLine = 0;
        SuperFun[i].nStep = 0;
        SuperFun[i].bTick = 0;
        SuperFun[i].lock  = 0;
  }
}

void task_get(fun_ p_fun_name)
{
	LOOP_L_N(i,MAXFUN){
	if (SuperFun[i].p_fun == p_fun_name) {
		   return;
	 }
	}

	LOOP_L_N(i,MAXFUN) {
    if (SuperFun[i].p_fun == NULL) {
        SuperFun[i].nLine = 0;
        SuperFun[i].nStep = 0;
        SuperFun[i].bTick = 0;
        SuperFun[i].lock  = 0;
        SuperFun[i].p_fun = p_fun_name;		
        break;
     }
	}
}

void task_exec(void)
{
	LOOP_L_N(i,MAXFUN) {
    if (SuperFun[i].p_fun != NULL && SuperFun[i].lock ==0){
        uint32_t curr_tick=HAL_GetTick();
        uint32_t step =curr_tick-SuperFun[i].bTick;
        SuperFun[i].bTick =curr_tick;
        SuperFun[i].p_fun(i,step);
     }
	}
}

void task_del_other(fun_ p_fun_name)
{
    LOOP_L_N(i,MAXFUN){
  	if (SuperFun[i].p_fun == p_fun_name)continue;
		SuperFun[i].p_fun = NULL;
		SuperFun[i].nLine = 0;
		SuperFun[i].nStep = 0;
		SuperFun[i].bTick = 0;
        SuperFun[i].lock  = 0;
	}
}

void task_del(fun_ p_fun_name)
{
	LOOP_L_N(i,MAXFUN){
	if (SuperFun[i].p_fun == p_fun_name) {
        SuperFun[i].nLine = 0;
        SuperFun[i].nStep = 0;
        SuperFun[i].p_fun = NULL;
        SuperFun[i].bTick = 0;
        SuperFun[i].lock  = 0;
        break;
		}
	}
}

void task_get_array(const fun_ arr[],int sum)
{
	LOOP_L_N(i,MAXFUN){
	  task_get(arr[i]);
	}
}

void task_del_array(const fun_ arr[],int sum)
{
  LOOP_L_N(i,MAXFUN){
	task_del(arr[i]);
  }
}

