# debussy

![alt tag](data/debussy-logo-large.png)

###### *Brought to you by Chifeng Wen*
A physically based high quality midi synthesizer for embedded system.

### Grant Permission to hidraw device on Linux
Create a file named 99-hidraw-permissions.rules at /etc/udev/rules.d with this line:
```KERNEL=="hidraw*", MODE="0666"```
Reboot the system, this rule will grant non-root user the access to all hidraw devices.
