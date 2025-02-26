#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/gpio.h"
#include "hardware/pwm.h"
#include "hardware/clocks.h"
#include "include/ssd1306.h"

// Definição de pinos
#define SENSOR_X 27        // Pino do sensor eixo X
#define SENSOR_Y 26        // Pino do sensor eixo Y
#define BUTTON_JOYSTICK 22 // Pino do botão do joystick

// Definição de pinos para o buzzer
#define BUZZER_PIN_A 10 // Pino do buzzer
#define BUZZER_PIN_B 21 // Pino da buzina

// Definição de pinos para os LEDs
#define BUTTON_A_PIN 5 // Pino do botão A
#define BUTTON_B_PIN 6 // Pino do botão B

// Definição de pinos para os LEDs
#define LED_R_PIN 13 // Pino do LED Vermelho
#define LED_G_PIN 11 // Pino do LED Verde
#define LED_B_PIN 12 // Pino do LED Azul

// Definição de pinos para o display OLED
#define ENDERECO_OLED 0x3C // Endereço do display OLED
#define I2C_PORT i2c1      // Porta I2C para comunicação com o display
#define I2C_SDA 14         // Pino SDA para I2C
#define I2C_SCL 15         // Pino SCL para I2C

// Tamanho do display OLED
#define WIDTH 128
#define HEIGHT 64

// Intervalo para considerar o motor parado
#define LIMIAR_MIN 1940
#define LIMIAR_MAX 2200

// Tempo para considerar o motor ocioso (10 segundos simulados)
#define TEMPO_OCIOSIDADE_MS 10000

// Tempo de debounce para os botões (em ms)
#define TEMPO_DEBOUNCE_MS 200

// Estrutura para o display SSD1306
ssd1306_t ssd;

// Enum para os estados do motor
typedef enum
{
    MOTOR_DESLIGADO,
    MOTOR_LIGADO_MOVIMENTO,
    MOTOR_LIGADO_PARADO,
    MOTOR_LIGADO_OCIOSO
} estado_motor_t;

// Variáveis de controle
volatile estado_motor_t estado_motor = MOTOR_DESLIGADO;

// Variáveis para controle de tempo
absolute_time_t tempo_inicio_parado;
volatile uint32_t last_interrupt_time = 0;

volatile bool buzina_ativada = false; // Variável para controlar a buzina

// Variável para controlar o tempo de movimento do motor (em ms)
uint32_t gasto_tempo = 0;

// Função para atualizar os LEDs com base no estado atual
void atualizar_leds()
{
    switch (estado_motor)
    {
    case MOTOR_LIGADO_MOVIMENTO:
        gpio_put(LED_R_PIN, 0); // Vermelho desligado
        gpio_put(LED_G_PIN, 1); // Verde ligado
        gpio_put(LED_B_PIN, 0); // Azul desligado
        break;
    case MOTOR_LIGADO_PARADO:
        gpio_put(LED_R_PIN, 1); // Vermelho ligado
        gpio_put(LED_G_PIN, 1); // Verde ligado (amarelo)
        gpio_put(LED_B_PIN, 0); // Azul desligado
        break;
    case MOTOR_LIGADO_OCIOSO:
        gpio_put(LED_R_PIN, 1); // Vermelho ligado
        gpio_put(LED_G_PIN, 0); // Verde desligado
        gpio_put(LED_B_PIN, 0); // Azul desligado
        break;
    case MOTOR_DESLIGADO:
    default:
        gpio_put(LED_R_PIN, 0); // Vermelho desligado
        gpio_put(LED_G_PIN, 0); // Verde desligado
        gpio_put(LED_B_PIN, 0); // Azul desligado
        break;
    }
}

void tocar_nota(uint pino_Buzzer, int frequencia, int duracao)
{
    if (frequencia > 0)
    {
        int slice_num = pwm_gpio_to_slice_num(pino_Buzzer);
        uint32_t freq_sistema = clock_get_hz(clk_sys);       // Frequência do sistema
        uint16_t wrap_valor = freq_sistema / frequencia - 1; // Define o valor de wrap

        pwm_set_wrap(slice_num, wrap_valor);
        pwm_set_gpio_level(pino_Buzzer, wrap_valor / 2); // Define duty cycle de 50%
        pwm_set_enabled(slice_num, true);                // Ativa o PWM

        sleep_ms(duracao);                 // Duração da nota
        pwm_set_enabled(slice_num, false); // Desativa o PWM
    }
    else
    {
        sleep_ms(duracao); // Pausa (nota silenciosa)
    }
}


void button_isr(uint gpio, uint32_t events)
{
    uint32_t current_time = to_ms_since_boot(get_absolute_time()); // Tempo em milissegundos

    if (current_time - last_interrupt_time > TEMPO_DEBOUNCE_MS)
    {
        last_interrupt_time = current_time; // Atualiza o tempo do último acionamento

        if (gpio == BUTTON_A_PIN)
        {
            if (estado_motor == MOTOR_DESLIGADO)
            {
                estado_motor = MOTOR_LIGADO_PARADO;
                tempo_inicio_parado = get_absolute_time(); // Inicia o contador de tempo parado
            }
            else
            {
                estado_motor = MOTOR_DESLIGADO;
            }
            atualizar_leds();
        }
        else if (gpio == BUTTON_B_PIN)
        {
            // Alterna o estado da buzina
            buzina_ativada = !buzina_ativada;
        }
    }
}

// Função para exibir o estado do motor no display
void exibir_estado_no_display()
{
    // Limpa o display
    ssd1306_fill(&ssd, false); // Preenche o display com branco (false)

    // Exibe o tempo de movimento do motor
    char tempo_movimento[20];
    sprintf(tempo_movimento, "Consumo:= %d L", gasto_tempo/1000);
    ssd1306_draw_string(&ssd, tempo_movimento, 0, 30);

    // Exibe o estado atual do motor
    switch (estado_motor)
    {
    case MOTOR_LIGADO_MOVIMENTO:
        ssd1306_draw_string(&ssd, "Em movimento", 0, 0);
        break;
    case MOTOR_LIGADO_PARADO:
        ssd1306_draw_string(&ssd, "Parado", 0, 0);
        break;
    case MOTOR_LIGADO_OCIOSO:
        ssd1306_draw_string(&ssd, "Ocioso", 0, 0);
        ssd1306_draw_string(&ssd, "DESLIGUE", 0, 10);
        break;
    case MOTOR_DESLIGADO:
    default:
        ssd1306_draw_string(&ssd, "Desligado", 0, 0);
        break;
    }

    // Envia os dados para o display
    ssd1306_send_data(&ssd);
}

// Função para inicializar o hardware
void init_hardware()
{
    // Inicialização da biblioteca stdio
    stdio_init_all();

    // Configuração dos GPIOs
    gpio_init(BUTTON_A_PIN);
    gpio_set_dir(BUTTON_A_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_A_PIN);
    gpio_set_irq_enabled_with_callback(BUTTON_A_PIN, GPIO_IRQ_EDGE_RISE, true, button_isr);

    gpio_init(BUTTON_B_PIN);
    gpio_set_dir(BUTTON_B_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_B_PIN);
    gpio_set_irq_enabled_with_callback(BUTTON_B_PIN, GPIO_IRQ_EDGE_RISE, true, button_isr);

    // Configuração do botão do joystick
    gpio_init(BUTTON_JOYSTICK);
    gpio_set_dir(BUTTON_JOYSTICK, GPIO_IN);
    gpio_pull_up(BUTTON_JOYSTICK);
    gpio_set_irq_enabled_with_callback(BUTTON_JOYSTICK, GPIO_IRQ_EDGE_RISE, true, button_isr);

    // Configuração dos pinos do buzzer
    gpio_init(BUZZER_PIN_A);
    gpio_set_function(BUZZER_PIN_A, GPIO_FUNC_PWM);
    gpio_init(BUZZER_PIN_B);
    gpio_set_function(BUZZER_PIN_B, GPIO_FUNC_PWM);

    // Inicialização dos LEDs
    gpio_init(LED_R_PIN);
    gpio_set_dir(LED_R_PIN, GPIO_OUT);
    gpio_init(LED_G_PIN);
    gpio_set_dir(LED_G_PIN, GPIO_OUT);
    gpio_init(LED_B_PIN);
    gpio_set_dir(LED_B_PIN, GPIO_OUT);
    atualizar_leds();

    // Inicialização do display SSD1306
    i2c_init(I2C_PORT, 400 * 1000); // Configura I2C com 400 kHz
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);
    ssd1306_init(&ssd, WIDTH, HEIGHT, false, ENDERECO_OLED, I2C_PORT);
    ssd1306_config(&ssd);      // Configura o display
    ssd1306_fill(&ssd, false); // Limpa o display inicialmente
    ssd1306_send_data(&ssd);   // Envia os dados para o display

    // Inicialização do ADC para o joystick
    adc_init();
    adc_gpio_init(SENSOR_X);
    adc_gpio_init(SENSOR_Y);
}

// Função principal
int main()
{
    init_hardware();

    while (1)
    {
        if (buzina_ativada)
        {
            tocar_nota(BUZZER_PIN_B, 300, 1000);
            buzina_ativada = false;
        }
        if (estado_motor != MOTOR_DESLIGADO)
        {
            adc_select_input(0);
            uint16_t leitura_x = adc_read();
            adc_select_input(1);
            uint16_t leitura_y = adc_read();

            bool dentro_limite_x = (leitura_x > LIMIAR_MIN && leitura_x < LIMIAR_MAX);
            bool dentro_limite_y = (leitura_y > LIMIAR_MIN && leitura_y < LIMIAR_MAX);

            if (dentro_limite_x && dentro_limite_y)
            {
                // Motor está parado
                if (estado_motor == MOTOR_LIGADO_MOVIMENTO)
                {
                    estado_motor = MOTOR_LIGADO_PARADO;
                    tempo_inicio_parado = get_absolute_time(); // Inicia o contador de tempo parado
                }
                else if (estado_motor == MOTOR_LIGADO_PARADO || estado_motor == MOTOR_LIGADO_OCIOSO)
                {
                    // Verifica se o motor está parado há mais de 10 segundos
                    if (absolute_time_diff_us(tempo_inicio_parado, get_absolute_time()) >= TEMPO_OCIOSIDADE_MS * 1000)
                    {
                        estado_motor = MOTOR_LIGADO_OCIOSO;
                        tocar_nota(BUZZER_PIN_A, 400, 150);
                        tocar_nota(BUZZER_PIN_A, 250, 150);
                    }
                }
            }
            else
            {
                // Motor está em movimento
                estado_motor = MOTOR_LIGADO_MOVIMENTO;
                gasto_tempo = gasto_tempo + 100;
            }
        }

        // Atualiza os LEDs e o display
        atualizar_leds();
        exibir_estado_no_display();

        sleep_ms(100);
    }
}