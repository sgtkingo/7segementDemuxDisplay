/*
 * File:   main.c
 * Author: Konecny Jiri
 *
 * Created on 3. 9. 2019, 10:36
 */
// PIC16F18323 Configuration Bit Settings

// 'C' source line config statements

// CONFIG1
#pragma config FEXTOSC = OFF    // FEXTOSC External Oscillator mode Selection bits (Oscillator not enabled)
#pragma config RSTOSC = HFINT1  // Power-up default value for COSC bits (HFINTOSC (1MHz))
#pragma config CLKOUTEN = OFF   // Clock Out Enable bit (CLKOUT function is disabled; I/O or oscillator function on OSC2)
#pragma config CSWEN = ON       // Clock Switch Enable bit (Writing to NOSC and NDIV is allowed)
#pragma config FCMEN = ON       // Fail-Safe Clock Monitor Enable (Fail-Safe Clock Monitor is enabled)

// CONFIG2
#pragma config MCLRE = ON       // Master Clear Enable bit (MCLR/VPP pin function is MCLR; Weak pull-up enabled)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config WDTE = ON        // Watchdog Timer Enable bits (WDT enabled, SWDTEN is ignored)
#pragma config LPBOREN = OFF    // Low-power BOR enable bit (ULPBOR disabled)
#pragma config BOREN = ON       // Brown-out Reset Enable bits (Brown-out Reset enabled, SBOREN bit ignored)
#pragma config BORV = LOW       // Brown-out Reset Voltage selection bit (Brown-out voltage (Vbor) set to 2.45V)
#pragma config PPS1WAY = ON     // PPSLOCK bit One-Way Set Enable bit (The PPSLOCK bit can be cleared and set only once; PPS registers remain locked after one clear/set cycle)
#pragma config STVREN = ON      // Stack Overflow/Underflow Reset Enable bit (Stack Overflow or Underflow will cause a Reset)
#pragma config DEBUG = OFF      // Debugger enable bit (Background debugger disabled)

// CONFIG3
#pragma config WRT = OFF        // User NVM self-write protection bits (Write protection off)
#pragma config LVP = ON         // Low Voltage Programming Enable bit (Low Voltage programming enabled. MCLR/VPP pin function is MCLR. MCLRE configuration bit is ignored.)

// CONFIG4
#pragma config CP = OFF         // User NVM Program Memory Code Protection bit (User NVM code protection disabled)
#pragma config CPD = OFF        // Data NVM Memory Code Protection bit (Data NVM code protection disabled)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.


#include <xc.h>

#define LED_STATUS RA5

#define DEMUX_OUT_1 RA2
#define DEMUX_OUT_2 RC0
#define DEMUX_OUT_3 RC1
#define DEMUX_OUT_4 RC2

#define DEMUX_IN_A RC3
#define DEMUX_IN_B RC4

#define _XTAL_FREQ 1000000

void Init(){
    PORTA=PORTC=0;
    ANSELA=ANSELC=0; //all pin as D
    
    TRISAbits.TRISA5=0; //Indicate LED out
    
    TRISAbits.TRISA2=0; //Demux OUT
    TRISCbits.TRISC0=0;
    TRISCbits.TRISC1=0;
    TRISCbits.TRISC2=0;
    
    TRISCbits.TRISC3=1; //Demux IN
    TRISCbits.TRISC4=1;
    
    WPUCbits.WPUC3=1; //WPU protection
    WPUCbits.WPUC4=1;
}

void ClearDevice(){
    DEMUX_OUT_1=1;
    DEMUX_OUT_2=1;
    DEMUX_OUT_3=1;
    DEMUX_OUT_4=1;
    
    LED_STATUS=0;
}

void TestDevice(){
    DEMUX_OUT_1=0;
    DEMUX_OUT_2=0;
    DEMUX_OUT_3=0;
    DEMUX_OUT_4=0;
    __delay_ms(500);
    DEMUX_OUT_1=1;
    DEMUX_OUT_2=1;
    DEMUX_OUT_3=1;
    DEMUX_OUT_4=1;
    
    LED_STATUS=1;
}

void DemuxSwitcher(){
    DEMUX_OUT_1=1;
    DEMUX_OUT_2=1;
    DEMUX_OUT_3=1;
    DEMUX_OUT_4=1;
    
    if(!DEMUX_IN_A && !DEMUX_IN_B){
        DEMUX_OUT_1=0;
    }
    if(DEMUX_IN_A && !DEMUX_IN_B){
        DEMUX_OUT_2=0;
    }
    if(!DEMUX_IN_A && DEMUX_IN_B){
        DEMUX_OUT_3=0;
    }
    if(DEMUX_IN_A && DEMUX_IN_B){
        DEMUX_OUT_4=0;
    }

}

void main(void) {
    Init();
    ClearDevice();
    TestDevice();
    
    while(1){
        DemuxSwitcher();
        asm("CLRWDT");
    }
}

