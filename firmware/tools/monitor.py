"""
Author: Jesus Minjares
Date:   07-24-2022
GitHub: https://github.com/jminjares4
Brief:  Read serial data with script to avoid using any additional software: 'putty' | 'screen'
"""
from dataclasses import field
import click
import serial
import time
from termcolor import colored
import pyfiglet

# default example
# python3 monitor.py --port="serial-port"
#
# custom example:
# python3 monitor.py --port="serial-port" --baudrate=115200 --timeout=30
@click.command()
@click.option('--port', '-p', required=True, help='Serial Port Number')
@click.option('--baudrate', '-b', default=9600, help='Serial Baudrate')
@click.option('--timeout', '-t', default=60, help='Serial loop timeout in seconds')
def main(port, baudrate, timeout):
    """
    Read serial data from serial port

    port (str)      : serial/COM port
    baudrate (int)  : baudrate of serial port: bit/sec
    timeout (int)   : serial timeout in seconds
    """

    # Display ascii text
    print(colored(pyfiglet.figlet_format("Serial Monitor"),'red'))
    # Display inputs                                          
    print("Port : %s, Baudrate : %i, timeout = %i"% 
            (port, baudrate, timeout))
    
    with serial.Serial(port, baudrate, timeout=1) as ser: # open serial port 
        start_time = time.time() # get current time
        exit = False # set exit flag
        # read data until timeout and exit is false
        while ((time.time() - start_time) < timeout) and not exit:
            # Read serial device
            line = ser.readline().decode('utf-8') # read and decode
            # Check if data was received
            if(len(line) > 0):
                print(line.rstrip('\r\n')) # strip \r\n
                # check exit
                if line == 'exit':
                    exit = True
        
    return # exit

if __name__=="__main__":
    # Call main function
    main()
