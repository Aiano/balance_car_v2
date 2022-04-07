# 开发记录

## 项目信息

| 信息类型     | 信息值                      |
| ------------ | --------------------------- |
| 名称         | 二轮平衡小车                |
| 版本         | 2.0                         |
| MCU          | stm32f103c8t6               |
| IDE          | stm32CubeMX(SW4STM32) clion |
| 项目启动时间 | 2022.3.31                   |

## IMU

- IMU=Inertial Measurement Unit
- 型号：MPU6050

### 引脚定义

| MPU6050 | STM32 |
| ------- | ----- |
| SDA     | PB9   |
| SCL     | PB8   |
| INT     | PA12  |
| VCC     | 5V    |
| GND     | GND   |
| XDA     | NC    |
| XCL     | NC    |
| AD0     | NC    |

## GY-521 HAL library

> https://github.com/leech001/MPU6050

|           |        |
| --------- | ------ |
| mpu6050.c | 源文件 |
| mpu6050.h | 头文件 |

### MPU6050

- 流行的运动传感器芯片
- 集成的加速度计可以读取三轴加速度值，陀螺仪可以读取三轴角速度值，数字运动处理器（DMP）可以输出四元数

> Arduino与MPU6050：https://zhuanlan.zhihu.com/p/20082486
>
> stm32与MPU6050：https://zhuanlan.zhihu.com/p/33773085

#### 最少默认启动步骤

1. 检查WHO_AM_I寄存器的值是否为0x68

2. 向Power Management Register1 写入一个0以启动mpu6050
3. 向采样频率分频器SMPLRT_DIV寄存器写入0x07，设置采样率为1kHz
4. 向加速度计配置ACCEL_CONFIG写入0x00，采用默认量程+-2g(g为重力加速度)
5. 向陀螺仪配置GYRO_CONFIG写入0x00，采用默认量程+-250degree/s

#### 采样率

- 陀螺仪输出频率8kHz（当数字低通滤波器DLPF关闭）或者1kHz（当DLPF开启），加速度计采输出频率1kHz
- 采样率Sample Rate = 陀螺仪输出频率 / (1+采样频率分配寄存器值)
- 这意味着，如果采样率Sample Rate大于1kHz，那么同一个加速度计数值可能被多次输出到FIFO、DMP和传感器寄存器

#### 加速度计数值转换

- 加速度计的三个值ACC_X、ACC_Y、ACC_Z分别表示加速度在三个坐标轴上的分量

- 读到的数据为16位有符号整数

- 2^(16-1)=32768，故数据范围是+-32768

- 假设设置的量程为+-2g，则真实值可用以下公式转换
  $$
  a_x=\frac{2g}{32768} \times ACC\_X
  $$

#### 陀螺仪数值转换

- 陀螺仪的三个值GYR_X、GYR_Y、GYR_Z分别表示绕X、Y、Z三个坐标轴的角速度

- 从原点向坐标轴方向看去，取正值为顺时针旋转，取负值时为逆时针旋转

- 读到的数据为16位有符号整数

- 假设设置的量程为250度/秒，真实值可用以下公式转换
  $$
  g_x=\frac{250}{32768}*GYR\_X
  $$

#### 中断

MPU6050可以配置的中断有：

1. 由新的参考振荡器

## HAL库数据手册

> https://www.jianshu.com/p/f226f48fd9ce

## 测量代码块运行时间

> Systick介绍：https://cloud.tencent.com/developer/article/1861964
>
> **SysTick定时器被捆绑在NVIC中，用于产生SYSTICK中断**（异常号：15）。在以前，大多操作系统需要一个硬件定时器来产生操作系统需要的滴答中断，作为整个系统的时基。例如，为多个任务许以不同数目的时间片，确保没有一个任务能霸占系统；或者把每个定时器周期的某个时间范围赐予特定的任务等，还有操作系统提供的各种定时功能，都与这个滴答定时器有关。因此，需要一个定时器来产生周期性的中断，而且最好还让用户程序不能随意访问它的寄存器，以维持操作系统“心跳”的节律。
>
> SysTick 是一个 24 位的倒计数定时器，当计到 0 时，将从 RELOAD 寄存器中自动重装载定时初值。只要不把它在 SysTick 控制及状态寄存器中的使能位清除， 就永不停息，图 13.1 中小结了 SysTick的相关寄存器。

>Function name
>	uint32_t HAL_GetTick (void )
>Function description
>	Provides a tick value in millisecond.
>Return values
>	• tick: value
>Notes
>	• This function is declared as __weak to be overwritten in case of other implementations in user file.

也就是说，HAL_GetTick函数默认是采用Systick产生1ms的时基，如果我们要采用别的时基，就重写这个函数。

```c
// main()函数中，HAL_Init()函数用于复位所有外设，初始化Flash闪存接口和Systick系统滴答定时器
/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	HAL_Init();

// HAL_Init()函数中，可以看到用HAL_InitTick()函数设置Systick为时基源，并配置1ms的时基
/* Use systick as time base source and configure 1ms tick (default clock after Reset is HSI) */
	HAL_InitTick(TICK_INT_PRIORITY);

// HAL_InitTick()函数中，通过设置uwTickFreq可以修改Systick中断频率
/* Configure the SysTick to have interrupt in 1ms time basis*/
  	if (HAL_SYSTICK_Config(SystemCoreClock / (1000U / uwTickFreq)) > 0U)
  	{
    	return HAL_ERROR;
  	}

// 设置为1kHz
	HAL_TickFreqTypeDef uwTickFreq = HAL_TICK_FREQ_DEFAULT;  /* 1KHz */

// 也可以修改为其他的频率
typedef enum
{
  HAL_TICK_FREQ_10HZ         = 100U,
  HAL_TICK_FREQ_100HZ        = 10U,
  HAL_TICK_FREQ_1KHZ         = 1U,
  HAL_TICK_FREQ_DEFAULT      = HAL_TICK_FREQ_1KHZ
} HAL_TickFreqTypeDef;
```



## HC-05 串口蓝牙模块

### PC连接步骤：

1. 模块上电，可以看到指示灯快速闪烁，代表没有连接
2. PC打开蓝牙，添加设备，找到HC-05配对，PIN是1234
3. 打开串口调试工具，连接串口，如果有多个就逐个尝试
4. 如果串口打开成功，则指示灯变为慢闪，每次连闪两下，代表连接成功

注意：已配对但是未打开串口的情况下，模块会保持快闪

## 枚举enum

> [enum教程](https://www.runoob.com/cprogramming/c-enum.html)
>
> [typedef enum用法](https://www.cnblogs.com/yaowen/p/4785342.html#:~:text=%E6%96%B9%E6%B3%95%E4%B8%89%EF%BC%9A%E7%94%A8typedef%E5%85%B3%E9%94%AE%E5%AD%97%E5%B0%86%E6%9E%9A%E4%B8%BE%E7%B1%BB%E5%9E%8B%E5%AE%9A%E4%B9%89%E6%88%90%E5%88%AB%E5%90%8D%EF%BC%8C%E5%B9%B6%E5%88%A9%E7%94%A8%E8%AF%A5%E5%88%AB%E5%90%8D%E8%BF%9B%E8%A1%8C%E5%8F%98%E9%87%8F%E5%A3%B0%E6%98%8E%EF%BC%9A)

## PID调节

### 直立环

PWM控制量最大值为200，Kp若设置为15，则偏角13.3°时就会满转，也就是说当小车倾斜超过13.3°后就无法提供更大的回复力，因此外界扰动不宜过大

