# SPDX-License-Identifier: LicenseRef-CSSL-1.0

#---------------------------------------------------------------------
#
#   Required Python Version
#     Python 3.x
#
#---------------------------------------------------------------------

#usage example:
#sudo python3 ci_ctl_qtel.py /dev/ttyUSB2 wup
#sudo python3 ci_ctl_qtel.py /dev/ttyUSB2 detach


import sys
import time
import serial

class qtel_ctl:
	#---------------
	#private methods
	#---------------
    def __init__(self, usb_port_at):
        self.QUECTEL_USB_PORT_AT = usb_port_at #ex : '/dev/ttyUSB2'
        self.modem = serial.Serial(self.QUECTEL_USB_PORT_AT, timeout=1)
        self.cmd_dict= {"wup": self.wup,"detach":self.detach}#dictionary of function pointers

    def __set_modem_state(self,ser,state):
	    self.__send_command(ser,"AT+CFUN={}\r".format(state))

    def __send_command(self,ser,com):
        ser.write(com.encode())
        time.sleep(0.1)
        ret=[]
        while ser.inWaiting()>0:
            print("waiting")
            msg=ser.readline()
            msg=msg.decode("utf-8")
            msg=msg.replace("\r","")
            msg=msg.replace("\n","")
            print(msg)
            if msg!="":
                ret.append(msg)
            else:
                print("msg empty")
        return ret

	#--------------
	#public methods
	#--------------
    def wup(self):#sending AT+CFUN=0, then AT+CFUN=1
        self.__set_modem_state(self.modem,'0')
        time.sleep(3)
        self.__set_modem_state(self.modem,'1')

    def detach(self):#sending AT+CFUN=0
        self.__set_modem_state(self.modem,'0')


if __name__ == "__main__":
    #argv[1] : usb port
    #argv[2] : qtel command (see function pointers dict "wup", "detach" etc ...)
	if len(sys.argv) >= 3:
		command = sys.argv[2]
		print(command)
		Module=qtel_ctl(sys.argv[1])
    #calling the function to be applied
		Module.cmd_dict[command]()
		print(Module.cmd_dict[command])
	else:
		print("To many arguments")
