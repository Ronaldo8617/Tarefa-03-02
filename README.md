# Tarefa 03/02

Este projeto foi desenvolvido utilizando  a placa de desenvolvimento BitDogLab, e envolve o controle de LEDs, exibição de mensagens em um display OLED SSD1306, e controle de uma matriz de LEDs. O sistema também responde a entradas de botões e interações via comunicação serial.

## Funcionalidades
- **Uso de Interrupções**: Todas as funcionalidades relacionadas aos botões são implementadas utilizando rotinas de interrupção, garantindo um desempenho eficiente e evitando polling contínuo.
- **Debouncing via Software**: Debouncing via Software: Implementado para evitar leituras errôneas dos botões devido ao bouncing mecânico.
- **Controle de LEDs**: O projeto controla LEDs (Verde e azul) com base nas entradas dos botões.
- **Exibição em Display SSD1306**: Exibe mensagens, números e letras em um display OLED SSD1306.
- **Matriz de LEDs 5x5**: Exibe números de 0 a 9 em uma matriz de LEDs 5x5.
- **Leitura de caracteres via Serial**: Recebe caracteres via comunicação serial e exibe informações relacionadas (letra ou número) no display OLED e na matriz de LEDs.

## Como Usar 
1. **Interação com o Sistema**:
   - Pressione o botão A para alternar o LED verde e exibir uma mensagem no display OLED.
   - Pressione o botão B para alternar o LED azul e exibir uma mensagem no display OLED.
   - Envie caracteres via comunicação serial. O sistema responderá de acordo com o caractere:
     - Se for uma letra, o display exibirá "Letra: [caractere]".
     - Se for um número, o display exibirá "Número: [número]" e a matriz de LEDs exibirá o número correspondente.

## Bibliotecas Usadas
- `ssd1306`: Biblioteca para controle do display OLED SSD1306.
- `pio_matrix`: Programa PIO para controlar a matriz de LEDs.
- `hardware/i2c`: Biblioteca para comunicação I2C no Raspberry Pi Pico.
- `hardware/pio`: Biblioteca para controle de PIOs.
- `hardware/clocks`: Biblioteca para controle de clocks.

## Como Compilar e Carregar o Código
1. Clone o repositório.
2. Compile o código e faça o upload para a placa de desenvolvimento BitDogLab.
3. Conecte a placa de desenvolvimento BitDogLab a uma porta USB para comunicação serial.

 **Vídeo Demonstrativo**:
Vídeo de Demonstração
https://youtu.be/MbdaigGABFg 


