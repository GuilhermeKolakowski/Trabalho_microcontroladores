#define F_CPU 16000000
#include <xc.h>
#include <util/delay.h>
<<<<<<< HEAD
#include <string.h>
#include <stdio.h>
<<<<<<< HEAD
#include <stdlib.h>

#define tam_string 100
char mensagem_enviada[tam_string] = "";

uint8_t duty = 0;
uint8_t canal_pwm_ativo = 0; // 1 = PWM A, 2 = PWM B
=======
=======
#include <stdio.h>
#include <stdlib.h>
#include <avr/interrupt.h>
>>>>>>> 978d6528f4a2da1332d857eccf35c37ad335f5b1

#define tam_string 100
char mensagem_enviada[tam_string] = "";

<<<<<<< HEAD
uint8_t menu_exibido = 0;
uint8_t estado_menu = 0;
uint8_t duty = 0;
>>>>>>> 5e40fbae7ac6e304a99ded7e912e7210879dfe8a
=======
volatile uint16_t contagem_pulsos = 0;
uint8_t duty = 0;
uint8_t canal_pwm_ativo = 0; // 1 = PWM A, 2 = PWM B
>>>>>>> 978d6528f4a2da1332d857eccf35c37ad335f5b1

// ----------- UART -----------

void enviar_mensagem(const char *mensagem_enviada) {
<<<<<<< HEAD
<<<<<<< HEAD
	for (int i = 0; i < tam_string; i++) {
		while ((UCSR0A & (1 << UDRE0)) == 0) {}
=======
	for (int i = 0; i < tam_string; i++) {
		while ((UCSR0A & (1 << UDRE0)) == 0);
>>>>>>> 978d6528f4a2da1332d857eccf35c37ad335f5b1
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
<<<<<<< HEAD
		// Espera at√© receber um dado
=======
>>>>>>> 978d6528f4a2da1332d857eccf35c37ad335f5b1
		while (!(UCSR0A & (1 << RXC0)));

		caractere = UDR0;

<<<<<<< HEAD
		// Se for Enter ('\n' ou '\r'), encerra a string
		if (caractere == '\n' || caractere == '\r') {
			// Ignora enter duplicado (ex: \r\n)
			if (i == 0) continue; // ignora enter sem mensagem
=======
		if (caractere == '\n' || caractere == '\r') {
			if (i == 0) continue;
>>>>>>> 978d6528f4a2da1332d857eccf35c37ad335f5b1
			buffer[i] = '\0';
			break;
		}

<<<<<<< HEAD
		// Armazena no buffer se n√£o ultrapassar o tamanho
=======
>>>>>>> 978d6528f4a2da1332d857eccf35c37ad335f5b1
		if (i < tam_string - 1) {
			buffer[i++] = caractere;
		}
	}

	enviar_mensagem("Mensagem recebida:");
	enviar_mensagem(buffer);
<<<<<<< HEAD
=======
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
>>>>>>> 5e40fbae7ac6e304a99ded7e912e7210879dfe8a
}

// ----------- PWM CONFIG -----------
void ativar_pwmA() {
<<<<<<< HEAD
=======
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

	EICRA |= (1 << ISC01);  // InterrupÁ„o na borda de descida
	EICRA &= ~(1 << ISC00);
	EIMSK |= (1 << INT0);   // Habilita INT0
}

void configurar_timer1_para_1s() {
	TCCR1A = 0;
	TCCR1B = (1 << WGM12) | (1 << CS12) | (1 << CS10); // Modo CTC, Prescaler 1024
	OCR1A = 15624; // (16MHz / 1024) * 1s - 1
	TIMSK1 |= (1 << OCIE1A); // InterrupÁ„o por comparaÁ„o
}

// ----------- PWM -----------

void ativar_pwmA() {
>>>>>>> 978d6528f4a2da1332d857eccf35c37ad335f5b1
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
=======
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
    if (porcentagem > 100) {
        enviar_mensagem("Erro: Percentual maior que 100%. Valor ajustado para 100%.");
        porcentagem = 100;
    } else if (porcentagem < 0) {
        enviar_mensagem("Erro: Percentual menor que 0%. Valor ajustado para 0%.");
        porcentagem = 0;
    }
    OCR0A = (porcentagem * 255) / 100;
}

void set_dutyB(uint8_t porcentagem) {
    if (porcentagem > 100) {
        enviar_mensagem("Erro: Percentual maior que 100%. Valor ajustado para 100%.");
        porcentagem = 100;
    } else if (porcentagem < 0) {
        enviar_mensagem("Erro: Percentual menor que 0%. Valor ajustado para 0%.");
        porcentagem = 0;
    }
    OCR0B = (porcentagem * 255) / 100;
>>>>>>> 5e40fbae7ac6e304a99ded7e912e7210879dfe8a
}

<<<<<<< HEAD
void exibir_menu() {
    enviar_mensagem("\n");
    enviar_mensagem("----- MENU -----");
    enviar_mensagem("1. Selecionar sentido (0 = A | 1 = B)");
    enviar_mensagem("2. Setar PWM (0 a 100)");
    enviar_mensagem("3. Exibir RPM e PWM atual");
    enviar_mensagem("Digite uma opcao:");
}

int main(void) {
<<<<<<< HEAD
	DDRD |= (1 << DDD6) | (1 << DDD5);

	// Configura PWM modo Fast PWM
	TCCR0A |= (1 << WGM01) | (1 << WGM00);
	TCCR0B |= (1 << CS01) | (1 << CS00); // Prescaler 64 (~976Hz)

	// Configura UART
	UBRR0 = 103; // 9600 baud
	UCSR0A = 0;
	UCSR0B = (1 << RXEN0) | (1 << TXEN0);
	UCSR0C = 0b00000110;
=======
    DDRD |= (1 << DDD6) | (1 << DDD5);

    TCCR0A |= (1 << WGM01) | (1 << WGM00);
    TCCR0B |= (1 << CS01) | (1 << CS00); // Prescaler 64 (976 Hz)

    UBRR0 = 103;
    UCSR0A = 0;
    UCSR0B = (1<<RXEN0) | (1<<TXEN0);
    UCSR0C = 0b00000110;
>>>>>>> 5e40fbae7ac6e304a99ded7e912e7210879dfe8a

    char recebido;
    char buffer[4] = {0}; // Buffer para armazenar at√© 3 d√≠gitos + terminador nulo
    uint8_t buffer_index = 0;

<<<<<<< HEAD
	// Inicializa PWM com canal A ativo por padr√£o e duty 0
	ativar_pwmA();
	set_dutyA(50);

	while (1) {
		receber_mensagem();
	}
=======
    while (1) {
        if (menu_exibido == 0) {
            exibir_menu();
            menu_exibido = 1;
        }

        recebido = receber_mensagem();

        if (recebido != '\0') {
            // Ignora Enter e CR
            if (recebido == '\n' || recebido == '\r') {
                continue;
            }
            if (recebido == '0') {
                estado_menu = 0;
                menu_exibido = 0;
                buffer_index = 0;
                memset(buffer, 0, sizeof(buffer));
                continue;
            }

            switch (estado_menu) {
                case 0: // Menu principal
                    if (recebido == '1') {
                        estado_menu = 1;
                        enviar_mensagem("\n");
                        enviar_mensagem("SELECIONE O SENTIDO DE GIRO DESEJADO:");
                        enviar_mensagem("1 - DIREITA");
                        enviar_mensagem("2 - ESQUERDA");
                        enviar_mensagem("Digite 0 para voltar");
                    } else if (recebido == '2') {
                        estado_menu = 2;
                        enviar_mensagem("\n");
                        enviar_mensagem("SELECIONE O PWM DESEJADO (0-100):");
                        enviar_mensagem("Digite 0 para voltar");
                    } else if (recebido == '3') {
                        estado_menu = 3;
                        enviar_mensagem("\n");
                        enviar_mensagem("PWM atual:");
                        char pwm_str[20];
                        snprintf(pwm_str, sizeof(pwm_str), "Duty: %d%%", duty);
                        enviar_mensagem(pwm_str);
                        enviar_mensagem("RPM atual: N/A"); // RPM n√£o implementado
                        enviar_mensagem("Digite 0 para voltar");
                    } else {
                        enviar_mensagem("\n");
                        enviar_mensagem("Erro: Comando invalido. Digite 1, 2, 3 ou 0 para voltar.");
                    }
                    break;

                case 1: // Sele√ß√£o de sentido
                    if (recebido == '1') {
                        ativar_pwmB();
                        enviar_mensagem("Direcao: DIREITA ativada.");
                        estado_menu = 0;
                        menu_exibido = 0;
                    } else if (recebido == '2') {
                        ativar_pwmA();
                        enviar_mensagem("Direcao: ESQUERDA ativada.");
                        estado_menu = 0;
                        menu_exibido = 0;
                    } else {
                        enviar_mensagem("Erro: Comando invalido. Digite 1, 2 ou 0 para voltar.");
                    }
                    break;

                case 2: // Configura√ß√£o de PWM
                    if (recebido >= '0' && recebido <= '9' && buffer_index < 3) {
                        buffer[buffer_index++] = recebido;
                    } else if (recebido == '\n' || recebido == '\r') {
                        if (buffer_index > 0) {
                            buffer[buffer_index] = '\0';
                            int valor = atoi(buffer);
                            if (valor >= 0 && valor <= 100) {
                                duty = valor;
                                set_dutyA(duty);
                                set_dutyB(duty);
                                char pwm_msg[30];
                                snprintf(pwm_msg, sizeof(pwm_msg), "PWM ajustado para %d%%", duty);
                                enviar_mensagem(pwm_msg);
                                estado_menu = 0;
                                menu_exibido = 0;
                            } else {
                                if (valor > 100) {
                                    enviar_mensagem("Erro: Percentual maior que 100%. Digite um valor entre 0 e 100.");
                                } else if (valor < 0) {
                                    enviar_mensagem("Erro: Percentual menor que 0%. Digite um valor entre 0 e 100.");
                                } else {
                                    enviar_mensagem("Erro: Valor invalido. Digite um numero entre 0 e 100.");
                                }
                            }
                            buffer_index = 0;
                            memset(buffer, 0, sizeof(buffer));
                        }
                    } else {
                        enviar_mensagem("Erro: Parametro invalido. Digite um numero entre 0 e 100 ou 0 para voltar.");
                        buffer_index = 0;
                        memset(buffer, 0, sizeof(buffer));
                    }
                    break;

                case 3: // Exibir RPM e PWM
                    enviar_mensagem("Erro: Comando invalido. Digite 0 para voltar.");
                    break;

                default:
                    enviar_mensagem("Erro: Estado de menu desconhecido. Reiniciando...");
                    estado_menu = 0;
                    menu_exibido = 0;
                    break;
            }
        }
    }
>>>>>>> 5e40fbae7ac6e304a99ded7e912e7210879dfe8a
}
=======
int main(void) {
	// PWM pinos como saÌda
	DDRD |= (1 << PD6) | (1 << PD5);

	// UART
	UBRR0 = 103; // 9600 baud
	UCSR0A = 0;
	UCSR0B = (1 << RXEN0) | (1 << TXEN0);
	UCSR0C = 0b00000110;

	// PWM modo Fast PWM, Prescaler 64
	TCCR0A |= (1 << WGM01) | (1 << WGM00);
	TCCR0B |= (1 << CS01) | (1 << CS00);

	// ConfiguraÁıes
	configurar_encoder();
	configurar_timer1_para_1s();

	// Inicializa PWM
	ativar_pwmA();
	set_dutyA(50);

	sei();

	while (1) {
		receber_mensagem(); // Aguarda e exibe mensagens do usu·rio
	}
}
>>>>>>> 978d6528f4a2da1332d857eccf35c37ad335f5b1
