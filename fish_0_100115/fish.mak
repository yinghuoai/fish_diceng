CC = iccavr
LIB = ilibw
CFLAGS =  -IC:\iccv7avr\include -e -D__ICC_VERSION=722 -DATMega128  -l -g -MLongJump -MHasMul -MEnhanced -Wf-use_elpm 
ASFLAGS = $(CFLAGS) 
LFLAGS =  -LC:\iccv7avr\lib -g -e:0x20000 -ucrtatmega.o -bfunc_lit:0x8c.0x20000 -dram_end:0x10ff -bdata:0x100.0x10ff -dhwstk_size:16 -beeprom:0.4096 -fihx_coff -S2
FILES = 11fish.o TWI_Master.o 

FISH:	$(FILES)
	$(CC) -o FISH $(LFLAGS) @FISH.lk   -lcatm128
11fish.o: C:\iccv7avr\include\iom128v.h C:\iccv7avr\include\macros.h C:\iccv7avr\include\AVRdef.h .\TWI_Master.h
11fish.o:	G:\wsg\SchoolTrain\FishRobot\机器鱼\机器鱼培训资料\机器鱼培训资料\机器鱼鱼体波程序\fish_0_100115\11fish.c
	$(CC) -c $(CFLAGS) G:\wsg\SchoolTrain\FishRobot\机器鱼\机器鱼培训资料\机器鱼培训资料\机器鱼鱼体波程序\fish_0_100115\11fish.c
TWI_Master.o: C:\iccv7avr\include\iom128v.h C:\iccv7avr\include\macros.h C:\iccv7avr\include\AVRdef.h .\TWI_Master.h
TWI_Master.o:	G:\wsg\SchoolTrain\FishRobot\机器鱼\机器鱼培训资料\机器鱼培训资料\机器鱼鱼体波程序\fish_0_100115\TWI_Master.c
	$(CC) -c $(CFLAGS) G:\wsg\SchoolTrain\FishRobot\机器鱼\机器鱼培训资料\机器鱼培训资料\机器鱼鱼体波程序\fish_0_100115\TWI_Master.c
