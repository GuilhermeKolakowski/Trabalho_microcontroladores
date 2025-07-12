#define F_CPU 16000000
#include <xc.h>                    
#include <util/delay.h>          
#include <stdio.h>                // Para sprintf, snprintf
#include <string.h>               // Para manipulação de strings (ex: strcmp)
#include <stdlib.h>               // Para conversão de string para inteiro (atoi)
#include <avr/interrupt.h>       

#define tam_string 100

char mensagem_enviada[tam_string] = "";        // Buffer para mensagens que serão enviadas pela UART
char mensagem_recebida[tam_string] = "";       // Buffer para armazenar mensagens recebidas pela UART

volatile uint16_t contagem_pulsos = 0;         // Contador de pulsos do encoder (para medir RPM)
uint8_t i_rx = 0;                              // Índice de recepção de caracteres UART
uint8_t duty = 0;                              // Armazena o duty cycle atual (0–100%)
volatile uint8_t tempo_passou = 0;             // Flag para sinalizar que 1 segundo se passou

// ---------- Enviar mensagem via UART ----------
void enviar_mensagem(const char *mensagem_enviada) {
	for (int i = 0; i < tam_string; i++) {
		while ((UCSR0A & (1 << UDRE0)) == 0); // Espera o registrador de envio estar pronto
		if (mensagem_enviada[i] == 0) break;  // Encerra se final da string
		UDR0 = mensagem_enviada[i];          // Envia caractere
	}
	UDR0 = '\n'; // Finaliza com quebra de linha
}

// ---------- Verificar e processar comandos UART ----------
void verificar_recepcao_serial() {
	if (UCSR0A & (1 << RXC0)) {       // Verifica se há caractere recebido
		char c = UDR0;                // Lê o caractere

		if (c == '\n' || c == '\r') { // Se for fim de linha, processa a string
			mensagem_recebida[i_rx] = '\0'; // Finaliza a string com caractere nulo
			i_rx = 0;                        // Reseta índice do buffer

			// Ignora comandos vazios
			if (mensagem_recebida[0] == '\0') {
			}
			// Comando para ativar PWM A (motor para direita)
			else if (strcmp(mensagem_recebida, "DIREITA") == 0) {
				ativar_pwmA();
				set_dutyA(duty);
				enviar_mensagem("PWM A ativado (DIREITA)");
			}
			// Comando para ativar PWM B (motor para esquerda)
			else if (strcmp(mensagem_recebida, "ESQUERDA") == 0) {
				ativar_pwmB();
				set_dutyB(duty);
				enviar_mensagem("PWM B ativado (ESQUERDA)");
			}
			// Comando para alterar o duty cycle
			else if (strncmp(mensagem_recebida, "DUTY", 4) == 0) {
				char *duty_str = &mensagem_recebida[4]; // Pega o que vem após "DUTY"
				while (*duty_str == ' ') duty_str++;    // Remove espaços extras

				uint8_t valido = 1; // Flag de verificação

				// Verifica se todos os caracteres são dígitos numéricos
				for (uint8_t i = 0; duty_str[i] != '\0'; i++) {
					if (duty_str[i] < '0' || duty_str[i] > '9') {
						valido = 0;
						break;
					}
				}

				int novo_duty = atoi(duty_str); // Converte string para inteiro

				// Verifica se duty está no intervalo permitido
				if (valido && novo_duty >= 0 && novo_duty <= 100) {
					duty = novo_duty;
					set_dutyA(duty);
					set_dutyB(duty);

					char resposta[32];
					snprintf(resposta, sizeof(resposta), "Duty atualizado: %u%%", duty);
					enviar_mensagem(resposta); // Confirma ao usuário
					} else {
					enviar_mensagem("Duty invalido (0-100)"); // Mensagem de erro
				}
			}
			// Qualquer outro comando é inválido
			else {
				enviar_mensagem("Comando invalido");
			}
		}
		else {
			// Se ainda está recebendo caracteres, armazena no buffer
			if (i_rx < tam_string - 1) {
				mensagem_recebida[i_rx++] = c;
			}
		}
	}
}

// ---------- Interrupção externa INT0 (encoder) ----------
ISR(INT0_vect) {
	contagem_pulsos++; // Incrementa a contagem de pulsos a cada transição detectada
}

// ---------- Interrupção do Timer1 (a cada 1 segundo) ----------
ISR(TIMER1_COMPA_vect) {
	tempo_passou = 1;
}

// ---------- Ativar canal PWM A (PD6) ----------
void ativar_pwmA() {
	TCCR0A |= (1 << COM0A1);     // Ativa saída PWM A (PD6)
	TCCR0A &= ~(1 << COM0B1);    // Desativa saída B
	PORTD &= ~(1 << PD5);        // Força PD5 em nível baixo (B desativado)
}

// ---------- Ativar canal PWM B (PD5) ----------
void ativar_pwmB() {
	TCCR0A |= (1 << COM0B1);     // Ativa saída PWM B (PD5)
	TCCR0A &= ~(1 << COM0A1);    // Desativa saída A
	PORTD &= ~(1 << PD6);        // Força PD6 em nível baixo (A desativado)
}

// ---------- Define duty cycle do canal A ----------
void set_dutyA(uint8_t porcentagem) {
	if (porcentagem > 100) porcentagem = 100;
	OCR0A = (porcentagem * 255) / 100; // Conversão percentual para 8 bits (0–255)
}

// ---------- Define duty cycle do canal B ----------
void set_dutyB(uint8_t porcentagem) {
	if (porcentagem > 100) porcentagem = 100;
	OCR0B = (porcentagem * 255) / 100;
}


int main(void) {
	// Configura os pinos PD5 e PD6 como saídas
	DDRD |= (1 << PD5) | (1 << PD6);

	UBRR0 = 103;                          // Baud rate 9600 para F_CPU = 16MHz
	UCSR0A = 0;
	UCSR0B = (1 << RXEN0) | (1 << TXEN0); // Habilita RX e TX
	UCSR0C = 0b00000110;                  // Formato: 8 bits, 1 stop, sem paridade

	TCCR0A |= (1 << WGM01) | (1 << WGM00);    // Modo Fast PWM
	TCCR0B |= (1 << CS01) | (1 << CS00);      // Prescaler 64

	DDRD &= ~(1 << PD2);        // PD2 como entrada
	PORTD |= (1 << PD2);        // Habilita pull-up
	EICRA |= (1 << ISC01);      // Interrupção na borda de descida
	EICRA &= ~(1 << ISC00);
	EIMSK |= (1 << INT0);       // Habilita interrupção externa INT0

	TCCR1B |= (1 << WGM12);                    // Modo CTC
	TCCR1B |= (1 << CS12) | (1 << CS10);       // Prescaler 1024
	OCR1A = 15624;                             // F_CPU / Prescaler = 16MHz / 1024 = 15625 por segundo
	TIMSK1 |= (1 << OCIE1A);                   // Habilita interrupção de comparação

	sei(); // Habilita interrupções globais

	duty = 0;
	set_dutyA(duty);
	set_dutyB(duty);

	while (1) {
		if (tempo_passou == 1) {
			tempo_passou = 0;

			uint16_t rpm = contagem_pulsos * 60; // Converte pulsos por segundo em RPM
			contagem_pulsos = 0;                 // Zera contador para próxima contagem

			char buffer[64];
			snprintf(buffer, sizeof(buffer), "RPM:%u | Duty:%u%%", rpm, duty);
			enviar_mensagem(buffer); // Envia dados via UART
		}

		// Checa se recebeu dados pela UART
		verificar_recepcao_serial();
	}
}
