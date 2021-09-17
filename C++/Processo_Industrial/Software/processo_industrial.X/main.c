/*
 * File:   main.c
 * Author: Carlos Eduardo Vitorino Gomes e Matheus Jacks
 *
 */

#include <xc.h>
#include <pic16f877a.h>
#include <stdio.h>

#define _XTAL_FREQ 4000000

#define BOMBA PORTDbits.RD7
#define MISTURADOR PORTDbits.RD4
#define AQUECEDOR PORTDbits.RD6
#define SENSOR_NIVEL PORTBbits.RB0
#define SENSOR_TEMPERATURA PORTBbits.RB3
#define SAIDA_AGUA PORTDbits.RD0

int contador = 0;
__bit misturou = 0;

void __interrupt() interruption(void);
void ligar_bomba(void);
void desligar_bomba(void);
void ligar_misturador(void);
void sair_agua(void);

void main(void) {
    TRISB = 0xFF;
    TRISD = 0;
    OPTION_REGbits.nRBPU = 0; // Pull-ups
    INTCONbits.GIE = 1; // INT global
    INTCONbits.PEIE = 1; // Int dos perifericos
    PIE1bits.TMR1IE = 1; // int do timer 1

    T1CONbits.TMR1CS = 0; // define Timer 1 como temporizador
    T1CONbits.T1CKPS0 = 1; // Define pre-escaler como 1:8
    T1CONbits.T1CKPS1 = 1; // Define pre-escaler como 1:8

    SAIDA_AGUA = 1;

    TMR1L = 0xDC; // 8 ultimos
    TMR1H = 0x0B; // 8 primeiros

    while (1) {
        if (!SENSOR_NIVEL) {
            desligar_bomba();
        } else {
            ligar_bomba();
        }
    }
}

void __interrupt() interruption(void) {

    if (TMR1IE) {
        PIR1bits.TMR1IF = 0;
        TMR1L = 0xDC;
        TMR1H = 0x0B;
        contador++;

        if (contador == 20) {
            misturou = 1;
            MISTURADOR = 0;
            contador = 0;
            T1CONbits.TMR1ON = 0;

            __delay_ms(100);
            sair_agua();
        }
    }

    return;
}

void ligar_bomba(void) {
    BOMBA = 1;
    MISTURADOR = 0;
    AQUECEDOR = 0;
}

void desligar_bomba(void) {
    BOMBA = 0;
    if (!SENSOR_TEMPERATURA) {
        AQUECEDOR = 0;

        if (!T1CONbits.TMR1ON && !misturou) {
            MISTURADOR = 1;
            T1CONbits.TMR1ON = 1;
        }

    } else {
        AQUECEDOR = 1;
        misturou = 0;
    }

}

void sair_agua(void) {
    MISTURADOR = 0;
    AQUECEDOR = 0;
    SAIDA_AGUA = 0;
    __delay_ms(2000);
    SAIDA_AGUA = 1;

}