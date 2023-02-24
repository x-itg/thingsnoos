#include "thingsnoos.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

// thingslists.c 业务部分，业务逻辑都规划在这里修改
// thingsnoos.c  函数库，尽量不去修改
// 将thingslists.c .h  thingsnoos.c .h文件移到工程当中
// 函数ThingsL_erRunSequence();放到主循环当中调用
// 函数ThingsL_TimeCntFun();放到系统定时中断中调用
// 经过以上操作 thingslist.c就可以拿来修改使用了
// thingslists.c .h文件 和 thingsnoos.c .h文件中的PCSET概念：
// 对于thingslists.c .h文件 和 thingsnoos.c .h文件而言一个系统有P、C、CS、T 等构成
// P (Parameter):        为系统当中的参数
// C (Case):             为系统当中的事情
// CS(Case'Sates):       为系统当中各事情的状态
// T (Trigger):          为系统系统当中的触发 它可以导致CS状态的转移  T可以分成PET 和 TET
//     PET (Paramamter Event Trigger)；系统当中参数型事件触发、硬件IO、DATA或者LCD等都可以归为PET
//     TET (Timer Event Trigger):系统当中时间型事件的触发
// 大致构成如下
//  C0------S0
//       |--S1
//       |--S2
//       |--S3          <==TET===>    Timer
//       |--S4          <==PET===>    P   <===>   LCD
//  C1------S0                        P   <===>   IO
//       |--S1                        P   <===>   DATA
//       |--S2
//  C3------S0
//       |--S1
//       |--S2
//       |--S3

// 空事件
signed char ThingsNullEvent(unsigned int EVT, unsigned short EVC, unsigned short EVL, void *p) { return 1; } // 这个函数用于填充不需要回调的菜单记录结构体相应位置当中，一个空函数

const char *NNNNNNNNNN[] = {
    "        "};
const char *ccModeCode[] = {
    "---------",
    "+++++++++",
    "#########",

};

const char *guModCode[] = {
    "无",
    "H2SO4", // 0
    "NaOH",  // 1
    "CaOH"   // 2

};

RUN_PARA runParaData; // 参数结构体

// 事件接收函数
JUMPTYPE funcEventRcv(unsigned char i) // 事件处理函数
{

    EventT et = ThingsL_EventGetAndDel(EvtRun, i); // 事件类型，事情序号，[抓取事情的关联事件类型]

    if (et == EventFindTrigger)
    {
        switch (thingsl_event_fifo[ThingsLeventNumMax].ev_cmd)
        {
        case 0x0001:
            break;
        case 0x0002:
            break;
        }
    }
    return JumpLeft;
}

// 预处理环节  默认不处理
JUMPTYPE nonePre(unsigned char i)
{
    return JumpLeft;
}

/****************************************************************
事情A
****************************************************************/
// 事情各事件的状态
//  STEP_0初始值
//  STEP_1已运行预处理函数
//  STEP_2已经运行定时函数
STEPSTA THINGStC0_Stp[2] = {
    STEP_0,
    STEP_0,

};

// C0的S0状态下的周期处理函数
// return JumpLeft时  S状态跳转至leftnextIndex状态
// return JumpRight时 S状态跳转至rightnextIndex状态
// return JumpStay_____时 S状态不变
JUMPTYPE C0_S0_Poll(unsigned char i) // S0定时函数  判断是否需要调速度
{
    return JumpLeft;
}

// C0的S1状态下的周期处理函数
// return JumpLeft时  S状态跳转至leftnextIndex状态
// return JumpRight时 S状态跳转至rightnextIndex状态
// return JumpStay_____时 S状态不变
JUMPTYPE C0_S1_Poll(unsigned char i) // S0定时函数  在这里加减速度
{
    return JumpLeft;
}

/****************************************************************
事情B
****************************************************************/
// 事情各事件的状态
//  STEP_0初始值
//  STEP_1已运行预处理函数
//  STEP_2已经运行定时函数
STEPSTA THINGStC1_Stp[2] = {
    STEP_0,
    STEP_0,

};

// C1的S0状态下的周期处理函数
// return JumpLeft时  S状态跳转至leftnextIndex状态
// return JumpRight时 S状态跳转至rightnextIndex状态
// return JumpStay_____时 S状态不变
JUMPTYPE C1_S0_Poll(unsigned char i) // S0定时函数  判断是否需要调速度
{
    return JumpLeft;
}
// C1的S1状态下周期处理函数
// return JumpLeft时  S状态跳转至leftnextIndex状态
// return JumpRight时 S状态跳转至rightnextIndex状态
// return JumpStay_____时 S状态不变
JUMPTYPE C1_S1_Poll(unsigned char i) // S0定时函数  在这里加减速度
{
    return JumpLeft;
}

/****************************************************************
事情C
****************************************************************/
// 事情各事件的状态
//  STEP_0初始值
//  STEP_1已运行预处理函数
//  STEP_2已经运行定时函数
STEPSTA THINGStC2_Stp[2] = {
    STEP_0,
    STEP_0,

};

// C2的S0状态下周期处理函数
// return JumpLeft时  S状态跳转至leftnextIndex状态
// return JumpRight时 S状态跳转至rightnextIndex状态
// return JumpStay_____时 S状态不变
JUMPTYPE C2_S0_Poll(unsigned char i) // S0定时函数  判断是否需要调速度
{
    return JumpLeft;
}

// C2的S1状态下周期处理函数
// return JumpLeft时  S状态跳转至leftnextIndex状态
// return JumpRight时 S状态跳转至rightnextIndex状态
// return JumpStay_____时 S状态不变
JUMPTYPE C2_S1_Poll(unsigned char i) // S0定时函数  在这里加减速度
{
    return JumpLeft;
}

// P
// ThingsP参数列表
const THINGSP ThingsP[] = {

    {/*LCD画面id*/ 44, /*LCD控件id*/ 2, /*LCD控件接收id*/ 2, /*变量的地址*/ &runParaData.AAAAAAAA, /*变量的存储地址*/ 0x0000, /*变量的范围下限*/ 0, /*变量的范围上限*/ 0, /*变量的默认值*/ 0, /*变量是指针的情况下指向的显示字符串*/ (unsigned char **)ccModeCode, /*变量是数值的情况下变量的小数点在的位数*/ 0, /*变量的类型*/ V_ENUM, /*所触发的事件类型*/ EvtNon, /*事件命令*/ 0x0001, /*事件长度*/ 1, /*参数被修改或被点击后的回调函数*/ ThingsNullEvent},
    {/*LCD画面id*/ 44, /*LCD控件id*/ 5, /*LCD控件接收id*/ 5, /*变量的地址*/ &runParaData.BBBBBBBB, /*变量的存储地址*/ 0x0000, /*变量的范围下限*/ 0, /*变量的范围上限*/ 0, /*变量的默认值*/ 0, /*变量是指针的情况下指向的显示字符串*/ (unsigned char **)NNNNNNNNNN, /*变量是数值的情况下变量的小数点在的位数*/ 0, /*变量的类型*/ V_UCHAR, /*所触发的事件类型*/ EvtNon, /*事件命令*/ 0x0001, /*事件长度*/ 1, /*参数被修改或被点击后的回调函数*/ ThingsNullEvent},
    {/*LCD画面id*/ 44, /*LCD控件id*/ 7, /*LCD控件接收id*/ 7, /*变量的地址*/ &runParaData.CCCCCCCC, /*变量的存储地址*/ 0x0000, /*变量的范围下限*/ 0, /*变量的范围上限*/ 0, /*变量的默认值*/ 0, /*变量是指针的情况下指向的显示字符串*/ (unsigned char **)NNNNNNNNNN, /*变量是数值的情况下变量的小数点在的位数*/ 1, /*变量的类型*/ V_FLOAT1, /*所触发的事件类型*/ EvtNon, /*事件命令*/ 0x0001, /*事件长度*/ 1, /*参数被修改或被点击后的回调函数*/ ThingsNullEvent},
    {/*LCD画面id*/ 44, /*LCD控件id*/ 9, /*LCD控件接收id*/ 9, /*变量的地址*/ &runParaData.DDDDDDDD, /*变量的存储地址*/ 0x0000, /*变量的范围下限*/ 0, /*变量的范围上限*/ 0, /*变量的默认值*/ 0, /*变量是指针的情况下指向的显示字符串*/ (unsigned char **)NNNNNNNNNN, /*变量是数值的情况下变量的小数点在的位数*/ 1, /*变量的类型*/ V_FLOAT1, /*所触发的事件类型*/ EvtNon, /*事件命令*/ 0x0001, /*事件长度*/ 1, /*参数被修改或被点击后的回调函数*/ ThingsNullEvent},
    {/*LCD画面id*/ 44, /*LCD控件id*/ 12, /*LCD控件接收id*/ 12, /*变量的地址*/ &runParaData.EEEEEEEE, /*变量的存储地址*/ 0x0000, /*变量的范围下限*/ 0, /*变量的范围上限*/ 1, /*变量的默认值*/ 0, /*变量是指针的情况下指向的显示字符串*/ (unsigned char **)NNNNNNNNNN, /*变量是数值的情况下变量的小数点在的位数*/ 1, /*变量的类型*/ V_NOP, /*所触发的事件类型*/ EvtRun, /*事件命令*/ 0x0000, /*事件长度*/ 1, /*参数被修改或被点击后的回调函数*/ ThingsL_PutEvent}, // 显示列表 回调事件  使用事件通道触发 ThingsL_PutEvent
    {/*LCD画面id*/ 44, /*LCD控件id*/ 18, /*LCD控件接收id*/ 18, /*变量的地址*/ &runParaData.FFFFFFFF, /*变量的存储地址*/ 0x0000, /*变量的范围下限*/ 0, /*变量的范围上限*/ 0, /*变量的默认值*/ 0, /*变量是指针的情况下指向的显示字符串*/ (unsigned char **)NNNNNNNNNN, /*变量是数值的情况下变量的小数点在的位数*/ 1, /*变量的类型*/ V_SHORT, /*所触发的事件类型*/ EvtNon, /*事件命令*/ 0x0001, /*事件长度*/ 1, /*参数被修改或被点击后的回调函数*/ ThingsNullEvent},
    {/*LCD画面id*/ 79, /*LCD控件id*/ 1, /*LCD控件接收id*/ 1, /*变量的地址*/ &runParaData.version[0], /*变量的存储地址*/ 0x0000, /*变量的范围下限*/ 0, /*变量的范围上限*/ 0, /*变量的默认值*/ 0, /*变量是指针的情况下指向的显示字符串*/ (unsigned char **)NNNNNNNNNN, /*变量是数值的情况下变量的小数点在的位数*/ 0, /*变量的类型*/ V_STRING8, /*所触发的事件类型*/ EvtNon, /*事件命令*/ 0x0001, /*事件长度*/ 1, /*参数被修改或被点击后的回调函数*/ ThingsNullEvent},
    {/*LCD画面id*/ 99, /*LCD控件id*/ 1, /*LCD控件接收id*/ 1, /*变量的地址*/ &runParaData.version[0], /*变量的存储地址*/ 0x0000, /*变量的范围下限*/ 0, /*变量的范围上限*/ 0, /*变量的默认值*/ 0, /*变量是指针的情况下指向的显示字符串*/ (unsigned char **)NNNNNNNNNN, /*变量是数值的情况下变量的小数点在的位数*/ 0, /*变量的类型*/ V_STRING8, /*所触发的事件类型*/ EvtNon, /*事件命令*/ 0x0001, /*事件长度*/ 1, /*参数被修改或被点击后的回调函数*/ ThingsNullEvent},

}; // 待加入：发记录的时候暂停屏幕刷新一会

// CS
// THINGS_C0事情的事件列表
const THINGS_CS Things_C0[2] = {
    {/*S0事件标题*/ "C0_S0", /*回调方式*/ STEP_PREPOLL, /*调用周期ms*/ 5, /*leftnextIndex状态JumpLeft后的下一状态*/ JumpS1, /*rightnextIndex状态JumpRight后的下一状态*/ JumpS0, /*预处理函数*/ nonePre, /*周期处理函数*/ C0_S0_Poll},
    {/*S1事件标题*/ "C0_S1", /*回调方式*/ STEP_PREPOLL, /*调用周期ms*/ 5, /*leftnextIndex状态JumpLeft后的下一状态*/ JumpS0, /*rightnextIndex状态JumpRight后的下一状态*/ JumpS0, /*预处理函数*/ nonePre, /*周期处理函数*/ C0_S0_Poll},
};

// CS
// THINGS_C1事情的事情1的事件列表
const THINGS_CS Things_C1[2] = {
    {/*S0事件标题*/ "C1_S0", /*回调方式*/ STEP_PREPOLL, /*调用周期ms*/ 5, /*leftnextIndex状态JumpLeft后的下一状态*/ JumpS1, /*rightnextIndex状态JumpRight后的下一状态*/ JumpS0, /*预处理函数*/ nonePre, /*周期处理函数*/ C1_S0_Poll},
    {/*S1事件标题*/ "C1_S1", /*回调方式*/ STEP_PREPOLL, /*调用周期ms*/ 5, /*leftnextIndex状态JumpLeft后的下一状态*/ JumpS0, /*rightnextIndex状态JumpRight后的下一状态*/ JumpS0, /*预处理函数*/ nonePre, /*周期处理函数*/ C1_S1_Poll},
};

// CS
// THINGS_C2事情的事情3的事件列表
const THINGS_CS Things_C2[2] = {
    {/*S0事件标题*/ "C2_S0", /*回调方式*/ STEP_PREPOLL, /*调用周期ms*/ 5, /*leftnextIndex状态JumpLeft后的下一状态*/ JumpS1, /*rightnextIndex状态JumpRight后的下一状态*/ JumpS0, /*预处理函数*/ nonePre, /*周期处理函数*/ C2_S0_Poll},
    {/*S1事件标题*/ "C2_S0", /*回调方式*/ STEP_PREPOLL, /*调用周期ms*/ 5, /*leftnextIndex状态JumpLeft后的下一状态*/ JumpS0, /*rightnextIndex状态JumpRight后的下一状态*/ JumpS0, /*预处理函数*/ nonePre, /*周期处理函数*/ C2_S1_Poll},
};

// C
// 事情列表=事情数组
THINGS_C Things_C[THINGCNUM] =
    {
        {/*事情的名称*/ "C0", /*当前事情的事件状态的驻留次数初始值*/ -1, /*事情的事件状态变量初始值*/ 0, /*事情的当前事件超时事件初始值*/ 0, /*轮到该事情轮询函数*/ ThingsL_GeneralSch, /*该事情的事件列表*/ &Things_C0[0], /*事情各事件的状态*/ &THINGStC0_Stp[0], /*事情的事件接收函数*/ funcEventRcv, /*事情所关心的事件类型*/ EvtNon + EvtRun},
        {/*事情的名称*/ "C1", /*当前事情的事件状态的驻留次数初始值*/ -1, /*事情的事件状态变量初始值*/ 0, /*事情的当前事件超时事件初始值*/ 0, /*轮到该事情轮询函数*/ ThingsL_GeneralSch, /*该事情的事件列表*/ &Things_C1[0], /*事情各事件的状态*/ &THINGStC1_Stp[0], /*事情的事件接收函数*/ funcEventRcv, /*事情所关心的事件类型*/ EvtNon + EvtRun},
        {/*事情的名称*/ "C2", /*当前事情的事件状态的驻留次数初始值*/ -1, /*事情的事件状态变量初始值*/ 0, /*事情的当前事件超时事件初始值*/ 0, /*轮到该事情轮询函数*/ ThingsL_GeneralSch, /*该事情的事件列表*/ &Things_C2[0], /*事情各事件的状态*/ &THINGStC2_Stp[0], /*事情的事件接收函数*/ funcEventRcv, /*事情所关心的事件类型*/ EvtNon + EvtRun},
};