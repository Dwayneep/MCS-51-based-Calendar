#include "reg51.h"
typedef unsigned char uchar;
typedef unsigned int uint;
sbit E=P2^7;
sbit RS=P2^6;
sbit RW=P2^5;
sbit K1=P3^1;
sbit K2=P3^0;
sbit K3=P3^2;
sbit K4=P3^3;
uchar secjmpflag=0;//+1S
uint second=50,minute=59,hour=11;
uint date=6,month=1,year=2023;
uint MonthVEC[12] = {31,31,28,31,30,31,30,31,31,30,31,30};//平年 12-1-2-..-11    %12 鲁棒性
uint S_MonthVEC[12] = {31,31,29,31,30,31,30,31,31,30,31,30};//闰年 12-1-2-..-11
uchar code WeekMod[7][3] = {"Mon","Tue","Wed","Thr","Fri","Sat","Sun"};
uint cur_location[6] = {0x84,0x87,0x8A,0x80+0x40+2,0x80+0x40+5,0x80+0x40+8};//cursor location
uchar K3_flag = 0;
uchar K4_flag = 0;
uint set=0;//按键设置

//**********************Dwyane   2023-01-05**********************

void display_Date_Week();
void display_TIME();
void adjust_time();
void adjust_date();//called by prev
void pressK1();//change cursor position
void pressK2();//adjust time
void pressK3();
void pressK4();
void delayShort();
void delayLong();
void Command(uchar com);
void showSingleChar(uchar ch);
void init();
void clear();
void setpositions();//cursor location
void timeacc();//seconds calculation by interrupt

void main(){
    init();
    clear();
    EA=1;
    ET1=1;
    TMOD=0X10;
    TH1=0X3C;//(65536-50000)/256
    TL1=0XB0;//(65536-50000)%256
    TR1=1;
    adjust_time();
    adjust_date();
    display_Date_Week();
    while (1)
    {
      if(secjmpflag >= 20 && (K1 & K2 & K3  & K4)!=0 ){
        secjmpflag -= 20; ++second;
        adjust_time();//加秒进位
        display_TIME();
      }//不按键一直走
      
      if((hour|minute|second)==0x00)  display_Date_Week();//每天0点更新日期

      if(K3 == 0 ){
        delayLong();//消颤
        delayLong();
        delayLong();
        K3_flag = 1;
        pressK3();
      }//K3

      if(K4 == 0 ){
        delayLong();//消颤
        delayLong();
        delayLong();
        K4_flag = 1;
        pressK4();
      }//K4

    }

} 

void timeacc() interrupt 3{
  TR1=0;
  TH1=0X3C;//(65536-50000)/256
  TL1=0XB0;//(65536-50000)%256
  TR1=1;
  ++secjmpflag;
}//计数

void adjust_time(){
  while(second >= 60){
          second-=60;
          ++minute;
            while(minute >= 60){
          minute -= 60;
          ++hour;
                while(hour >= 24){
              hour -= 24;
              ++date;
              adjust_date();
      }
    }
  }
}

void adjust_date(){
  if(year % 400==0 || (year % 4 == 0 && year % 100!=0)){
      while(date >  S_MonthVEC[month % 12]){
        date -= S_MonthVEC[month % 12];
        ++month;
          while (month > 12 )
          {
            month -= 12;
            ++year;
          }
    }
  }//闰年判断
  else{
    while(date >  MonthVEC[month% 12]){
        date -= MonthVEC[month% 12];
        ++month;
          while (month > 12 )
          {
            month -= 12;
            ++year;
          }
    }
  }//平年判断
}//!! month-1

void display_TIME(){
  Command(0x80+0x40+1);//换行
  showSingleChar('0'+hour / 10);//H
  showSingleChar('0'+hour % 10);
  showSingleChar(':');
  showSingleChar('0'+minute / 10);//M
  showSingleChar('0'+minute % 10);
  showSingleChar(':');
  showSingleChar('0'+second / 10);//S
  showSingleChar('0'+second % 10);
}

void display_Date_Week(){
  //显示日期
  int temp,W,tempM,tempY;
  Command(0x80+1);//缩进
  //showSingleChar('0'+year/1000);//y
  showSingleChar('0'+year/1000);//y
	showSingleChar('0'+year/100%10);
	showSingleChar('0'+year/10%10);
	showSingleChar('0'+year%10);
  showSingleChar('-');
  showSingleChar('0'+month/10);//m
	showSingleChar('0'+month%10);
  showSingleChar('-');
  showSingleChar('0'+date/10);//d
	showSingleChar('0'+date%10);
  showSingleChar(' ');//缩进
  //星期判断 基姆拉尔森计算公式

  if (month < 3) {
        tempM = month + 12;
        tempY = year - 1;
  }
  else{
    tempM = month; tempY = year;
  }
  W = (date + 2 * tempM + 3 * (tempM +1) / 5 + tempY + tempY / 4 - tempY / 100 + tempY / 400 ) % 7; 
  for (temp = 0; temp < 3; temp++) showSingleChar((uchar)WeekMod[W][temp]);
  
}

void delayShort(){
		uint n=2000;
		while(n--);
}

void delayLong(){
	  uint n=10000;
		while(n--);
}

void Command(uchar com){
	P0=com;	  //com输入命令码 通过P2送给LCD
	RS=0;      //RS=0 写命令
  RW=0;
	delayShort();	  
	E=1;      //LCD使能端E 高电平
	delayShort();
	E=0;       //LCD使能端E 低电平
}

void showSingleChar(uchar ch){
  RS=1;
  RW=0;
  E=0;
  P0=ch;
  delayLong();
  E=1;
  E=0;
}

void init(){
	E=0;       
	Command(0x38);
  Command(0x0C);
	Command(0x06);
	Command(0x01);
	Command(0x80);
}//1602 INIT

void clear(){
  E=0;
	Command(0x01);
}

void setpositions(){
  Command(0x0F);
  Command(cur_location[set]);
}//光标闪烁

void pressK1(){
  ++set;
  if(set >= 6) set -= 6;
  setpositions();
  delayLong();
}

void pressK2(){
  switch (set)
  {
  case 0:
  ++year;
  display_TIME();
  display_Date_Week();
    break;

  case 1:
  ++month;
  if (month > 12) {
    month -= 12;
    ++year;
  }
  display_TIME();
  display_Date_Week();
    break;

  case 2:
  ++date;
  adjust_date();
  display_TIME();
  display_Date_Week();
    break;

  case 3:
  second += 3600;
  adjust_time();
  display_TIME();
  display_Date_Week();
    break;

  case 4:
  second += 60;
  adjust_time();
  display_TIME();
  display_Date_Week();
    break;

  case 5:
  ++second;
  adjust_time();
  display_TIME();
  display_Date_Week();
    break;
  
  default:
    break;
  }
}//switch语句选择


void pressK3(){
  set = 0;
  setpositions();
  //秒针停止，光标闪烁                                                      

  while(1){
    delayLong();
    if(K3_flag != 0 ) K3_flag &= K3;
    //showSingleChar('0'+K3_flag);

    if(K3_flag == 0) {
      Command(0x0C);//与setpositions()相对，关闭光标
      display_Date_Week();
      display_TIME();
      break;
      }
      if(K1 == 0 ){
        delayLong();
        pressK1();
      }
      if(K2 == 0 ){
        delayLong();
        pressK2();
      }
  }

}

void pressK4(){                
  uint op=0;
  uint a=2586,b=4682;//预定义需要逻辑运算的操作数
  uchar store[3]={'&','|',' ^'};
  uint temp,result,bas;
  result = ((a & b | year) & (month & second ) & minute & (hour & date)); 
  clear();
  showSingleChar('0'+a/1000);
	showSingleChar('0'+a/100%10);
	showSingleChar('0'+a/10%10);
	showSingleChar('0'+a%10);

  showSingleChar('?');

  showSingleChar('0'+b/1000);
  showSingleChar('0'+b/100%10);
  showSingleChar('0'+b/10%10);
  showSingleChar('0'+b%10);

   while(1){
    delayLong();
    if(K4_flag != 0 ) K4_flag &= K4;
    if(K4_flag == 0) {
      Command(0x0C);
      clear();
      display_Date_Week();
      display_TIME();
      break;
      }

    if(K1 == 0 ){
      delayLong();
    Command(0x0F);
    Command(0x80+4);
    }
    if(K2 == 0 ){
        delayLong();
        ++op;
        while(op >2) op -=3;
      Command(0x0F);//显示操作
      Command(0x80+4);
      showSingleChar(store[op]);
      delayLong();
      Command(0x0C);//换行
      Command(0x80+0x40);
      switch (op)
      {
      case 0:result =a & b;//AND
        break;
      case 1:result =a | b;//OR
        break;
      case 2:result =a ^ b;//XOR
        break;
      default:
        break;
      }

      showSingleChar('0'+result/1000);//print result 
      showSingleChar('0'+result/100%10);
      showSingleChar('0'+result/10%10);
      showSingleChar('0'+result%10);

      }

      }
  }
