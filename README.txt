
Controle PWM Bidirecional com Leitura de RPM via UART
======================================================

Este projeto foi desenvolvido para o microcontrolador ATmega328P e permite controlar um motor em duas direções (esquerda e direita) com PWM, além de medir a rotação do motor em RPM com base em pulsos de um encoder.

A comunicação com o sistema é feita via monitor serial (UART), possibilitando o envio de comandos e o recebimento de informações em tempo real.

Funcionalidades
---------------
- Controle de rotação do motor com PWM (0% a 100%)
- Direção ajustável (DIREITA ou ESQUERDA)
- Cálculo de RPM a partir de pulsos no pino PD2 (INT0)
- Comunicação via UART (9600 bps)
- Envio de status automático a cada 1 segundo

Requisitos
----------
- Microcontrolador ATmega328P
- Comunicação UART ativa
- Encoder conectado ao pino PD2 (INT0)
- Ponte H conectada aos pinos PD5 (PWM B) e PD6 (PWM A)

Conexões
--------
| Componente         | Pino ATmega328P |
|--------------------|-----------------|
| Motor PWM Direita  | PD6 (OC0A)      |
| Motor PWM Esquerda | PD5 (OC0B)      |
| Encoder (pulso)    | PD2 (INT0)      |
| UART TX            | PD1 (TXD)       |
| UART RX            | PD0 (RXD)       |

Comunicação Serial
------------------
- Baud rate: 9600 bps
- Formato: 8 bits de dados, 1 stop bit, sem paridade

Comandos Disponíveis
--------------------

➤ DIREITA
Ativa o canal A (PD6) e aplica o duty cycle atual no motor para girar para a direita.

➤ ESQUERDA
Ativa o canal B (PD5) e aplica o duty cycle atual no motor para girar para a esquerda.


➤ DUTY XXX
Define o duty cycle do PWM, onde XXX é um valor de 0 a 100 (em porcentagem).

Exemplos:
DUTY 50    // Define o PWM como 50%
DUTY 100   // Define o PWM como 100%
DUTY 0     // Desliga o motor

Mensagens Enviadas pelo Sistema
-------------------------------
A cada 1 segundo, o sistema envia automaticamente ao monitor serial uma linha com o estado atual:

RPM:<valor> | Duty:<valor>%

Observações
-----------
- Ao iniciar, o PWM é iniciado com duty 0%, ou seja, o motor estará parado até que um valor seja definido com DUTY.

Exemplo de Uso
-----------------------
1. Enviar DUTY 60 → define o PWM em 60%
2. Enviar DIREITA → motor começa a girar para a direita com 60%
3. Observar mensagens como:
   RPM:135 | Duty:60%
4. Enviar ESQUERDA → motor muda o sentido mantendo o mesmo PWM
