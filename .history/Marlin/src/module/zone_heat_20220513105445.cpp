#define HERE_EXEC
#include "zone_heat.h"
#include "..\core\serial.h"
#include "temperature.h"

extern Temperature thermalManager;

SoftwareSerial sw_uart(PB7,PB8);

int spit_string(char *str,const char *c,const char *p[],int cnt)
{
    int i =0;
    char *token;
    while(*str==' ')str++;
    token = strtok((char*)str,c);
    while(token !=nullptr && i<cnt){
       p[i]=token;
       token = strtok(NULL, c);
       i++;
     }
    return i;
}

void cmd_string(const char*cmd,char const *format,...)
{
	char buf[128]={0,};
	va_list argList;
	va_start(argList,format);
	vsprintf(buf,format,argList);
	va_end(argList);
  sw_uart.print(cmd);
  sw_uart.print(" ");
  sw_uart.print(buf);
  sw_uart.print("\n");
}

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
		      memcpy(msg_buf[msg_pos].buf,p_rece_buf->buf,p_rece_buf->index);
          msg_buf[msg_pos].used =MSG_USE;
        }
    }
	  p_rece_buf->end_flag =0;
	  p_rece_buf->index=0;
  }
}

uint8_t check_msg_cmd(uint8_t* mem, char const* cmd)
{
	int i=0;
	for(;;)
	{
		if (cmd[i] =='\0' && (mem[i] == ' ' || mem[i]=='\0'))	return 1;
		if (cmd[i] !=mem[i]) return 0;
		i++;
	}
}

void msg_cmd_bt103(uint8_t *pmsg)
{
  COMM_BIT_CLS(bit_turnoff_heat);
}

#define GET_PARAM_CT(XXX) {\
  token = strtok(NULL," ");\
	if(token==NULL)return;\
	par_temp.compile_time =atoi(token);\
  param_bit_updata|=MaskBits[XXX];\
  debug_string("%d %d\n",XXX,par_temp.compile_time);\
}

#define GET_PARAM_PID(XXX) {\
  token = strtok(NULL," ");\
	if(token==NULL)return;\
	par_temp.pid##XXX[0] =atof(token);\
  token = strtok(NULL," ");\
	if(token==NULL)return;\
	par_temp.pid##XXX[1] =atof(token);\
  token = strtok(NULL," ");\
	if(token==NULL)return;\
	par_temp.pid##XXX[2] =atof(token);\
  debug_string("%d %.2f,%.2f,%.2f\n",XXX,par_temp.pid##XXX[0],par_temp.pid##XXX[1],par_temp.pid##XXX[2]);\
  param_bit_updata|=MaskBits[XXX];\
}

#define GET_PARAM_STEP(XXX){\
  token = strtok(NULL," ");\
	if(token==NULL)return;\
	par_temp.keepstep =atoi(token);\
  token = strtok(NULL," ");\
	if(token==NULL)return;\
	par_temp.slowstep =atoi(token);\
  debug_string("%d %d,%d\n",XXX,par_temp.keepstep,par_temp.slowstep);\
  param_bit_updata|=MaskBits[XXX];\
}

#define GET_PARAM_USED(XXX){\
  token = strtok(NULL," ");\
	if(token==NULL)return;\
	par_temp.used[0] =atoi(token);\
  token = strtok(NULL," ");\
	if(token==NULL)return;\
	par_temp.used[1] =atoi(token);\
  token = strtok(NULL," ");\
	if(token==NULL)return;\
	par_temp.used[2] =atoi(token);\
  token = strtok(NULL," ");\
	if(token==NULL)return;\
	par_temp.used[3] =atoi(token);\
  token = strtok(NULL," ");\
	if(token==NULL)return;\
	par_temp.used[4] =atoi(token);\
  token = strtok(NULL," ");\
	if(token==NULL)return;\
	par_temp.used[5] =atoi(token);\
  token = strtok(NULL," ");\
	if(token==NULL)return;\
	par_temp.used[6] =atoi(token);\
  token = strtok(NULL," ");\
	if(token==NULL)return;\
	par_temp.used[7] =atoi(token);\
  token = strtok(NULL," ");\
	if(token==NULL)return;\
	par_temp.used[8] =atoi(token);\
  debug_string("%d %d,%d,%d,%d,%d,%d,%d,%d,%d\n",XXX,par_temp.used[0],par_temp.used[1],par_temp.used[2],par_temp.used[3],\
  par_temp.used[4],par_temp.used[5],par_temp.used[6],par_temp.used[7],par_temp.used[8]);\
  param_bit_updata|=MaskBits[XXX];\
}

#define GET_TEMPLIMIT(XXX){\
  token = strtok(NULL," ");\
	if(token==NULL)return;\
	par_temp.min_temp =atoi(token);\
  token = strtok(NULL," ");\
	if(token==NULL)return;\
	par_temp.max_temp =atoi(token);\
  param_bit_updata|=MaskBits[XXX];\
  debug_string("%d %d,%d\n",XXX,par_temp.min_temp,par_temp.max_temp);\
}

void msg_cmd_bt104(uint8_t *pmsg)
{
  char *token;
  while(*pmsg==' ')pmsg++;
  token = strtok((char*)pmsg," ");
  if(token==NULL)return;
  int indxe =atoi(token);
  switch (indxe)
  {
    case 0:GET_PARAM_CT(0)break;
    case 1:GET_PARAM_PID(1)break;
    case 2:GET_PARAM_PID(2)break;
    case 3:GET_PARAM_PID(3)break;
    case 4:GET_PARAM_PID(4)break;
    case 5:GET_PARAM_PID(5)break;
    case 6:GET_PARAM_PID(6)break;
    case 7:GET_PARAM_PID(7)break;
    case 8:GET_PARAM_PID(8)break;
    case 9:GET_PARAM_PID(9)break;
    case 10:GET_PARAM_STEP(10)break;
    case 11:GET_PARAM_USED(11);break;
    case 12:GET_TEMPLIMIT(12);break;
    default:break;
  }
}

void msg_cmd_bt105(uint8_t *pmsg)
{
  COMM_BIT_CLS(bit_flash_init);
}

void msg_cmd_bt100(uint8_t *pmsg)
{
#define ITEM_CNT  (2*SUM_TEMP_ZONE)
   const char *p[ITEM_CNT];
   if(spit_string((char*)pmsg," ",p,ITEM_CNT)==ITEM_CNT)
   {
      //char str[128]="";
      //char *pstr=str;
      LOOP_L_N(i,SUM_TEMP_ZONE){
        comm_dat.zone_temp[i]=atoi(p[i]);
        //pstr+=sprintf(pstr,"%d ",comm_dat.zone_temp[i]);
      }
      LOOP_L_N(i,SUM_TEMP_ZONE){
        comm_dat.zooe_state[i]=atoi(p[i+SUM_TEMP_ZONE]);
        //pstr+=sprintf(pstr,"%d ",comm_dat.zooe_state[i]);
      }
      //serial_print_P(str);
      //serial_print_P("\n");

      COMM_BIT_CLS(bit_request_temp);
   }
}

void msg_cmd_bt101(uint8_t *pmsg)
{
  const char *p[SUM_TEMP_ZONE];
  if(spit_string((char*)pmsg," ",p,SUM_TEMP_ZONE)==SUM_TEMP_ZONE)
  {
     LOOP_L_N(i,SUM_TEMP_ZONE){
       comm_dat.zone_pwm[i]=atoi(p[i]);
     }
     COMM_BIT_CLS(bit_requset_pwm);
  }
}

const msg_cmd_t const_msg_cmd[]=
{
  CMD_BT100,msg_cmd_bt100,
  CMD_BT101,msg_cmd_bt101,
  CMD_BT103,msg_cmd_bt103,
  CMD_BT104,msg_cmd_bt104,
  CMD_BT105,msg_cmd_bt105,
};

void msg_deal(void)
{
  LOOP_L_N(i,MSG_SIZE){
    if(msg_buf[i].used==MSG_USE){
      to_uper(msg_buf[i].buf);
      LOOP_L_N(j,GetItemSum(const_msg_cmd)){
         if (const_msg_cmd[j].cmd == NULL || const_msg_cmd[j].func==NULL) continue;
         if (check_msg_cmd(msg_buf[i].buf,const_msg_cmd[j].cmd)){
            int len =strlen(const_msg_cmd[j].cmd);
            const_msg_cmd[j].func(&msg_buf[i].buf[len]);
         }
      }
      msg_buf[i].used =MSG_UNUSE;
      memset(msg_buf[i].buf,0,COMM_FRAME);
    }
  }
}

void printf_nothing(char const *format,...){}
void printf_string(char const *format,...)
{
	char buf[128]={0,};
	va_list argList;
	va_start(argList,format);
	vsprintf(buf,format,argList);
	va_end(argList);
  serial_print_P(buf);
}

void task_start_up(uint8_t id,int32_t ntick)
{
  crBegin(id, ntick);
  task_del_other(task_start_up);
  par_config =const_par;
  debug_string("task_start_up\n");
  do{
    debug_string("close heating\n");
    COMM_BIT_SET(bit_turnoff_heat);
    cmd_string(CMD_T103,"");
    crWait(id,1000);
  }while(COMM_BIT_IS1(bit_turnoff_heat));

  do{
    param_bit_updata =0;
    cmd_string(CMD_T104,"");
    crWait(id,2000);
    debug_string("read flash:%x\n",param_bit_updata);
  }while(param_bit_updata !=SYNC_MASK);
  par_config =par_temp;
 
  debug_string("compile time %d:%d\n",par_config.compile_time,const_par.compile_time);
  if(par_config.compile_time!=const_par.compile_time){
    do{
    debug_string("write flash\n");
    COMM_BIT_SET(bit_flash_init);
    cmd_string(CMD_T105,"0 %d",const_par.compile_time);
    crWait(id,10);
    #define __SEND_PID(XXX) cmd_string(CMD_T105,"%d %.2f %.2f %.2f",XXX,const_par.pid##XXX[0],const_par.pid##XXX[1],const_par.pid##XXX[2]);crWait(id,10)
    __SEND_PID(1);
    __SEND_PID(2);
    __SEND_PID(3);
    __SEND_PID(4);
    __SEND_PID(5);
    __SEND_PID(6);
    __SEND_PID(7);
    __SEND_PID(8);
    __SEND_PID(9);
    cmd_string(CMD_T105,"10 %d %d",const_par.keepstep,const_par.slowstep);
    crWait(id,10);
    cmd_string(CMD_T105,"11 %d %d %d %d %d %d %d %d %d",const_par.used[0],const_par.used[1],const_par.used[2],const_par.used[3],const_par.used[4],\
                                                              const_par.used[5], const_par.used[6],const_par.used[7],const_par.used[8]);
    crWait(id,10);
    cmd_string(CMD_T105,"12 %d %d",const_par.min_temp,const_par.max_temp);
    crWait(id,100);
    }while(COMM_BIT_IS1(bit_flash_init));
    par_config =const_par;
  }

  task_get(task_request_pwm);
  task_get(task_request_temp);
  COMM_BIT_CLS(bit_setup_finish);
  debug_string("start ok\n");
  crDel(id);
  crFinish(id)     
}

void task_request_temp(uint8_t id,int32_t ntick)
{
	crBegin(id, ntick);
  COMM_BIT_SET(bit_request_temp);
  cmd_string(CMD_T100,"");
	crWait(id,2000);
	crFinish(id)
}

void task_request_pwm(uint8_t id,int32_t ntick)
{
   crBegin(id, ntick);
   COMM_BIT_SET(bit_requset_pwm);
   cmd_string(CMD_T101,"");
   crWait(id,5000);
   crFinish(id)
}

void task_pid_autotune(uint8_t id,int32_t ntick)
{
   static char strtemp[64];
   static char *p;
   crBegin(id, ntick);
   memset(strtemp,0,sizeof(strtemp));
   p=strtemp;
   SERIAL_ECHOLNPGM(STR_PID_AUTOTUNE_START);
   thermalManager.disable_all_heaters();
   comm_bit_updata|=MaskBits[bit_auto_pid];
   bed_string("%s E-%d C%d S%d\n",CMD_T303,g_m303.h,g_m303.c,(int)g_m303.temp);
   crWait(id,2000);
   if((comm_bit_updata &MaskBits[bit_auto_pid])==0){
      do{
        comm_bit_updata |=MaskBits[bit_pid_state];
        bed_string("%s\n",CMD_T304);
        crWait(id,2000UL);
        if((comm_bit_updata & MaskBits[bit_pid_state])!=0){
            p+=sprintf(p,"pid state comm err\n");
           break;
        }
        thermalManager.print_heater_states(H_E0);
        SERIAL_EOL();
      }while((pid_bit_updata &MaskBits[pid_bit_act])!=0);

      if((pid_bit_updata &MaskBits[pid_bit_succ])!=0){
        p+=sprintf(p,"\nPID[%d] Autotune finished!\n",g_m303.h);
#define TYPECASE(XXX)  case XXX:p+=sprintf(p,"Last Kp:%.2f\nLast Ki:%.2f\nLast Kd:%.2f\nok\n",par_config.pid##XXX[0],par_config.pid##XXX[1],par_config.pid##XXX[2]);break;
        switch(g_m303.h)
        {
            TYPECASE(1)TYPECASE(2)TYPECASE(3)TYPECASE(4)
            TYPECASE(5)TYPECASE(6)TYPECASE(7)TYPECASE(8)
            TYPECASE(9)
        }
      }else{
        p+=sprintf(p,"autotune pid fail\n");
      }

      if((pid_bit_updata &MaskBits[pid_bit_overtime])!=0){
        p+=sprintf(p,"autotune pid over time\n");
      }

   }else
   {
     p+=sprintf(p,"autotune comm err\n");
   }

   SERIAL_ECHOLNPGM_P((const char*)strtemp);
   pid_bit_updata =0;
   ui.reset_status();
   crDel(id);
   crFinish(id)
}


