# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.1.0)
set(toolchainVersion 13_3_Rel1)
set(picotoolVersion 2.1.0)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Inicializa o SDK do Pico
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()

# Defina a versão do SDK que você está utilizando
set(sdkVersion 2.1.0)
set(toolchainVersion 13_3_Rel1)
set(picotoolVersion 2.1.0)

# Caminho para o arquivo pico-vscode.cmake
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()

# Defina o tipo de placa (pico_w, por exemplo)
set(PICO_BOARD pico_w CACHE STRING "Board type")

# Puxa o SDK do Raspberry Pi Pico
include(pico_sdk_import.cmake)

# Defina o nome do projeto
project(Tarefa_03_02 C CXX ASM)

# Inicializa o SDK do Raspberry Pi Pico
pico_sdk_init()

# Adiciona o executável
add_executable(Tarefa_03_02 
    Tarefa_03_02.c 
    inc/ssd1306.c  # Se você tem esse arquivo
)

# Define o nome e a versão do programa
pico_set_program_name(Tarefa_03_02 "Tarefa_03_02")
pico_set_program_version(Tarefa_03_02 "0.1")

# Habilita a saída sobre UART e USB
pico_enable_stdio_uart(Tarefa_03_02 0)
pico_enable_stdio_usb(Tarefa_03_02 1)

pico_generate_pio_header(Tarefa_03_02 ${CMAKE_CURRENT_LIST_DIR}/pio_matrix.pio)

target_sources(Tarefa_03_02 PRIVATE Tarefa_03_02.c)

# Adiciona a biblioteca padrão
target_link_libraries(Tarefa_03_02 PRIVATE
    pico_stdlib
    hardware_i2c
    hardware_pio
    hardware_adc
    pico_bootrom
    hardware_clocks  # Certifique-se de ter a dependência do I2C se for usar o SSD1306
)

# Define os diretórios de inclusão
target_include_directories(Tarefa_03_02 PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
    inc  # Certifique-se de que o diretório inc esteja correto
)

# Gera saídas extras (como o arquivo UF2)
pico_add_extra_outputs(Tarefa_03_02)


