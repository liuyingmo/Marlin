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

void to_uper( uint8_t *s )
{
	while (*s){
		if ((*s>='a') && (*s<='z')){
			*s = *s - 'a' + 'A';
		}
		s ++;
	}
}

void to_lower( uint8_t *s )
{
	while (*s){
		if ((*s>='A') && (*s<='Z')){
			*s = *s - 'A' + 'a';
		}
		s ++;
	}
}

uint16_t msg_find_unused(void)
{
  LOOP_L_N(i,MSG_SIZE){
    if(msg_buf[i].used == MSG_UNUSE) return i;
  }
	return 0xffff;
}

uint16_t msg_find_used(void)
{
	LOOP_L_N(i,MSG_SIZE){
		if(msg_buf[i].used == MSG_USE) return i;
	}
	return 0xffff;
}

void init_msg_buf(void)
{
	LOOP_L_N(i,MSG_SIZE){
		msg_buf[i].used =MSG_UNUSE;
	}
}

void sw_uart_recv(sw_uart_rece_buf_t *p_rece_buf,char c)
{
   uint16_t msg_pos =0;

   if(c =='\r' || c =='\n') p_rece_buf->end_flag++;
   else {
        p_rece_buf->end_flag =0;
        if(p_rece_buf->index<COMM_FRAME){
          p_rece_buf->buf[p_rece_buf->index++] =c;
        }
   }

  if(p_rece_buf->end_flag>0)
  {
    if(p_rece_buf->index){
        msg_pos =msg_find_unused();
        if(msg_pos!=0xffff){
		      memcpy(msg_buf[msg_pos].buf,p_rece_buf->buf,COMM_FRAME);
          msg_buf[msg_pos].used =MSG_USE;
        }
    }
	  p_rece_buf->end_flag =0;
	  p_rece_buf->index=0;
    memset(p_rece_buf->buf,0,COMM_FRAME);
  }
}

const msg_cmd_t const_msg_cmd[]=
{
  //本机主动
  {CMD_BT100,msg_cmd_bt100},
  {CMD_BT101,msg_cmd_bt101},
  {CMD_BT102,msg_cmd_bt102},
  {CMD_BT103,msg_cmd_bt103},
  {CMD_BT104,msg_cmd_bt104},
  {CMD_BT105,msg_cmd_bt105},
  {CMD_BT303,msg_cmd_bt303},
  {CMD_BT304,msg_cmd_bt304},
  {CMD_BT107,msg_cmd_bt107},
  //从机主动
  {CMD_D101,msg_cmd_d101},
  {CMD_D102,msg_cmd_d102},
  {CMD_D103,msg_cmd_d103},
};

