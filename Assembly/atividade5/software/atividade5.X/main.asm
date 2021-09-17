;*******************************************************************************
; UFSC- Universidade Federal de Santa Catarina
; Projeto: Carro de transporte - Exercício 5
; Autor: Carlos Eduardo Vitorino Gomes
; Matrícula: 20102086   
; Se apertar botão, o carro anda até a comporta, enche e volta a posição inicial
;
;*******************************************************************************

#include <P16F877A.INC> 	; Arquivo de include do uC usado, no caso PIC16F877A


;Palavra de configura??o, desabilita e habilita algumas op??es internas


  __CONFIG  _CP_OFF & _CPD_OFF & _DEBUG_OFF & _LVP_OFF & _WRT_OFF & _BODEN_OFF & _PWRTE_OFF & _WDT_OFF & _XT_OSC

;***** defini??o de Vari?veis  *****************************
;cria constantes para o programa

  	cblock 0x20
		tempo0		
		tempo1			; Vari?veis usadas na rotina de delay.
		tempo2			; Vari?veis usadas na rotina de delay.
	endc 




;******** Vetor de Reset do uC**************

 org 0x00		                ; Vetor de reset do uC.
 goto inicio		            ; Desvia para o inicio do programa.


;***************** cria??o de subrotinas******************************************
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
 return 
;****************** Inicio do programa *****************************************

inicio:
	clrf	PORTA		; Inicializa os Port's. Coloca todas pinos em 0.
	clrf	PORTB
	clrf	PORTC
	clrf	PORTD
	clrf	PORTE

	banksel	TRISA		; Seleciona banco de mem?ria 1
	movlw	0xff		; 1111 1111   1 = Input   0 = Output
	movwf	TRISB		; Configura PortB como entrada.
	movlw	0x00
	movwf	TRISD		; Configura PortD como sa?da.

	movlw	0x00
	movwf	OPTION_REG	; Configura Op??es:
				; Pull-Up habilitados.
				; Interrup??o na borda de subida do sinal no pino B0.
				; Timer0 incrementado pelo oscilador interno.
				; Prescaler WDT 1:1.
				; Prescaler Timer0 1:2.


	banksel PORTA		; Seleciona banco de mem?ria 0.



;*********************** Loop principal ****************************************
sensorA: 	  
    btfss   PORTB,1
    goto    loop
    goto    sensorA    

loop:
    btfss   PORTB,0
    goto    andar
    goto    loop    
     
andar:
    bsf	    PORTD,1
    btfss   PORTB,2
    goto    carregar
    goto    andar   
    
carregar:
    bcf	    PORTD,1
    bsf	    PORTD,0
    btfsc   PORTB,3
    goto    carregar
 
    movlw   .4
    call    delay_ms
    bcf	    PORTD,0
    goto    voltar
    
    
voltar:
    bsf	    PORTD,2
    btfsc   PORTB,1
    goto    voltar
    bcf	    PORTD,2
    goto    sensorA 
    end			    ; Fim do Programa.