#Scorpion Fire Control

The Scorpion is a programmable, hall sensor-based mosfet controller that is a drop-in replacement for the mechanical trigger in V2 gearbox airsoft guns. Minor modification of the trigger, tappet plate, selector plate, and gearbox shell is required, and the cut-off lever is left out when installed.

![Installed Scorpion](https://github.com/IntelligentDevices/Scorpion/blob/master/docs/board-only.jpg)

##Install 
###Gearbox
The front opening must be widened with a dremel to allow the USB port to be accessible once installed.

![Gearbox Shell Modification](https://github.com/IntelligentDevices/Scorpion/blob/master/docs/shell.jpg)

> - Compare the width of the actual USB port to the forward wiring port on the gearbox to ensure you only take off as much metal as required to allow the USB cable to be plugged in.

With the gearbox modified, the Scorpion controller should drop in to the place where the mechanical trigger was easily, and attach to the gearbox using the screw from the mechanical trigger.

###Magnets
Magnets need to be glued to various parts to interact with the hall sensors.
 
![Magnet Installation](https://github.com/IntelligentDevices/Scorpion/blob/master/docs/magnets.jpg)

> - Use super glue to secure the magnets to the various parts.

####Tappet Plate
Position the magnet in the center of the bottom of the tappet plate, on its side, so that in the forward position the magnet is directly over the hall sensor, as shown above.

####Trigger
The ear hanging off the top of the trigger should be trimmed with a dremel, to allow clearance over the wiring. The magnet should be secured at the back edge of the trigger on the cut area, as shown above.

####Selector Plate
The copper slip on the bottom of the selector plate should be removed, and the magnet glued to the bottom-front corner, with clearance for the tab that slides into the gearbox, as shown above.

###Wiring
Make sure you route the wiring so that the motor will not rub on it when running. 

###Tuning
Once the gearbox is reassembled, the Scorpion should be plugged in and connected to using the Arduino development environment. In the  top section of the source code are several constants which may need to be adjusted to compensate for tolerance differences between gearboxes and parts, and for variations in magnet install position.

