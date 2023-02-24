#ifndef __THINGSLISTS_H
#define __THINGSLISTS_H

#include "main.h"
#include "thingslists.h"

//thingslists.c 业务部分，业务逻辑都规划在这里修改
//thingsnoos.c  函数库，尽量不去修改
//将thingslists.c .h  thingsnoos.c .h文件移到工程当中 
//函数ThingsL_erRunSequence();放到主循环当中调用
//函数ThingsL_TimeCntFun();放到系统定时中断中调用 
//经过以上操作 thingslist.c就可以拿来修改使用了
//thingslists.c .h文件 和 thingsnoos.c .h文件中的PCSET概念：
//对于thingslists.c .h文件 和 thingsnoos.c .h文件而言一个系统有P、C、CS、T 等构成
//P (Parameter):        为系统当中的参数
//C (Case):             为系统当中的事情
//CS(Case'Sates):       为系统当中各事情的状态
//T (Trigger):          为系统系统当中的触发 它可以导致CS状态的转移  T可以分成PET 和 TET
//    PET (Paramamter Event Trigger)；系统当中参数型事件触发、硬件IO、DATA或者LCD等都可以归为PET
//    TET (Timer Event Trigger):系统当中时间型事件的触发
//大致构成如下
// C0------S0
//      |--S1
//      |--S2     
//      |--S3          <==TET===>    Timer
//      |--S4          <==PET===>    P   <===>   LCD
// C1------S0                        P   <===>   IO
//      |--S1                        P   <===>   DATA
//      |--S2
// C3------S0
//      |--S1
//      |--S2
//      |--S3


#define THINGCNUM 3 // 事情总数
#define MAXLCDLIST 400
#define LCDMAXRX 100
typedef struct
{
    unsigned short recHid;
    unsigned short recKid;
    unsigned char pauseSnedFlag;

    unsigned char none;
    unsigned char AAAAAAAA; // 采样模式
    unsigned char BBBBBBBB; // 显示的留几号瓶显示1-25这25个瓶子
    float CCCCCCCC;
    float DDDDDDDD;
    unsigned char EEEEEEEE; // 运行状态：0待机 1运行
    signed short FFFFFFFF;  // 温度
    unsigned char doorCnt;
    unsigned char Year;
    unsigned char Month;
    unsigned char Week;
    unsigned char Date;
    unsigned char Hour;
    unsigned char Minute;
    unsigned char Second;
    unsigned char flag;
    unsigned char nyrsfm[6];
    unsigned char version[8]; // 版本号

} RUN_PARA;

#define getHangNum() sizeof(ParaList) / sizeof(ParaList[0]) // 参数数量


extern const char *NNNNNNNNNN[];
extern const char *ccModeCode[];
extern const char *llModeCode[];
extern const char *ggModeCode[];
extern const char *ggStateCode[];
extern const char *passWdCode[];
extern const char *combtrCode[];
extern const char *guModCode[];



#endif