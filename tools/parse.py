#usr/bin/env python3

import os

def parse(parsefile,outfile):
    '''
		parse four command from txt file. 
		Including:
			SPIRead
			SPIWrite
			WAIT_CALDONE
			WAIT		
	'''
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
'''
	Example:SPIWrite(0x3DF,0x01);
'''
    addr,val = line.split()[1].split(',')
    code = 'SPIWrite' + '(0x' + addr + ',0x' + val + ');\n'
    fout.writelines(code)
    
def SPIRead(fout, line):
'''
	Example:SPIRead_HLevel(0x05E);
'''
    addr = line.split()[1]
    code = "SPIRead_HLevel(0x" + addr + ");\n"
    fout.writelines(code)

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

    if index >= '1' and index <= '8':
        code = "WAIT_CALDONE(" + index + ");\n" + "//" + line
        fout.writelines(code)
    else:
        fout.writelines('\n**********************************\n Errors! Parse WAIT_CALDONE() fail, please upgrade your code\n**********************************\n')

def WAIT(fout, line):
'''
	Example:delay_ad9362(20000);
'''
    time = line.split()[1]
    code = 'delay_ad9362(' + time + '000);\n'
    fout.writelines(code)

 
if __name__ == '__main__':
    path = "./"
    files = os.listdir(path)
    s = []
    for file in files:
        if not os.path.isdir(file) and file!='parse.py' and file.split('Covert_')[0]!='':
            infile = path + file
            outfile = path + 'Covert_' + file
            parse(infile, outfile)
