###### 一、注意：

> thingslists.c 业务部分，业务逻辑都规划在这里修改
> thingsnoos.c  函数库，尽量不去修改

###### 二、使用步骤

1. 将thingslists.c .h  thingsnoos.c .h文件移到工程当中 

2. 函数ThingsL_erRunSequence();放到主循环当中调用

3. 函数ThingsL_TimeCntFun();放到系统定时中断中调用 

4. 经过以上操作 thingslist.c就可以拿来修改使用了

###### 三、概念

- **PCST**概念（对于thingslists.c .h文件 和 thingsnoos.c .h文件而言一个系统有P、C、CS、T 等构成）：
  
  > **P**  (Parameter):系统当中的参数；
  > **C**  (Case):系统当中的事情；
  > **CS**(Case'Sates):系统当中各事情的状态；
  > **T**  (Trigger):系统当中的触发;它可以导致CS状态的转移,T可以分成PET 和 TET
  > 
  > **PET** (Paramamter Event Trigger)；系统当中参数型事件触发、硬件IO、DATA或者LCD等都可以归为PET；
  > 
  > **TET**(Timer Event Trigger):系统当中时间型事件的触发。

- 大致构成示意如下:
  
  ```
  //事情Case0有状态S0 S1 S2 S3 S4构成
  //事情Case1有状态S0 S1 S2 构成
  //事情Case2有状态S0 S1 S2 S3 构成
  //C中S状态和P、T之间互相影响
  C0------S0
       |--S1
       |--S2  
       |--S3 <==TET===> Timer
       |--S4 <==PET===> P <===> LCD
  C1------S0            P <===> IO
       |--S1            P <===> DATA
       |--S2
  C3------S0
       |--S1
       |--S2
       |--S3
  ```