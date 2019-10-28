//Author: Jiri Konecny 
// PIC18LF46K22 Configuration Bit Settings

#include "PICF18LF46K22_ConfigSFR.h"
#include "BDA543RD_FunctionLib.h"

#define BTN_A_P PORTBbits.RB0
#define BTN_A_N PORTBbits.RB1
#define BTN_B_P PORTBbits.RB2
#define BTN_B_N PORTBbits.RB3

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
    
    ANSELB=0;
    TRISBbits.RB0=1;  
    TRISBbits.RB1=1;
    TRISBbits.RB2=1;
    TRISBbits.RB3=1;
    
    Init_BDA543RD();
}

void ClearDevice(){
    Clear_BDA543RD();
    indexer=0;
    mode=true;
}

void TestDevice(){
    Test_BDA543RD();
    
    for(int i=0;i<getNumberOfDisplay();i++){
        setCharToBufferByIndex('-', i);
    }
    showBufferDataToDisplayAuto(1000);
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
        if(!(counter_ms%(1000/2)) && (counter_ms>0))setDotsByIndex(!getDotsByIndex(1),1);
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
        if(mode)parseIntNumberToBufferBy2(mins, seconds);
        else parseIntNumberToBufferBy2(hours, mins);
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

        showBufferDataToDisplayByIndex(indexer);
        indexer++;
        if(indexer>=getNumberOfDisplay())indexer=0;
    }
    
    if(PIR5bits.TMR4IF){
        PIR5bits.TMR4IF=0;
        timeStep();
    }
}
