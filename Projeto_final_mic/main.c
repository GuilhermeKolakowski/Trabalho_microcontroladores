#define F_CPU 16000000
#include <xc.h>
#include <util/delay.h>
#include <string.h>
#include <stdio.h>

#define tam_string 100
char mensagem_enviada[tam_string]= "";
char mensagem_recebida[tam_string]= "";

uint8_t menu_exibido = 0;
uint8_t estado_menu = 0;
uint8_t duty = 0;
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

char receber_mensagem(){
	if ((UCSR0A & (1<<RXC0)) != 0) {
		return UDR0;
		} else {
		return '\0';
	}
}

// ----------- PWM CONFIG -----------

void ativar_pwmA() {
	TCCR0A |=  (1 << COM0A1);
	TCCR0A &= ~(1 << COM0B1);
	PORTD &= ~(1 << PD5);
}

void ativar_pwmB() {
	TCCR0A |=  (1 << COM0B1);
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

void exibir_menu() {
	enviar_mensagem("\n");
	enviar_mensagem("----- MENU -----");
	enviar_mensagem("1. Selecionar sentido (0 = A | 1 = B)");
	enviar_mensagem("2. Setar PWM (0 a 100)");
	enviar_mensagem("3. Exibir RPM e PWM atual");
	enviar_mensagem("Digite uma opcao:");
}

int main(void) {
	DDRD |= (1 << DDD6) | (1 << DDD5);

	TCCR0A |= (1 << WGM01) | (1 << WGM00);
	TCCR0B |= (1 << CS01) | (1 << CS00); // Prescaler 64 (976 Hz)

	UBRR0 = 103;
	UCSR0A = 0;
	UCSR0B = (1<<RXEN0) | (1<<TXEN0);
	UCSR0C = 0b00000110;

	char recebido;

	while (1) {
		if (menu_exibido == 0) {
			exibir_menu();
			menu_exibido = 1;
		}

		recebido = receber_mensagem();

		if (recebido != '\0') {
			// Ignora Enter
			if (recebido == '\n' || recebido == '\r') {
				continue;
			}
			if (recebido == '0') {
				estado_menu = 0;
				menu_exibido = 0;
				continue;
			}

			switch (estado_menu) {
				case 0:
				if (recebido == '1') {
					estado_menu = 1;
					enviar_mensagem("\n");
					enviar_mensagem("SLECIONE O SENTIDO DE GIRO DESEJADO:");
					enviar_mensagem("1 - DIREITA");
					enviar_mensagem("2 - ESQUERDA");
					enviar_mensagem("Digite 0 para voltar");
					} else if (recebido == '2') {
					estado_menu = 2;
					enviar_mensagem("\n");
					enviar_mensagem("SLECIONE O PWM DESEJADO ex(50%):");
					enviar_mensagem("Digite 0 para voltar");
					} else if (recebido == '3') {
					estado_menu = 3;
					enviar_mensagem("\n");
					enviar_mensagem("PWM atual:");
					enviar_mensagem("RPM atual:");
					enviar_mensagem("Digite 0 para voltar");
					} else {
					enviar_mensagem("\n");
					enviar_mensagem("Opcao invalida.");
				}
				break;

				case 1:
				if (recebido == '1') {
					ativar_pwmB();
					enviar_mensagem("Direcao: DIREITA ativada.");
					} else if (recebido == '2') {
					ativar_pwmA();
					enviar_mensagem("Direcao: ESQUERDA ativada.");
					} else {
					enviar_mensagem("Opcao invalida. Digite 1 ou 2 ou 0 para voltar.");
				}
				break;

				case 2:
				enviar_mensagem("\nDigite o valor do PWM (0 a 100):");
				_delay_ms(500);  // pequena pausa opcional

				char c1 = receber_mensagem();
				char c2 = receber_mensagem();
				char c3 = receber_mensagem();

				// Monta número (ex: '7''5' ? 75)
				int valor = 0;
				if (c1 >= '0' && c1 <= '9') {
					valor = (c1 - '0');

					if (c2 >= '0' && c2 <= '9') {
						valor = valor * 10 + (c2 - '0');

						if (c3 >= '0' && c3 <= '9') {
							valor = valor * 10 + (c3 - '0');
						}
					}
				}

				if (valor >= 0 && valor <= 100) {
					duty = valor;
					set_dutyA(duty);
					set_dutyB(duty);
					} else {
					enviar_mensagem("Valor invalido. Digite entre 0 e 100.");
				}
				break;

				case 3:
				break;
			}

		}
	}
}
