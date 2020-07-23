# pioload
Simple example of loading data to a Sony PlayStation via an Xplorer - Using RPi GPIO

PSX Side software needs PsyQ to be built.

RPi Side needs WiringPi.

<pre>

Wiring - Im working on it. You can work it out from the RPi side source code.

Taken from Nocash PSX Spex - https://problemkaputt.de/psx-spx.htm and wiringPi

  XP                    
  DB25  Direction     Signal    RPI GPIO.Pin    RPI.BCM.GPIO 
  ==========================================================
  2       In          DATA0 
  3       In          DATA1 
  4       In          DATA2 
  5       In          DATA3 
  6       In          DATA4 
  7       In          DATA5 
  8       In          DATA6 
  9       In          DATA7 
  10      Out         /ACK  
  11      Out         BUSY  
  12      Out         PE    
  13      Out         SLCT  
  17      In          /SEL
  18-25   --          GND
  ==========================================================


</pre>
