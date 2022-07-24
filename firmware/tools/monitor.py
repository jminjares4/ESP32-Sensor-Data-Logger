from glob import glob
import click
import serial
import time
from termcolor import colored
import pyfiglet

# example
# python3 monitor.py --port="serial-port" --baudrate=115200

@click.command()
@click.option('--port', '-p', required=True, help='Serial Port Number')
@click.option('--baudrate', '-b', default=9600, help='Serial Baudrate')
@click.option('--timeout', '-t', default=60, help='Serial loop timeout')
def main(port, baudrate, timeout):
    #
    print(colored(pyfiglet.figlet_format("Serial Monitor"), 'green'))
                                                                        
    print("Port : %s, Baudrate : %i, timeout = %i" % 
            (port, baudrate, timeout))
        
    with serial.Serial(port, baudrate, timeout=1) as ser:
        start_time = time.time()
        exit = False
        while ((time.time() - start_time) < timeout) and not exit:
            #Attempt to read line from serial device
            line = ser.readline().decode('utf-8');
            #Check data is valid
            if(len(line) > 0):
                print(line.rstrip('\r\n'))
                #Do we exit?
                if line == 'exit':
                    exit = True

if __name__=="__main__":
    main()
