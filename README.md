MIDI-USB-SHIELD
===============

This is the firmware source code for OpenPipe MIDI-USB Shield.  

For more details, check out the product page at  
http://openpipe.cc/products/midi-usb-shield/

OpenPipe invests time and resources providing this open source design, 
please support OpenPipe and open-source hardware by purchasing 
products from OpenPipe! 

Build instructions
------------------
    git clone https://github.com/abcminiuser/lufa.git
    git clone https://github.com/openpipelabs/midi-usb-shield.git
    cd midi-usb-shield
    make

Firmware Update
---------------
Enter DFU mode powering the shield while HWB jumper is ON

    sudo make dfu
    
License & Attribution
---------------------
Designed by OpenPipe (www.openpipe.cc).

GPLv3 license. Check LICENSE.txt for more information  
All text above must be included in any redistribution