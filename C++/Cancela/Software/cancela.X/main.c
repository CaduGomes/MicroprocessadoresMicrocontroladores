/*
 * File:   main.c
 * Author: Carlos Eduardo Vitorino Gomes e Matheus Rocha Jacks
 *
 */

#include <xc.h>
#include <pic16f877a.h>
#include <stdio.h>

#define _XTAL_FREQ 4000000

#define MOTOR_DIR PORTDbits.RD7
#define MOTOR_ESQ PORTDbits.RD6
#define SIRENE PORTDbits.RD0
#define CANCELA_ABRINDO PORTDbits.RD1
#define CANCELA_FECHANDO PORTDbits.RD2
#define SENSOR_TREM PORTBbits.RB0
#define SENSOR_CANCELA_ABERTA PORTBbits.RB1
#define SENSOR_CANCELA_FECHADA PORTBbits.RB2
#define SENSOR_CARRO PORTBbits.RB3

void __interrupt() trem(void);
void abrirCancela(void);
void fecharCancela(void);
void luzesPiscando(void);

int flag = 0;

void main(void) {
    TRISB = 0xFF;
    TRISD = 0;
    OPTION_REG = 0b01111111;
    OPTION_REGbits.INTEDG = 1;
    INTCONbits.GIE = 1;
    INTCONbits.INTE = 1;
    CANCELA_ABRINDO = 0;
    CANCELA_FECHANDO = 0;
    SIRENE = 0;
    MOTOR_DIR = 0;
    MOTOR_ESQ = 0;

    while (1) {
        if (!SENSOR_CARRO) {
            abrirCancela();
        }

        if (flag) {
            CANCELA_ABRINDO = 0;
            CANCELA_FECHANDO = 0;
            SIRENE = 0;
            MOTOR_DIR = 0;
            MOTOR_ESQ = 0;
        }

        flag = 0;
    }
}

void __interrupt() trem(void) {
    if (INTF) {
        flag = 1;

        if (SENSOR_CANCELA_FECHADA && !SENSOR_CANCELA_ABERTA) {
            for (int i = 0; i < 10; i++) {
                luzesPiscando();
            }

            MOTOR_ESQ = 1;
            MOTOR_DIR = 0;
            CANCELA_FECHANDO = 1;
            CANCELA_ABRINDO = 0;
        } else if (SENSOR_CANCELA_FECHADA && SENSOR_CANCELA_ABERTA) {
            MOTOR_ESQ = 1;
            MOTOR_DIR = 0;
            CANCELA_FECHANDO = 1;
            CANCELA_ABRINDO = 0;
        }

        while (1) {
            if (!SENSOR_CANCELA_FECHADA) {
                MOTOR_DIR = 0;
                MOTOR_ESQ = 0;
                CANCELA_FECHANDO = 0;
                CANCELA_ABRINDO = 0;

                for (int i = 0; i < 10; i++) {
                    luzesPiscando();
                }

                break;
            }

            luzesPiscando();
        }

        INTF = 0;
    }

    return;
}

void abrirCancela(void) {
    CANCELA_FECHANDO = 0;
    CANCELA_ABRINDO = 1;
    MOTOR_DIR = 1;
    MOTOR_ESQ = 0;

    while (!flag) {
        if (!SENSOR_CANCELA_ABERTA) {
            MOTOR_DIR = 0;
            CANCELA_ABRINDO = 0;
            MOTOR_ESQ = 0;
            CANCELA_FECHANDO = 0;
            if (SENSOR_CARRO) {
                __delay_ms(8000);
                fecharCancela();
            }
            break;
        }
    }

    return;

}

void fecharCancela(void) {
    CANCELA_ABRINDO = 0;
    CANCELA_FECHANDO = 1;
    MOTOR_ESQ = 1;
    MOTOR_DIR = 0;

    while (!flag) {
        if (!SENSOR_CANCELA_FECHADA) {
            MOTOR_DIR = 0;
            CANCELA_ABRINDO = 0;
            MOTOR_ESQ = 0;
            CANCELA_FECHANDO = 0;

            break;
        }
    }

    return;
}

void luzesPiscando(void) {
    CANCELA_ABRINDO = 1;
    CANCELA_FECHANDO = 0;
    SIRENE = 1;
    __delay_ms(500);

    CANCELA_ABRINDO = 0;
    CANCELA_FECHANDO = 1;
    SIRENE = 0;
    __delay_ms(500);

    return;
}