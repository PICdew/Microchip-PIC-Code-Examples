/*
 * File:   main.c
 * Author: frico
 *
 * Simple echo program
 * Listen on Rx pin and echo it on TX pin
 * Created on November 24, 2013, 3:38 PM
 */

#include <xc.h>
#include "fuses.h"
#include <stdio.h>

#define _XTAL_FREQ 8000000 //necessary for delay_ms funcion


#define SDI         PORTBbits.RB1
#define CLK         PORTBbits.RB2
#define SS          PORTBbits.RB3
#define TRIS_SDI    TRISBbits.TRISB1
#define TRIS_CLK    TRISBbits.TRISB2
#define TRIS_SS     TRISBbits.TRISB3


/*MAX31855 32 BIT data register*/
typedef union {
        unsigned long val;
	struct {
        unsigned int OC:1;
        unsigned int SCG:1;
        unsigned int SCV:1;
        unsigned  :1; //reserved
        unsigned int Tint1:4;
        unsigned int Tint2:8;
        unsigned int faultBit:1;
        unsigned  :1; //reserved
        unsigned int Text1:6;
        unsigned int Text2:8; //max bitfield is 8

	} fields;
	
} MAXreg;


void main(void) {

    char i=0;

    long int Text=0,Tint=0;
    unsigned long int raw=0;
    char  calInt;
    MAXreg MAXdata;
  
    //oscilator configuration
    OSCCON=0b01111100;  //internal RC @ 8mhz
    TRISAbits.TRISA6=0; //clock out em RA6, fuse defined


    //interrupt configuration
    INTCON=0;           //disable all itnterrupts

    //general periferal configuration
    CCP1CONbits.CCP1M=0;    //disable CCP1
    RCSTAbits.SPEN=0;       // disable serial port
    SSPCONbits.SSPEN=0; //SSP disabled


    //spi specific conf
    TRIS_SDI=1;
    TRIS_CLK=0;
    TRIS_SS=0;

  
    SS=1;

    while(1)
    {
        raw=0;
        SS=0;
        CLK=0;
        //read 32bit register from spi slave
        __delay_us(1);
        for(i=0;i<32;i++)
        {
            CLK=1;
            __delay_us(1);
            raw+=SDI;
            raw=raw<<1;
            CLK=0;
            __delay_us(1);

        }
        SS=1;
        MAXdata.val=raw;

        int a,b,c,d;
        //25 400
        //x val
        calInt=20; //empirical offset

        Tint= (( MAXdata.fields.Tint2*16 + MAXdata.fields.Tint1 )* 25 )/400 - calInt;
        Text= ((MAXdata.fields.Text2*64 + MAXdata.fields.Text1)* 25 )/100;



 
        
        __delay_ms(100);
    }
}
