#define F_CPU 16000000
#include <xc.h>
#include <util/delay.h>
#include <stdio.h>      // sprintf/snprintf
#include <string.h>     // strcmp, strlen etc
#include <stdlib.h>     //  atoi
#include <avr/interrupt.h>

#define tam_string 100
char mensagem_enviada[tam_string] = "";

volatile uint16_t contagem_pulsos = 0;   // Contador de pulsos do encoder
char mensagem_recebida[tam_string] = ""; // Armazena mensagem recebida via UART
uint8_t i_rx = 0;                        // Índice de leitura UART
uint8_t duty = 0;                        // Duty cycle atual do PWM

volatile uint8_t tempo_passou = 0;       // Variavel que indica que 1 segundo se passou

// ---------- Função para enviar mensagens via UART ----------
void enviar_mensagem(const char *mensagem_enviada) {
	for (int i = 0; i < tam_string; i++) {
		while ((UCSR0A & (1 << UDRE0)) == 0); // Espera o registrador estar pronto
		if (mensagem_enviada[i] == 0) break;  // Fim da string
		UDR0 = mensagem_enviada[i];          // Envia caractere
	}
	UDR0 = '\n'; // Envia nova linha após a mensagem
}

// ---------- Recepção UART ----------
void verificar_recepcao_serial() {
	if (UCSR0A & (1 << RXC0)) {   // Verifica se chegou dado
		char c = UDR0;            // Lê caractere recebido

		// Fim da mensagem
		if (c == '\n' || c == '\r') {
			mensagem_recebida[i_rx] = '\0'; // Finaliza string
			i_rx = 0;

			// Se a string for vazia, ignora
			if (mensagem_recebida[0] == '\0') {
				// Nada a fazer
			}
			// Comando: DIREITA
			else if (strcmp(mensagem_recebida, "DIREITA") == 0) {
				ativar_pwmA();         // Ativa canal A
				set_dutyA(duty);       // Aplica duty atual
				enviar_mensagem("PWM A ativado (DIREITA)");
			}
			// Comando: ESQUERDA
			else if (strcmp(mensagem_recebida, "ESQUERDA") == 0) {
				ativar_pwmB();         // Ativa canal B
				set_dutyB(duty);       // Aplica duty atual
				enviar_mensagem("PWM B ativado (ESQUERDA)");
			}
			// Comando: DUTY XXX
			else if (strncmp(mensagem_recebida, "DUTY", 4) == 0) {
				char *duty_str = &mensagem_recebida[4];

				while (*duty_str == ' ') duty_str++; // Ignora espaços após "DUTY"

				// Valida se a string contém apenas dígitos
				uint8_t valido = 1;
				for (uint8_t i = 0; duty_str[i] != '\0'; i++) {
					if (duty_str[i] < '0' || duty_str[i] > '9') {
						valido = 0;
						break;
					}
				}

				// Converte string para inteiro
				int novo_duty = atoi(duty_str);

				// Verifica se está no intervalo permitido
				if (valido && novo_duty >= 0 && novo_duty <= 100) {
					duty = novo_duty;
					set_dutyA(duty);
					set_dutyB(duty);

					char resposta[32];
					snprintf(resposta, sizeof(resposta), "Duty atualizado: %u%%", duty);
					enviar_mensagem(resposta);
					} else {
					enviar_mensagem("Duty invalido (0-100)");
				}
			}
			// Comando não reconhecido
			else {
				enviar_mensagem("Comando invalido");
			}
		}
		else {
			// Armazena caractere na string se ainda houver espaço
			if (i_rx < tam_string - 1) {
				mensagem_recebida[i_rx++] = c;
			}
		}
	}
}

// ---------- Interrupção pulso do encoder ----------
ISR(INT0_vect) {
	contagem_pulsos++; // Incrementa contador a cada pulso
}

// ---------- Interrupção: Timer1 a cada 1 segundo ----------
ISR(TIMER1_COMPA_vect) {
	tempo_passou = 1; // Marca que 1 segundo passou
}

// ---------- PWM ----------
void ativar_pwmA() {
	TCCR0A |= (1 << COM0A1);     // Ativa canal A
	TCCR0A &= ~(1 << COM0B1);    // Desativa canal B
	PORTD &= ~(1 << PD5);        // Garante PD5 em nível baixo
}

void ativar_pwmB() {
	TCCR0A |= (1 << COM0B1);     // Ativa canal B
	TCCR0A &= ~(1 << COM0A1);    // Desativa canal A
	PORTD &= ~(1 << PD6);        // Garante PD6 em nível baixo
}

void set_dutyA(uint8_t porcentagem) {
	if (porcentagem > 100) porcentagem = 100;
	OCR0A = (porcentagem * 255) / 100; // Converte porcentagem para valor PWM
}

void set_dutyB(uint8_t porcentagem) {
	if (porcentagem > 100) porcentagem = 100;
	OCR0B = (porcentagem * 255) / 100;
}

// ---------- Função principal ----------
int main(void) {
	DDRD |= (1 << PD5) | (1 << PD6); //PD5 e PD6 como saída

	UBRR0 = 103; //9600 baud
	UCSR0A = 0;
	UCSR0B = (1 << RXEN0) | (1 << TXEN0); // Habilita RX e TX
	UCSR0C = 0b00000110;                  // 8 bits, 1 stop, sem paridade

	
	TCCR0A |= (1 << WGM01) | (1 << WGM00); // Configura Timer0 para Fast PWM
	TCCR0B |= (1 << CS01) | (1 << CS00); // Prescaler 64

	
	DDRD &= ~(1 << PD2); // PD2 como entrada
	PORTD |= (1 << PD2); // Pull-up
	EICRA |= (1 << ISC01); // Borda de descida
	EICRA &= ~(1 << ISC00);
	EIMSK |= (1 << INT0);  // Habilita INT0

	// Configura Timer1 para interrupção a cada 1 segundo
	TCCR1B |= (1 << WGM12);              // CTC
	TCCR1B |= (1 << CS12) | (1 << CS10); // Prescaler 1024
	OCR1A = 15624;                       // 16MHz / 1024 = 15625 Hz ? 1s
	TIMSK1 |= (1 << OCIE1A);             // Habilita interrupção

	sei(); // Habilita interrupções globais

	// Configura PWM com duty inicial
	duty = 0;
	set_dutyA(duty);
	set_dutyB(duty);

	// Loop principal
	while (1) {
		// Verifica se 1 segundo se passou
		if (tempo_passou == 1) {
			tempo_passou = 0;

			uint16_t rpm = contagem_pulsos * 60; // RPM = voltas/s * 60
			contagem_pulsos = 0;

			char buffer[64];
			snprintf(buffer, sizeof(buffer), "RPM:%u | Duty:%u%%", rpm, duty);
			enviar_mensagem(buffer);
		}

		verificar_recepcao_serial(); // Verifica entrada UART
	}
}
