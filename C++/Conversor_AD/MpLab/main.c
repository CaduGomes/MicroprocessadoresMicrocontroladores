/*
 * File:   AD.c
 * Author: Matheus Jacks e Carlos Gomes
 *
 */

#include <xc.h>          //***inclus?o da biblioteca do compilador
#include <pic16f877a.h>  //***inclus?o da biblioteca do chip espec?fico
#include <stdio.h>       //***inclus?o da biblioteca standard padr?o "C"

//****** congiguration bits  **************************************************
#pragma config WDTE = OFF   //Desabilita o uso do WDT
#pragma config FOSC = HS     //define uso do clock externo EM 4 OU 20mHZ
//se usar o XT no m?ximo 4Mhz
#pragma config PWRTE = ON   //habilita reset ao ligar
#pragma config BOREN = ON   //Habilita o reset por Brown-out 

#define _XTAL_FREQ 4000000 //***Defini a frequencia do clock, 4Mhz neste caso

#define SEL_UNIDADE PORTBbits.RB2
#define SEL_DEZENA PORTBbits.RB3
#define SEL_CENTENA PORTBbits.RB4

int valor = 0; // var para guardar valor da convers?o
int celsius = 0;
int indice = 0;
int digits[] = {0, 0, 0};

unsigned char TABELA[] = {
    0xC0, // 0
    0xF9, // 1
    0xA4, // 2
    0xB0, // 3
    0x99, // 4
    0x92, // 5
    0x82, // 6
    0xF8, // 7
    0x80, // 8
    0x90, // 9
    0x3F // -
};

float mapToCelsius(float value) {
    return -55.0f + ((150.0f + 55.0f) / (255.0f - 0.0f)) * (value - 0.0f);
}

void separateDigits(int value, int result[]) {
    int num = value;

    for (int i = 2; i > -1; i--) {
        result[i] = num % 10;
        num /= 10;
    }
}

void __interrupt() TrataInt(void) {
    if (TMR1IF) //foi a interrup??o de estouro do timer1 que chamou a int?
    {
        PIR1bits.TMR1IF = 0; //reseta o flag da interrup??o
        TMR1L = 0x8F; //carga do valor inicial no contador (65536-625)=
        TMR1H = 0xFD; //64911. Quando estourar contou 625, 200hz

        indice++; // Incrementa indice

        switch (indice) // Sele??o de casos com indice
        {

            case 1: // Caso seja 1...
                SEL_DEZENA = 1; // Retira a sele??o do display anterior
                SEL_CENTENA = 1; // Retira a sele??o do display anterior
                SEL_UNIDADE = 0; // Retira a sele??o do display anterior

                PORTD = TABELA[digits[2] < 0 ? digits[2] * (-1) : digits[2]]; // Mostra valor da unidade
                break;


            case 2: // Caso seja 4...
                SEL_UNIDADE = 1; // Retira a sele??o do display anterior
                SEL_CENTENA = 1; // Retira a sele??o do display anterior
                SEL_DEZENA = 0; // Retira a sele??o do display anterior

                PORTD = TABELA[digits[1] < 0 ? digits[1] * (-1) : digits[1]]; //Mostra valor da dezena
                break;

            case 3: // Caso seja 4...
                indice = 0; // Reinicia indice
                SEL_UNIDADE = 1; // Retira a sele??o do display anterior
                SEL_DEZENA = 1; // Retira a sele??o do display anterior
                SEL_CENTENA = 0; // Retira a sele??o do display anterior

                if (digits[2] < 0 || digits[1] < 0)
                    PORTD = TABELA[10]; //Mostra valor da dezena
                else
                    PORTD = TABELA[digits[0]]; //Mostra valor da dezena
                
                break;
        }
    }
    return;
}


void main(void) {
    //**inicializa??es************************************************
    TRISB = 0b00100000; //configura pinos de entrada(1)e sa?da (0)
    TRISD = 0b00000000;
    INTCONbits.GIE = 1; //Habiliita a int global
    INTCONbits.PEIE = 1; //Habilita a int dos perif?ricos
    PIE1bits.TMR1IE = 1; //Habilita int do timer 1

    // configura o timer 1 **
    T1CONbits.TMR1ON = 0; //Define timer 1 como contador interno (Fosc/4), temporizador
    T1CONbits.T1OSCEN = 1; //Ativa oscilador interno
    T1CONbits.T1CKPS0 = 1; //bit pra configurar pre-escaler, nesta caso 1:8
    T1CONbits.T1CKPS1 = 1; //bit pra configurar pre-escaler, nesta caso 1:8

    TMR1L = 0x8F; //carga do valor inicial no contador (65536-625)=
    TMR1H = 0xFD; //64911. Quando estourar contou 625, 200hz
    //   Base de tempo para timer
    //   625 * 8us = 5ms = 200hz
    //   4mhz/4 =1Mhz/8(pre-escaler)= 125khz
    //   125khz = 8us per?odo *625 = 5000us = 200hz

    T1CONbits.TMR1ON = 1; //Liga o timer

    //*** configura o conversor anal?gico/digital*******************************
    //ficar? somente com AN0 como entrada anal?gica. Tem tabela no datasheet
    ADCON1bits.PCFG0 = 1; //configura as entradas anal?gicas
    ADCON1bits.PCFG1 = 1; //configura as entradas anal?gicas
    ADCON1bits.PCFG2 = 1; //configura as entradas anal?gicas
    ADCON1bits.PCFG3 = 1; //configura as entradas anal?gicas

    //define o clock de convers?o
    ADCON0bits.ADCS0 = 0; //confirmando default Fosc/2
    ADCON0bits.ADCS1 = 0; //confirmando default Fosc/2

    ADCON1bits.ADFM = 0; //Default ? 0. Pra mostra que pegar? os dados em 8 bits 
    //no ADRESH, pois est? justifica ? esquerda
    // Passando pra 1 pode pegar os dados em 10 bits 
    //usando os dois registradores ADRESHL justificado ?
    //direita

    //inicializa registradores do AD
    ADRESH = 0x00;
    ADCON0bits.ADON = 1; //Liga AD

    PORTD = TABELA[1];

    while (1) {
        ADCON0bits.CHS0 = 0; //configura canal 0 como entrada anal?gica
        ADCON0bits.CHS1 = 0; //configura canal 0 como entrada anal?gica
        ADCON0bits.CHS2 = 0; //configura canal 0 como entrada anal?gica

        ADCON0bits.GO = 1; //converte
        __delay_us(10); //tempo de convers?o
        valor = ADRESH; // passa valores convertido do reg para a vari?vel

        celsius = mapToCelsius(valor);

        separateDigits(celsius, digits);
    }
}