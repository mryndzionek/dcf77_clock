dc77_clock
==========

Introduction
------------
This code is a my take on (blinkenlight)[https://blog.blinkenlight.net/experiments/dcf77/]
impressive DCF77 decoder running on (NUCLEO-L053R8)[http://www.st.com/content/st_com/en/products/evaluation-tools/product-evaluation-tools/mcu-eval-tools/stm32-mcu-eval-tools/stm32-mcu-nucleo/nucleo-l053r8.html] board. All the BSP code is generated using STM32CubeMX.
The main `tick` clock of the system comes from square wave output of DS3231 RTC clock configured to 1024Hz. 
This way clock drift problems are eliminated. The overall concurrency handling mechanisms are inspired by:

* (QP framework)[http://www.state-machine.com/doc/concepts.html]
* (Contiki)[http://contiki.sourceforge.net/docs/2.6/index.html]
* (OOSMOS)[http://oosmos.com/]
* (Atom)[https://github.com/tomahawkins/atom]

Basically `message passing`, `state machines` and `continuation-passing style` efficiently implemented in C.
This approach has its drawbacks and requires some attention but fine grained concurrency and minimal footprint
is worth the effort. The code currently provides (blinkenlights)[https://blog.blinkenlight.net/experiments/dcf77/]
phase detection algorithm and sync mark binning. Date and time decoding is missing. The project outputs binary trace
on one of the MCU UART ports. This allows gathering logs and analysing performance. Below some examples:

![trace1](https://github.com/mryndzionek/picoTracer/raw/master/logs/2016-06-18-18:25:21-ttyUSB0:576000:8N1.png)
![trace2](https://github.com/mryndzionek/picoTracer/raw/master/logs/2016-06-19-13:00:00-ttyUSB0:576000:8N1.png)
![trace3](https://github.com/mryndzionek/picoTracer/raw/master/logs/2016-06-19-22:17:15-ttyUSB0:576000:8N1.png)

License
-------
  - MPLv2

Contact
-------
If you have questions, contact Mariusz Ryndzionek at:

<mryndzionek@gmail.com>
