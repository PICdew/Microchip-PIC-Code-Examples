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


#define SDI PORTBbits.RB1
#define SDO PORTBbits.RB2
#define SS PORTBbits.RB3
#define CLK PORTBbits.RB4

typedef union{
    struct {
    unsigned char lsB1:8;
    unsigned char lsB2:8;
    unsigned char lsB3:8;
    unsigned char lsB4:8;
    } byte;
    unsigned long val;
} int32_t;



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

void WriteSPI(unsigned int databyte)
{
    PIR1bits.SSPIF=0;          // clear SSP interrupt bit
    SSPBUF = databyte;          // Write data byte to the buffer to initiate transmission
    while(!PIR1bits.SSPIF);    // Wait for interrupt flag to go high indicating transmission is complete
}

unsigned int ReadSPI(void)
{
    unsigned int databyte;

    PIR1bits.SSPIF=0;           // Clear SSP interrupt bit
    SSPBUF = 0x00;              // Write dummy data byte to the buffer to initiate transmission
    while(!SSPSTATbits.BF);     // Wait for interrupt flag to go high indicating transmission is complete

    databyte = SSPBUF;          // Read the incoming data byte
    return (databyte);
}

void main(void) {

    long int Text=0,Tint=0;
    MAXreg MAXdata;
    int32_t raw;
    char calInt;
    //oscilator configuration
    OSCCON=0b01111100;  //internal RC @ 8mhz
    TRISAbits.TRISA6=0; //clock out em RA6, fuse defined


    //interrupt configuration
    INTCON=0;           //disable all itnterrupts

    //general periferal configuration
    CCP1CONbits.CCP1M=0;    //disable CCP1
    RCSTAbits.SPEN=0;       // disable serial port


    //spi specific conf
    TRISBbits.TRISB1=1; //data in
    TRISBbits.TRISB2=0; //data out
    TRISBbits.TRISB4=0; //clk out
    TRISBbits.TRISB3=0; //SS'

    SSPCONbits.CKP=0;   //low level idle state
    SSPCONbits.SSPM0=0; //master mode @ osc/16
    SSPCONbits.SSPM1=1; //master mode @ osc/16
    SSPCONbits.SSPM2=0; //master mode @ osc/16
    SSPCONbits.SSPM3=0; //master mode @ osc/16


    SSPSTATbits.SMP=0;
    SSPSTATbits.CKE=1;

    SSPCONbits.SSPEN=1; //SSP enabeled


    while(1)
    {

        //read raw 32bit
        SS=0;
        __delay_us(2);
        raw.byte.lsB4=ReadSPI();
        raw.byte.lsB3=ReadSPI();
        raw.byte.lsB2=ReadSPI();
        raw.byte.lsB1=ReadSPI();
        SS=1;

        MAXdata.val=raw.val;

        calInt=0; //empirical offset

        Tint= (( MAXdata.fields.Tint2*16 + MAXdata.fields.Tint1 )* 25 )/400 - calInt;
        Text= ((MAXdata.fields.Text2*64 + MAXdata.fields.Text1)* 25 )/100;

        __delay_ms(500);
    }
}
