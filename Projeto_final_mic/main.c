/*
 * main.c
 *
 * Created: 6/10/2025 1:29:09 PM
 *  Author: Guilherme
 */ 
#define F_CPU 16000000
#include <xc.h>
#include <util/delay.h>

#define tam_string 100
//exemplo2
char mensagem_enviada[tam_string]= "";
char mensagem_recebida[tam_string]= "";

uint8_t select_sentido = 1;

// ----------- UART -----------
void enviar_mensagem(const char *mensagem_enviada) {
	for (int i = 0; i < tam_string; i++) {
		while ((UCSR0A & (1 << UDRE0)) == 0) {
			// Espera at� o buffer estar pronto para envio
		}

		if (mensagem_enviada[i] == 0) {
			break; // Fim da string
		}
		UDR0 = mensagem_enviada[i];
	}
	_delay_ms(10);
	UDR0 = '\n';
}
void receber_mensagem(){
	if((UCSR0A & (1<<RXC0)) != 0){
		//Verifica se tem algo para receber
		char byte_recebido = UDR0;
		if(byte_recebido == '1'){
			select_sentido = 1;
		}else if(byte_recebido == '0'){
			select_sentido = 0;
		}
	}
}

// ----------- PWM CONFIG -----------

void ativar_pwmA() {
	TCCR0A |=  (1 << COM0A1); // Ativa OC0A (PD6)
	TCCR0A &= ~(1 << COM0B1); // Desativa OC0B (PD5)
	PORTD &= ~(1 << PD5);     // Garante PD5 em 0V
}

void ativar_pwmB() {
	TCCR0A |=  (1 << COM0B1); // Ativa OC0B (PD5)
	TCCR0A &= ~(1 << COM0A1); // Desativa OC0A (PD6)
	PORTD &= ~(1 << PD6);     // Garante PD6 em 0V
}

void set_dutyA(uint8_t porcentagem) {
	if (porcentagem > 100) {
		porcentagem = 100;
	}
	OCR0A = (porcentagem * 255) / 100;
}

void set_dutyB(uint8_t porcentagem) {
	if (porcentagem > 100) {
		porcentagem = 100;
	}
	OCR0B = (porcentagem * 255) / 100;
}


int main(void) {

	// PD5 e PD6 como sa�da
	DDRD |= (1 << DDD6) | (1 << DDD5);

	// PWM Timer0 - Fast PWM
	TCCR0A |= (1 << WGM01) | (1 << WGM00);
	TCCR0B |= (1 << CS01) | (1 << CS00); // Prescaler 64 (976 Hz)


	//Comunica��o serial
	UBRR0 = 103; //CONFIGURANDO BAUD RATE = 9600 pois u2x0 � 0
	UCSR0A = 0; // U2X0 = 0 (AFETA O BAUDRATE) se U2X0 = 1 muda a formula e pega metade da frequencia
	UCSR0B = (1<<RXEN0) | (1<<TXEN0); // 0b00011000
	UCSR0C = 0b00000110; //00 para setar assincrono. UPM1 E UMP0 desligados. USBS mexe no stopbit, final da informa��o longo ou curto. Setando UCsZ1 e 0 para 8bits. UCPOLmodo assincrono � 0
	
	while (1) {
		if (select_sentido == 0) {
			ativar_pwmA();
			set_dutyA(50);
		} else if(select_sentido == 1) {
			ativar_pwmB();
			set_dutyB(50);
		}else{
			//	Enviar uma mensagem de erro
			//enviar_mensagem("DIRECAO INVALIDA");
		}
		receber_mensagem();
	}
}