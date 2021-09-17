/*
 * File:   main.c
 * Author: Carlos Eduardo Vitorino Gomes
 *
 * Created on 11 de Agosto de 2021, 14:55
 */


#include <xc.h>
#include <pic16f877a.h>
#include <stdio.h>

#define _XTAL_FREQ 4000000

volatile __bit recieved_intf = 0;

void __interrupt() emergencia(void) {
    if (INTF) {
        recieved_intf = 1;
        PORTDbits.RD1 = 0;
        PORTDbits.RD0 = 1;
        __delay_ms(3000);
        PORTDbits.RD1 = 0;
        PORTDbits.RD0 = 0;
        INTF = 0;
    }
    return;
}

void ativaPrensa(void) {
    if (!recieved_intf) {
        PORTDbits.RD1 = 1;
        PORTDbits.RD0 = 0;
        __delay_ms(3000);
    }
    if (!recieved_intf) {
        PORTDbits.RD1 = 0;
        PORTDbits.RD0 = 1;
        __delay_ms(3000);
    }


    PORTDbits.RD0 = 0;
    PORTDbits.RD1 = 0;
    return;
}

void main(void) {
    TRISB = 0b00000111;
    TRISD = 0;
    OPTION_REG = 0b01111111;
    OPTION_REGbits.INTEDG = 1;
    INTCONbits.GIE = 1;
    INTCONbits.INTE = 1;
    PORTDbits.RD0 = 0;
    PORTDbits.RD1 = 0;
    while (1) {
        recieved_intf = 0;
        if (PORTBbits.RB1 == 0 && PORTBbits.RB2 == 0) {
            ativaPrensa();
        }

    }
}
