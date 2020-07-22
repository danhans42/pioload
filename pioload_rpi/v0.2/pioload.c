
/* XP COMMS FOR RPI
   by danhans42 - danhans42@gmail.com
   For RPi ZeroW/RPI3B/3A+ 

   v0.1 - basic proof of concept - EXE upload only.
   v0.2 - EXE upload and BIN upload facility, also can reset console via BCM GPIO26

*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <wiringPi.h>
#include <unistd.h>
#include <time.h>

FILE *fp;

//OUTPUTS
#define D0     17
#define D1     18
#define D2     27
#define D3     22
#define D4     23
#define D5     24
#define D6     25
#define D7     4
#define SEL    12
//INPUTS
#define SLCT   16
#define PE     20
#define BUSY   21
#define ACK    5
#define RESET  26
int i=0;

unsigned int f_size;

char syncbyte;

char *ptr;
unsigned long load_addr;

void SendByte(char);
void SetupPins();
void ReadByxte();
void SendEXE();
void SendBIN();
void ErrorMsg();

int main (int argc, char *argv[])
{
	printf ("Xplorer PSX-EXE Upload Tool v0.2\n"); 
	printf ("github.com/danhans42\n\n") ;

  	SetupPins();

	if (argc <= 1) {
		printf("Not Enough Arguments\n");
    		exit(0);
  	}
// Reset Console
	if (strcmp(argv[1],"r") == 0) {
		printf("Resetting PSX....\n");
		digitalWrite (RESET, LOW) ;   // Off
		delay(5);
		digitalWrite (RESET, HIGH); 
	}






//upload EXE
	else if (strcmp(argv[1],"x") == 0) {
  		fp = fopen(argv[2], "rb");
  		if (fp==NULL){
     			printf("File Not found\n\n");
    			exit(0);
  		}
		fseek(fp, 0, SEEK_END);
	  	f_size = ftell(fp);
		printf ("File: %s [%u bytes]\n",argv[2], f_size);
		SendEXE();
	}
// upload bin
  	else if (strcmp(argv[1],"u") == 0) {
		fp = fopen(argv[2], "rb");
		if (argc <= 3) {
			printf("Not Enough Arguments\n");
    			exit(0);
  		}
		if (fp==NULL){
     			printf("File Not found\n\n");
    			exit(0);
  		}
		fseek(fp, 0, SEEK_END);
	  	f_size = ftell(fp);
		load_addr = strtoul(argv[3],&ptr,16);
 		printf ("File: %s [%u bytes]\n",argv[2], f_size);
		printf ("Addr: 0x%s\n",argv[3]);
		SendBIN();
        }
//unknown command
  	else {
     		printf("Unknown Command\n");
  	}

}

void SendBIN() {

 	fseek(fp, 0, SEEK_SET);
  	SendByte(0x64);
  	printf ("Sending...\n");

	SendByte((load_addr & 0xFF));
  	SendByte((load_addr >> 8)  & 0xFF);
  	SendByte((load_addr >> 16) & 0xFF);
  	SendByte((load_addr >> 24) & 0xFF);

	SendByte((f_size & 0xFF));
  	SendByte((f_size >> 8)  & 0xFF);
  	SendByte((f_size >> 16) & 0xFF);
  	SendByte((f_size >> 24) & 0xFF);
  	for (i = 0;i<f_size; i++){
  		SendByte(fgetc(fp));
  	}

  	printf("Done.\n");
}

void SendEXE() {

	clock_t t;

	t = clock();

	fseek(fp, 0, SEEK_SET);

	printf ("Send: SYNC\n");

	SendByte(0x63);
	printf ("Send: Header\n");

	for (i = 0;i<2048; i++){
   		SendByte(fgetc(fp));
  	}
	i=0;

	printf ("Send: XADDR\n");
	fseek(fp,16,SEEK_SET);

	SendByte(fgetc(fp));
	SendByte(fgetc(fp));
	SendByte(fgetc(fp));
	SendByte(fgetc(fp));

	printf ("Send: ADDR\n");
	fseek(fp,24,SEEK_SET);
	SendByte(fgetc(fp));
	SendByte(fgetc(fp));
	SendByte(fgetc(fp));
	SendByte(fgetc(fp));

	printf ("Send: EXE Length\n");
	SendByte(fgetc(fp));
	SendByte(fgetc(fp));
	SendByte(fgetc(fp));
	SendByte(fgetc(fp));

	fseek(fp,2048,SEEK_SET);
	printf ("Send: EXE Data\n");
	for (i = 0;i<(f_size-2048); i++){
		SendByte(fgetc(fp));
	}
	t = clock()-t;
	double time_taken = ((double)t)/CLOCKS_PER_SEC;
	printf ("\nTransfer Completed in %.3lf seconds\n", time_taken );
}


void SendByte (char byte)
{
	digitalWriteByte(byte);
	digitalWrite (SEL, HIGH) ;
	do {
    		digitalWrite (SEL, HIGH) ;
	} while (digitalRead(ACK) == 0 );

   	digitalWrite (SEL, LOW);

	while (digitalRead(ACK) == 1 );
	digitalWrite (SEL, LOW);
	digitalWriteByte(0);
}

void ReadByte() {
 // function for reading data from PSX
}

void SetupPins() {
	wiringPiSetupGpio () ;     //USE BCM GPIO Numbering 

	pinMode (D0,   OUTPUT);      //Data Input Pins (from PSX)
	pinMode (D1,   OUTPUT);
	pinMode (D2,   OUTPUT);
	pinMode (D3,   OUTPUT);
	pinMode (D4,   OUTPUT);
	pinMode (D5,   OUTPUT);
	pinMode (D6,   OUTPUT);
	pinMode (D7,   OUTPUT);
	pinMode (SEL,  OUTPUT);

	pinMode (SLCT, INPUT);    //Data Output Pins (from PSX)
	pinMode (PE,   INPUT);
	pinMode (BUSY, INPUT);
	pinMode (ACK,  INPUT);

	pinMode (RESET, OUTPUT);

	digitalWrite (SEL, LOW);
	delay(100);
}

void ErrorMsg() {
	printf("Insufficient Commandline Arguments\n");
	exit(0);
}

