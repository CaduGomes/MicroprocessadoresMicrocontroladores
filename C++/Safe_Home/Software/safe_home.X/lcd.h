void Lcd_Port(char a) {
    if (a & 1)
        LCD_4 = 1;
    else
        LCD_4 = 0;

    if (a & 2)
        LCD_5 = 1;
    else
        LCD_5 = 0;

    if (a & 4)
        LCD_6 = 1;
    else
        LCD_6 = 0;

    if (a & 8)
        LCD_7 = 1;
    else
        LCD_7 = 0;
}

void Lcd_Cmd(char a) {
    LCD_RS = 0; // => RS = 0
    Lcd_Port(a);
    LCD_E = 1; // => E = 1
    __delay_ms(4);
    LCD_E = 0; // => E = 0
}

void Lcd_Clear() {
    Lcd_Cmd(0);
    Lcd_Cmd(1);
}

void Lcd_Set_Cursor(char a, char b) {
    char temp, z, y;
    if (a == 1) {
        temp = 0x80 + b - 1;
        z = temp >> 4;
        y = temp & 0x0F;
        Lcd_Cmd(z);
        Lcd_Cmd(y);
    } else if (a == 2) {
        temp = 0xC0 + b - 1;
        z = temp >> 4;
        y = temp & 0x0F;
        Lcd_Cmd(z);
        Lcd_Cmd(y);
    }
}

void Lcd_Init() {
    Lcd_Port(0x00);
    __delay_ms(20);
    Lcd_Cmd(0x03);
    __delay_ms(5);
    Lcd_Cmd(0x03);
    __delay_ms(11);
    Lcd_Cmd(0x03);
    /////////////////////////////////////////////////////
    Lcd_Cmd(0x02);
    Lcd_Cmd(0x02);
    Lcd_Cmd(0x08);
    Lcd_Cmd(0x00);
    Lcd_Cmd(0x0C);
    Lcd_Cmd(0x00);
    Lcd_Cmd(0x06);
}

void Lcd_Write_Char(char a) {
    char temp, y;
    temp = a & 0x0F;
    y = a & 0xF0;
    LCD_RS = 1; // => RS = 1
    Lcd_Port(y >> 4); //Data transfer
    LCD_E = 1;
    __delay_us(40);
    LCD_E = 0;
    Lcd_Port(temp);
    LCD_E = 1;
    __delay_us(40);
    LCD_E = 0;
}

void Lcd_Write_String(char *a) {
    int i;
    for (i = 0; a[i] != '\0'; i++)
        Lcd_Write_Char(a[i]);
}

void Lcd_Shift_Right() {
    Lcd_Cmd(0x01);
    Lcd_Cmd(0x0C);
}

void Lcd_Shift_Left() {
    Lcd_Cmd(0x01);
    Lcd_Cmd(0x08);
}