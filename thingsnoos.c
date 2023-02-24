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
//    PET (Paramamter Event Trigger)；系统当中参数型事件触发、硬件IO、DATA或者LCD等都可以归为PET
//    TET (Timer Event Trigger):系统当中时间型事件的触发
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

__IO unsigned int SchcurrentIindex = 0; // 调度器当前i  事情列表中的序号i
__IO unsigned int SchcurrentJindex = 0; // 调度器当前j  事情列表中的序号j
unsigned short needSetIndex = 0xffff;
unsigned char needPauseFlag = 0;

// 储存的读取驱动这里添加
unsigned char ThingsReadOneParaFromEE(unsigned char *pBuffer, unsigned short uin_Addr, unsigned short uin_Size)
{
    return 1;
}

// 储存的显示驱动这里添加
unsigned char ThingsWriteOneParaToEE(unsigned char *pBuffer, unsigned short uin_Addr, unsigned short uin_Size)
{
    return 1;
}

// Thingsnoos各变量的长度获取接口
unsigned char ThingsgetVarLen(VAR_TYPE vt)
{
    switch (vt)
    {
    case V_NOP:
    case V_ENUM:
    case V_CHAR:
    case V_UCHAR:
        return 1;
    case V_SHORT:
    case V_USHORT:
        return 2;
    case V_INT:
    case V_UINT:
    case V_FLOAT:
    case V_FLOAT0:
    case V_FLOAT1:
    case V_FLOAT2:
    case V_FLOAT3:
        return 4;
    case V_PASSWORD:
        return 6;
    case V_STRING1:
        return 1;
    case V_STRING2:
        return 2;
    case V_STRING3:
        return 3;
    case V_STRING4:
        return 4;
    case V_STRING5:
        return 5;
    case V_STRING6:
        return 6;
    case V_STRING7:
        return 7;
    case V_STRING8:
        return 8;
    case V_STRING9:
        return 9;
    case V_STRING10:
        return 10;
    case V_STRING11:
        return 11;
    case V_STRING12:
        return 12;
    case V_STRING13:
        return 13;
    case V_STRING14:
        return 14;
    case V_STRING15:
        return 15;
    case V_STRING16:
        return 16;
    case V_STRING17:
        return 17;
    case V_STRING18:
        return 18;
    case V_STRING19:
        return 19;
    case V_STRING20:
        return 20;
    case V_STRING21:
        return 21;
    case V_STRING22:
        return 22;
    case V_STRING23:
        return 23;
    case V_STRING24:
        return 24;
    case V_STRING25:
        return 25;
    case V_STRING26:
        return 26;
    case V_STRING27:
        return 27;
    case V_STRING28:
        return 28;
    case V_STRING29:
        return 29;
    case V_STRING30:
        return 30;
    default:
        return 0;
    }
}

// 将数值按数据类型转换成字符串
// 功能描述  : 依据数据类型，将数据源的内容打印成字符串到目标地址当中
// 输入参数  :pStr目标地址，EVV数据源地址，数据类型
void ThingsNum2Str(char *pStr, void *EVV, VAR_TYPE vt)
{
    switch (vt)
    {
    case V_CHAR:
        sprintf((char *)pStr, "%d", *((signed char *)EVV));
        break;
    case V_UCHAR:
        sprintf((char *)pStr, "%d", *((unsigned char *)EVV));
        break;
    case V_SHORT:
        sprintf((char *)pStr, "%d", *((signed short *)EVV));
        break;
    case V_USHORT:
        sprintf((char *)pStr, "%d", *((unsigned short *)EVV));
        break;
    case V_INT:
        sprintf((char *)pStr, "%d", *((signed int *)EVV));
        break;
    case V_UINT:
        sprintf((char *)pStr, "%d", *((unsigned int *)EVV));
        break;
    case V_FLOAT:
        sprintf((char *)pStr, "%f", *((float *)EVV));
        break;
    case V_FLOAT0:
        sprintf((char *)pStr, "%.0f", *((float *)EVV));
        break;
    case V_FLOAT1:
        sprintf((char *)pStr, "%.1f", *((float *)EVV));
        break;
    case V_FLOAT2:
        sprintf((char *)pStr, "%.2f", *((float *)EVV));
        break;
    case V_FLOAT3:
        sprintf((char *)pStr, "%.3f", *((float *)EVV));
        break;
    default:
        break;
    }
}

// 201.01  2  =>20101
// 201.    2  =>20100
//.01     2  =>1
// 0.01    2  =>1
// 0.000   2  =>0
//-201.1  1  =>-2011
//-201.12 2  =>-20112
// 12345   1  =>12345.0
// 输入参数  :  pstr字符串，dilen小数点在第几位
int ThingsStr2inthandle(char *pstr, unsigned char dilen)
{
    int res = 0;
    unsigned char fuflag = 0;
    unsigned char cnt = 0;
    unsigned char dilentemp = dilen;
    if (strlen(pstr) > 100) // 输入太长了放弃
    {
        return 0;
    }
    if (*pstr == '-')
    {
        fuflag = 1;
        pstr++;
    }
    while (*pstr >= '0' && *pstr <= '9') // 如果是数字才进行转换，数字0~9的ASCII码：48~57
    {
        res = 10 * res + (*pstr++) - '0'; // 字符'0'的ASCII码为48,48-48=0刚好转化为数字0
    }

    if (*pstr == '.') // 第一次遇到可能是小数点
    {
        pstr++; // 有小数点继续放大
        cnt = 0;
        while (*pstr >= '0' && *pstr <= '9' && dilen > 0) // 如果是数字才进行转换，数字0~9的ASCII码：48~57
        {
            cnt++;
            dilen--;
            res = 10 * res + (*pstr++) - '0'; // 字符'0'的ASCII码为48,48-48=0刚好转化为数字0
        }
        while (cnt < dilentemp) // 97.50只输入97.5
        {
            cnt++;
            res = 10 * res;
        }
    }
    else // 没有小数点还要追加 继续放大
    {
        while (dilen > 0)
        {
            dilen--;
            res = 10 * res;
        }
    }

    if (fuflag == 1)
    {
        res = 0 - res;
    }
    return res;
}

// 功能描述  : 纯数字零结尾的字符串在指定位置插入点
// 示例：如字符串"123456" alen6 dlen2 字符串变成"1234.56"
void ThingsInsertDot(char *pStr, unsigned short dlen)
{
    unsigned short i = 0;
    unsigned short alen = 0;
    alen = strlen(pStr);
    if (alen > dlen && alen > 1 && dlen > 0)
    {
        pStr[alen + 1] = 0; // 结束符号
        for (i = alen; i > alen - dlen; i--)
        {
            pStr[i] = pStr[i - 1];
        }
        pStr[i] = '.';
    }
}

// 功能描述  : 数据比较
// 输入参数  :  p1  被比较的数据源地址 p2  被比较的数据源地址  len 所比较的地址长度
unsigned char ThingsDataCmp(unsigned char *p1, unsigned char *p2, unsigned char len)
{
    unsigned short i;
    for (i = 0; i < len; i++)
    {
        if ((*p1++) != (*p2++))
        {
            return 0;
        }
    }
    return 1; // 全部相等
}

// 功能描述  : 将数值转换成BCD码
unsigned char ThingsNum_2_Bcd(unsigned int num)
{
    return (unsigned char)(((num / 10) << 4) | (num % 10));
}

// 功能描述  : 将BCD码转成数值
unsigned char ThingsBcd_2_Num(unsigned int bcd)
{
    return (unsigned char)(((bcd & 0xf0) >> 4) * 10 + (bcd & (0x0f)));
}

// 根据显示参数的列表读出所有存放在EEPROM中的数据
void ThingsLoadParaFromEErom(unsigned short NUM)
{
    unsigned short i;

    signed char vChar = 0;
    unsigned char uChar = 0;
    signed short vShort = 0;
    unsigned short uShort = 0;
    signed int vInt = 0;
    unsigned int uInt = 0;
    float uvfloat = 0;
    for (i = 0; i < NUM; i++) // 按顺序检索
    {
        if (ThingsP[i].eeromaddr != 0x0000) // 存储地址非零表示这个数据需要存储和载入
        {

            switch (ThingsP[i].vtype)
            {

            case V_STRING1:
            case V_STRING2:
            case V_STRING3:
            case V_STRING4:
            case V_STRING5:
            case V_STRING6:
            case V_STRING7:
            case V_STRING8:
            case V_STRING9:
            case V_STRING10:
            case V_STRING11:
            case V_STRING12:
            case V_STRING13:
            case V_STRING14:
            case V_STRING15:
            case V_STRING16:
            case V_STRING17:
            case V_STRING18:
            case V_STRING19:
            case V_STRING20:
                // 根据数值类型判断需要读取的长度
                ThingsReadOneParaFromEE((unsigned char *)ThingsP[i].pVar, ThingsP[i].eeromaddr, ThingsgetVarLen(ThingsP[i].vtype));
                break;
            case V_NOP:
            case V_ENUM:
            case V_UCHAR:
                ThingsReadOneParaFromEE((unsigned char *)&uChar, ThingsP[i].eeromaddr, 1);
                if (uChar > ThingsP[i].max || uChar < ThingsP[i].min)
                    (*(unsigned char *)ThingsP[i].pVar) = ThingsP[i].def;
                else
                    (*(unsigned char *)ThingsP[i].pVar) = uChar;
                break;
            case V_CHAR:
                ThingsReadOneParaFromEE((unsigned char *)&vChar, ThingsP[i].eeromaddr, 1);
                if (vChar > ThingsP[i].max || vChar < ThingsP[i].min)
                    (*(signed char *)ThingsP[i].pVar) = ThingsP[i].def;
                else
                    (*(signed char *)ThingsP[i].pVar) = vChar;
                break;
            case V_SHORT:
                ThingsReadOneParaFromEE((unsigned char *)&vShort, ThingsP[i].eeromaddr, 2);
                if (vShort > ThingsP[i].max || vShort < ThingsP[i].min)
                    (*(signed short *)ThingsP[i].pVar) = ThingsP[i].def;
                else
                    (*(signed short *)ThingsP[i].pVar) = vShort;
                break;
            case V_USHORT:
                ThingsReadOneParaFromEE((unsigned char *)&uShort, ThingsP[i].eeromaddr, 2);
                if (uShort > ThingsP[i].max || uShort < ThingsP[i].min)
                    (*(unsigned short *)ThingsP[i].pVar) = ThingsP[i].def;
                else
                    (*(unsigned short *)ThingsP[i].pVar) = uShort;
                break;
            case V_INT:
                ThingsReadOneParaFromEE((unsigned char *)&vInt, ThingsP[i].eeromaddr, 4);
                if (vInt > ThingsP[i].max || vInt < ThingsP[i].min)
                    (*(signed int *)ThingsP[i].pVar) = ThingsP[i].def;
                else
                    (*(signed int *)ThingsP[i].pVar) = vInt;
                break;
            case V_UINT:
                ThingsReadOneParaFromEE((unsigned char *)&uInt, ThingsP[i].eeromaddr, 4);
                if (uInt > ThingsP[i].max || uInt < ThingsP[i].min)
                    (*(unsigned int *)ThingsP[i].pVar) = ThingsP[i].def;
                else
                    (*(unsigned int *)ThingsP[i].pVar) = uInt;
                break;
            case V_FLOAT:
            case V_FLOAT0:
            case V_FLOAT1:
            case V_FLOAT2:
            case V_FLOAT3:
                ThingsReadOneParaFromEE((unsigned char *)&uvfloat, ThingsP[i].eeromaddr, 4);
                if (uvfloat > ThingsP[i].max || uvfloat < ThingsP[i].min)
                    (*(float *)ThingsP[i].pVar) = ThingsP[i].def;
                else
                    (*(float *)ThingsP[i].pVar) = uvfloat;
                break;
            default:
                break;
            }
        }
    }
}

// 通用调度器
// 把这个放到每个自定义的[事情任务函数]中
// Put this function address into each custom task function
signed short ThingsL_GeneralSch(unsigned char i, signed short Config, signed int Stay)
{
    // 类似二维数组  步骤列表的序号用j来表示吧、事情列表的序号都用i表示哦
    // i 和 j这两个变量名称跟宏定义很多东西相关不要改掉
    signed short j;
    SchcurrentIindex = i;

    signed char stayleftright = JumpStay_____;
    ThingsL_EventRcvFunc; // 事件接收处理  具体事件具体在函数中处理

    if (Stay != ThingsL_MainPollCode) // 驻留
    {
        ThingsL_PollNum = Stay;
    }
    if (Config >= 0) // 指定状态的条件
    {
        ThingsL_MState = Config; // 指定状态
    }
    if (ThingsL_PollNum == ThingsL_StopSwitchCode) // 当前事情序号i停止运转
    {
        ThingsL_MTimeCnt = 0;
        return ThingsL_StopSwitchCode; // 暂停switch 不需要进行进switch case了
    }

    j = ThingsL_MState; // 获取步骤号  一件事情的状态号就是步骤列表的运行序号j
    SchcurrentJindex = j;

    // 预处理 环节
    // 步骤中的定义不是只运行
    if (ThingsL_StepSegMod != STEP_POLL && ThingsL_StepSegFlag == STEP_0) // 不是只运行Poll函数   步骤环节标志还是初始值只运行一
    {
        ThingsL_MTimeCnt = 0;
        ThingsL_StepSegFlag = STEP_1;
        stayleftright = ThingsL_StepPreFunc; // 执行预处理函数并取得其返回值
        if (ThingsL_StepSegMod == STEP_PRE)  // 如果只需要运行预处理函数的那种模式 就恢复成STEP_0
        {
            ThingsL_StepSegFlag = STEP_0;
        }
        if (stayleftright == JumpOver) // 获取预处理函数的返回值
        {
            ThingsL_MState = ThingsL_StepLeftNextIndex;
            ThingsL_StepSegFlag = STEP_0; // 使得定时函数也不会运行
        }
        if (stayleftright >= 0) // 函数返回>=0则直接按返回值跳转
        {
            ThingsL_MState = stayleftright;
            ThingsL_StepSegFlag = STEP_0; // 使得定时函数也不会运行
        }
    }

    // 定时处理环节
    // 步骤中定义不是只运行预处理函数
    if (ThingsL_StepSegMod != STEP_PRE && ThingsL_StepSegFlag != STEP_0) // 不是只运行Pre 函数  运行过预处理环节
    {
        ThingsL_StepSegFlag = STEP_2;
        if (ThingsL_MTimeCnt >= ThingsL_MTimeMax)
        {
            ThingsL_MTimeCnt = 0;
            ThingsL_PollNumSelfSub;
            stayleftright = ThingsL_StepPollFunc;                     // 执行定时回调函数
            if ((stayleftright != JumpStay_____) && ThingsL_JumpIsEn) // 非驻留住跳转到下一个步骤
            {
                if (stayleftright >= 0)
                {
                    ThingsL_StepSegFlag = STEP_0;   // 复位步骤环节标志
                    ThingsL_MState = stayleftright; // 函数返回值是正数的话直接把函数返回值当作序号进行调准
                    return Config;
                }
                else
                {                                  // 函数返回值是负数的话则根据预设的序号进行跳转
                    ThingsL_StepSegFlag = STEP_0;  // 复位步骤环节标志
                    if (stayleftright == JumpLeft) // 向左边跳转
                    {
                        ThingsL_MState = ThingsL_StepLeftNextIndex; // 更到下一个步骤 事情的状态值改变
                    }
                    else // 非左即右
                    {
                        ThingsL_MState = ThingsL_StepRightNextIndex; // 更到下一个步骤 事情的状态值改变
                    }
                }
            }
        }
    }

    return Config;
}

// 每个事件时间计值的++
// 放到毫秒中断计数中
// Please place this function where it is called in 1 millisecond cycle
void ThingsL_TimeCntFun(void)
{
    unsigned char i = 0;
    for (i = 0; i < THINGCNUM; i++)
    {
        Things_C[i].ThingsL_TimeCnt++;
    }
}

// 流程控制循环体
// 放到主循环当中
// PLEASE USE MY DIY SIMPLE POLL OS : THINGSLISTS  gary zxb
// Put this function into the main loop
void ThingsL_erRunSequence(void)
{
    unsigned char i = 0;

    for (i = 0; i < THINGCNUM; i++)
    {
        // i传入在List序号，
        // 第二个参数传入-4 State不受影响  不会操作到ThingsL_JumpOn跳转开关
        ThingsL_I = i;
        (*Things_C[i].ThingsL_Poll)(i,
                                    ThingsL_MainPollCode,  // ConfigState  负数不会被赋值到结构体
                                    ThingsL_MainPollCode); // Stay   正数和ThingsL_PauseCaseCode会被赋值到结构体 其他负数不会被赋值到结构体
    }

    // needSetIndex串口接收的时候就被标记
    // 数值串口接收的时候就已经被改变
    if (needSetIndex != 0xffff) // 触发事件
    {

        // 把具体事件参数传入事件队列  一般就是那个generalEvent其实就是ThingsL_PutEvent  放入事件的内容
        // 事件路由
        (*ThingsP[needSetIndex].pEventFunc)(ThingsP[needSetIndex].EVT,   // 事件类型
                                            ThingsP[needSetIndex].EVC,   // 事件命令
                                            ThingsP[needSetIndex].EVL,   // 事件长度
                                            ThingsP[needSetIndex].pVar); // 事件数据值

        // 判断该行参数是否需要保存
        if (ThingsP[needSetIndex].eeromaddr != 0x0000) // 非0x0000就表示数据需要存储
        {

            ThingsWriteOneParaToEE((unsigned char *)ThingsP[needSetIndex].pVar, // 存储变量的值内存地址
                                   ThingsP[needSetIndex].eeromaddr,             // 所存EEPROM的地址
                                   ThingsgetVarLen(ThingsP[needSetIndex].vtype) // 长度
            );                                                                  // 存入变量
        }

        needSetIndex = 0xffff; // 执行完成后清除
    }
}

// 不按步骤列表中定义的标准流程执行的独立事情函数*******************
// 不按步骤列表独立执行的函数***********如果使用独立函数则把这个函数放到事情列表中
// 但仍可以受指令控制的独立函数************************************
signed short ThingsCollectPoll(unsigned char i, signed short ConfigState, signed int Stay)
{
    // 负数则为一些流程控制的指令
    if (Stay != ThingsL_MainPollCode)
    {
        ThingsL_PollNum = Stay;
    }
    if (ConfigState > 0)
    {
        ThingsL_MState = ConfigState;
    }
    ThingsL_J = ThingsL_MState;
    // 具体流程
    switch (ThingsL_MState) // sppuerList[i].SupState 这里根据事情的填充不一样的条件
    {
    case 0: // 空闲状态啥都不必做
        ThingsL_PollNumSelfSub;
        if (ThingsL_JumpIsEn)
            ThingsL_MState = 1;
        break;
    case 1:
        ThingsL_PollNumSelfSub;
        if (ThingsL_JumpIsEn)
            ThingsL_MState = 2;
        break;
    case 2:
        ThingsL_PollNumSelfSub;
        if (ThingsL_JumpIsEn)
            ThingsL_MState = 0;
        break;
    }

    return ThingsL_MState;
}

// 这个函数是拿来给别的外部用的
// SendThingsExtCode("事情A",1,ThingsL_PauseCaseCode);        //停留在事情A的步骤1
// SendThingsExtCode("事情A",1,2);                            //停留在事情A的步骤1并驻留2次后继续步骤流转
// SendThingsExtCode("事情A",1,2);                            //停留在事情A的步骤1并驻留2次后继续步骤流转
// SendThingsExtCode("事情A",ThingsL_StaGoon,ThingsL_StaGoon);//事情A 不指定事情的状态（步骤）继续步骤流转
// SendThingsExtCode("事情A",1,ThingsL_StaGoon);              //事情A 指定事情的状态（步骤）继续步骤流转
unsigned char SendThingsExtCode(char *pChar, signed short State, signed int Stay)
{
    unsigned short i;
    unsigned char findedflag = 0;
    unsigned char str1[6];
    unsigned char str2[6];
    memcpy(str1, pChar, 5);
    str1[5] = 0; // 比较前5个字符是否完全一致

    for (i = 0; i < THINGCNUM; i++)
    {
        memcpy(str2, Things_C[i].text, 5);
        str2[5] = 0; // 比较前5个字符是否完全一致
        if (strcmp((char const *)str1, (char const *)str2) == 0)
        {
            (*Things_C[i].ThingsL_Poll)(i, State, Stay); // 匹配就调用一次
            findedflag = 1;
        }
    }
    return findedflag; // 没有找到就返回零
}

// 事件数组，数组成员一共21个，序号20放当前事件
// 所有产生的事件都会放到这个数组当中
// 事件的产生：利用ThingsL_PutEvent函数，传入事件类型，事件数据，事件数据长度，事件产生后计数自加1，超出数组深度则返回-1
// 事件的取得：利用ThingsL_GetEvent函数，传入事件类型，如有类型匹配的事件，返回该事件下标，无复合类型的事件则返回-1
// 事件的消耗：利用ThingsL_DetEvent函数，传入事件数组的下标，一般在Get到事件后 需要把这个事件从队列中删除, 这里仅仅是把事件控制结构体中的事件计数自减并将[id]-[cnt]之间的事件数组集体前移
ThingsL_EVENT_List thingsl_event_fifo[ThingsLeventNumMax + 1]; // 事件的队列列表
ThingsL_Event_Con thingsl_event_control = {
    // 事件队列的控制参数只需要一个计数器
    .cnt = 0,
};

// etype   事件的类型
// ev_cmd  事件的指令
// ev_Data 事件的数据
// ev_len  事件数据的长度
// 返回：成功写入返回队列序号，不成功返回负1
signed char ThingsL_PutEvent(unsigned int etype, unsigned short ev_cmd, unsigned short ev_len, void *ev_Data)
{
    if (thingsl_event_control.cnt < ThingsLeventNumMax) // 不满
    {
        thingsl_event_fifo[thingsl_event_control.cnt].ev_type = etype;      // 事件类型
        thingsl_event_fifo[thingsl_event_control.cnt].ev_cmd = ev_cmd;      // 事件的16位指令
        thingsl_event_fifo[thingsl_event_control.cnt].ev_len = ev_len;      // 事件的数据的长度
        thingsl_event_fifo[thingsl_event_control.cnt].ev_Data = ev_Data;    // 事件的数据
        thingsl_event_fifo[thingsl_event_control.cnt].ev_thingsgetmask = 0; // 事情刚放上队列 没有被任何事情得到
        thingsl_event_control.cnt++;
        return thingsl_event_control.cnt; // 现存事件个数
    }
    return -1;
}
// etype 需要检索的事件类型,事件的接收方调用这个函数形参输入事件的类型表示只读取特定类型的事件
// 返回：检索到的第一个复合事件类型的数组下标，不成功返回负1
signed char ThingsL_GetEvent(unsigned int etype)
{
    unsigned char i = 0;
    for (i = 0; i < thingsl_event_control.cnt; i++) // 检索事件数组，一旦检索到返回下标
    {
        if (thingsl_event_fifo[i].ev_type == etype)
        {
            return i;
        }
    }
    return -1;
}

// id 需要删除的事件 一般在Get到事件后 需要把这个事件从队列中删除 这里仅仅是把事件控制结构体中的事件计数自减并将[id]-[cnt]之间的事件数组集体前移
// 返回：事件正常被删除则返回事件计数值，未被正常删除则
signed char ThingsL_DetEvent(signed char id)
{
    unsigned char i;
    if (id < thingsl_event_control.cnt &&
        id >= 0 &&                      // 要删除的事件的下标范围正常
        thingsl_event_control.cnt >= 1) // 事件数组中有事件存放
    {
        for (i = id; i < thingsl_event_control.cnt - 1; i++) // 下标为id的事件被取走了，[id]-[cnt]之间的事件数组集体前移
        {
            thingsl_event_fifo[i].ev_type = thingsl_event_fifo[i + 1].ev_type; // 事件类型
            thingsl_event_fifo[i].ev_cmd = thingsl_event_fifo[i + 1].ev_cmd;   // 事件的cmd
            thingsl_event_fifo[i].ev_len = thingsl_event_fifo[i + 1].ev_len;   // 事件的数据长度
            thingsl_event_fifo[i].ev_Data = thingsl_event_fifo[i + 1].ev_Data; // 事件的数据
            thingsl_event_fifo[i].ev_thingsgetmask = thingsl_event_fifo[i + 1].ev_thingsgetmask;
        }
        thingsl_event_control.cnt--; // 事件计数  这样算事件被消耗掉了
        return thingsl_event_control.cnt;
    }
    else
    {
        return -1;
    }
}

// 遍历事件队列，待所有关联事情获取过改事件后删除这个事件 并将队列前移
EventT ThingsL_EventGetAndDel(unsigned int Evtype, unsigned char ThingsNum)
{
    signed char ei = -1;
    signed char detflag = 0;
    EventT evRtFlg = NoneEvent;

    ei = ThingsL_GetEvent(Evtype); // 得到队列里首个类型匹配的序号

    if (ei >= 0) // 得到事件序号
    {
        thingsl_event_fifo[ThingsLeventNumMax].ev_type = thingsl_event_fifo[ei].ev_type;                   // 事件的类型
        thingsl_event_fifo[ThingsLeventNumMax].ev_cmd = thingsl_event_fifo[ei].ev_cmd;                     // 事件命令
        thingsl_event_fifo[ThingsLeventNumMax].ev_Data = thingsl_event_fifo[ei].ev_Data;                   // 事件的数据
        thingsl_event_fifo[ThingsLeventNumMax].ev_len = thingsl_event_fifo[ei].ev_len;                     // 事件的数据长度
        thingsl_event_fifo[ThingsLeventNumMax].ev_thingsgetmask = thingsl_event_fifo[ei].ev_thingsgetmask; // 事件的事情掩码标记
        if ((thingsl_event_fifo[ei].ev_thingsgetmask & (0x00000001 << ThingsNum)) == 0)                    // 这件事情从来没有被这件事抓到
        {
            evRtFlg = EventFindTrigger; // 得到事件、并触发
        }

        thingsl_event_fifo[ei].ev_thingsgetmask |= (0x00000001 << ThingsNum); // 这个时间已经被这
        // 这个事件其他事情是否想要得到 并且已经得到
        for (unsigned char i = 0; i < THINGCNUM; i++) // 遍历其他事情
        {
            if ((((Things_C[i].eventmask) & Evtype) != 0) && // 这件事也想得到这类事件
                (((thingsl_event_fifo[ei].ev_thingsgetmask) & (0x00000001 << i)) == 0) &&
                ThingsNum != i) // 其他事情想要得到这个事件 但是 还没有被得到
            {
                detflag = 1;
            }
        }
        if (detflag == 0)
        {
            ThingsL_DetEvent(ei); // 等所有想得到这类事件的事情都得到后再进行删除
        }
    }

    return evRtFlg;
}

//----------------------------------------------------
//------------------------------------LCD-------------
//----------------------------------------------------
// 以下是LCD显示部分   可以把LCD的轮询显示和触发都放到事情列表当中  这样事情列表统一管理
//----------------------------------------------------
//------------------------------------LCD------------
//----------------------------------------------------

unsigned char lcdSendBuf[50];
unsigned char lcdRcvBuf[LCDMAXRX];
extern UART_HandleTypeDef huart2;

void lcdSend(unsigned char *buf, unsigned int len)
{
    HAL_UART_Transmit(&huart2, buf, len, 1000);
}

// 获取时间发送指令
void GetTimeSend(void)
{
    // EE 82 FF FC FF FF
    lcdSendBuf[0] = 0xee;
    lcdSendBuf[1] = 0x82;
    lcdSendBuf[2] = 0xff;
    lcdSendBuf[3] = 0xfc;
    lcdSendBuf[4] = 0xff;
    lcdSendBuf[5] = 0xff;
    lcdSend(lcdSendBuf, 6);
}
// 获取当前页面id发送指令
void ReadCurrentPageFunc(void)
{
    // EE B1 01 FF FC FF FF
    lcdSendBuf[0] = 0xee;
    lcdSendBuf[1] = 0xb1;
    lcdSendBuf[2] = 0x01;
    lcdSendBuf[3] = 0xff;
    lcdSendBuf[4] = 0xfc;
    lcdSendBuf[5] = 0xff;
    lcdSendBuf[6] = 0xff;
    lcdSend(lcdSendBuf, 7);
}
// 切换画面
// hid：画面id
void lcdQie(unsigned short hid)
{
    lcdSendBuf[0] = 0xEE;
    lcdSendBuf[1] = 0xB1;
    lcdSendBuf[2] = 0x00;
    lcdSendBuf[3] = hid >> 8;
    lcdSendBuf[4] = hid >> 0;
    lcdSendBuf[5] = 0xFF;
    lcdSendBuf[6] = 0xFC;
    lcdSendBuf[7] = 0xFF;
    lcdSendBuf[8] = 0xFF;
    lcdSend(lcdSendBuf, 9);
}

void lcddaiji(void)
{ // EE 77 01 FF 00 00 0A FF FC FF FF
  //  EE 60 00 FF FC FF FF
  //  EE 77 01 FF 01 00 B4 FF FC FF FF
    lcdSendBuf[0] = 0xEE;
    lcdSendBuf[1] = 0x77;
    lcdSendBuf[2] = 0x01;
    lcdSendBuf[3] = 0xFF;
    lcdSendBuf[4] = 0x01;
    lcdSendBuf[5] = 0x00;
    lcdSendBuf[6] = 0xB4;
    lcdSendBuf[7] = 0xFF;
    lcdSendBuf[8] = 0xFC;
    lcdSendBuf[9] = 0xFF;
    lcdSendBuf[10] = 0xFF;
    lcdSend(lcdSendBuf, 10);
}
// 设置按钮的状态
void lcdSetButtonValue(unsigned short hid, unsigned short kid, unsigned char Value)
{
    lcdSendBuf[0] = 0xEE;
    lcdSendBuf[1] = 0xB1;
    lcdSendBuf[2] = 0x10;
    lcdSendBuf[3] = hid >> 8;
    lcdSendBuf[4] = hid >> 0;
    lcdSendBuf[5] = kid >> 8;
    lcdSendBuf[6] = kid >> 0;
    lcdSendBuf[7] = Value;
    lcdSendBuf[8] = 0xFF;
    lcdSendBuf[9] = 0xFC;
    lcdSendBuf[10] = 0xFF;
    lcdSendBuf[11] = 0xFF;
    lcdSend(lcdSendBuf, 12);
}

// 设置文本值 字符串需0结尾识别
void SetTextValue(unsigned short hid, unsigned short kid, char *str)
{
    unsigned int i = 0;
    lcdSendBuf[i++] = 0xEE;
    lcdSendBuf[i++] = 0xB1;
    lcdSendBuf[i++] = 0x10;
    lcdSendBuf[i++] = hid >> 8;
    lcdSendBuf[i++] = hid >> 0;
    lcdSendBuf[i++] = kid >> 8;
    lcdSendBuf[i++] = kid >> 0;
    while (*str)
    {
        lcdSendBuf[i++] = (*str++);
        if (i > 50)
        {
            break;
        }
    }
    lcdSendBuf[i++] = 0xFF;
    lcdSendBuf[i++] = 0xFC;
    lcdSendBuf[i++] = 0xFF;
    lcdSendBuf[i++] = 0xFF;
    lcdSend(lcdSendBuf, i);
}

// 添加历史记录
void AddHistoryRecord(unsigned short hid, unsigned short kid, char *str)
{
    unsigned int i = 0; // EE B1 52 00 4D 00 01 20 20 31 3B 30 30 30 30 30 30 30 30 30 30 30 30 3B BF AA BB FA 3B FF FC FF FF
    lcdSendBuf[i++] = 0xEE;
    lcdSendBuf[i++] = 0xB1;
    lcdSendBuf[i++] = 0x52;
    lcdSendBuf[i++] = hid >> 8;
    lcdSendBuf[i++] = hid >> 0;
    lcdSendBuf[i++] = kid >> 8;
    lcdSendBuf[i++] = kid >> 0;
    while (*str)
    {
        lcdSendBuf[i++] = (*str++);
        if (i > 50)
        {
            break;
        }
    }
    lcdSendBuf[i++] = 0xFF;
    lcdSendBuf[i++] = 0xFC;
    lcdSendBuf[i++] = 0xFF;
    lcdSendBuf[i++] = 0xFF;

    runParaData.pauseSnedFlag = 1; // 暂停其他与屏幕的交互
    // 加点延时会好点
    lcdSend(lcdSendBuf, i);
}

/******************************************
函 数 名: ergoRecFindHeadTail
功能描述  : 检索整个参数列表，找到匹配画面id的参数行
输入参数  : pHead开头的画面id pTail结尾的画面id pCurrent当前检索参数行号
输出参数  :
返 回 值: 0没匹配到  1匹配到了
日    期  : 2022年11月1日
作    者  : gary
修改内容  : 新生成函数
******************************************/
unsigned char ergoRecFindHeadTail(unsigned short hid, unsigned short *pHead, unsigned short *pTail, unsigned short *pCurrent)
{
    unsigned int i = 0;
    *pCurrent = 0;
    for (i = 0; ThingsP[i].hid != hid; i++)
    {
        *pHead = i;
        *pCurrent = i;
        if (i >= MAXLCDLIST)
        {
            *pHead = 0;
            *pCurrent = 0;
            return 0; // 找不到就放弃
        }
    }
    for (; ThingsP[i].hid == hid; i++)
    {
        *pTail = i;
        if (i >= MAXLCDLIST)
        {
            *pTail = MAXLCDLIST;
            return 0; // 找不到就放弃
        }
    }
    return 1;
}
// 数据发送
// 当前页面 所有数据的实时显示 反馈给屏
void lcdDataSend(unsigned short hid)
{
    static unsigned short Currenthid = 0;            // 暂存的画面id号检测画面id是否发生变化用
    static unsigned char getHeadTailSuccessFlag = 0; //
    static unsigned short CurrentSendIndex = 0;      // 序号 从0 开始查表
    static unsigned short RecHeadIndex = 0;          // 一个画面的第一个记录索引号
    static unsigned short RecTailIndex = 0;          // 一个画面的最后一个记录索引号
    static unsigned short GetTimeCnt = 0;            //
    unsigned char lcdStr[32];                        // 发送的字符串
    if (hid < 42)                                    // 不正常  采样器页面从42开始
    {
        return;
    }
    GetTimeCnt++;

    if (Currenthid != hid || getHeadTailSuccessFlag == 0) // 当画面id发生变化时：遍历整个记录表格找出 当前画面的表头 表尾 如果没招到复位当前发送表序号
    {
        getHeadTailSuccessFlag = ergoRecFindHeadTail(hid, &RecHeadIndex, &RecTailIndex, &CurrentSendIndex);
    }
    Currenthid = hid;

    if (getHeadTailSuccessFlag == 0) // 没有遍历到头尾记录那就不要继续发送了
    {
        return;
    }

    if (CurrentSendIndex < RecHeadIndex || CurrentSendIndex > RecTailIndex) // 不在范围内则转到头行记录
    {
        CurrentSendIndex = RecHeadIndex; // 从头行记录的序号开始
    }

    switch (ThingsP[CurrentSendIndex].vtype)
    {
    case V_NOP:
        lcdSetButtonValue(ThingsP[CurrentSendIndex].hid,                       // 画面id
                          ThingsP[CurrentSendIndex].kid,                       // 控件id
                          *(unsigned char *)(ThingsP[CurrentSendIndex].pVar)); // 按钮值
        break;
    case V_CHAR:
    case V_UCHAR:
    case V_SHORT:
    case V_USHORT:
    case V_INT:
    case V_UINT:
        ThingsNum2Str((char *)lcdStr,                     // 字符串输出地址
                      ThingsP[CurrentSendIndex].pVar,     // 值
                      ThingsP[CurrentSendIndex].vtype);   // 数值类型
        ThingsInsertDot((char *)lcdStr,                   // 字符串输出地址
                        ThingsP[CurrentSendIndex].dilen); // 小数点从右往左数的插入位数
        SetTextValue(ThingsP[CurrentSendIndex].hid,       // 画面id
                     ThingsP[CurrentSendIndex].kid,       // 控件id
                     (char *)lcdStr);                     // 字符串
        break;
    case V_FLOAT:
    case V_FLOAT0: // 浮点数自带小数点不需要插入小数点
    case V_FLOAT1:
    case V_FLOAT2:
    case V_FLOAT3:
        ThingsNum2Str((char *)lcdStr,                   // 字符串输出地址
                      ThingsP[CurrentSendIndex].pVar,   // 值
                      ThingsP[CurrentSendIndex].vtype); // 数值类型
        SetTextValue(ThingsP[CurrentSendIndex].hid,     // 画面id
                     ThingsP[CurrentSendIndex].kid,     // 控件id
                     (char *)lcdStr);                   // 字符串
        break;
    case V_STRING1:
    case V_STRING2:
    case V_STRING3:
    case V_STRING4:
    case V_STRING5:
    case V_STRING6:
    case V_STRING7:
    case V_STRING8:
    case V_STRING9:
    case V_STRING10:
    case V_STRING11:
    case V_STRING12:
    case V_STRING13:
    case V_STRING14:
    case V_STRING15:
    case V_STRING16:
    case V_STRING17:
    case V_STRING18:
    case V_STRING19:
    case V_STRING20:                                          // 值地址指向字符串地址
        SetTextValue(ThingsP[CurrentSendIndex].hid,           // 画面id
                     ThingsP[CurrentSendIndex].kid,           // 控件id
                     (char *)ThingsP[CurrentSendIndex].pVar); // 字符串
        break;
    case V_PASSWORD: // 如果轮询到密码就不用发送显示只要接受那里处理一下好了
        break;
    case V_ENUM:
        SetTextValue(ThingsP[CurrentSendIndex].hid,                                                                // 画面id
                     ThingsP[CurrentSendIndex].kid,                                                                // 控件id
                     (char *)ThingsP[CurrentSendIndex].pEnum[*((unsigned char *)ThingsP[CurrentSendIndex].pVar)]); // 值表示的枚举类型序号
        break;
    default:
        break;
    }

    CurrentSendIndex++; // 发送下一条记录
}

// 接收判断解码
// 读取画面id rethid
// 控件id retkid
// retrurn 控件类型
// 页面号的读取 和 当前时间的识别 在这里处理掉了
unsigned char lcdDecode(char *inBuf, unsigned short *rethid, unsigned short *retkid)
{
    unsigned short hid = 0; // 画面id
    unsigned short kid = 0; // 控件id
    unsigned char kty = 0;  // 控件类型
    unsigned int i = 0;
    unsigned char Temp = 0;
    if (inBuf[i++] != 0xEE)
    {
        *retkid = 0;
        return 0;
    }
    Temp = inBuf[i++];
    if (Temp != 0xB1 && Temp != 0xF7) // 屏幕控件 时间读取值
    {
        *retkid = 0;
        return 0;
    }

    if (
        (*(inBuf + 2) == 0x01) &&
        (*(inBuf + 5) == 0xFF) &&
        (*(inBuf + 6) == 0xFC) &&
        (*(inBuf + 7) == 0xFF) &&
        (*(inBuf + 8) == 0xFF)) // 读取画面命令的返回
    {
        hid = *(inBuf + 3); // 画面id
        hid = hid << 8;
        hid = hid | *(inBuf + 4);
        *rethid = hid;
        return 1;
    }
    if ( // 220413092929
        (*(inBuf + 0) == 0xEE) &&
        (*(inBuf + 1) == 0xF7) &&
        (*(inBuf + 9) == 0xFF) &&
        (*(inBuf + 10) == 0xFC) &&
        (*(inBuf + 11) == 0xFF) &&
        (*(inBuf + 12) == 0xFF)) // EE F7 22 04 03 13 09 29 39 FF FC FF FF 年月周天时分秒22年4月分周3 9点29分39秒
    {
        runParaData.Year = ThingsBcd_2_Num((unsigned int)(*(inBuf + 2)));   // 年
        runParaData.Month = ThingsBcd_2_Num((unsigned int)(*(inBuf + 3)));  // 月
        runParaData.Week = ThingsBcd_2_Num((unsigned int)(*(inBuf + 4)));   // 周
        runParaData.Date = ThingsBcd_2_Num((unsigned int)(*(inBuf + 5)));   // 日
        runParaData.Hour = ThingsBcd_2_Num((unsigned int)(*(inBuf + 6)));   // 时
        runParaData.Minute = ThingsBcd_2_Num((unsigned int)(*(inBuf + 7))); // 分
        runParaData.Second = ThingsBcd_2_Num((unsigned int)(*(inBuf + 8))); // 秒
        runParaData.flag = 1;                                               // 已经读取到当前时间
        runParaData.nyrsfm[0] = *(inBuf + 2);
        runParaData.nyrsfm[1] = *(inBuf + 3);
        runParaData.nyrsfm[2] = *(inBuf + 5);
        runParaData.nyrsfm[3] = *(inBuf + 6);
        runParaData.nyrsfm[4] = *(inBuf + 7);
        runParaData.nyrsfm[5] = *(inBuf + 8);
        *retkid = 2;
        return 2;
    }
    if (inBuf[i] != 0x11 && inBuf[i] != 0x14) //
    {
        *retkid = 0;
        return 0;
    }
    i++;

    hid = inBuf[i++]; // 画面id
    hid = hid << 8;
    hid = hid | inBuf[i++];
    kid = inBuf[i++]; // 控件id
    kid = kid << 8;
    kid = kid | inBuf[i++];
    *rethid = hid;
    *retkid = kid;
    kty = inBuf[i++]; // 控件类型
    if (kty == 0x10 || kty == 0x1A || kty == 0x11)
    {
        // 0x10 按钮
        // 0x1A 下拉菜单
        // 0x11 文本数值
        return kty; // 控件类型
    }
    return 1;
}

/******************************************
函 数 名: getRecIndex
功能描述  : 从参数列表中匹配到第一个画面id和接收控件id值相当的那一行参数
输入参数  :  hid画面id   rkid接收控件id
输出参数  :
返 回 值: 匹配到的参数处于的行号
日    期  : 2022年11月1日
作    者  : gary
修改内容  : 新生成函数
******************************************/
unsigned short getRecIndex(unsigned short hid, unsigned rkid)
{
    unsigned short i = 0;

    for (i = 0; i < MAXLCDLIST; i++) // 暂时定一下 20条记录
    {
        if (ThingsP[i].hid == hid && ThingsP[i].rkid == rkid)
        {
            return i;
        }
    }
    return 0;
}

// 连接屏幕DC80480M070时的接口串口接收，如果没有屏幕就不用调用这个函数
// 把这个函放到串口空闲中断中
void lcdrxProcess(UART_HandleTypeDef *huart)
{
    // unsigned short RxLen;
    unsigned char RecKty = 0;
    unsigned short recIndex = 0;
    signed int recSint = 0;
    double dlongIn = 0.0;
    unsigned short hidtmp = 0;
    unsigned short kidtmp = 0;

    if ((__HAL_UART_GET_FLAG(huart, UART_FLAG_IDLE) != RESET)) // 空闲中断
    {
        __HAL_UART_CLEAR_IDLEFLAG(huart);
        HAL_UART_DMAStop(huart);
        // RxLen = LCDMAXRX - huart->hdmarx->Instance->NDTR; // 接收到的字节长度
        RecKty = lcdDecode((char *)lcdRcvBuf, &hidtmp, &kidtmp); // 页面号的读取 和 当前时间的识别 在这里处理掉了
        // 获取时间指令
        //  EE 82 FF FC FF FF
        //  EE F7 22 04 03 13 09 29 39 FF FC FF FF
        if (RecKty == 0x01) // 仅仅读取了画面id
        {
            runParaData.recHid = hidtmp;
            runParaData.recKid = kidtmp;
            recIndex = getRecIndex(runParaData.recHid, runParaData.recKid);
        }
        if (RecKty == 0x10) // 按键
        {

            runParaData.doorCnt = 0; // 有操作延时关门
            runParaData.recHid = hidtmp;
            runParaData.recKid = kidtmp;
            recIndex = getRecIndex(runParaData.recHid, runParaData.recKid);
            *(signed char *)ThingsP[recIndex].pVar = lcdRcvBuf[9];
            needSetIndex = recIndex; // 主循环里调用防止在中断里面回调时间过长而增加不确定性
            needPauseFlag = 1;       // 暂停下读取
            // SendThingsExtCode("页面A",ThingsL_StopSwitchCode,ThingsL_StopSwitchCode);
            // SendThingsExtCode("页面A",0,0);
        }
        if (RecKty == 0x1A) // 下拉菜单
        {
            runParaData.doorCnt = 0; // 有操作延时关门
            runParaData.recHid = hidtmp;
            runParaData.recKid = kidtmp;
            recIndex = getRecIndex(runParaData.recHid, runParaData.recKid);
            *(unsigned char *)ThingsP[recIndex].pVar = lcdRcvBuf[8];
            needSetIndex = recIndex; // 主循环里调用防止在中断里面回调时间过长而增加不确定性
        }
        if (RecKty == 0x11) // 文本
        {
            runParaData.doorCnt = 0; // 有操作延时关门
            runParaData.recHid = hidtmp;
            runParaData.recKid = kidtmp;
            recIndex = getRecIndex(runParaData.recHid, runParaData.recKid); // 查到序号
            switch (ThingsP[recIndex].vtype)                                // 根据类型
            {
            case V_PASSWORD: // 密码只要接收 不要发送
                ((char *)ThingsP[recIndex].pVar)[0] = lcdRcvBuf[8];
                ((char *)ThingsP[recIndex].pVar)[1] = lcdRcvBuf[9];
                ((char *)ThingsP[recIndex].pVar)[2] = lcdRcvBuf[10];
                ((char *)ThingsP[recIndex].pVar)[3] = lcdRcvBuf[11];
                ((char *)ThingsP[recIndex].pVar)[4] = lcdRcvBuf[12];
                ((char *)ThingsP[recIndex].pVar)[5] = lcdRcvBuf[13];
                ((char *)ThingsP[recIndex].pVar)[6] = lcdRcvBuf[14];
                ((char *)ThingsP[recIndex].pVar)[7] = lcdRcvBuf[15];
                break;
            case V_STRING1:
            case V_STRING2:
            case V_STRING3:
            case V_STRING4:
            case V_STRING5:
            case V_STRING6:
            case V_STRING7:
            case V_STRING8:
            case V_STRING9:
            case V_STRING10:
            case V_STRING11:
            case V_STRING12:
            case V_STRING13:
            case V_STRING14:
            case V_STRING15:
            case V_STRING16:
            case V_STRING17:
            case V_STRING18:
            case V_STRING19:
            case V_STRING20:
                strcpy(ThingsP[recIndex].pVar, (const char *)&lcdRcvBuf[8]);
                break;
            case V_CHAR:
                recSint = ThingsStr2inthandle((char *)&lcdRcvBuf[8], ThingsP[recIndex].dilen);
                *(signed char *)ThingsP[recIndex].pVar = (signed char)recSint;
                break;
            case V_UCHAR:
                recSint = ThingsStr2inthandle((char *)&lcdRcvBuf[8], ThingsP[recIndex].dilen);
                *(unsigned char *)ThingsP[recIndex].pVar = (unsigned char)recSint;
                break;
            case V_SHORT:
                recSint = ThingsStr2inthandle((char *)&lcdRcvBuf[8], ThingsP[recIndex].dilen);
                *(signed short *)ThingsP[recIndex].pVar = (signed short)recSint;
                break;
            case V_USHORT:
                recSint = ThingsStr2inthandle((char *)&lcdRcvBuf[8], ThingsP[recIndex].dilen);
                *(unsigned short *)ThingsP[recIndex].pVar = (unsigned short)recSint;
                break;
            case V_INT:
                recSint = ThingsStr2inthandle((char *)&lcdRcvBuf[8], ThingsP[recIndex].dilen);
                *(signed int *)ThingsP[recIndex].pVar = (signed int)recSint;
                break;
            case V_UINT:
                recSint = ThingsStr2inthandle((char *)&lcdRcvBuf[8], ThingsP[recIndex].dilen);
                *(unsigned int *)ThingsP[recIndex].pVar = (unsigned int)recSint;
                break;
            case V_FLOAT:
            case V_FLOAT0:
            case V_FLOAT1:
            case V_FLOAT2:
            case V_FLOAT3:
                dlongIn = atof((const char *)&lcdRcvBuf[8]);
                *(float *)ThingsP[recIndex].pVar = (float)dlongIn;
                break;
            default:
                break;
            }

            needSetIndex = recIndex; // 主循环里调用防止在中断里面回调时间过长而增加不确定性
        }
        HAL_UART_Receive_DMA(huart, lcdRcvBuf, LCDMAXRX); // 再次开启DMA接收
    }
}