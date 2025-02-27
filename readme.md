# Detector de Motor Ocioso com Display OLED

Este projeto é um **detector de motor ocioso** que utiliza um sensor analógico para detectar movimento. Se o motor permanecer parado por mais de 10 segundos, um buzzer será acionado para alertar o usuário. Além disso, um display **OLED SSD1306** exibe informações sobre o estado do motor, tempo ocioso e consumo estimado.

## 🎥 Demonstração
Assista ao vídeo de demonstração: [Vídeo - Projeto](https://youtu.be/Wm3yKWKy5mI)

## 🛠️ Componentes Utilizados
- **Raspberry Pi Pico**
- **Joystick analógico** (utilizado para simular a movimentação do motor)
- **Buzzer Piezoelétrico**
- **Botões para interação**
- **LEDs RGB** para indicar estados do motor
- **Display OLED SSD1306**
- **Resistores e jumpers**

## 📌 Funcionamento
1. O sistema monitora a movimentação do joystick através do **ADC**.
2. O **estado do motor** é atualizado com base nos valores lidos:
   - **Em movimento**: Valores fora dos limites predefinidos.
   - **Parado**: Dentro dos limites, mas por pouco tempo.
   - **Ocioso**: Parado por mais de **10 segundos**.
3. Os **LEDs** indicam os estados:
   - 🟢 **Verde**: Motor em movimento.
   - 🟡 **Amarelo**: Motor ligado, mas parado.
   - 🔴 **Vermelho**: Motor ocioso (parado por muito tempo).
4. O buzzer **emite um alerta sonoro** caso o motor esteja ocioso.
5. O display OLED exibe informações sobre **consumo de combustível**, **tempo ocioso** e **ID da viagem**.
6. O botão **A** liga/desliga o motor.
7. O botão **B** registra uma nova viagem e reinicia os contadores.

## 📜 Código Fonte
O código-fonte é escrito em **C** e utiliza a biblioteca **Pico SDK** para configurar os periféricos:
- **GPIO** para LEDs, botões e buzzer.
- **ADC** para leitura do joystick.
- **PWM** para controle do buzzer.
- **I2C** para comunicação com o display OLED.

## 🔧 Como Configurar e Rodar o Projeto
1. Instale o **Pico SDK** no seu ambiente de desenvolvimento.
2. Clone o repositório do projeto.
3. Compile o código usando **CMake**:
   ```sh
   mkdir build
   cd build
   cmake ..
   make
   ```
4. Envie o arquivo `.uf2` gerado para o Raspberry Pi Pico.
