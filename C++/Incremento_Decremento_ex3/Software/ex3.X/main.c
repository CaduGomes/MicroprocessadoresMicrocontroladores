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

void incrementar(int *val);
void decrementar(int *val);

void main(void) {
    TRISB = 0b00000111;
    TRISD = 0;
    OPTION_REG = 0b01111111;
    int val = 0;
    PORTDbits.RD0 = 0;
    PORTDbits.RD1 = 0;
    while (1) {
        if (PORTBbits.RB1 == 0) {
            incrementar(&val);
        } else if (PORTBbits.RB2 == 0) {
            decrementar(&val);
        }
        if (val == 10)
            PORTDbits.RD0 = 1;
        else if (val == 0)
            PORTDbits.RD1 = 1;
        else {
            PORTDbits.RD0 = 0;
            PORTDbits.RD1 = 0;
        }


    }
}

void incrementar(int *val) {
    if (*val == 10)
        return;

    __delay_ms(250);
    (*val)++;

    return;
}

void decrementar(int *val) {
    if (*val == 0)
        return;

    __delay_ms(250);
    (*val)--;

    return;
}
