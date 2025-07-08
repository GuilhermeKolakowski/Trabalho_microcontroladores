#define F_CPU 16000000
#include <xc.h>
#include <util/delay.h>
#include <stdio.h>
#include <string.h>
#include <avr/interrupt.h>

#define tam_string 100
char mensagem_enviada[tam_string] = "";

volatile uint16_t contagem_pulsos = 0;
char mensagem_recebida[tam_string] = "";
uint8_t i_rx = 0;

volatile uint8_t tempo_passou = 0;  // Flag de 1 segundo

// ----------- UART -----------

void enviar_mensagem(const char *mensagem_enviada) {
	for (int i = 0; i < tam_string; i++) {
		while ((UCSR0A & (1 << UDRE0)) == 0);
		if (mensagem_enviada[i] == 0) break;
		UDR0 = mensagem_enviada[i];
	}
	UDR0 = '\n';
}

// ----------- RECEPÇÃO UART -----------

void verificar_recepcao_serial() {
	if (UCSR0A & (1 << RXC0)) {
		char c = UDR0;

		if (c == '\n' || c == '\r') {
			mensagem_recebida[i_rx] = '\0';
			i_rx = 0;

			if (mensagem_recebida[0] == '\0') {
				// ignora string vazia (gerada só pelo enter)
				} else if (strcmp(mensagem_recebida, "DIREITA") == 0) {
				ativar_pwmA();
				set_dutyA(40);
				enviar_mensagem("PWM A ativado - 40%");
				} else if (strcmp(mensagem_recebida, "ESQUERDA") == 0) {
				ativar_pwmB();
				set_dutyB(40);
				enviar_mensagem("PWM B ativado - 40%");
				} else {
				enviar_mensagem("Comando invalido");
			}
			} else {
			if (i_rx < tam_string - 1) {
				mensagem_recebida[i_rx++] = c;
			}
		}
	}
}

// ----------- INTERRUPÇÕES -----------

ISR(INT0_vect) {
	contagem_pulsos++;
}

// Timer1: Gera interrupção a cada 1 segundo
ISR(TIMER1_COMPA_vect) {
	tempo_passou = 1;
}

// ----------- PWM -----------

void ativar_pwmA() {
	TCCR0A |= (1 << COM0A1);
	TCCR0A &= ~(1 << COM0B1);
	PORTD &= ~(1 << PD5);
}

void ativar_pwmB() {
	TCCR0A |= (1 << COM0B1);
	TCCR0A &= ~(1 << COM0A1);
	PORTD &= ~(1 << PD6);
}

void set_dutyA(uint8_t porcentagem) {
	if (porcentagem > 100) porcentagem = 100;
	OCR0A = (porcentagem * 255) / 100;
}

void set_dutyB(uint8_t porcentagem) {
	if (porcentagem > 100) porcentagem = 100;
	OCR0B = (porcentagem * 255) / 100;
}

// ----------- MAIN -----------

int main(void) {
	// PWM pinos como saída
	DDRD |= (1 << PD5) | (1 << PD6);

	// UART
	UBRR0 = 103;
	UCSR0A = 0;
	UCSR0B = (1 << RXEN0) | (1 << TXEN0);
	UCSR0C = 0b00000110;

	// PWM Timer0 (Fast PWM)
	TCCR0A |= (1 << WGM01) | (1 << WGM00);
	TCCR0B |= (1 << CS01) | (1 << CS00); // Prescaler 64

	// INT0 para encoder
	DDRD &= ~(1 << PD2);
	PORTD |= (1 << PD2);
	EICRA |= (1 << ISC01);
	EICRA &= ~(1 << ISC00);
	EIMSK |= (1 << INT0);

	// Timer1 - CTC mode para gerar interrupção a cada 1 segundo
	TCCR1B |= (1 << WGM12);              // Modo CTC
	TCCR1B |= (1 << CS12) | (1 << CS10); // Prescaler 1024
	OCR1A = 15624;                       // (16MHz / 1024) * 1s - 1 = 15624
	TIMSK1 |= (1 << OCIE1A);             // Habilita interrupção por comparação

	sei(); // Habilita interrupções

	// Loop principal
	while (1) {
		if (tempo_passou) {
			tempo_passou = 0;

			uint16_t rpm = contagem_pulsos * 60;
			contagem_pulsos = 0;

			char buffer[32];
			snprintf(buffer, sizeof(buffer), "RPM:%u", rpm);
			enviar_mensagem(buffer);
		}

		verificar_recepcao_serial(); // Verifica UART
	}
}
