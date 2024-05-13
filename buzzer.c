#include <reg52.h>
#include <intrins.h>
#include"i2c.h"
#define uchar unsigned char
#define uint unsigned int
sbit melody = P2^5;

sbit LSA = P2^2;
sbit LSB = P2^3;
sbit LSC = P2^4;
sbit BTN = P3^2;
sbit temp= P1^5;
bit btnPressed = 0;
bit playMelodyEnabled = 0; 
void At24c02Write(unsigned char ,unsigned char );
unsigned char At24c02Read(unsigned char );
#define Segment P0

int check=0;
uchar flag = 0;
uchar revData,play=1;
uchar buffer1,buffer2;
uchar updateConstRevData = 1;

#define pitch_P		0
#define pitch_C5	64580
#define pitch_Db5	64634
#define pitch_D5	64685
#define pitch_Eb5	64732
#define pitch_E5	64778
#define pitch_F5	64820
#define pitch_Gb5	64860
#define pitch_G5	64898
#define pitch_Ab5	64934
#define pitch_A5	64968
#define pitch_Bb5	65000
#define pitch_B5	65030
#define pitch_C6	65058
#define pitch_Db6	65085
#define pitch_D6	65110
#define pitch_Eb6	65134
#define pitch_E6	65157
#define pitch_F6	65178
#define pitch_Gb6	65198
#define pitch_G6	65217
#define pitch_Ab6	65235
#define pitch_A6	65252
#define pitch_Bb6	65268
#define pitch_B6	65283

#define MelodyPin	melody

int seg_7, dich, tg;

unsigned char Bai1[8] = {0x00, 0x00, 0x00, 0x06, 0x00, 0x30, 0x77, 0x7c};
unsigned char Bai2[8] = {0x00, 0x00, 0x00, 0x5b, 0x00, 0x30, 0x77, 0x7c};
unsigned char Bai[8];
int dieukhien[8];

unsigned int qtrN, qtrN_us, wholeN, wholeN_us, halfN, halfN_us, eighthN, eighthN_us,
			 sixteenN, sixteenN_us, thirtyTwoN, thirtyTwoN_us, sixtyFourN, sixtyFourN_us,
			 sixN, sixN_us, twelveN, twelveN_us, twentyFourN, twentyFourN_us;

void Delay10MC(unsigned char);
void DigDisplay(void);

void T2ISR(void) interrupt 5 {
	MelodyPin = ~MelodyPin;
	TF2 = 0;
}

void MelodyTempo(int Tempo){
	qtrN = ((60000/Tempo));
	wholeN = qtrN*4;
	halfN = (qtrN*2);
	eighthN = (qtrN/2);
	sixteenN = (qtrN/4);
	thirtyTwoN = (qtrN/8);
	sixtyFourN = (qtrN/16);
	sixN = (wholeN/6);
	twelveN = (wholeN/12);
	twentyFourN = (wholeN/24);
}

void PlayNote(const unsigned char PitchH, const unsigned char PitchL){
	RCAP2H = PitchH;
	RCAP2L = PitchL;
	TH2 = PitchH;
	TL2 = PitchL;
	TF2 = 0;
	TR2 = 1;
	ET2 = 1;
	EA = 1;
}

void EndNote(void){
	EA = 0;
	TR2 = 0;
	TF2 = 0;
	MelodyPin = 1;
}

void delay_ms(unsigned int ms){
	for(;ms>0;ms--)
		{
			DigDisplay();}
		}

void Melodyplay(const int Pitch, unsigned int interval){
	if(Pitch!=0)
		PlayNote(Pitch >> 8, Pitch);
	delay_ms(interval);
	EndNote();
}
void initUART(void) {
    TMOD = 0x20;  // Timer 1, mode 2
    TH1 = 0xFD;   // Baud rate 9600
    TR1 = 1;      // B?t d?u Timer 1
    SCON = 0x50;  // UART mode 1, 8-bit data, 1 stop bit, 1 start bit
    EA = 1;       // Cho phép ng?t toàn c?c
    ES = 1;       // Cho phép ng?t UART
}
void serial_Send (unsigned char x)
{
	SBUF = x;	//nap gia tri vao s=SUBF
	while (TI==0);	 //doi den khi data duoc gui hoan tat
	TI=0;	 //clear co TI ve 0 de thuc hien lan gui tiep theo
}

void DigDisplay(void)
{
	unsigned char i;
	unsigned int j;
	for(i=0;i<8;i++)
	{
		switch(i)	 //Î»Ñ¡£¬Ñ¡ÔñµãÁÁµÄÊýÂë¹Ü£¬
		{
			case(0):
				LSA=0;LSB=0;LSC=0; break;//ÏÔÊ¾µÚ0Î»
			case(1):
				LSA=1;LSB=0;LSC=0; break;//ÏÔÊ¾µÚ1Î»
			case(2):
				LSA=0;LSB=1;LSC=0; break;//ÏÔÊ¾µÚ2Î»
			case(3):
				LSA=1;LSB=1;LSC=0; break;//ÏÔÊ¾µÚ3Î»
			case(4):
				LSA=0;LSB=0;LSC=1; break;//ÏÔÊ¾µÚ4Î»
			case(5):
				LSA=1;LSB=0;LSC=1; break;//ÏÔÊ¾µÚ5Î»
			case(6):
				LSA=0;LSB=1;LSC=1; break;//ÏÔÊ¾µÚ6Î»
			case(7):
				LSA=1;LSB=1;LSC=1; break;//ÏÔÊ¾µÚ7Î»	
		}
		Segment=Bai[i];
		//·¢ËÍ¶ÎÂë
		j=7;						 //É¨Ãè¼ä¸ôÊ±¼äÉè¶¨
		while(j--);	
		Segment = 0x00;//ÏûÒþ
	}
}

void serial_ISR(void) interrupt 4 {

    if (RI){
			revData = SBUF;
			
	   if (check==0) {  buffer1=SBUF; check=1;}
		else if(check == 1)  {  buffer2=SBUF; check=0; flag=1;}
			RI = 0;
       	
        }
    }
void playReceivedNote(uchar note, uchar duration) {
    uint pitch = pitch_P; // m?c d?nh là không có âm thanh
	uint dura;
    switch(note) {
			case 1: pitch = pitch_C5; break;	
			case 2: pitch = pitch_Db5; break;	
			case 3: pitch = pitch_D5; break;
			case 4: pitch =pitch_Eb5; break;	
			case 5: pitch = pitch_E5; break;	
			case 6: pitch = pitch_F5; break;
			case 7: pitch =pitch_Gb5; break;
			case 8: pitch =pitch_G5; break;	
			case 9: pitch = pitch_Ab5; break;	
			case 10: pitch =pitch_A5; break;
			case 11: pitch = pitch_Bb5; break;
			case 12: pitch =pitch_B5; break;
			case 13: pitch =pitch_C6; break;
			case 14: pitch = pitch_Db6; break;
			case 15: pitch = pitch_D6; break;
			case 16: pitch = pitch_Eb6; break;
			case 17: pitch =pitch_E6; break;
			case 18: pitch =pitch_F6; break;
			case 19: pitch =pitch_Gb6 ; break;
			case 20: pitch =pitch_G6	; break;
			case 21: pitch = pitch_Ab6 ; break;
			case 22: pitch =pitch_A6	; break;
			case 23: pitch =pitch_Bb6 ; break;
			case 24: pitch = pitch_B6 ; break;

        default: pitch = pitch_P; 
    }
	MelodyTempo(100);
	switch (duration)
            {
                case 1: dura=qtrN; break;
                case 2: dura=wholeN; break;
                case 3: dura= halfN; break;
                case 4: dura=eighthN; break;
                case 5: dura=sixteenN; break;
                case 6: dura=thirtyTwoN; break;
                case 7: dura=sixtyFourN; break;
                case 8: dura=sixN; break;
                case 9: dura=twelveN; break;
                case 10: dura=twentyFourN; break;
                default: dura=0;
            }
     if (pitch != pitch_P) {
        Melodyplay(pitch,dura);
    }
		 
    delay_ms(duration);
    EndNote();
	temp=!temp;
	}
void At24c02Write(unsigned char addr,unsigned char dat)
{
	I2cStart();
	I2cSendByte(0xa0);//·¢ËÍÐ´Æ÷¼þµØÖ·
	I2cSendByte(addr);//·¢ËÍÒªÐ´ÈëÄÚ´æµØÖ·
	I2cSendByte(dat);	//·¢ËÍÊý¾Ý
	I2cStop();

}

uchar At24c02Read(unsigned char addr)
{
	uchar num;
	I2cStart();
	I2cSendByte(0xa0); //·¢ËÍÐ´Æ÷¼þµØÖ·
	I2cSendByte(addr); //·¢ËÍÒª¶ÁÈ¡µÄµØÖ·
	I2cStart();
	I2cSendByte(0xa1); //·¢ËÍ¶ÁÆ÷¼þµØÖ·
	num=I2cReadByte(); //¶ÁÈ¡Êý¾Ý
	I2cStop();
	return num;	
}
void playMelodyFromEEPROM() {
    unsigned int addr = 0;
    unsigned char storedData1;
	unsigned char storedData2;
    do {
		delay_ms(50);
        storedData1 = At24c02Read(addr);
		addr++;
		storedData2 = At24c02Read(addr);
        playReceivedNote(storedData1, storedData2); 
		EA = 1;
        addr++;
        /*if (addr >255)
		{
        break; 
        }*/
    } while (addr<255);  
}	
void initINT(){
	EA = 1;     
    EX0 = 1;    
    IT0 = 1;    

    EX1 = 1;   
    IT1 = 1;    
}
void externalInterrupt0() interrupt 0
{
    play=!play;
	//serial_Send('0');
}

void externalInterrupt1() interrupt 2
{
	serial_Send('2');
    // X? lý khi có ng?t ngo?i INT1
} 
void  At24c02clear(){
	unsigned char Addr=0; 
	do {
	 At24c02Write(Addr, 0xff);
	 Addr++;
	}
	while  (Addr<=255);
}
void main(){
	int i;
	unsigned char eepromAddr = 0;
	P0 = 0x00;

	//initINT();
 	initUART();
//	At24c02clear();
   
/*	 At24c02Write(0, 0x04);
	 delay_ms(10);
   //  eepromAddr++;
     At24c02Write(1, 0x04);
	 delay_ms(10);
    // eepromAddr++;  
	 At24c02Write(2, 0x04);
	 delay_ms(10);
   //  eepromAddr++;
     At24c02Write(3, 0x02);
	 delay_ms(10);
    // eepromAddr++;*/  	
	while(1)
	{
		if( BTN == 0 ){
		delay_ms(20);
		 	while(	BTN == 0);
			btnPressed=1 ; 

		}
		if (btnPressed==1)
		 {
		 	temp=0; 
           // playMelodyEnabled = !playMelodyEnabled;
            //if (playMelodyEnabled) {
			//temp=!temp;  
            playMelodyFromEEPROM();	//}
			  btnPressed=0 ; 
       	 }
		
		if(flag ==1)
			{
				if(revData == 0xff){  
						for (i = 0; i < 8; i++)
					{
							Bai[i] = Bai1[i];
					}
				}
				if(revData == 0xfe){  
						for (i = 0; i < 8; i++)
					{
							Bai[i] = Bai2[i];
					}
				}

				//ghi vao eeprom buffer1
				 At24c02Write(eepromAddr, buffer1);
				 delay_ms(10);
                 eepromAddr++;
                 if (eepromAddr > 255) eepromAddr = 0; 
				//ghi vao eeprom buffer2
                 At24c02Write(eepromAddr, buffer2);
				 delay_ms(10);
                 eepromAddr++;
                 if (eepromAddr > 255) eepromAddr = 0;   
                									   
				if (play==1)
				{
					playReceivedNote(buffer1,buffer2);

					EA = 1;
					flag=0;
					serial_Send('1');
				}	  
	   			  

			} 
	}
}	
