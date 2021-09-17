;*******************************************************************************
; UFSC- Universidade Federal de Santa Catarina
; Projeto: Carro de transporte - Exercício 5
; Autor: Carlos Eduardo Vitorino Gomes
; Matrícula: 20102086   
; Se apertar botão, o carro anda até a comporta, enche e volta a posição inicial
;
;*******************************************************************************

#include <P16F877A.INC>

  __CONFIG  _CP_OFF & _CPD_OFF & _DEBUG_OFF & _LVP_OFF & _WRT_OFF & _BODEN_OFF & _PWRTE_OFF & _WDT_OFF & _XT_OSC

;***** defini??o de Vari?veis  *****************************
;cria constantes para o programa

	cblock 0x20
	
		tempo0		
		tempo1
		tempo2
		number
				
	endc 



;******** Vetor de Reset do uC**************

 org 0x00		                ; Vetor de reset do uC.
 goto inicio		            ; Desvia para o inicio do programa.
 
 ;***************** criação de subrotinas******************************************
delay_ms:
 	movwf	tempo2		
conta:  	movlw	.255
	movwf	tempo1		; Carrega tempo1 (Unidade de ms).
	movlw	.255
	movwf	tempo0		; Carrega tempo0 (Para contar 1ms).
	nop					; Perde tempo.
	decfsz	tempo0,F	; Fim de tempo0 ?
	goto	$-2			; N?o - Volta duas instru??es.
						; Sim - Passou-se 1ms.
	decfsz	tempo1,F	; Fim de tempo1 ?
	goto	$-6			; N?o - Volta seis instru??es.
	
    decfsz tempo2, F
    goto   conta
				; Sim.
 return   			; Retorna.


;****************** Inicio do programa *****************************************

inicio:
	clrf	PORTA		; Inicializa os Port's. Coloca todas pinos em 0.
	clrf	PORTB
	clrf	PORTC
	clrf	PORTD
	clrf	PORTE

	banksel	TRISA
	movlw	0xff
	movwf	TRISB		; Configura PortB como entrada
	movlw	0x00
	movwf	TRISC		; Configura PortC como saida
	movlw	0x00
	movwf	TRISA		; Configura PortA como saida
	movlw	0x00
	movwf	TRISD		; Configura PortD como saida

	movlw	0x00
	movwf	OPTION_REG	; Configura Op??es:
				; Pull-Up habilitados.
				; Interrup??o na borda de subida do sinal no pino B0.
				; Timer0 incrementado pelo oscilador interno.
				; Prescaler WDT 1:1.
				; Prescaler Timer0 1:2.
	
	

	banksel PORTA		; Seleciona banco de mem?ria 0.

;*********************** Loop principal ****************************************
loop: 
    movf    PORTB,W
    btfss   PORTB,0
    goto    checkIncrementar
    movf    PORTB,W
    btfss   PORTB,1
    goto    checkDecrementar
    movf    PORTB,W
    btfss   PORTB,2
    goto    confirmNumber
    goto    loop	
     
incrementar:
    incf    PORTD
    movlw   .1
    call    delay_ms
    goto    loop   
    

decrementar: 
    decf    PORTD
    movlw   .1
    call    delay_ms
    goto    loop
    
checkIncrementar:
    goto    checkMax
    continueIncrement:
	btfss	PORTD,0
	goto	incrementar
	btfsc	PORTD,1
	goto	incrementar
	btfsc	PORTD,2
	goto	incrementar
	btfss	PORTD,3
	goto	incrementar 
	movlw	0x07
	addwf	PORTD
	movlw   .1
	call    delay_ms
	goto    loop   
   
checkDecrementar:
    goto    checkZero
    continueDecrement: 
	btfsc	PORTD,0
	goto	decrementar
	btfsc	PORTD,1
	goto	decrementar
	btfsc	PORTD,2
	goto	decrementar
	btfsc	PORTD,3
	goto	decrementar
	movlw	0x07
	subwf	PORTD
	movlw   .1
	call    delay_ms
	goto    loop   
    
checkZero:   
    movf    PORTD, W
    sublw   .0
    btfss   STATUS, 2
    goto    continueDecrement
     
    movlw   .1
    call    delay_ms
    goto    loop
     
checkMax:    
    movf    PORTD, W
    sublw   0x99
    btfss   STATUS, 2
    goto    continueIncrement
     
    movlw   .1
    call    delay_ms
    goto    loop
    
confirmNumber:
    movf    PORTD, W
    movwf   number
    clrf    PORTD
    goto    secondLoop
     
secondLoop:
    movf    PORTB,W
    btfss   PORTB,3
    goto    pulse
    goto    secondLoop	
  
pulse:
    btfss   PORTD,0
    goto    incrementPulse
    btfsc   PORTD,1
    goto    incrementPulse
    btfsc   PORTD,2
    goto    incrementPulse
    btfss   PORTD,3
    goto    incrementPulse 
    movlw   0x07
    addwf   PORTD
    movlw   .1
    call    delay_ms
    goto    checkNumber
    
    incrementPulse:
	incf    PORTD
	movlw   .1
	call    delay_ms
	goto    checkNumber
		
checkNumber:
    MOVF    number, W
    subwf   PORTD, 0
    btfss   STATUS, 2
    goto    secondLoop
    clrf    PORTD
    bsf	    PORTC,4
    movlw   .8
    call    delay_ms
    bcf	    PORTC,4
    goto    loop
     
    end	