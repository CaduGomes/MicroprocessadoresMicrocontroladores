/*
 * File:   main.c
 * Author: Carlos Eduardo Vitorino Gomes
 *
 * Created on 11 de Agosto de 2021, 17:00
 */


#include <xc.h>
#include <pic16f877a.h>
#include <stdio.h>

#define _XTAL_FREQ 4000000

#define TANQUEA PORTDbits.RD0
#define TANQUEB PORTDbits.RD1
#define TANQUE_PRINCIPAL PORTDbits.RD6
#define MOTOR PORTDbits.RD7
#define INICIAR PORTBbits.RB1
#define SENSOR PORTBbits.RB3

__bit emergenciaBit = 0;

void encherPrincipal();
void misturar();
void finalizar();

void __interrupt() emergencia(void) {
    if (INTF) {
        emergenciaBit = 1;
        TANQUE_PRINCIPAL = 1;
        TANQUEA = 0;
        TANQUEB = 0;
        MOTOR = 0;

        INTF = 0;
    }

    return;
}

void main(void) {
    TRISB = 0xFF;
    TRISD = 0;
    OPTION_REG = 0b01111111;
    OPTION_REGbits.INTEDG = 1;
    INTCONbits.GIE = 1;
    INTCONbits.INTE = 1;
    TANQUEA = 0;
    TANQUEB = 0;
    TANQUE_PRINCIPAL = 1;
    SENSOR = 0;

    while (1) {
        if (!INICIAR) {
            encherPrincipal();
        }
        emergenciaBit = 0;

    }
}

void encherPrincipal() {
    __delay_ms(500);
    TANQUE_PRINCIPAL = 0;
    __delay_ms(50);
    TANQUEA = 1;
    TANQUEB = 1;
            if(emergenciaBit) {
            return;
        }
    misturar();
}

void misturar() {
    while (1) {
        if (!SENSOR) {
            __delay_ms(500);
            TANQUEA = 0;
            TANQUEB = 0;
            __delay_ms(200);
            MOTOR = 1;
            __delay_ms(8000);
            MOTOR = 0;
            break;
        }
        if(emergenciaBit) {
            return;
        }
    }
    finalizar();
}

void finalizar() {
    
    TANQUE_PRINCIPAL = 1;
    __delay_ms(5000);
    TANQUEA = 0;
    TANQUEB = 0;
    return;
}
