#define F_CPU 16000000
#include <xc.h>
#include <util/delay.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define tam_string 100
char mensagem_enviada[tam_string] = "";

uint8_t duty = 0;
uint8_t canal_pwm_ativo = 0; // 1 = PWM A, 2 = PWM B

// ----------- UART -----------

void enviar_mensagem(const char *mensagem_enviada) {
	for (int i = 0; i < tam_string; i++) {
		while ((UCSR0A & (1 << UDRE0)) == 0) {}
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
		// Espera até receber um dado
		while (!(UCSR0A & (1 << RXC0)));

		caractere = UDR0;

		// Se for Enter ('\n' ou '\r'), encerra a string
		if (caractere == '\n' || caractere == '\r') {
			// Ignora enter duplicado (ex: \r\n)
			if (i == 0) continue; // ignora enter sem mensagem
			buffer[i] = '\0';
			break;
		}

		// Armazena no buffer se não ultrapassar o tamanho
		if (i < tam_string - 1) {
			buffer[i++] = caractere;
		}
	}

	enviar_mensagem("Mensagem recebida:");
	enviar_mensagem(buffer);
}

// ----------- PWM CONFIG -----------

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
	DDRD |= (1 << DDD6) | (1 << DDD5);

	// Configura PWM modo Fast PWM
	TCCR0A |= (1 << WGM01) | (1 << WGM00);
	TCCR0B |= (1 << CS01) | (1 << CS00); // Prescaler 64 (~976Hz)

	// Configura UART
	UBRR0 = 103; // 9600 baud
	UCSR0A = 0;
	UCSR0B = (1 << RXEN0) | (1 << TXEN0);
	UCSR0C = 0b00000110;


	// Inicializa PWM com canal A ativo por padrão e duty 0
	ativar_pwmA();
	set_dutyA(50);

	while (1) {
		receber_mensagem();
	}
}