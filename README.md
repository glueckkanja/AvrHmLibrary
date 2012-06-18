AvrHmLibrary
============

A Library for AVR ATmega8 microcontrollers (and potentially others) implementing the HomeMatic&reg;/BidCos&reg; home automation protocol.

- Based on and inspired by: [culfw](http://culfw.de) and [Fhem](http://fhem.de) by Rudolf Koenig
- Licensed under the [GPL (v2)](http://www.gnu.org/licenses/old-licenses/gpl-2.0.html)
- HomeMatic&reg; und BidCoS&reg; are registered trademarks of eQ-3 AG

Status
------
Alpha Version:

- Basic HomeMatic&reg; communication should work.
- Pairing with CCU and/or remote (tested with one single model) should work.
- Basic commands (set, remote) should work to control PWM output.

Open Issues:

- ACK/NACK handling
- separate universally usable code from specific application projekt
- better handling of some message details
- optimization
- and many more...