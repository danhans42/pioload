/* PIOLOAD by danhans42
   
   A simple EXE loader that uses the parallel port of an Xplorer/Xploder cheat cart
   Raspberry Pi. Comms is one way only (PC -> PSX) so far.
   
   Version History
   ===============
   
   14/02/2020 0.1 - First Version (It works, just loads EXE's thats it)
   20/03/2020 0.3 - Ability to load data to an address (useful for Yaroze/Emulators etc)
   
   Future Enhancements (Todo List)
   * Prettier Look                                    
   * Able to load LIBPS into RAM from CD

*/

#include <sys/types.h>
#include <libetc.h>
#include <stdlib.h>
#include <libgte.h>
#include <libgpu.h>
#include <libgs.h>
#include <kernel.h>

#define STACKP 0x801ffff0			
#define OT_LENGTH 1 				
#define PACKETMAX 18 				
#define SCREEN_WIDTH  320 			
#define	SCREEN_HEIGHT 256 			

GsOT myOT[2]; 						
GsOT_TAG myOT_TAG[2][1<<OT_LENGTH]; 
PACKET GPUPacketArea[2][PACKETMAX]; 
short CurrentBuffer = 0; 			

void graphics(); 					
void display();
inline u_char xpGetBit(); 			
void pcSendByte (char);   		
void LoadEXE();
void LoadBIN();          			
void LoadLIBPS();
void DumpBIOS();
char pcGetByte ();        		
u_long GetLongData (void);

char *xplorer1 = (char*)0x1F060001;  
char *xplorer2 = (char*)0x1F060002;

char *xplorer3 = (char*)0x1F040000;
		
u_char header[2048];      			
u_long x_addr;					

static struct XF_HDR * head;
//	int id;
	int i=0;

char sync;					
u_long addr;					
u_long fsize;					
u_long loop;					
char * write_addr;

void main() 
{
	graphics(); 				
	FntLoad(960, 256); 			
	SetDumpFnt(FntOpen(5, 20, 320, 240, 1, 512));
	display();
	FntPrint("PIOLOAD v0.3\n\n\nWaiting for RPI..\n");
	display();
	while (1) 			
	{
		loop = 0;
		if (xpGetBit() == 0) {
			sync = pcGetByte();	
			if (sync == 0x63) LoadEXE();
			if (sync == 0x64) LoadBIN();
			if (sync == 0x62) DumpBIOS();
		}
	}
}

void graphics()
{
	SetVideoMode(1);
	ResetGraph(0);
	GsInitGraph(SCREEN_WIDTH, SCREEN_HEIGHT, GsINTER|GsOFSGPU, 1, 0);
	GsDefDispBuff(0, 0, 0, SCREEN_HEIGHT); 
	myOT[0].length = OT_LENGTH;
	myOT[1].length = OT_LENGTH;
	myOT[0].org = myOT_TAG[0];
	myOT[1].org = myOT_TAG[1];
	GsClearOt(0,0,&myOT[0]);
	GsClearOt(0,0,&myOT[1]);
}

void display()
{
	FntFlush(-1);
	CurrentBuffer = GsGetActiveBuff();
	GsSetWorkBase((PACKET*)GPUPacketArea[CurrentBuffer]);	
	GsClearOt(0,0, &myOT[CurrentBuffer]);	
	DrawSync(0);	
	VSync(0);	
	GsSwapDispBuff();
	GsSortClear(0,0,0, &myOT[CurrentBuffer]);	
	GsDrawOt(&myOT[CurrentBuffer]);
}
			 
void pcSendByte(char byte)
{
 	char temp;
	temp = ((byte>>6)&3) + 4 ;            	
	*xplorer1 = temp;          
	*xplorer1 = temp + 8;     
	while (xpGetBit()!=0);              
	temp = (byte>>3)&7;
	*xplorer1 = temp + 8;    
	*xplorer1 = temp;        
	while (xpGetBit()==0);            
	temp = (byte&7);  
	*xplorer1 = temp;        
	*xplorer1 = temp + 8; 
	while (xpGetBit()!=0);             
	*xplorer1 = 0;     
}
char pcGetByte()
{
	char byte;
	while (xpGetBit()!=0);
	byte = *xplorer1;
	*xplorer1 = 8;
	while (xpGetBit()==0);
	*xplorer1 = 0;
 	return byte;
}
	
u_long GetLongData(void)
{
	u_long dat;
	dat = (pcGetByte());    
	dat |= (pcGetByte()) <<8;
	dat |= (pcGetByte()) <<16;
	dat |= (pcGetByte()) << 24;     
	return (dat);      
}

inline u_char xpGetBit()
{  
	return ( (*xplorer2 & 1)^1 );
}

void LoadEXE ()
{
	FntPrint("PIOLOAD v0.3\n\n\nSync Recieved\n");
	display();
	for (loop=0;loop<2048;loop++){
		header[loop] = pcGetByte();
	}
	FntPrint("PIOLOAD v0.3\n\n\nHeader Recieved\n");
	display();
	loop=0;
	x_addr = GetLongData();
	FntPrint("PIOLOAD v0.3\n\n\nXADDR Recieved: %X \n",x_addr);
	display();
	write_addr =(char *) GetLongData();
	FntPrint("PIOLOAD v0.3\n\n\nADDR Recieved: %X \n",write_addr);
	display();	
    fsize = GetLongData();
	FntPrint("PIOLOAD v0.3\n\n\nEXE Size Recieved: %Xh \n",fsize);
	display();
	for (loop=0;loop<fsize;loop++){
		*write_addr = pcGetByte();
		write_addr++;
	}
	FntPrint("PIOLOAD v0.3\n\n\nEXECUTE!\n");
	display();
	
	ResetGraph(3);              
	StopCallback();
    head = (struct XF_HDR *)header;
	head->exec.s_addr = STACKP;
	head->exec.s_size = 0;
	EnterCriticalSection();
	Exec(&(head->exec),1,0);
}

void LoadBIN () {
	write_addr =(char *) GetLongData();
	//write_addr =(char *)hitmod;
	FntPrint("PIOLOAD v0.3\n\n\nLoad Address: %X \n",write_addr);
    display();	
    fsize = GetLongData();
	FntPrint("PIOLOAD v0.3\n\n\nData Length : %Xh \n",fsize);
	display();
	for (loop=0;loop<fsize;loop++){
		*write_addr = pcGetByte();
		write_addr++;
	}
	FntPrint("PIOLOAD v0.3\n\n\nWaiting for RPI..\n");
	display();	
}

void DumpBIOS() {

	
	sync = pcGetByte();
	*xplorer3 = sync;
/*   for (i = 0;i<0x80000; i++){
	pcSendByte(*(char*)(0x0bfc00000 + i));
   }
   i = 0;*/
   
}