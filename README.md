
# Tarefa: Roteiro de FreeRTOS #1 - EmbarcaTech 2025

Autor: **Pedro Camilo**

Curso: Residência Tecnológica em Sistemas Embarcados

Instituição: EmbarcaTech - HBr

Brasília, junho de 2025

---

### 🎯 Objetivo do projeto
Desenvolver um sistema embarcado multitarefa na placa BitDogLab (Raspberry Pi Pico) utilizando o FreeRTOS. O sistema deve gerenciar três periféricos de forma concorrente (LED RGB, Buzzer e Botões), aplicando conceitos de criação, suspensão e retomada de tarefas.

### 🧩 Componentes usados
* **Hardware:**
    * Placa BitDogLab com Raspberry Pi Pico W

* **Software e Firmware:**
    * FreeRTOS Kernel
    * Raspberry Pi Pico SDK
    * Linguagem de programação C

* **Ferramentas de Desenvolvimento:**
    * Visual Studio Code (VSCode) com a extensão C/C++
    * ARM GCC Toolchain (arm-none-eabi-gcc)
    * CMake

---

### ⚡ Pinagem dos dispositivos
A tabela a seguir descreve a conexão dos periféricos com os pinos da GPIO no Raspberry Pi Pico.

| Periférico | Pino GPIO | Descrição |
| :--- | :---: | :--- |
| LED Vermelho | 13 | Componente do LED RGB |
| LED Verde | 11 | Componente do LED RGB |
| LED Azul | 12 | Componente do LED RGB |
| Buzzer | 21 | Emissor de bipes sonoros |
| Botão A | 5 | Botão de entrada para suspender/retomar o LED |
| Botão B | 6 | Botão de entrada para suspender/retomar o buzzer|

---

### 🧪 Como compilar e executar

**Pré-requisitos:**
1.  Ambiente de desenvolvimento configurado (VSCode, GCC, CMake).
2.  `pico-sdk` e `FreeRTOS-Kernel` clonados dentro da estrutura do projeto.
3.  O arquivo `FreeRTOSConfig.h` configurado na pasta `include/`.

**Passos para Compilação:**
Abra um terminal na raiz do projeto e execute os seguintes comandos:

bash
# 1. Criar um diretório de build
mkdir build

# 2. Entrar no diretório de build
cd build

# 3. Gerar os arquivos de make com o CMake
cmake ..

# 4. Compilar o projeto
make

## O que acontece se todas as tarefas tiverem a mesma prioridade?

Quando eu defino a mesma prioridade pra todo mundo (LED, buzzer e botões), o FreeRTOS divide o tempo do processador igualmente entre elas. Ele usa uma técnica chamada Round-Robin.

Funciona assim:

A tarefa do LED executa por um curto período de tempo.
Depois, o sistema pausa ela e dá a vez para a tarefa do buzzer.
Depois, para a tarefa dos botões.
E aí o ciclo recomeça.
Como essa troca é super rápida, a gente tem a impressão de que tudo está acontecendo ao mesmo tempo. A mágica acontece de verdade quando uma tarefa usa o vTaskDelay(). Ao fazer isso, ela sai da fila e avisa o sistema que não precisa mais da vez. Isso otimiza tudo e garante que ninguém fique ocioso.

## Qual tarefa consome mais tempo da CPU?
A tarefa que mais consome CPU é a button_monitor_task.

Pode parecer estranho, já que ela só lê dois pinos, mas isso se dá devido a frequência. As tarefas do LED e do buzzer passam a maior parte do tempo "dormindo" por causa dos longos vTaskDelay() (500ms e 950ms). Elas só acordam, fazem algo muito rápido e voltam a dormir.

Já a tarefa dos botões acorda a cada 100ms pra ficar perguntando: "Alguém me apertou?". Mesmo que a verificação seja rápida, ela acontece com muito mais frequência que as outras, então, no acumulado, é ela que mais mantém o processador ocupado.

## Quais seriam os riscos de usar polling sem prioridades?
Usar polling sem definir bem as prioridades poderia nos trazer alguns riscos. Vejo dois riscos principais:

Sistema Lento e Não Responsivo: Se a tarefa dos botões tivesse uma prioridade muito baixa, ela poderia ser "atropelada" por outras tarefas. O sistema poderia demorar muito para perceber que um botão foi pressionado, porque a tarefa de verificação nunca teria sua vez de rodar. Apertamos, e nada acontece de imediato. Péssima experiência!

Travamento do Sistema: Esse é o risco pior. Se eu desse prioridade máxima para a tarefa de polling e, por engano, esquecesse de colocar o vTaskDelay() nela, ela não liberaria a CPU. Ela ficaria em um loop infinito, verificando os botões sem parar e nunca liberaria o processador. As tarefas do LED e do buzzer, por terem prioridade menor, nunca executariam. O sistema basicamente travaria, respondendo apenas a essa tarefa.

Resumindo, a gestão de prioridades é crucial pra garantir que o sistema seja eficiente e responsivo, sem que uma tarefa acabe prejudicando todo o resto.

---

## 📜 Licença
GNU GPL-3.0.
