//Author: Jiri Konecny 
// PIC18LF46K22 Configuration Bit Settings

#include "PICF18LF46K22_ConfigSFR.h"

#define DMX_A PORTEbits.RE0
#define DMX_B PORTEbits.RE1

#define BTN_A_P PORTBbits.RB0
#define BTN_A_N PORTBbits.RB1
#define BTN_B_P PORTBbits.RB2
#define BTN_B_N PORTBbits.RB3

#define demuxSelectSize 4

#define numbersValueSize 4
#define numbersDataSize 10
#define testDataSize 8

const char demuxSelect[]={0b00000000, 0b00000001, 0b00000010, 0b00000011};
char demuxData[]={0xFF, 0xFF, 0xFF, 0xFF};

const char numbersData[]={ 0b11000000 /*0*/ ,0b11111001 /*1*/, 0b10100100  /*2*/,0b10110000 /*3*/,0b10011001 /*4*/,0b10010010 /*5*/,0b10000010 /*6*/,0b11111000 /*7*/,0b10000000 /*8*/,0b10010000 /*9*/};
int numbersValue[]={0,0,0,0};

const char charsetData[]={0b11111101 /*-*/};

const char selectDot[]={0b11111111, 0b01111111};
char dotSelector[]={false,true,false,false};

bit mode;
int indexer=0;
unsigned int seconds=0,mins=0,hours=0,counter_ms=0;

void InitDevice(){
    OSCCON=0b01111100; //osc setting, 16 MHz, internal by FOSCH
    INTCON=0b11100000; //int setting, global,pir, timer0
    
    PIE5bits.TMR4IE=1; //timer4 int owerflow enable
    
    INTCON2bits.NOT_RBPU=1; //potrB pullup enable
    WPUBbits.WPUB0=1;
    WPUBbits.WPUB1=1;
    WPUBbits.WPUB2=1;
    WPUBbits.WPUB3=1;
    
    //TOCON control 7SEGMENT DISPLAY
    T0CON=0b01000011; //timer0 setting, PS 1/16 * 2 (FOST/4) 
    //T4CON control RTC
    T4CON=0b00000011; //timer4 setting, POST-S 1/1, PS 1/16 (FOST/4) 
    
    ANSELD=0;
    TRISD=0;
        
    ANSELEbits.ANSE0=0;
    ANSELEbits.ANSE1=0;
    TRISEbits.RE0=0;
    TRISEbits.RE1=0;
    
    ANSELB=0;
    TRISBbits.RB0=1;  
    TRISBbits.RB1=1;
    TRISBbits.RB2=1;
    TRISBbits.RB3=1;
}

void ClearDevice(){
    PORTD=1;
    DMX_A=DMX_B=0;
    indexer=0;
    mode=true;
}

void TestDevice(){
    for(int i=0;i<demuxSelectSize;i++){
        PORTD=0xFF;
        __delay_ms(1);
        
        PORTE=demuxSelect[i];
        PORTD=0x00;
        __delay_ms(500);
    }
    for(int i=0;i<demuxSelectSize;i++){
        PORTD=0xFF;
        __delay_ms(1);
        PORTE=demuxSelect[i];
        
        PORTD=0b01111111;
        for(int j=0;j<testDataSize;j++){
            PORTD<<=1;
            __delay_ms(100);
        }
        PORTD=0x00;
        __delay_ms(250);
    } 
}
void parseValues(int F, int S){
        numbersValue[2]=S/10; 
        numbersValue[3]=S%10;
        numbersValue[0]=F/10; 
        numbersValue[1]=F%10;      
}

void CheckButtons(){
    if (BTN_A_P&&BTN_A_N&&BTN_B_P&&BTN_B_N)return;
    if (!(BTN_A_P||BTN_A_N||BTN_B_P||BTN_B_N)){
            mode=!mode;
            __delay_ms(500);
    }
    if(!mode){
        if((!BTN_A_P) && (mins<60)){
            asm("NOP");
            if(!BTN_A_P)mins++;
        }
        if((!BTN_A_N) && (mins>0)){
            asm("NOP");
            if(!BTN_A_N)mins--;
        }
        if((!BTN_B_P) && (hours<24)){
            asm("NOP");
            if(!BTN_B_P)hours++;
        }
        if((!BTN_B_N) && (hours>0)){
            asm("NOP");
            if(!BTN_B_N)hours--;
        }
    }else {
        if((!BTN_A_P) && (seconds<60)){
            asm("NOP");
            if(!BTN_A_P)seconds++;
        }
        if((!BTN_A_N) && (seconds>0)){
            asm("NOP");
            if(!BTN_A_N)seconds--;
        }
        if((!BTN_B_P) && (mins<60)){
            asm("NOP");
            if(!BTN_B_P)mins++;
        }
        if((!BTN_B_N) && (mins>0)){
            asm("NOP");
            if(!BTN_B_N)mins--;
        }    
    }
}

void timeStep(){
        counter_ms++;
        if(!(counter_ms%(1000/2)) && (counter_ms>0))dotSelector[1]=!dotSelector[1];
        if(!(counter_ms%1000) && (counter_ms>0)){
            seconds++;
            counter_ms=0;
        }
        if(!(seconds%60) && seconds!=0){
            mins++;
            seconds=0;
        }
        if(!(mins%60) && mins!=0){
            hours++;
            mins=0;
        }
        if(!(hours%24) && hours!=0){
            hours=0;
        }
        
        for(int i=0;i<demuxSelectSize;i++){
            if(numbersValue[i]>=numbersDataSize)numbersValue[i]=numbersDataSize-1;
            if(numbersValue[i]<0)numbersValue[i]=0;
            
            demuxData[i]=numbersData[numbersValue[i]];
            demuxData[i]&=selectDot[dotSelector[i]];
        }  
        if(mode)parseValues(mins,seconds);
        else parseValues(hours,mins);
}


void main(void) {
    InitDevice();
    ClearDevice();
    TestDevice();
    T0CONbits.TMR0ON=1;
    T4CONbits.TMR4ON=1;
    
    while(1){
        asm("NOP");
        asm("CLRWDT");
        CheckButtons();
        __delay_ms(50);        
    }
}

void interrupt IRS(void){
    if(INTCONbits.TMR0IF){
        INTCONbits.TMR0IF=0;
        
        PORTD=0xFF;
        PORTE=demuxSelect[indexer];
        __delay_us(100);
        
        PORTD=demuxData[indexer];           
         
         indexer++;
         if(indexer>=demuxSelectSize)indexer=0;
    }
    
    if(PIR5bits.TMR4IF){
        PIR5bits.TMR4IF=0;
        timeStep();
    }
}
