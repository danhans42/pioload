# ----------------------------
# PlayStation 1 Psy-Q MAKEFILE
# ----------------------------
all:
	del pioload.exe
	ccpsx -O2 -Xo$80100000 pioload.c -opioload.cpe,pioload.sym,mem.map
	cpe2x /ce pioload.cpe
	del pioload.cpe
    del pioload.sym
    del mem.map
rom:
	ccpsx -O2 -Xo$801E8000 pioload.c -opioloadr.cpe,pioload.sym,mem.map
	cpe2x /ce pioloadr.cpe
	del pioloadr.cpe
    del pioload.sym
    del mem.map
	copy /B romprod/romprod.rom + pioloadr.exe pioload.rom
	del pioloadr.exe


