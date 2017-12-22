#usr/bin/env python3

#This file aims to write LTE Script data to eeprom.
#Address Range:0x00~0x16c0

import os

BaseAddr = 0x00

def parse(parsefile,outfile):
    
    with open(parsefile) as fin, open(outfile,'w') as fout:
        for line in fin.readlines():
            if "SPIWrite" in line:
                SPIWrite(fout, line)
                pass
            elif "SPIRead" in line:
                SPIRead(fout, line)
                pass
            elif "WAIT_CALDONE" in line:
                WAIT_CALDONE(fout, line)
            elif "WAIT" in line:
                WAIT(fout, line)
                pass

def SPIWrite(fout,line):
    global BaseAddr
    addr,val = line.split()[1].split(',')
    #code = 'SPIWrite' + '(0x' + addr + ',0x' + val + ');\n'
    code = 'Status=eeWrAD(' + hex(BaseAddr) + ', 0xA, 0x' + val + ', 0x' + addr + ');\n'
    fout.writelines(code)
    BaseAddr = BaseAddr + 0x4
    
def SPIRead(fout, line):
    global BaseAddr
    addr = line.split()[1]
    #code = "SPIRead_HLevel(0x" + addr + ");\n"
    code = 'Status=eeWrAD(' + str(hex(BaseAddr)) + ', 0xB, 0, 0x' + addr + ');\n'
    fout.writelines(code)
    BaseAddr = BaseAddr + 0x4

def WAIT_CALDONE(fout, line):
    '''
    Index:      Command

        1)      	        WAIT_CALDONE	BBPLL,2000	// Wait for BBPLL to lock, Timeout 2sec, Max BBPLL VCO Cal Time: 225.000 us (Done when 0x05E[7]==1)
	2)			WAIT_CALDONE	RXCP,100	// Wait for CP cal to complete, Max RXCP Cal time: 600.000 (us)(Done when 0x244[7]==1)
	3)			WAIT_CALDONE	TXCP,100	// Wait for CP cal to complete, Max TXCP Cal time: 600.000 (us)(Done when 0x284[7]==1)
	4)			WAIT_CALDONE	RXFILTER,2000	// Wait for RX filter to tune, Max Cal Time: 5.585 us (Done when 0x016[7]==0)
	5)			WAIT_CALDONE	TXFILTER,2000	// Wait for TX filter to tune, Max Cal Time: 2.889 us (Done when 0x016[6]==0)
	6)			WAIT_CALDONE	BBDC,2000	// BBDC Max Cal Time: 6575.521 us. Cal done when 0x016[0]==0
	7)			WAIT_CALDONE	RFDC,2000	// Wait for cal to complete (Done when 0x016[1]==0)
	8)			WAIT_CALDONE	TXQUAD,2000	// Wait for cal to complete (Done when 0x016[4]==0)

    '''
    index = '0'
    cmd = line.split()[1].split(',')[0]
    if cmd == 'BBPLL':
	    index = '1'
    elif cmd == 'RXCP':
	    index = '2'
    elif cmd == 'TXCP':
	    index = '3'
    elif cmd == 'RXFILTER':
	    index = '4'
    elif cmd == 'TXFILTER':
	    index = '5'
    elif cmd == 'BBDC':
	    index = '6'
    elif cmd == 'RFDC':
	    index = '7'
    elif cmd == 'TXQUAD':
	    index = '8'
    global BaseAddr
    if index >= '1' and index <= '8':
        #code = "WAIT_CALDONE(" + index + ");\n" + "//" + line
        code = 'Status=eeWrAD('+str(hex(BaseAddr))+', 0xD, '+ str(index) + ',0);\n'
        fout.writelines(code)
    else:
        fout.writelines('\n**********************************\n Errors! Parse WAIT_CALDONE() fail, please upgrade your code\n**********************************\n')
    BaseAddr = BaseAddr + 0x4

def WAIT(fout, line):
    global BaseAddr
    time = line.split()[1]
    #code = 'delay_ad9362(' + time + '000);\n'
    code = 'Status=eeWrAD('+str(hex(BaseAddr))+',0xC,'+str(time)+',0);\n'
    fout.writelines(code)
    BaseAddr = BaseAddr + 0x4

 
if __name__ == '__main__':
   parse("LTE_AD936X_40M.txt","LTE_eeprom_WR.txt")
