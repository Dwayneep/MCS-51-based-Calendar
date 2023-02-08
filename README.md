# MCS-51-based-Calendar
Multi-function calendar run on MSC-51 chip | 基于51单片机的多功能万年历

The standalone, perpetual calendar program on the 8051 single-chip microcontroller features timekeeping, time and date setting capabilities, and can perform basic logic operations such as AND or NOT on 4-digit or smaller unsigned integers. The program displays the results in real-time on an LCD1602 screen. Its operation is not dependent on a separate timing chip, but instead relies on interrupt capabilities within the microprocessor.
在51单片机上运行的独立万年历程序，带有计时，设置时间日期，且可以对4位及以下无符号整型数进行与或非等基本逻辑操作，并在LCD1602液晶屏上实时显示。该程序的走时不依赖独立计时芯片，依靠处理芯片内部的中断运行。

To ensure accurate timing, this independent perpetual calendar program for a 51 single-chip microcomputer calculates each clock cycle and adjusts the high and low eight bits of the timer. The program operates through a loop in the main function, where pressing the K3 and K4 buttons triggers the corresponding processing logic, with K1 changing the operating position and K2 adding 1 to each bit. To maintain logical soundness, the program safeguards against illegal inputs for the date and accurately judges both leap years and average years. Additionally, attention is given to the accuracy of the timekeeping, as the internal time judgment is performed as infrequently as possible per second to minimize clock cycle occupancy, thereby reducing long-term timing deviation.
计算每个时钟时钟周期，再更改定时器的高八位和低八位，使得程序可以精确地每秒走时。在主函数内设置循环，当K3,K4按键被按下过后，进入相应的处理逻辑。即，K1更改操作位置，K2对各位加1。同时应当保证程序的健壮性，对非法输入日期进行合法化，对闰年平年的判断正确无误；与此同时应该注意走时的精确性，例如，程序内部对走时的判断，每秒应尽量做少的判断以减少时钟周期的占用，以保证在长期的计时中不会出现太大的偏差。

The K1 button changes the operands such as the month, day, and year. The K2 button modifies the time. The K3 button ends the setting process. The K4 button performs predefined logical operations
K1 更改月日年等操作数 K2 修改时间 K3 结束设置过程 K4 进行逻辑运算（操作数已预定义）
