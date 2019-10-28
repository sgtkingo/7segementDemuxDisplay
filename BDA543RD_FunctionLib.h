/* Microchip Technology Inc. and its subsidiaries.  You may use this software 
 * and any derivatives exclusively with Microchip products. 
 * 
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS".  NO WARRANTIES, WHETHER 
 * EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED 
 * WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A 
 * PARTICULAR PURPOSE, OR ITS INTERACTION WITH MICROCHIP PRODUCTS, COMBINATION 
 * WITH ANY OTHER PRODUCTS, OR USE IN ANY APPLICATION. 
 *
 * IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
 * INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND 
 * WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS 
 * BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE 
 * FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS 
 * IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF 
 * ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *
 * MICROCHIP PROVIDES THIS SOFTWARE CONDITIONALLY UPON YOUR ACCEPTANCE OF THESE 
 * TERMS. 
 */

/* 
 * File:   
 * Author: 
 * Comments:
 * Revision history: 
 */

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef BDA543RD_FUNCTIONLIB
#define	BDA543RD_FUNCTIONLIB

#include <xc.h> // include processor files - each processor file is guarded.
#include "BDA543RD_CharMap.h"

#define DMX_A PORTEbits.RE0
#define DMX_B PORTEbits.RE1

#define displayIndexSize 4
#define segmentsSize 8

const char demuxSelect[]={0b00000000, 0b00000001, 0b00000010, 0b00000011};
const char selectDot[]={0b11111111, 0b01111111};

char bufferData[]={0xFF, 0xFF, 0xFF, 0xFF};
char dotSelector[]={false,false,false,false};

void Init_BDA543RD();
void Test_BDA543RD();
void Clear_BDA543RD();
void ClearBuffer();
unsigned int getNumberOfDisplay();

void setDataToBufferByIndex(char data, unsigned int index);
char getDataFromBufferByIndex(unsigned int index);

void setDotsByIndex(char value, unsigned int index);
char getDotsByIndex(unsigned int index);
void setDotsToAllIndex(char valueIndex0, char valueIndex1,char valueIndex2,char valueIndex3);

void copyDotToBufferByIndex(unsigned int index);
void copyDostToBuffer();

void showBufferDataToDisplayByIndex(unsigned int index);
void showBufferDataToDisplayAuto(unsigned int time_ms);

void setNumberToBufferByIndex(unsigned int number, unsigned int index);
void parseIntNumberToBufferBy2(int First2, int Second2);
void parseIntNumberToBufferBy4(int Value4);

void setCharToBufferByIndex(char mychar, unsigned int index);


void Init_BDA543RD(){
    ANSELD=0;
    TRISD=0;
        
    ANSELEbits.ANSE0=0;
    ANSELEbits.ANSE1=0;
    TRISEbits.RE0=0;
    TRISEbits.RE1=0;
}

void Test_BDA543RD(){
    for(int i=0;i<displayIndexSize;i++){
        PORTD=0xFF;
        __delay_ms(1);
        
        PORTE=demuxSelect[i];
        PORTD=0x00;
        __delay_ms(500);
    }
    for(int i=0;i<displayIndexSize;i++){
        PORTD=0xFF;
        __delay_ms(1);
        PORTE=demuxSelect[i];
        
        PORTD=0b01111111;
        for(int j=0;j<segmentsSize;j++){
            PORTD<<=1;
            __delay_ms(100);
        }
        PORTD=0x00;
        __delay_ms(250);
    } 
}

void Clear_BDA543RD(){
    PORTD=0xFF;
    DMX_A=0;
    DMX_B=0;
    
    ClearBuffer();
}

void ClearBuffer(){
    for(int i=0;i<displayIndexSize;i++){
        bufferData[i]=0x00;
        dotSelector[i]=false;
    }
}

unsigned int getNumberOfDisplay(){
    return displayIndexSize;
}

void setDataToBufferByIndex(char data, unsigned int index){
    if(index >= displayIndexSize)return;
    
    bufferData[index]=data;
}

char getDataFromBufferByIndex(unsigned int index){
    if(index >= displayIndexSize)return;
    
    return bufferData[index];
}

void setDotsByIndex(char value, unsigned int index){
    if(index >= displayIndexSize)return;
    dotSelector[index]=(value & 0x01 );
}

char getDotsByIndex(unsigned int index){
    if(index >= displayIndexSize)return false;
    return dotSelector[index];
}

void setDotsToAllIndex(char valueIndex0, char valueIndex1,char valueIndex2,char valueIndex3){
    dotSelector[0]=(valueIndex0 & 0x01 );
    dotSelector[1]=(valueIndex1 & 0x01 );
    dotSelector[2]=(valueIndex2 & 0x01 );
    dotSelector[3]=(valueIndex3 & 0x01 );
}

void copyDotToBufferByIndex(unsigned int index){
    if(index >= displayIndexSize)return;
    
    bufferData[index]&=selectDot[dotSelector[index]];
}

void copyDostToBuffer(){
    for (int i=0;i<displayIndexSize;i++){
        bufferData[i]&=selectDot[dotSelector[i]];
    }
}

void showBufferDataToDisplayByIndex(unsigned int index){
     if(index >= displayIndexSize)return;
     
    PORTD=0xFF;
    PORTE=demuxSelect[index];
    __delay_us(100);
    
    copyDotToBufferByIndex(index);
    PORTD=bufferData[index];  
}

void showBufferDataToDisplayAuto(unsigned int time_ms){
    copyDostToBuffer();
    while(--time_ms){
        for(int i=0;i<displayIndexSize;i++){
        PORTD=0xFF;
        PORTE=demuxSelect[i];
        __delay_us(100); 

        PORTD=bufferData[i];
        __delay_ms(1); 
        } 
    }
}

void setNumberToBufferByIndex(unsigned int number, unsigned int index){
    if(index >= displayIndexSize)return;
    if(number >= sizeof(numbersData)/sizeof(char))return;
    
    bufferData[index]=numbersData[number];
}

void parseIntNumberToBufferBy2(int First2, int Second2){
    setNumberToBufferByIndex(Second2/10, 2);
    setNumberToBufferByIndex(Second2%10, 3);
    setNumberToBufferByIndex(First2/10, 0);
    setNumberToBufferByIndex(First2%10, 1); 
}

void parseIntNumberToBufferBy4(int Value4){
    setNumberToBufferByIndex(Value4/1000, 0);
    setNumberToBufferByIndex(Value4/100, 1);
    setNumberToBufferByIndex(Value4/10, 2);
    setNumberToBufferByIndex(Value4%10, 3); 
}

void setCharToBufferByIndex(char mychar, unsigned int index){
    if(index >= displayIndexSize)return;
    bufferData[index]=getChar(mychar);
}



#endif	/* BDA543RD_FUNCTIONLIB */

