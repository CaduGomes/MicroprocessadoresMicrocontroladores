/*
 * File:   main.c
 * Author: cadug
 *
 * Created on 31 de Agosto de 2021, 13:37
 */


#include <xc.h>
#include <pic16f877a.h>
#include <stdio.h>

#define _XTAL_FREQ 4000000

#define LED PORTBbits.RB0

void __interrupt() interruption(void);

void main(void) {
    TRISB = 0xFF;
    TRISD = 0;
    OPTION_REGbits.nRBPU = 0; // Pull-ups
    
    OPTION_REGbits.T0CS = 1; // Define como contador externo
    OPTION_REGbits.T0SE = 1; // Define 
    OPTION_REGbits.PS0 = 1; // Pull-ups

    INTCONbits.GIE = 1; // INT global
    INTCONbits.TMR0IE = 1; // Int dos perifericos

    TMR0 = 246;

    LED = 1;

    while (1) {

    }

    return;
}
