#define F_CPU 16000000
#include <xc.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdlib.h>
#include <avr/interrupt.h>

#define tam_string 100
char mensagem_enviada[tam_string] = "";

volatile uint16_t contagem_pulsos = 0;
uint8_t duty = 0;
uint8_t canal_pwm_ativo = 0; // 1 = PWM A, 2 = PWM B

// ----------- UART -----------

void enviar_mensagem(const char *mensagem_enviada) {
	for (int i = 0; i < tam_string; i++) {
		while ((UCSR0A & (1 << UDRE0)) == 0);
		if (mensagem_enviada[i] == 0) break;
		UDR0 = mensagem_enviada[i];
	}
	_delay_ms(10);
	UDR0 = '\n';
}

void receber_mensagem() {
	char buffer[tam_string] = "";
	uint8_t i = 0;
	char caractere;

	while (1) {
		while (!(UCSR0A & (1 << RXC0)));

		caractere = UDR0;

		if (caractere == '\n' || caractere == '\r') {
			if (i == 0) continue;
			buffer[i] = '\0';
			break;
		}

		if (i < tam_string - 1) {
			buffer[i++] = caractere;
		}
	}

	enviar_mensagem("Mensagem recebida:");
	enviar_mensagem(buffer);
}

// ----------- ENCODER e RPM -----------

ISR(INT0_vect) {
	contagem_pulsos++;
}

ISR(TIMER1_COMPA_vect) {
	char buffer[32];
	uint16_t rpm = contagem_pulsos * 60;
	contagem_pulsos = 0;

	sprintf(buffer, "RPM: %u", rpm);
	enviar_mensagem(buffer);
}

void configurar_encoder() {
	DDRD &= ~(1 << PD4);    // PD4 como entrada
	PORTD |= (1 << PD4);    // Pull-up ativado

	EICRA |= (1 << ISC01);  // Interrupção na borda de descida
	EICRA &= ~(1 << ISC00);
	EIMSK |= (1 << INT0);   // Habilita INT0
}

void configurar_timer1_para_1s() {
	TCCR1A = 0;
	TCCR1B = (1 << WGM12) | (1 << CS12) | (1 << CS10); // Modo CTC, Prescaler 1024
	OCR1A = 15624; // (16MHz / 1024) * 1s - 1
	TIMSK1 |= (1 << OCIE1A); // Interrupção por comparação
}

// ----------- PWM -----------

void ativar_pwmA() {
	TCCR0A |= (1 << COM0A1);
	TCCR0A &= ~(1 << COM0B1);
	PORTD &= ~(1 << PD5); // Garante 0V no outro canal
	canal_pwm_ativo = 1;
}

void ativar_pwmB() {
	TCCR0A |= (1 << COM0B1);
	TCCR0A &= ~(1 << COM0A1);
	PORTD &= ~(1 << PD6); // Garante 0V no outro canal
	canal_pwm_ativo = 2;
}

void set_dutyA(uint8_t porcentagem) {
	if (porcentagem > 100) porcentagem = 100;
	OCR0A = (porcentagem * 255) / 100;
}

void set_dutyB(uint8_t porcentagem) {
	if (porcentagem > 100) porcentagem = 100;
	OCR0B = (porcentagem * 255) / 100;
}

int main(void) {
	// PWM pinos como saída
	DDRD |= (1 << PD6) | (1 << PD5);

	// UART
	UBRR0 = 103; // 9600 baud
	UCSR0A = 0;
	UCSR0B = (1 << RXEN0) | (1 << TXEN0);
	UCSR0C = 0b00000110;

	// PWM modo Fast PWM, Prescaler 64
	TCCR0A |= (1 << WGM01) | (1 << WGM00);
	TCCR0B |= (1 << CS01) | (1 << CS00);

	// Configurações
	configurar_encoder();
	configurar_timer1_para_1s();

	// Inicializa PWM
	ativar_pwmA();
	set_dutyA(50);
///teste raian
	sei();

	while (1) {
		receber_mensagem(); // Aguarda e exibe mensagens do usuário
	}
}
