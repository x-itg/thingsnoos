#ifndef __THINGSNOOS_H
#define __THINGSNOOS_H
#include "thingslists.h"
#include "main.h"


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


#define ThingsLeventNumMax 20
// 事件路由
#define EvtNon (0 << 0)
#define EvtKey (1 << 0)
#define EvtCom (1 << 1)
#define EvtRun (1 << 2) // 一般事件
#define EvtTy4 (1 << 3)
#define EvtTy5 (1 << 4)

// 不指定继续（状态值为负数），或指定继续（状态值大于等于0）   跳转放行
#define ThingsL_PollNumSelfSub                                           \
    if (ThingsL_PollNum > 0 && ThingsL_PollNum != ThingsL_PauseCaseCode) \
    {                                                                    \
        ThingsL_PollNum--;                                               \
    }                                                                                       // 驻留次数自减 如果为附属就不赋值                                                                                                            //循环次数自减  不暂停case的情况下才能自减
#define ThingsL_PollNum (Things_C[i].ThingsL_JumpOn)                                    // 驻留次数
#define ThingsL_JumpIsEn (ThingsL_PollNum <= 0 && ThingsL_PollNum != ThingsL_PauseCaseCode) // 减到0或负值才能单次跳转反向，不然就是循环次数
#define ThingsL_MState (Things_C[i].ThingsL_State)                                      // 当前事情的状态
#define ThingsL_MTimeCnt (Things_C[i].ThingsL_TimeCnt)                                  // 当前时间的毫秒计数
#define ThingsL_EventRcvFunc (*Things_C[i].eventrcv)(i)                                 // 事件接收
#define ThingsL_MTimeMax (Things_C[i].plistlist[j].internalCntMax)                      // 定时回调的超时数
#define ThingsL_StepSegMod (Things_C[i].plistlist[j].StepSegMod)                        // 步骤中三种模式  第一种只运行预处理函数 第二章只运行定时函数 第三种先运行预处理函数再运行定时函数
#define ThingsL_StepSegFlag (Things_C[i].pstepsta[j])                                   // 步骤中当前环节标志
#define ThingsL_StepPreFunc (*Things_C[i].plistlist[j].funStepPre)(i)                   // 预处理环节
#define ThingsL_StepPollFunc (*Things_C[i].plistlist[j].funStepJue)(i)                  // 间隔调用环节
#define ThingsL_StepLeftNextIndex (Things_C[i].plistlist[j].leftnextIndex)              // 当前步骤记录行 标记的下一条 执行的步骤序号
#define ThingsL_StepRightNextIndex (Things_C[i].plistlist[j].rightnextIndex)
#define ThingsL_I SchcurrentIindex    // 当前事情列表序号
#define ThingsL_J SchcurrentJindex    // 当前步骤列表序号
#define ThingsL_MainPollCode (-555)   // 主调用[不会操作到ThingsL_JumpOn、ThingsL_State、ThingsL_TimeCnt]
#define ThingsL_StopSwitchCode (-444) // 暂停switch 跳转关闭  将不会运行switch
#define ThingsL_PauseCaseCode (-333)  // 暂停case 跳转关闭  停留在case中  停止步骤的跳转 一直在步骤循环调用
#define ThingsL_StaGoon (-222)
typedef enum Jumptype
{
    JumpStay_____ = -4, // 驻留     具体步骤环节函数返回0时
    JumpLeft,           // 向左跳转 具体步骤环节函数返回1时
    JumpRight,          // 向右跳转 具体步骤环节函数返回2时 另外一个方向
    JumpOver,           // 跳过定时环节函数
    JumpS0,
    JumpS1,
    JumpS2,
    JumpS3,
    JumpS4,
    JumpS5,
    JumpS6,
    JumpS7,
    JumpS8,
    JumpS9,
    JumpS10,
    JumpS11,
    JumpS12,
    JumpS13,
    JumpS14,
    JumpS15,
    JumpS16,
    JumpS17,
    JumpS18,
    JumpS19,
    JumpS20, // 超过20 的状态 直接赋数值吧  这样
} JUMPTYPE;
// 环节模式
typedef enum STEP
{
    STEP_PRE = 0, // 只运行预处理环节
    STEP_POLL,    // 只运行定时回调环节
    STEP_PREPOLL, // 先立即执行预处理函数 再超时后执行定时回调函数
} STEPSMINI;
// 一件事情所涉及到的步骤都放在这个表里
// 步骤名
// 环节模式
// 环节状态
// 定时回调的定时值 一般为毫秒
// 下一步骤事情左跳(环节函数返回顺序执行时事情状态所赋值的值)状态号 一般可以定义成沿着步骤号的增加而递增
// 下一步骤事情右跳(环节函数返回跳转执行时事情状态所赋值的值)状态号 一般可以在状态不满足的情况下状态的返回 步骤的重复执行这样
// 预处理环节函数  其返回值有三种可能的值：正常顺序执行、跳转执行、驻留表示重复执行该定时环节
// 定时回到函数    其返回值有三种可能的值：正常顺序执行、跳转执行、驻留表示重复执行该定时环节
typedef struct listsTaskFrame
{
    const char *title;             // 步骤的标题 在外部调用的时候可以查找匹配的字符串找出步骤列表中的序号
    STEPSMINI StepSegMod;          // 步骤环节预设的三种模式 第一种只运行预处理函数，第二种只运行定时函数，第三种先运行预处理函数再运行定时函数
    signed int internalCntMax;     // 定时调用的超时时间单位毫秒
    unsigned short leftnextIndex;  // （预设的）下一左侧状态序号
    unsigned short rightnextIndex; // （预设的）下一右侧状态序号
    JUMPTYPE(*funStepPre)
    (unsigned char); // 每个步骤的预处理函数
    JUMPTYPE(*funStepJue)
    (unsigned char); // 按步骤列表中的internalCntMax定时回调返回正数才会退出当前状态

} THINGS_CS;
// 环节状态
typedef enum STEPSTATUS
{
    STEP_0 = 0, // 初始值
    STEP_1,     // 已运行预处理函数
    STEP_2,     // 已经运行定时函数
} STEPSTA;

//-----------------------事情List-------------------------------------------------------------------
// 事情名称                    外外部指令控制时候可以查找事件名称的并找到序号再调用
// 驻留次数                    受外部指令调用时候可以设定驻留次数
// 环节模式                    步骤的三种环节模式：步骤环节预设的三种模式 第一种只运行预处理函数，第二种只运行定时函数，第三种先运行预处理函数再运行定时函数
// 环节状态                    当前环节状态
// 定时器计数                  每间事情的时钟计数器
// 这件事对应的步骤列表中首行记录地址
// 这件事情相关参数地址
// 所关心事件的掩码
// 是否已经被这件事情获得该类型事件的标记
typedef struct ThingsL_perFrame
{
    char *text;                                                            // 一般可以设置成2个中文1个英文0作为结束符号  这样作为事情的名称  外部查找发送指令时能用到
    signed short ThingsL_JumpOn;                                           // 驻留计数
    signed short ThingsL_State;                                            // 一件事情里面的第几个小case 这件事情的进程状态
    signed int ThingsL_TimeCnt;                                            // 毫秒计数
    signed short (*ThingsL_Poll)(unsigned char, signed short, signed int); // 事情函数 事情列表中其实放着就是调度器函数ThingsL_GeneralSch
    const THINGS_CS *plistlist;                                       // 步骤指针
    STEPSTA *pstepsta;
    // void * thingsrundata;                                             //事情的运行参数的指针
    JUMPTYPE(*eventrcv)
    (unsigned char);        // 每件事情的 事件接收函数
    unsigned int eventmask; // 所关心事件的掩码，事情识别到所关心事件类型（每种事件类型以单独位为1的32位整数表示）后，该行事情的eventmask变量中事件类型对应的那位清零
} THINGS_C;

typedef unsigned char u8;
typedef unsigned char __1_byte;
typedef char s8;
typedef unsigned short u16;
typedef short s16;
typedef unsigned int u32;
typedef int s32;
typedef int __S32_bit;

typedef struct
{
    unsigned char b0 : 1;
    unsigned char b1 : 1;
    unsigned char b2 : 1;
    unsigned char b3 : 1;
    unsigned char b4 : 1;
    unsigned char b5 : 1;
    unsigned char b6 : 1;
    unsigned char b7 : 1;
} __8_bits;

typedef union
{
    __8_bits bits;
    __1_byte U8;
    __1_byte S8;
} __8_type;

typedef struct
{
    unsigned char b0 : 1;
    unsigned char b1 : 1;
    unsigned char b2 : 1;
    unsigned char b3 : 1;
    unsigned char b4 : 1;
    unsigned char b5 : 1;
    unsigned char b6 : 1;
    unsigned char b7 : 1;
    unsigned char b8 : 1;
    unsigned char b9 : 1;
    unsigned char b10 : 1;
    unsigned char b11 : 1;
    unsigned char b12 : 1;
    unsigned char b13 : 1;
    unsigned char b14 : 1;
    unsigned char b15 : 1;

} __16_bits;

typedef union
{
    struct
    {
        unsigned char byte0;
        unsigned char byte1;
    } byte;
    __16_bits bits;
    int S16;
    unsigned int U16;
} __16_type;

typedef struct
{
    unsigned char b0 : 1;
    unsigned char b1 : 1;
    unsigned char b2 : 1;
    unsigned char b3 : 1;
    unsigned char b4 : 1;
    unsigned char b5 : 1;
    unsigned char b6 : 1;
    unsigned char b7 : 1;
    unsigned char b8 : 1;
    unsigned char b9 : 1;
    unsigned char b10 : 1;
    unsigned char b11 : 1;
    unsigned char b12 : 1;
    unsigned char b13 : 1;
    unsigned char b14 : 1;
    unsigned char b15 : 1;
    unsigned char b16 : 1;
    unsigned char b17 : 1;
    unsigned char b18 : 1;
    unsigned char b19 : 1;
    unsigned char b20 : 1;
    unsigned char b21 : 1;
    unsigned char b22 : 1;
    unsigned char b23 : 1;
    unsigned char b24 : 1;
    unsigned char b25 : 1;
    unsigned char b26 : 1;
    unsigned char b27 : 1;
    unsigned char b28 : 1;
    unsigned char b29 : 1;
    unsigned char b30 : 1;
    unsigned char b31 : 1;

} __32_bits;
typedef union
{
    struct
    {
        unsigned char byte0;
        unsigned char byte1;
        unsigned char byte2;
        unsigned char byte3;

    } byte;
    __32_bits bits;
    unsigned int U32;
    int S32;
} __32_type;

typedef enum
{
    V_NOP,      // 一级字符串型号（即将*pVar指向的值根据值类型转换成字符串传给屏幕显示）（0-255）例如按钮控件直接发值的 null variable type
    V_CHAR,     // 一级字符串型号（即将*pVar指向的值根据值类型转换成字符串传给屏幕显示） char
    V_UCHAR,    // 一级字符串型号（即将*pVar指向的值根据值类型转换成字符串传给屏幕显示） unsigned char
    V_SHORT,    // 一级字符串型号（即将*pVar指向的值根据值类型转换成字符串传给屏幕显示） short
    V_USHORT,   // 一级字符串型号（即将*pVar指向的值根据值类型转换成字符串传给屏幕显示） unsigned short
    V_INT,      // 一级字符串型号（即将*pVar指向的值根据值类型转换成字符串传给屏幕显示） integer
    V_UINT,     // 一级字符串型号（即将*pVar指向的值根据值类型转换成字符串传给屏幕显示） unsigned integer
    V_FLOAT,    // 一级字符串型号（即将*pVar指向的值根据值类型转换成字符串传给屏幕显示） 浮点数打印出不限小数位按%f打印
    V_FLOAT0,   // 一级字符串型号（即将*pVar指向的值根据值类型转换成字符串传给屏幕显示） 浮点数打印出0位小数点 相当于整数
    V_FLOAT1,   // 一级字符串型号（即将*pVar指向的值根据值类型转换成字符串传给屏幕显示） 浮点数打印出1位小数点
    V_FLOAT2,   // 一级字符串型号（即将*pVar指向的值根据值类型转换成字符串传给屏幕显示） 浮点数打印出2位小数点
    V_FLOAT3,   // 一级字符串型号（即将*pVar指向的值根据值类型转换成字符串传给屏幕显示） 浮点数打印出3位小数点
    V_ENUM,     // 二级字符串（即用*pVar的值再定位pEnum[*(unsigned char *)pVar]中的字符串传给屏幕显示）
    V_PASSWORD, // 零级字符串（即pVar为字符串首地址传给屏幕显示） string 字符串当密码用  长度为6
    V_STRING1,  // 零级字符串（即pVar为字符串首地址传给屏幕显示） string
    V_STRING2,  // 零级字符串（即pVar为字符串首地址传给屏幕显示） string
    V_STRING3,  // 零级字符串（即pVar为字符串首地址传给屏幕显示） string
    V_STRING4,  // 零级字符串（即pVar为字符串首地址传给屏幕显示） string
    V_STRING5,  // 零级字符串（即pVar为字符串首地址传给屏幕显示） string
    V_STRING6,  // 零级字符串（即pVar为字符串首地址传给屏幕显示） string
    V_STRING7,  // 零级字符串（即pVar为字符串首地址传给屏幕显示） string
    V_STRING8,  // 零级字符串（即pVar为字符串首地址传给屏幕显示） string
    V_STRING9,  // 零级字符串（即pVar为字符串首地址传给屏幕显示） string
    V_STRING10, // 零级字符串（即pVar为字符串首地址传给屏幕显示） string
    V_STRING11, // 零级字符串（即pVar为字符串首地址传给屏幕显示） string
    V_STRING12, // 零级字符串（即pVar为字符串首地址传给屏幕显示） string
    V_STRING13, // 零级字符串（即pVar为字符串首地址传给屏幕显示） string
    V_STRING14, // 零级字符串（即pVar为字符串首地址传给屏幕显示） string
    V_STRING15, // 零级字符串（即pVar为字符串首地址传给屏幕显示） string
    V_STRING16, // 零级字符串（即pVar为字符串首地址传给屏幕显示） string
    V_STRING17, // 零级字符串（即pVar为字符串首地址传给屏幕显示） string
    V_STRING18, // 零级字符串（即pVar为字符串首地址传给屏幕显示） string
    V_STRING19, // 零级字符串（即pVar为字符串首地址传给屏幕显示） string
    V_STRING20, // 零级字符串（即pVar为字符串首地址传给屏幕显示） string
    V_STRING21, // 零级字符串（即pVar为字符串首地址传给屏幕显示） string
    V_STRING22, // 零级字符串（即pVar为字符串首地址传给屏幕显示） string
    V_STRING23, // 零级字符串（即pVar为字符串首地址传给屏幕显示） string
    V_STRING24, // 零级字符串（即pVar为字符串首地址传给屏幕显示） string
    V_STRING25, // 零级字符串（即pVar为字符串首地址传给屏幕显示） string
    V_STRING26, // 零级字符串（即pVar为字符串首地址传给屏幕显示） string
    V_STRING27, // 零级字符串（即pVar为字符串首地址传给屏幕显示） string
    V_STRING28, // 零级字符串（即pVar为字符串首地址传给屏幕显示） string
    V_STRING29, // 零级字符串（即pVar为字符串首地址传给屏幕显示） string
    V_STRING30, // 零级字符串（即pVar为字符串首地址传给屏幕显示） string

} VAR_TYPE;

typedef enum CUEVENT // 一步骤下的环节状态
{
    NoneEvent = -2,   // 没找到事件
    EventFindTrigger, // 找到事件并触发
    EventFindNoTri,   // 虽然找到但是无需触发
} EventT;
typedef struct _eventList // 事件的表
{
    unsigned int ev_type;
    unsigned int ev_thingsgetmask; // 事件被事情a b c d e f得到的掩码  等每件想要得到事件的事情都取得后再进行删除（广播），对还没删除的事件不能重复触发（这个功能还没做好）
    unsigned short ev_cmd;
    unsigned char ev_len;
    void *ev_Data;
} ThingsL_EVENT_List;

typedef struct _eventControl // 步骤
{
    unsigned char cnt; // 现存事件个数
} ThingsL_Event_Con;

// 密码提示
typedef enum
{
    TIPS_NONE = 0,
    TIPS_AGAIN,
    ERR_NOTSAME,
    ERR_LENTH,
    TIPS_SETSUCCESS,
    TIPS_SHURU,
    TIPS_FAIL,
    TIPS_TRUE,
    TIPS_DOOR,

} MM_TYPE;
typedef struct RecFrame
{
    unsigned short hid;                                                              // 画面id
    unsigned short kid;                                                              // 控件id显示的控件号
    unsigned short rkid;                                                             // 变量接受的控件号
    void *pVar;                                                                      // point to the variable
    unsigned short eeromaddr;                                                        // 数据的存储地址  该值非0x0000时需要存储
    signed int min;                                                                  // 最小值
    signed int max;                                                                  // 最大值
    signed int def;                                                                  // 默认值
    unsigned char **pEnum;                                                           // 2级指针加下标变成一级指针：pEnum[*pVar]
    unsigned char dilen;                                                             // 1的话表示  101表示10.1  --------------------------------
    VAR_TYPE vtype;                                                                  // 值类型跟控件类型相关 如果是0则为按钮类型
    unsigned int EVT;                                                                // 事件值类型
    unsigned short EVC;                                                              // 事件值命令
    unsigned short EVL;                                                              // 事件值长度
    signed char (*pEventFunc)(unsigned int, unsigned short, unsigned short, void *); // 在接受到屏幕发来的指令后，先检索记录序号，检索到序号后将 *pVar这个地址作为形参传入这个回调，回调函数里面根据这个地址判断是哪个参数并进行相应的动作 这样就只要写一个回调函数就行了

} THINGSP;

extern const THINGSP ThingsP[]; // 显示的参数列表extern const REC_FRAME ThingsP[];                //显示的参数列表
extern unsigned short needSetIndex;
extern RUN_PARA runParaData;

void ThingsNum2Str(char *pStr, void *EVV, VAR_TYPE vt);
signed char ThingsL_PutEvent(unsigned int etype, unsigned short ev_cmd, unsigned short ev_len, void *ev_Data);
unsigned char ThingsgetVarLen(VAR_TYPE vt);
void ThingsInsertDot(char *pStr, unsigned short dlen);
unsigned char ThingsDataCmp(unsigned char *p1, unsigned char *p2, unsigned char len);
void ThingsLoadParaFromEErom(unsigned short NUM);
unsigned char ThingsNum_2_Bcd(unsigned int num);
unsigned char ThingsBcd_2_Num(unsigned int bcd);
signed short ThingsCollectPoll(unsigned char i, signed short ConfigState, signed int Stay);
extern unsigned char ThingsReadOneParaFromEE(unsigned char *pBuffer, unsigned short uin_Addr, unsigned short uin_Size);
extern unsigned char ThingsWriteOneParaToEE(unsigned char *pBuffer, unsigned short uin_Addr, unsigned short uin_Size);
signed short ThingsL_GeneralSch(unsigned char i, signed short Config, signed int Stay);
void ThingsL_erRunSequence(void);
void ThingsL_TimeCntFun(void);

unsigned char ergoRecFindHeadTail(unsigned short hid, unsigned short *pHead, unsigned short *pTail, unsigned short *pCurrent);
unsigned short getRecIndex(unsigned short hid, unsigned rkid);
int ThingsStr2inthandle(char *pstr, unsigned char dilen);
void GetTimeSend(void);
void ReadCurrentPageFunc(void);
void lcdQie(unsigned short hid);
void lcdSetButtonValue(unsigned short hid, unsigned short kid, unsigned char Value);
void SetTextValue(unsigned short hid, unsigned short kid, char *str);

unsigned char lcdDecode(char *inBuf, unsigned short *rethid, unsigned short *retkid);
extern THINGS_C Things_C[THINGCNUM];
extern ThingsL_EVENT_List thingsl_event_fifo[ThingsLeventNumMax + 1];
EventT ThingsL_EventGetAndDel(unsigned int Evtype, unsigned char ThingsNum);

signed char ThingsL_GetEvent(unsigned int etype);
signed char ThingsL_DetEvent(signed char id);
extern __IO unsigned int SchcurrentIindex; // 调度器当前i  事情列表中的序号i
extern __IO unsigned int SchcurrentJindex; // 调度器当前j  步骤列表中的序号j
extern unsigned char needPauseFlag;

#endif