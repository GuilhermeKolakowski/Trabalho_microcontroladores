#define F_CPU 16000000
#include <xc.h>
#include <util/delay.h>
#include <stdio.h>
#include <avr/interrupt.h>

#define tam_string 100   // Define o tamanho máximo de strings
char mensagem_enviada[tam_string] = ""; // Variavel de envio de mensagem via UART

volatile uint16_t contagem_pulsos = 0; // Variável que armazena a quantidade de voltas do motor por segundo

// ----------- UART -----------
void enviar_mensagem(const char *mensagem_enviada) {
	for (int i = 0; i < tam_string; i++) {
		while ((UCSR0A & (1 << UDRE0)) == 0); // Espera até o registrador estar pronto para enviar
		if (mensagem_enviada[i] == 0) break;  // Se chegou ao final da string, sai do loop
		UDR0 = mensagem_enviada[i];          // Envia o caractere atual
	}
	_delay_ms(10);      // Pequeno atraso para garantir transmissão
	UDR0 = '\n';        // Envia caractere de nova linha
}

// ----------- INTERRUPÇÃO ENCODER -----------
ISR(INT0_vect) {
	contagem_pulsos++; // Incrementa a contagem a cada borda de descida no PD2
}
// ----------- PWM -----------
void ativar_pwmA() {
	TCCR0A |= (1 << COM0A1);
	TCCR0A &= ~(1 << COM0B1);
	PORTD &= ~(1 << PD5); // Garante 0V no outro canal
}

void ativar_pwmB() {
	TCCR0A |= (1 << COM0B1);
	TCCR0A &= ~(1 << COM0A1);
	PORTD &= ~(1 << PD6); // Garante 0V no outro canal
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
	// Define PD5 (OC0B) e PD6 (OC0A) como saídas para PWM
	DDRD |= (1 << PD5) | (1 << PD6);

	UBRR0 = 103;                   // Configura baud rate para 9600 (com F_CPU = 16 MHz)
	UCSR0A = 0;
	UCSR0B = (1 << RXEN0) | (1 << TXEN0); // Habilita transmissão e recepção
	UCSR0C = 0b00000110;           // Configura modo assíncrono, 8 bits de dados, 1 stop bit, sem paridade

	TCCR0A |= (1 << WGM01) | (1 << WGM00); // Configura Timer0 para Fast PWM
	TCCR0B |= (1 << CS01) | (1 << CS00);   // Prescaler de 64

	DDRD &= ~(1 << PD2);     // Configura PD2 como entrada (pino INT0)
	PORTD |= (1 << PD2);     // Habilita resistor de pull-up interno

	EICRA |= (1 << ISC01);   // Configura para borda de descida
	EICRA &= ~(1 << ISC00);
	EIMSK |= (1 << INT0);    // Habilita interrupção INT0

	sei();                   // Habilita interrupções globais
	
	ativar_pwmB();
	set_dutyB(40);
	while (1) {
		contagem_pulsos = 0;          // Zera contador de pulsos
		_delay_ms(1000);              // Aguarda 1 segundo
		uint16_t rpm = contagem_pulsos * 60; // Calcula RPM (voltas por segundo * 60)

		char buffer[32];
		snprintf(buffer, sizeof(buffer), "RPM:%u", rpm); // Monta string com valor do RPM
		enviar_mensagem(buffer);       // Envia valor via UART
	}
}