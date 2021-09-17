/*
 * Author: Carlos Gomes
 */

#include <xc.h>
#include <pic16f877a.h>
#include <stdio.h>

//****** congiguration bits  **************************************************
#pragma config WDTE = ON   //Habilita o uso do WDT
#pragma config FOSC = HS   //define uso do clock externo EM 4 OU 20mHZ
#pragma config PWRTE = ON  //habilita reset ao ligar
#pragma config BOREN = ON  //Habilita o reset por Brown-out 

#define _XTAL_FREQ 4000000

#define ATIVAR_SH PORTBbits.RB3

#define DETECTOR_MOV PORTBbits.RB4
#define ALARME PORTDbits.RD0

#define TRANCA PORTDbits.RD1

#define LED PORTBbits.RB7

#define KEYPAD_1 PORTCbits.RC3
#define KEYPAD_2 PORTCbits.RC1
#define KEYPAD_3 PORTCbits.RC2
#define KEYPAD_A PORTCbits.RC4
#define KEYPAD_B PORTCbits.RC5
#define KEYPAD_C PORTCbits.RC6
#define KEYPAD_D PORTCbits.RC7

#define LCD_E PORTDbits.RD2
#define LCD_RS PORTDbits.RD3
#define LCD_4 PORTDbits.RD4
#define LCD_5 PORTDbits.RD5
#define LCD_6 PORTDbits.RD6
#define LCD_7 PORTDbits.RD7

#include "lcd.h"
#include "configs.h"

void teclado(void);

int mapToCelsius(double value) {
    return ( -55.0 + ((150.0 + 55.0) / (255.0 - 0.0)) * (value - 0.0));
}

void __interrupt() interruption();

void alarme(void);

int password_validator(void);

void termometro(void);

__bit SH_ativo = 0;
__bit Alarme_armado = 0;
__bit Alarme_ativo = 0;
int size = 0;
int password[4];

unsigned int contador0 = 0, contador1 = 0;
signed int valor = 0;

void main(void) {

    // Função que irá configurar as portas, conversos A/C e Timer
    config();

    Lcd_Init();
    Lcd_Set_Cursor(1, 1);
    Lcd_Write_String("Safe Home");
    Lcd_Set_Cursor(2, 1);
    Lcd_Write_String("desativado! ");

    ALARME = 0;
    LED = 1;
    while (1) {
        CLRWDT();
        if (!ATIVAR_SH) {
            SH_ativo = 1;
            Lcd_Clear();
            Lcd_Set_Cursor(1, 1);
            Lcd_Write_String("Safe Home");
            Lcd_Set_Cursor(2, 1);
            Lcd_Write_String("foi ativado! ");
            Lcd_Set_Cursor(1, 1);
            CLRWDT();
            __delay_ms(1000);
            while (SH_ativo) {
                alarme();
                termometro();
                CLRWDT();
            }
        }
    }
}

void alarme(void) {

    if (DETECTOR_MOV || !Alarme_armado)
        return;
    
    
    CLRWDT();
    if (!Alarme_ativo) {
        T1CONbits.TMR1ON = 1; //Liga o timer 1
        Alarme_ativo = 1;
    }
    
    LED = 0;
    
    CLRWDT();
    ALARME = 1;
    __delay_ms(400);
    LED = 1;
    ALARME = 0;

    CLRWDT();
    Lcd_Clear();
    Lcd_Set_Cursor(1, 1);
    Lcd_Write_String("Digite a senha:");
    Lcd_Set_Cursor(2, 1);
    while (1) {
        CLRWDT();
        if (size == 4) {
            int i = password_validator();
            if (i)
                break;
            else {
                Lcd_Clear();
                Lcd_Set_Cursor(1, 1);
                Lcd_Write_String("Digite a senha:");
                Lcd_Set_Cursor(2, 1);
            }

        }

        teclado();

    }
    CLRWDT();
    __delay_ms(300);

}

void termometro(void) {
    CLRWDT();
    char buffer[20];
    Lcd_Clear();
    Lcd_Set_Cursor(1, 1);
    Lcd_Write_String("Temp Atual:");
    Lcd_Set_Cursor(2, 1);
    
    ADCON0bits.GO = 1; //converte
    __delay_us(20);
    valor = ADRESH;
    sprintf(buffer, "%d", mapToCelsius(valor));
    Lcd_Write_String(buffer);
    CLRWDT();
    __delay_ms(500);
}

void __interrupt() interruption() {
    CLRWDT();
    if (INTF) {
        INTF = 0;
        if (!SH_ativo || Alarme_armado)
            return;
        LED = 0;
        ALARME = 1;
        Alarme_armado = 1;
        Lcd_Clear();
        Lcd_Set_Cursor(1, 1);
        Lcd_Write_String("Sistema");
        Lcd_Set_Cursor(2, 1);
        Lcd_Write_String("Armado");
        CLRWDT();
        __delay_ms(1500);
        LED = 1;
        ALARME = 0;
    }

    if (TMR1IF) //foi a interrup??o de estouro do timer1 que chamou a int?
    {
        PIR1bits.TMR1IF = 0; //reseta o flag da interrup??o
        TMR1L = 0xDC; //reinicia contagem com 3036
        TMR1H = 0x0B;

        //comandos pra tratar a interrup??o
        contador1++;
        if (contador1 == 20) {
            if(Alarme_ativo){
            TMR0IE = 1; // ativa  timer 0
            TRANCA = 1;
            }
            T1CONbits.TMR1ON = 0; //  desativa o timer1
            contador1 = 0;
        }
    }
    else if (TMR0IF) //foi a interrup??o de estouro do timer1 que chamou a int?
    {
        TMR0IF = 0;
        TMR0 = 0;

        //comandos pra tratar a interrup??o
        contador0++;
        if (contador0 == 1000) {
            contador0 = 0;
            ALARME = ~ALARME;
            LED = ~LED;
        }
    }

    return;
}

void teclado(void) {
    CLRWDT();
    KEYPAD_B = KEYPAD_C = KEYPAD_D = 0;
    KEYPAD_A = 1;
    if (KEYPAD_1 == 1) {
        password[size] = 1;
        size++;
        Lcd_Write_Char('1');
        __delay_ms(500);
    }
    if (KEYPAD_2 == 1) {
        password[size] = 2;
        size++;
        Lcd_Write_Char('2');
        __delay_ms(500);
    }
    if (KEYPAD_3 == 1) {
        password[size] = 3;
        size++;
        Lcd_Write_Char('3');
        __delay_ms(500);
    }
    // Segunda coluna
    KEYPAD_A = KEYPAD_C = KEYPAD_D = 0;
    KEYPAD_B = 1;
    if (KEYPAD_1 == 1) {
        password[size] = 4;
        size++;
        Lcd_Write_Char('4');
        __delay_ms(500);
    }
    if (KEYPAD_2 == 1) {
        password[size] = 5;
        size++;
        Lcd_Write_Char('5');
        __delay_ms(500);
    }
    if (KEYPAD_3 == 1) {
        password[size] = 6;
        size++;
        Lcd_Write_Char('6');
        __delay_ms(500);
    }
    // Terceira coluna
    KEYPAD_A = KEYPAD_B = KEYPAD_D = 0;
    KEYPAD_C = 1;
    if (KEYPAD_1 == 1) {
        password[size] = 7;
        size++;
        Lcd_Write_Char('7');
        __delay_ms(500);
    }
    if (KEYPAD_2 == 1) {
        password[size] = 8;
        size++;
        Lcd_Write_Char('8');
        __delay_ms(500);
    }
    if (KEYPAD_3 == 1) {
        password[size] = 9;
        size++;
        Lcd_Write_Char('9');
        __delay_ms(500);
    }
    // Quarta coluna
    KEYPAD_A = KEYPAD_B = KEYPAD_C = 0;
    KEYPAD_D = 1;
    if (KEYPAD_1 == 1) {
        password[size] = 10;
        size++;
        Lcd_Write_Char('*');
        __delay_ms(500);
    }
    if (KEYPAD_2 == 1) {
        password[size] = 0;
        size++;
        Lcd_Write_Char('0');
        __delay_ms(500);
    }
    if (KEYPAD_3 == 1) {
        password[size] = 12;
        size++;
        Lcd_Write_Char('#');
        __delay_ms(500);
    }
}

int password_validator(void) {
    CLRWDT();
    if (password[0] == 1 && password[1] == 2 && password[2] == 3 && password[3] == 4) {
        TMR0IE = 0;
        TRANCA = 0;
        LED = 1;
        ALARME = 0;
        Lcd_Clear();
        Lcd_Set_Cursor(1, 1);
        Lcd_Write_String("Senha Correta!");
        CLRWDT();
        __delay_ms(2000);
        Lcd_Clear();
        Lcd_Set_Cursor(1, 1);
        Lcd_Write_String("Sistema");
        Lcd_Set_Cursor(2, 1);
        Lcd_Write_String("Desarmado");
        CLRWDT();
        __delay_ms(2000);
        for (int i = 0; i < 4; i++) {
            password[i] = 0;
        }

        Alarme_armado = 0;
        size = 0;
        Alarme_ativo = 0;

        return 1;
    }
    for (int i = 0; i < 4; i++) {
        password[i] = 0;
    }

    size = 0;

    Lcd_Clear();
    Lcd_Set_Cursor(1, 1);
    Lcd_Write_String("Senha incorreta!");
    CLRWDT();
    __delay_ms(2000);
    return 0;

}