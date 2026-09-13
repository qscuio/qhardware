# 大规模 PCB 时钟系统：参考时钟、时钟发生器、PLL、CDC 与高速接口

> 本文整理自本 session 的全部技术讨论，主题包括：  
> 大规模 PCB 上不同芯片/模块如何使用不同时钟、为什么需要参考时钟、为什么需要时钟发生器，以及 PLL、Clock Domain、CDC、SerDes、CDR、DDR、SPI、I²C 等概念之间的关系。

---

## 目录

1. [问题一：大规模 PCB 上，不同芯片和模块之间怎么处理时钟？](#问题一大规模-pcb-上不同芯片和模块之间怎么处理时钟)
2. [核心结论：不是全板共用一个工作时钟](#核心结论不是全板共用一个工作时钟)
3. [为什么 CPU 几 GHz，而板上的参考时钟可能只有几十或几百 MHz？](#为什么-cpu-几-ghz而板上的参考时钟可能只有几十或几百-mhz)
4. [为什么不能整个系统都使用一个 3 GHz 时钟？](#为什么不能整个系统都使用一个-3-ghz-时钟)
5. [时钟域 Clock Domain](#时钟域-clock-domain)
6. [不同时钟域之间如何传数据：CDC](#不同时钟域之间如何传数据cdc)
7. [简单控制信号如何跨时钟域](#简单控制信号如何跨时钟域)
8. [高速数据如何处理：SerDes、PLL 与 CDR](#高速数据如何处理serdespll-与-cdr)
9. [DDR 为什么又不一样：源同步](#ddr-为什么又不一样源同步)
10. [SPI 和 I²C 的时钟方式](#spi-和-ic-的时钟方式)
11. [“同步”其实有多种不同含义](#同步其实有多种不同含义)
12. [Clock Generator 在大型 PCB 中的作用](#clock-generator-在大型-pcb-中的作用)
13. [为什么高速系统里经常看到 156.25 MHz](#为什么高速系统里经常看到-15625-mhz)
14. [CPU 的 GHz 到底是什么](#cpu-的-ghz-到底是什么)
15. [常见接口的时钟机制对照](#常见接口的时钟机制对照)
16. [问题二：为什么需要参考时钟和时钟发生器？](#问题二为什么需要参考时钟和时钟发生器)
17. [参考时钟到底解决什么问题](#参考时钟到底解决什么问题)
18. [为什么不能全部依赖芯片内部振荡器](#为什么不能全部依赖芯片内部振荡器)
19. [为什么还需要独立的 Clock Generator](#为什么还需要独立的-clock-generator)
20. [Clock Buffer 的作用](#clock-buffer-的作用)
21. [共同参考时钟为什么能避免长期频率漂移](#共同参考时钟为什么能避免长期频率漂移)
22. [Jitter 与 Jitter Cleaner](#jitter-与-jitter-cleaner)
23. [PLL 为什么必须有参考](#pll-为什么必须有参考)
24. [Crystal、XO、Clock Generator、Clock Buffer、Jitter Cleaner、PLL 的区别](#crystalxoclock-generatorclock-bufferjitter-cleanerpll-的区别)
25. [最终统一理解](#最终统一理解)

---

# 问题一：大规模 PCB 上，不同芯片和模块之间怎么处理时钟？

原始问题：

> 现在的大规模 PCB 上边是怎么在不同的芯片、模块之间同步时钟的？  
> 比如说 CPU 可能几个 GHz，低速器件可能几 MHz。

现代大型 PCB 的基本思路并不是：

> 全板所有芯片共用一个频率。

而是：

> 少量高质量参考时钟 → 各芯片内部 PLL 倍频/分频 → 每个模块形成自己的时钟域 → 不同时钟域之间通过协议或者 CDC 电路交换数据。

一个简化模型：

```text
                 25 MHz 晶振
                      │
              ┌───────▼────────┐
              │ Clock Generator │
              │ / Clock Buffer  │
              └───┬────┬────┬──┘
                  │    │    │
             100MHz   │   25MHz
             PCIe REF │   PHY REF
                  │   │
                  │   └────► FPGA
                  │           │
                  │         PLL
                  │      ┌────┴────┐
                  │    250M      125M
                  │
               ┌──▼──┐
               │ CPU │
               │ PLL │
               └──┬──┘
                  │
                3 GHz
```

---

# 核心结论：不是全板共用一个工作时钟

大系统一般存在多个不同的时钟域：

```text
CPU clock domain
ASIC clock domain
FPGA clock domain
PHY clock domain
CPLD clock domain
```

它们可能：

- 频率不同
- 相位不同
- 参考源不同
- 启停时间不同
- 电压域不同

系统仍然可以正常工作。

真正要做的是：

> 在时钟域边界正确处理数据同步。

---

# 为什么 CPU 几 GHz，而板上的参考时钟可能只有几十或几百 MHz？

CPU 的 3 GHz 通常不是 PCB 上传输的一根 3 GHz 时钟线。

常见方式是：

```text
100 MHz Reference Clock
          │
          ▼
         PLL
          │
          ├── 3.2 GHz   CPU Core
          ├── 1.6 GHz   Cache
          ├── 800 MHz   Interconnect
          └── 200 MHz   Peripheral
```

所以：

> GHz 级核心时钟往往只存在于芯片内部。

PCB 外部只需要提供：

```text
25 MHz
50 MHz
100 MHz
125 MHz
156.25 MHz
```

之类相对低频、质量较高的参考时钟。

这样做可以降低：

- EMI
- 串扰
- 时钟树布线难度
- 走线 skew
- 高速反射问题
- 整板功耗

---

# 为什么不能整个系统都使用一个 3 GHz 时钟？

3 GHz 的周期：

\[
T = \frac{1}{3GHz} \approx 333ps
\]

PCB 上信号传播速度大约在：

```text
150 ~ 180 ps/cm
```

量级。

所以如果两根时钟线相差 1 cm，传播延迟就可能相差约：

```text
~160 ps
```

而 3 GHz 的一个周期才：

```text
333 ps
```

这意味着仅仅几毫米到一厘米的走线差异，就可能占据一个时钟周期中非常大的比例。

例如：

```text
CLK ───────────────► 芯片 A
CLK ────────────────────► 芯片 B
```

可能变成：

```text
A 看到上升沿：0 ps
B 看到上升沿：150 ps
```

这种不同节点看到同一个时钟边沿的时间差叫：

> Clock Skew，时钟偏斜。

因此频率越高：

> 越不适合把一个全局高频时钟直接撒遍整个 PCB。

---

# 时钟域 Clock Domain

一个通信/交换机/服务器板卡可能类似：

```text
                ┌───────────┐
100MHz REF ────►│    CPU    │
                │ 3GHz Core │
                └─────┬─────┘
                      │ PCIe
                      ▼
                ┌───────────┐
100MHz REF ────►│   ASIC    │
                │ 800MHz    │
                └─────┬─────┘
                      │
                     MDIO
                      │
                      ▼
                ┌───────────┐
25MHz REF ─────►│ Ethernet  │
                │   PHY     │
                └───────────┘

50MHz OSC ─────► CPLD

FPGA:
100MHz REF
    │
   PLL
 ┌──┼────┐
 ▼  ▼    ▼
50M 125M 250M
```

这里已经至少存在：

```text
CPU clock domain
ASIC clock domain
FPGA 50 MHz domain
FPGA 125 MHz domain
FPGA 250 MHz domain
PHY clock domain
CPLD clock domain
```

即使在一颗 FPGA 或 SoC 内部，也经常存在多个独立时钟域。

---

# 不同时钟域之间如何传数据：CDC

CDC：

> Clock Domain Crossing

意思就是：

> 数据从一个时钟域进入另一个时钟域。

例如：

```text
CPU / 高速接口时钟域

        data
         │
         ▼
   ┌──────────┐
   │ Async FIFO│
   └──────────┘
         │
         ▼

FPGA 250MHz 时钟域
```

异步 FIFO 可以：

```text
快的一边写
        │
        ▼
   ┌─────────┐
   │ Async   │
   │ FIFO    │
   └─────────┘
        │
        ▼
慢的一边读
```

双方不需要时钟边沿对齐。

---

# 简单控制信号如何跨时钟域

假设一个异步信号：

```text
start
```

从一个时钟域送到另一个时钟域。

如果目标时钟正好在 `start` 翻转附近采样：

```text
      start
________/‾‾‾‾‾

Target CLK
_____/‾\____
      ↑
   正好撞上
```

触发器可能进入：

> Metastability，亚稳态。

常见解决方法是两级同步器：

```text
异步信号
   │
   ▼
┌─────┐
│ FF1 │
└──┬──┘
   ▼
┌─────┐
│ FF2 │
└──┬──┘
   ▼
同步后的信号
```

FPGA RTL 中常见写法：

```verilog
always @(posedge clk) begin
    sync1 <= async_signal;
    sync2 <= sync1;
end
```

对于不同类型的数据，CDC 的手段也不同：

- 单 bit 状态：两级同步器
- 脉冲：pulse synchronizer / toggle synchronizer
- 握手数据：request/acknowledge
- 多 bit 数据：握手或者 FIFO
- 高吞吐数据流：Async FIFO
- FIFO 指针跨域：常用 Gray Code

---

# 高速数据如何处理：SerDes、PLL 与 CDR

以 PCIe 为例。

PCB 上常见：

```text
100 MHz REFCLK
```

但 PCIe 链路可能运行：

```text
8 GT/s
16 GT/s
32 GT/s
```

显然：

```text
100 MHz ≠ 16 GHz
```

实际过程更接近：

```text
100 MHz REFCLK
      │
      ▼
     PLL
      │
      ▼
高速 SerDes
      │
      ▼
高速串行数据
```

参考时钟只是：

> 高速 SerDes 内部 PLL 的频率基准。

---

## 接收端为什么不用另外传一根超高速时钟？

现代高速串行协议通常使用：

> Clock Data Recovery，CDR

发送端只发送高速串行数据：

```text
TX
        DATA
──────────────► RX
                 │
                 ▼
                CDR
                 │
                 ▼
          Recovered Clock
```

接收端从数据边沿和编码特征中恢复采样时钟。

例如：

```text
1 0 1 1 0 1 0 0 1
  ↑   ↑     ↑
```

接收端根据这些跳变恢复出正确采样相位。

典型协议：

- PCIe
- SATA
- USB 3.x
- SGMII
- 10GBASE-KR
- 更高速 Ethernet SerDes

---

# DDR 为什么又不一样：源同步

DDR 常见的是：

> Source Synchronous，源同步。

发送数据的人同时发送：

- Data
- Strobe

例如：

```text
CPU / DDR Controller

DQ ───────────────► DRAM
DQ ───────────────► DRAM
DQ ───────────────► DRAM
DQS ──────────────► DRAM
```

DQS：

> Data Strobe

接收端使用和数据一起到达的 DQS 采样 DQ。

理想情况：

```text
DATA
─────[ 有效数据 ]─────
          ↑
         DQS
```

因此 DDR Layout 中经常要严格控制：

- DQ 与 DQS 的长度关系
- Byte Lane 内的 skew
- CK/CA 的匹配
- 拓扑
- 阻抗

PCB 上常见的蛇形走线，很多就是在进行：

> Delay Matching / Length Matching

---

# SPI 和 I²C 的时钟方式

## SPI

SPI 通常由 Master 主动发送时钟：

```text
Master
   │
   ├── SCLK ─────► Slave
   ├── MOSI ─────►
   └── MISO ◄─────
```

CPU 即使运行：

```text
3 GHz
```

SPI 也完全可以只有：

```text
10 MHz
20 MHz
50 MHz
```

CPU 内部会经过外设总线、分频器等得到 SPI 时钟。

概念上可以理解成：

```text
CPU 高速时钟
     │
     ▼
Peripheral Bus
     │
     ▼
Divider
     │
     ▼
SPI SCLK
```

---

## I²C

I²C 更低速：

```text
CPU / MCU
   │
   ▼
I2C Controller
   │
   ├── SCL
   └── SDA
```

常见 SCL：

```text
100 kHz
400 kHz
1 MHz
```

I²C 从设备根本不需要知道：

> CPU Core 是 1 GHz、3 GHz 还是 4 GHz。

它只关心 SCL 和 SDA。

---

# “同步”其实有多种不同含义

“芯片之间怎么同步时钟”这个问题，实际可能对应至少四种情况。

---

## 1. 共享参考时钟

例如：

```text
Clock Generator
      │
  ┌───┼───┐
  ▼   ▼   ▼
CPU FPGA ASIC
```

常见：

```text
PCIe REFCLK
Ethernet REFCLK
```

要求可能包括：

- 频率准确度
- jitter
- duty cycle
- phase
- skew
- 差分阻抗

---

## 2. 发送方把时钟一起送过去

例如 SPI：

```text
SCLK ─────►
DATA ─────►
```

或者 DDR：

```text
DQS ──────►
DQ  ──────►
```

这是典型源同步思想。

---

## 3. 时钟嵌入数据，接收端恢复

例如：

```text
PCIe
SGMII
10GBASE-KR
SATA
USB 3.x
```

流程：

```text
Reference Clock
       │
      PLL
       │
     SerDes
       │
DATA + embedded timing
       │
      CDR
```

---

## 4. 双方完全异步

例如：

```text
Clock Domain A
      │
      ▼
 Async FIFO
      │
      ▼
Clock Domain B
```

依靠：

- FIFO
- handshake
- synchronizer
- Gray Code

处理。

---

# Clock Generator 在大型 PCB 中的作用

大型板子经常有专门的时钟器件：

```text
Clock Generator
Clock Buffer
Jitter Cleaner
PLL
XO
TCXO
OCXO
```

典型结构：

```text
                25MHz XO
                   │
                   ▼
           ┌─────────────┐
           │ Clock Gen   │
           └──────┬──────┘
       ┌──────────┼─────────┐
       │          │         │
     100MHz    156.25MHz   25MHz
       │          │         │
      PCIe        PHY      MCU
       │
      FPGA
```

Clock Generator 不是简单地：

> 输出一个全板统一频率。

它可能同时生成：

```text
25 MHz
50 MHz
100 MHz
125 MHz
156.25 MHz
312.5 MHz
```

给不同模块使用。

---

# 为什么高速系统里经常看到 156.25 MHz

在交换机、Ethernet ASIC、SerDes 系统里经常能看到：

```text
156.25 MHz
```

它通常是一个很常见的高速通信参考时钟。

例如：

```text
156.25 MHz
      │
      ▼
     PLL
      │
      ▼
高速 SerDes
```

注意：

> 156.25 MHz 是参考时钟，不等于链路数据速率。

不能理解成：

```text
156.25 MHz REFCLK
=
156.25 Mbps 数据
```

两者不是一个概念。

---

# CPU 的 GHz 到底是什么

比如外部输入：

```text
25 MHz Crystal / XO
```

CPU 内部：

```text
25 MHz
  │
  ▼
 PLL
  │
  └──► 3 GHz
```

3 GHz 通常主要存在于 CPU 芯片内部的 Clock Tree。

内部还会继续分成不同频率：

```text
                 PLL
                  │
     ┌────────────┼────────────┐
     ▼            ▼            ▼
CPU Core       L3 Cache       Bus
3 GHz          1.5 GHz      500 MHz
                                │
                       ┌────────┴────────┐
                       ▼                 ▼
                     USB               I2C
                   125 MHz             100 MHz
                                        │
                                     Divider
                                        │
                                      400 kHz
```

所以即使同一个 SoC：

> 也可能有几十甚至上百个时钟域。

---

# 常见接口的时钟机制对照

| 接口 | 时钟方式 | 核心思想 |
|---|---|---|
| I²C | SCL | Master 输出低速时钟 |
| SPI | SCLK | Master 把时钟和数据一起传 |
| DDR | CK + DQS | 源同步，DQS 伴随数据 |
| PCIe | REFCLK + SerDes/CDR | 参考时钟 + 高速串行 |
| SGMII | SerDes/CDR | 时钟信息嵌入串行数据 |
| 10GBASE-KR | SerDes/CDR | 高速串行 + 接收端恢复时钟 |

---

# 问题二：为什么需要参考时钟和时钟发生器？

原始问题：

> 为什么需要参考时钟和时钟发生器？

最核心的一句话：

> 参考时钟提供“时间的尺子”，时钟发生器负责把这把尺子变成系统里各种器件需要的不同尺子。

例如：

```text
25 MHz 晶振
    │
    ▼
参考时钟
    │
    ▼
CPU 内部 PLL
    │
    ▼
3 GHz
```

CPU 内部 PLL 需要一个可靠的基准，才能判断：

```text
我产生的到底是：
2.95 GHz？
3.00 GHz？
3.08 GHz？
```

参考时钟本质上解决的是：

> 频率基准问题。

---

# 参考时钟到底解决什么问题

PLL 虽然可以产生很高的频率，但是：

> 它需要一个准确的外部基准，来判断自己跑快了还是跑慢了。

所以：

```text
Reference Clock
      │
      ▼
     PLL
      │
      ▼
High Speed Clock
```

参考时钟就像：

> 一把标准尺。

而 PLL 产生的高速时钟，是根据这把尺放大的。

---

# 为什么不能全部依赖芯片内部振荡器

很多 MCU / SoC 确实有内部 RC 振荡器。

例如：

- ESP32
- STM32
- 各类 MCU

没有外部晶振时，也可以启动。

但是内部 RC 振荡器会受到：

```text
温度
电压
工艺差异
老化
```

影响。

理论上：

```text
8 MHz
```

实际可能：

```text
7.8 MHz
8.1 MHz
```

对于：

- GPIO 控制
- LED
- 简单状态机
- 一般低速逻辑

往往够用。

但对于：

```text
USB
PCIe
Ethernet
DDR
SerDes
ADC/DAC
无线通信
```

就可能不够。

这些系统通常需要更准确、低 jitter 的参考。

---

# 为什么还需要独立的 Clock Generator

如果系统很简单：

```text
Crystal
   │
   ▼
 MCU
   │
  PLL
   ▼
CPU Clock
```

可能根本不需要独立 Clock Generator。

但复杂板卡可能有：

```text
CPU
FPGA
Switch ASIC
Ethernet PHY
PCIe Switch
CPLD
SerDes Retimer
```

各自要求：

```text
25 MHz
50 MHz
100 MHz
125 MHz
156.25 MHz
```

如果每颗芯片都单独配一个晶振：

```text
25M XO     → CPU
100M XO    → PCIe
156.25M XO → ASIC
25M XO     → PHY
100M XO    → FPGA
```

问题包括：

- 器件数量增加
- PCB 面积增加
- 成本增加
- 每个参考源独立漂移
- 相互频率关系不容易保证
- EMI 源增加
- 时钟管理复杂

所以更常见：

```text
               25MHz XO
                  │
                  ▼
          ┌──────────────┐
          │ Clock Gen    │
          └──────┬───────┘
         ┌───────┼────────┐
         ▼       ▼        ▼
      100MHz   125MHz   156.25MHz
         │       │        │
       PCIe     PHY      ASIC
```

---

# Clock Buffer 的作用

如果多个器件都需要同一个频率：

```text
100 MHz
```

不能简单把一根高速时钟线随便分叉：

```text
             ┌→ CPU
100MHz ──────┼→ FPGA
             ├→ PCIe Switch
             └→ NIC
```

因为高速分叉可能造成：

- 阻抗不连续
- 反射
- 幅度变化
- skew
- jitter
- 信号完整性恶化

Clock Buffer 会把一路输入复制成多个独立驱动输出：

```text
                ┌→ CLK0
100MHz ─► Buffer├→ CLK1
                ├→ CLK2
                └→ CLK3
```

每一路有自己的驱动器。

---

# 共同参考时钟为什么能避免长期频率漂移

假设两个 SerDes 使用完全独立的振荡源。

一个实际频率：

```text
156.249 MHz
```

另一个：

```text
156.251 MHz
```

短时间看差异很小。

但只要：

```text
发送平均速率 ≠ 接收平均速率
```

缓冲区占用就会长期向某个方向漂移。

最终可能出现：

```text
FIFO Overflow
```

或者：

```text
FIFO Underflow
```

共同参考时钟可以让多个相关模块的长期平均频率保持一致或具有明确的频率关系。

---

# Jitter 与 Jitter Cleaner

理想时钟：

```text
|----|----|----|----|----|
```

实际时钟边沿会抖：

```text
|---|-----|---|------|---|
```

这个边沿时间误差叫：

> Jitter，抖动。

高速系统特别在意 jitter，因为它会压缩采样裕量。

例如：

- PCIe
- Ethernet SerDes
- 高速 ADC
- 高速 DAC
- CDR
- RF 系统

板上可能出现：

```text
Oscillator
     │
     ▼
Jitter Cleaner
     │
     ▼
Clock Generator
```

Jitter Cleaner 一般也是基于 PLL，但重点优化：

> 输出时钟相位噪声和抖动性能。

---

# PLL 为什么必须有参考

PLL 的基本结构可以理解成：

```text
Reference Clock
       │
       ▼
   Phase Detector ◄──────┐
       │                 │
       ▼                 │
     Filter              │
       │                 │
       ▼                 │
      VCO                │
       │                 │
       └──── Divider ────┘
```

例如希望：

```text
100 MHz → 3 GHz
```

可以把 3 GHz 输出除以 30：

```text
3GHz / 30 = 100MHz
```

PLL 比较：

```text
Reference = 100 MHz
Feedback  = Output / 30
```

如果反馈变成：

```text
99.9 MHz
```

说明输出偏慢，PLL 会提高 VCO 频率。

如果反馈：

```text
100.1 MHz
```

说明输出偏快，PLL 会降低 VCO 频率。

最终锁定在：

```text
3.000 GHz
```

所以：

> PLL 可以产生高速时钟，但它需要参考时钟才能知道“高速时钟到底准不准”。

---

# Crystal、XO、Clock Generator、Clock Buffer、Jitter Cleaner、PLL 的区别

| 名称 | 主要作用 |
|---|---|
| Crystal / 晶体 | 谐振元件，本身通常不是完整时钟输出器 |
| XO / Oscillator | 内含振荡电路，可以直接输出时钟 |
| Clock Generator | 从一个参考源生成一个或多个所需频率 |
| Clock Buffer | 把一路时钟复制成多路独立输出 |
| Jitter Cleaner | 改善输入时钟的抖动和相位噪声 |
| PLL | 锁定参考，并产生与参考具有固定频率/相位关系的时钟 |
| TCXO | 温补晶振，频率稳定度更好 |
| OCXO | 恒温晶振，稳定度更高，但成本/功耗/体积也更高 |

这些功能经常集成在同一颗芯片里。

例如一颗器件可能同时实现：

```text
PLL
+
Clock Generator
+
Clock Buffer
+
Jitter Cleaner
```

---

# 最终统一理解

现代大规模 PCB 的时钟系统可以用下面这张图概括：

```text
                     ┌──────────────┐
Crystal / XO ───────►│ Clock Gen    │
                     └──────┬───────┘
             ┌──────────────┼─────────────┐
             │              │             │
          100MHz         156.25MHz      25MHz
             │              │             │
             ▼              ▼             ▼
           CPU            ASIC           PHY
            │               │             │
           PLL             PLL           PLL
            │               │             │
          3GHz            800MHz        125MHz
            │               │             │
            │   PCIe / SerDes             │
            ├──────────────►│             │
            │               │    SGMII    │
            │               ├────────────►│
            │               │             │
          SPI CLK          MDIO          CDR
          20MHz
            │
            ▼
          Sensor
```

最重要的几句话：

1. **大规模 PCB 不会让所有芯片共用同一个工作频率。**
2. **PCB 上通常分布的是较低频、质量很高的参考时钟，而不是 CPU 内部几 GHz 的核心时钟。**
3. **每颗芯片通过 PLL / Divider 等产生自己的内部工作时钟。**
4. **不同模块构成多个 Clock Domain。**
5. **跨 Clock Domain 的数据通过 CDC 电路处理，而不是强行让所有时钟边沿对齐。**
6. **高速串行协议往往使用 SerDes + CDR，从数据中恢复采样时钟。**
7. **DDR、SPI 等常使用 Source Synchronous，把采样时钟或 Strobe 和数据一起发送。**
8. **Clock Generator 的本质是基于统一参考，为整块板生成多个所需的稳定时钟。**
9. **Clock Buffer 用来安全地把一路时钟分配给多个负载。**
10. **参考时钟本质上就是系统的“时间尺子”。**

可以把整个系统记成一句话：

> **Crystal/XO 提供时间基准，Clock Generator 负责分配和变换基准，各芯片内部 PLL 生成真正的工作时钟，不同时钟域之间再通过 CDC 或协议完成数据交换。**

---

## 原始问题记录

### Q1

> 现在的大规模 pcb 上边是怎么在不同的芯片，模块之间同步时钟的?比如说CPU可能几个g，低俗器件可能几m

### Q2

> 为什么需要参考时钟和时钟发生器

---

## 建议后续继续学习的顺序

如果后续继续沿这个方向学习，可以按下面顺序：

```text
Clock / Oscillator
        ↓
PLL
        ↓
Clock Tree
        ↓
Clock Domain
        ↓
CDC
        ↓
Source Synchronous
        ↓
SerDes
        ↓
CDR
        ↓
Jitter / Phase Noise
        ↓
Eye Diagram
```

然后再结合实际协议：

```text
SPI / I2C
   ↓
DDR
   ↓
PCIe
   ↓
SGMII
   ↓
10GBASE-KR
```

这样再去看大型通信设备、服务器、交换机的原理图，`CLK / REFCLK / PLL / SERDES / DQS / RXCLK / TXCLK` 这些信号会更容易建立整体概念。
