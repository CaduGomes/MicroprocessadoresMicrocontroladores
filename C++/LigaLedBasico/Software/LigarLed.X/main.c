/*
 * File:   main.c
 * Author: Carlos Eduardo Vitorino Gomes
 *
 * Created on 11 de Agosto de 2021, 13:48
 */


#include <xc.h>
#include <pic16f877a.h>
#include <stdio.h>

#define _XTAL_FREQ 4000000

void main(void) {
    TRISB = 0xFF;
    TRISD = 0;
    OPTION_REG = 0b01111111;
    PORTDbits.RD0 = 1;
    while (1) {
        if (PORTBbits.RB0 == 0) {
            PORTDbits.RD0 = 1;
        } else {
            PORTDbits.RD0 = 0;
        }

    }
}
