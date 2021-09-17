void config_ports(void) {
    TRISB = 0b01111111;
    TRISD = 0;
    TRISC = 0b00001111;
    OPTION_REG = 0b01111111; // Pullup ligado e interrupt externo ligado
    OPTION_REGbits.INTEDG = 1;
    INTCONbits.GIE = 1; //Habiliita a int global
    INTCONbits.INTE=1;
    INTCONbits.PEIE = 1; //Habilita a int dos perif?ricos
    PIE1bits.TMR1IE = 1; //Habilita int do timer 1
}

void config_timer(void) {
    //*** configura o timer 1 *****************************************
    T1CONbits.TMR1CS = 0; //Define timer 1 como temporizador (Fosc/4)
    T1CONbits.T1CKPS0 = 1; //bit pra configurar pre-escaler, nesta caso 1:8
    T1CONbits.T1CKPS1 = 1; //bit pra configurar pre-escaler, nesta caso 1:8

    TMR1L = 0xDC; //carga do valor inicial no contador (65536-62500)
    TMR1H = 0x0B; //3036. Quando estourar contou 62500, passou 0,5s   

    OPTION_REGbits.PSA = 1;
    OPTION_REGbits.PS0 = 1;
    OPTION_REGbits.PS1 = 1; //Prescaler value divides 256
    OPTION_REGbits.PS2 = 1;
    
    
    TMR0 = 0; //TMR0 Initiation
    T0CS = 0; //Choosing to work with internal clk
    T0SE = 0; //Reacting on Low2High edge



}

void config_ac(void) {
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

    ADCON0bits.CHS0 = 0; //configura canal 0 como entrada anal?gica
    ADCON0bits.CHS1 = 0; //configura canal 0 como entrada anal?gica
    ADCON0bits.CHS2 = 0; //configura canal 0 como entrada anal?gica
}

void config() {
    config_ports();
    config_ac();
    config_timer();
}