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

#define _XTAL_FREQ 8000000 //necessary for delay_ms funcion

typedef char int8;
typedef unsigned char uint8;
typedef int int16;
typedef unsigned int  uint16;


void interrupt ISR()
{

    // UART RX byte
    if(PIR1bits.RCIF)
    {
        PIR1bits.RCIF=0;

        if(!OERR) // no overrun
        {
            // TX
            while(!TXSTAbits.TRMT);    // make sure buffer full bit is high before transmitting
            TXREG=RCREG;
        }
            
        else //in case of overrun
        {
          RCSTAbits.CREN=0;
          RCSTAbits.CREN=1;
        }
     }



}
void main(void) {

    //oscilator configuration
    OSCCON=0b01111100;  //internal RC @ 8mhz
    TRISAbits.TRISA6=0; //clock out em RA6, fuse defined


    //interrupt configuration
    INTCON=0;           //disable all itnterrupts
    INTCONbits.GIE=1;   //enable interruts
    INTCONbits.PEIE=1;  //select periferal interrupts
    PIE1bits.RCIE=1;    // selectuart rx interrupt;

    //periferal configuration
    CCP1CONbits.CCP1M=0; //disable CCP1
    SSPCONbits.SSPEN=0; //disable ssp

    //enable serial port
    RCSTAbits.SPEN=1; //serial enabeled

    //baud
    SPBRG=51; //9600

    //tx confg
    TXSTAbits.TX9=0; //8 bit mode
    TXSTAbits.TXEN=1;//enable tx
    TXSTAbits.SYNC=0;//assinc
    TXSTAbits.BRGH=1;//high speed
    
    //rx config
    RCSTAbits.RX9=0; //8 bit mode
    RCSTAbits.CREN=1; //enables continous receive
    RCSTAbits.ADDEN=0; //disable address detection


    //GPIO SETUP
//    TRISBbits.TRISB2=1;
//  TRISBbits.TRISB5=0;

   while(1)
    {

    }
    

}
