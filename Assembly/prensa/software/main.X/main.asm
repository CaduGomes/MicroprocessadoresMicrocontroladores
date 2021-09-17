;*******************************************************************************
; UFSC- Universidade Federal de Santa Catarina
; Projeto: Exerc?cios
; Autor: Roderval Marcelino
;
; Exercicio 02 - Prensa
; Programa Prensa, aciona com dois bot?es simultaneamente, o pist?o desce por 2s
; em seguida sobre por 2s. Um bot?o de emerg?ncia sobe o pist?o.
; 
; *******************************************************************************

#include <P16F877A.INC> 	; Arquivo de include do uC usado, no caso PIC16F877A



; Palavra de configura??o, desabilita e habilita algumas op??es internas
  __CONFIG  _CP_OFF & _CPD_OFF & _DEBUG_OFF & _LVP_OFF & _WRT_OFF & _BODEN_OFF & _PWRTE_OFF & _WDT_OFF & _XT_OSC


;***** defini??o de Vari?veis  *****************************
;cria constantes para o programa

  	cblock 0x20
	
		tempo0		
		tempo1			; Vari?veis usadas na rotina de delay.
		filtro
		temp			; Vari?vel tempor?ria.
		tempo2
				
	endc 
	
	cblock 0x70
		work_temp
		status_temp		; Vari?veis usadas para salvar contexto no momento das interrup??es.
		fsr_temp
		pclath_temp
		
	endc



;********* vari?veis de Entrada ************
#define	    BT_EMG	PORTB,0		; Est? ligada RB0 (Pino 33 do uC)
#define	    BT_1	PORTB,1		; Est? ligada RB1 (Pino 34 do uC)
#define	    BT_2	PORTB,2		; Est? ligada RB2 (Pino 35 do uC)


;********* vari?veis de Sa?da ***************
#define	    SOBE	PORTD,0	        ; Sa?da ligada RD0 (Pino 19 do uC) 
#define	    DESCE	PORTD,1	        ; Sa?da ligada RD1 (Pino 20 do uC)


;******** Vetor de Reset do uC**************
 org 0x00		                ; Vetor de reset do uC.
 goto inicio		            ; Desvia para o inicio do programa.

;******** Vetor de Interrup??es do uC **************
 org 0x04	                	; Vetor de interrup??es do uC.

salva_contexto:
	movwf	work_temp	        ; Salva o registrador de trabalho
	swapf	STATUS,W	        ; Salva o registrador de status
	movwf	status_temp
	movf	FSR,W
	movwf	fsr_temp	        ; Salva registrador FSR.
	movf	PCLATH,W
	movwf	pclath_temp	        ; Salva registrador PCLATH.
	clrf	PCLATH		        ; Seleciona p?gina de mem?ria 0.
	clrf	STATUS	        	; Seleciona banco de mem?ria 0.

	btfss	INTCON,INTF	        ; Foi interrup??o externa pino RB0?
	goto	sai_int		        ; N?o - Sai da interrup??o.


; Tratamento da interrup??o de mudan?a de estado no PortB


int_rb0:
	bcf	DESCE		; Desce desliga
	bsf	SOBE		; Sobe prensa

volta:    btfss BT_EMG  ; Continua apetardo INT? Ent?o fica aqui
	      goto volta	

sai_int_rb0:
	bcf INTCON,INTF		; Limpa flag da interrup??o de mudan?a de estado no PortB .

sai_int
	movf	pclath_temp,W
	movwf	PCLATH		; Recupera registrador PCLATH.
	movf	fsr_temp,W
	movwf	FSR		; Recupera registrador FSR.
	swapf	status_temp,W
	movwf	STATUS		; Recupera registrador STATUS.
	swapf	work_temp,F
	swapf	work_temp,W	; Recupera registrador Work.
 retfie


;*************** Rotinas e Sub-Rotinas *****************************************

; Rotina de delay 
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



trata_entrada:
		
		bsf	DESCE		; Desce prensa
		bcf	SOBE		; Sobe prensa
		movlw	.8
		call	delay_ms	; Delay de 250 ms. Tempo para Acionar prensa
		
		
		bcf	DESCE		; Desce desliga
		bsf	SOBE		; Sobe prensa
		
    return





;****************** Inicio do programa *****************************************

inicio:
	clrf	PORTA		; Inicializa os Port's. Coloca todas pinos em 0.
	clrf	PORTB
	clrf	PORTC
	clrf	PORTD
	clrf	PORTE

	banksel	TRISA		; Seleciona banco de mem?ria 1
	movlw	0x00
	movwf   TRISA		; Configura PortA todas com sa?da
	movlw	0x07
	movwf	TRISB		; Configura PortB como entrada (RB0, RB1, RB2)
	movlw	0x00
	movwf	TRISC		; Configura PortC todas com sa?da
	movlw	0x00
	movwf	TRISD		; Configura PortD todas com sa?da
	movlw	0x00
	movwf	TRISE		; Configura PortE todas com sa?da

	movlw	0x00
	movwf	OPTION_REG	; Configura Op??es:
				; Pull-Up habilitados.
				; Interrup??o na borda de subida do sinal no pino RB0.
				; Timer0 incrementado pelo oscilador interno.
				; Prescaler WDT 1:1.
				; Prescaler Timer0 1:2.

	movlw	0x90
	movwf	INTCON		; Habilita interrup??o RB0.

	movlw	0x00
	movwf	PIE1		; Desabilita interrup??es perif?ricas.

	movlw	0x00
	movwf	PIE2		; Desabilita interrup??es perif?ricas.

	movlw	0x07
	movwf	ADCON0		; Desliga conversor A/D, PortA e PortE com I/O digital.

	movlw	0x07
	movwf	CMCON		; Desliga comparadores internos.

	movlw	0x00
	movwf	CVRCON		; Desliga m?dulo de ref?rencia interna de tens?o.

	banksel PORTA		; Seleciona banco de mem?ria 0.


;*********************** Loop principal ****************************************
loop:
	btfsc	BT_1		    ; Bot?o "1" foi acionado?
	goto 	loop		    ; N?o - Volta uma instru??es.
	btfsc	BT_2		    ; Se sim, Bot?o "2" tamb?m foi acionado?
	goto	loop	    ; N?o - Volta tr?s instru??es.
	call	trata_entrada	    ; Se sim, trata a a??o

	goto 	loop		    ; Vai para loop. Deixa lendo as entradas 

    end			            ; Fim do Programa.