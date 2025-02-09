#include <stdio.h>  // Biblioteca padrão para entrada e saída
#include <stdlib.h> // Biblioteca padrão para funções gerais
#include "pico/stdlib.h" // Biblioteca para o Raspberry Pi Pico
#include "hardware/i2c.h" // Biblioteca para comunicação I2C
#include "inc/ssd1306.h" // Biblioteca para o display SSD1306
#include "inc/font.h" // Biblioteca para fontes do display
#include "hardware/pio.h" // Biblioteca para Programmable I/O (PIO)
#include "hardware/clocks.h" // Biblioteca para controle de clocks
#include "pio_matrix.pio.h" // Biblioteca para controle da matriz de LEDs via PIO

// Definições de pinos
#define LED_RED_PIN 13 // Pino do LED vermelho
#define LED_GREEN_PIN 11 // Pino do LED verde
#define LED_BLUE_PIN 12 // Pino do LED azul
#define BUTTON_A_PIN 5 // Pino do botão A
#define BUTTON_B_PIN 6 // Pino do botão B
#define MATRIX_PIN 7 // Pino da matriz de LEDs
#define NUM_PIXELS 25 // Número de pixels na matriz de LEDs (5x5)
#define I2C_PORT i2c1 // Porta I2C utilizada
#define I2C_SDA 14 // Pino SDA do I2C
#define I2C_SCL 15 // Pino SCL do I2C
#define ENDERECO 0x3C // Endereço I2C do display SSD1306
#define WIDTH 128 // Largura do display SSD1306
#define HEIGHT 64 // Altura do display SSD1306

// Variáveis globais
volatile bool button_a_pressed = false; // Flag para indicar se o botão A foi pressionado
volatile bool button_b_pressed = false; // Flag para indicar se o botão B foi pressionado
absolute_time_t last_button_a_time; // Último tempo em que o botão A foi pressionado
absolute_time_t last_button_b_time; // Último tempo em que o botão B foi pressionado
ssd1306_t ssd; // Estrutura para o display SSD1306

// Função para converter valores RGB em um valor de cor de 32 bits
uint32_t rgb_color(double r, double g, double b) {
    unsigned char R = r * 255; // Converte o valor de vermelho para 8 bits
    unsigned char G = g * 255; // Converte o valor de verde para 8 bits
    unsigned char B = b * 255; // Converte o valor de azul para 8 bits
    return (G << 24) | (R << 16) | (B << 8); // Combina os valores em um único valor de 32 bits
}

// Função para exibir um número na matriz de LEDs
void display_number(uint8_t number, PIO pio, uint sm) {
    // Tabela de padrões para os números de 0 a 9
    const uint32_t timer_frames[10][25] = {
        {0, 1, 1, 1, 0, 0, 1, 0, 1, 0, 0, 1, 0, 1, 0, 0, 1, 0, 1, 0, 0, 1, 1, 1, 0}, // 0
        {0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 1, 0}, // 1
        {0, 1, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 1, 0}, // 2
        {0, 1, 1, 1, 0, 0, 0, 0, 1, 0, 0, 1, 1, 1, 0, 0, 0, 0, 1, 0, 0, 1, 1, 1, 0}, // 3
        {0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 1, 0, 0, 1, 0, 1, 0, 0, 1, 0, 1, 0}, // 4
        {0, 1, 1, 1, 0, 0, 0, 0, 1, 0, 0, 1, 1, 1, 0, 0, 1, 0, 0, 0, 0, 1, 1, 1, 0}, // 5
        {0, 1, 1, 1, 0, 0, 1, 0, 1, 0, 0, 1, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0}, // 6
        {0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 1, 0}, // 7
        {0, 1, 1, 1, 0, 0, 1, 0, 1, 0, 0, 1, 1, 1, 0, 0, 1, 0, 1, 0, 0, 1, 1, 1, 0}, // 8
        {0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 1, 0, 0, 1, 0, 1, 0, 0, 1, 1, 1, 0}  // 9
    };

    // Itera sobre os 25 LEDs da matriz
    for (int i = 0; i < NUM_PIXELS; i++) {
        // Define a cor do LED com base no padrão da tabela
        uint32_t color = (timer_frames[number][i] == 1) ? rgb_color(0.3, 0, 0) : rgb_color(0, 0, 0);
        // Envia o comando para o LED
        pio_sm_put_blocking(pio, sm, color);
    }
}

// Função de interrupção dos botões
void gpio_irq_handler(uint gpio, uint32_t events) {
    if (gpio == BUTTON_A_PIN) {
        // Verifica se o botão A foi pressionado com debounce
        absolute_time_t current_time = get_absolute_time();
        if (absolute_time_diff_us(last_button_a_time, current_time) >= 200000) {
            button_a_pressed = true; // Ativa a flag do botão A
            last_button_a_time = current_time; // Atualiza o tempo do último pressionamento
        }
    } else if (gpio == BUTTON_B_PIN) {
        // Verifica se o botão B foi pressionado com debounce
        absolute_time_t current_time = get_absolute_time();
        if (absolute_time_diff_us(last_button_b_time, current_time) >= 200000) {
            button_b_pressed = true; // Ativa a flag do botão B
            last_button_b_time = current_time; // Atualiza o tempo do último pressionamento
        }
    }
}

// Função para exibir uma mensagem no display SSD1306
void display_message(const char *message) {
    ssd1306_fill(&ssd, false); // Limpa o display
    ssd1306_draw_string(&ssd, message, 8, 10); // Desenha a mensagem no display
    ssd1306_send_data(&ssd); // Atualiza o display com a mensagem
}

// Função principal
int main() {
    stdio_init_all(); // Inicializa a comunicação serial

    // Configuração do I2C para o display SSD1306
    i2c_init(I2C_PORT, 400 * 1000); // Inicializa o I2C com frequência de 400 kHz
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C); // Configura o pino SDA como I2C
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C); // Configura o pino SCL como I2C
    gpio_pull_up(I2C_SDA); // Habilita pull-up no pino SDA
    gpio_pull_up(I2C_SCL); // Habilita pull-up no pino SCL
    ssd1306_init(&ssd, WIDTH, HEIGHT, false, ENDERECO, I2C_PORT); // Inicializa o display SSD1306
    ssd1306_config(&ssd); // Configura o display
    ssd1306_send_data(&ssd); // Envia os dados para o display
    ssd1306_fill(&ssd, false); // Limpa o display
    ssd1306_send_data(&ssd); // Atualiza o display

    // Configuração dos LEDs
    gpio_init(LED_RED_PIN); // Inicializa o pino do LED vermelho
    gpio_set_dir(LED_RED_PIN, GPIO_OUT); // Configura o pino como saída
    gpio_init(LED_BLUE_PIN); // Inicializa o pino do LED azul
    gpio_set_dir(LED_BLUE_PIN, GPIO_OUT); // Configura o pino como saída
    gpio_init(LED_GREEN_PIN); // Inicializa o pino do LED verde
    gpio_set_dir(LED_GREEN_PIN, GPIO_OUT); // Configura o pino como saída

    // Configuração dos botões
    gpio_init(BUTTON_A_PIN); // Inicializa o pino do botão A
    gpio_set_dir(BUTTON_A_PIN, GPIO_IN); // Configura o pino como entrada
    gpio_pull_up(BUTTON_A_PIN); // Habilita pull-up no pino do botão A
    gpio_init(BUTTON_B_PIN); // Inicializa o pino do botão B
    gpio_set_dir(BUTTON_B_PIN, GPIO_IN); // Configura o pino como entrada
    gpio_pull_up(BUTTON_B_PIN); // Habilita pull-up no pino do botão B

    // Configuração do PIO para a matriz de LEDs
    PIO pio = pio0; // Usa o PIO 0
    uint offset = pio_add_program(pio, &pio_matrix_program); // Adiciona o programa PIO
    uint sm = pio_claim_unused_sm(pio, true); // Obtém uma máquina de estado (SM) disponível
    pio_matrix_program_init(pio, sm, offset, MATRIX_PIN); // Inicializa o programa PIO

    // Configuração das interrupções dos botões
    gpio_set_irq_enabled_with_callback(BUTTON_A_PIN, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);
    gpio_set_irq_enabled_with_callback(BUTTON_B_PIN, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);

    bool cor = true; // Variável para alternar a cor do LED

    // Loop principal
    while (true) {
        cor = !cor; // Alterna a variável de cor

        // Processamento dos botões
        if (button_a_pressed) {
            button_a_pressed = false; // Reseta a flag do botão A
            gpio_put(LED_GREEN_PIN, !gpio_get(LED_GREEN_PIN)); // Alterna o estado do LED verde
            printf("LED VERDE ALTERNADO!\n"); // Exibe mensagem no terminal
            display_message("LED VERDE ALTERNADO!"); // Exibe mensagem no display
        }
        if (button_b_pressed) {
            button_b_pressed = false; // Reseta a flag do botão B
            gpio_put(LED_BLUE_PIN, !gpio_get(LED_BLUE_PIN)); // Alterna o estado do LED azul
            printf("LED AZUL ALTERNADO!\n"); // Exibe mensagem no terminal
            display_message("LED AZUL ALTERNADO!"); // Exibe mensagem no display
        }

        // Processamento da entrada serial sem bloquear
        if (stdio_usb_connected()) {
            int c = getchar_timeout_us(0); // Lê um caractere da UART sem bloquear
            if (c != PICO_ERROR_TIMEOUT) { // Verifica se um caractere foi recebido
                char msg[20];
                snprintf(msg, sizeof(msg), "RECEBIDO: %c", c); // Formata a mensagem
                printf("%s\n", msg); // Exibe no terminal
                display_message(msg); // Exibe no display

                // Tabela de lookup para mapear caracteres para ações
                if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z')) {
                    // Verifica se o caractere é uma letra maiúscula ou minúscula
                    char letra_msg[20];
                    snprintf(letra_msg, sizeof(letra_msg), "Letra: %c", c); // Formata a mensagem da letra
                    printf("%s\n", letra_msg); // Exibe no terminal
                    display_message(letra_msg); // Exibe no display
                }  
                if (c >= '0' && c <= '9') {
                    int num = c - '0';  // Converte char para número inteiro
                    char num_msg[20];
                    snprintf(num_msg, sizeof(num_msg), "Numero: %d", num); // Formata a mensagem do número
                    printf("%s\n", num_msg); // Exibe no terminal
                    display_message(num_msg);  // Exibe no display OLED
                    display_number(num, pio, sm);  // Envia para matriz de LED
                }
            }
        }

        sleep_ms(10); // Aguarda 10 ms antes de continuar o loop
    }
}