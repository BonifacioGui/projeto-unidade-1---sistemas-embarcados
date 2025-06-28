# Projeto de Controle de LED com ESP32 e MQTT

Este projeto faz parte da avaliação da Unidade 1 da disciplina de Sistemas Embarcados do curso de ADS do IFPE. O objetivo é controlar o estado de um LED (ligado/desligado) na placa de desenvolvimento ESP32 utilizando o protocolo MQTT.

**Autor:** Guilherme Bonifácio Feitosa
**Data:** 28 de Junho de 2025
**Matrícula:** 20211ADSGR0214

---

## Descrição do Projeto

A aplicação implementada no ESP32 funciona como um cliente MQTT que se conecta a uma rede Wi-Fi de 2.4GHz e a um broker MQTT público. Após a conexão, o cliente se inscreve no tópico ` /ifpe/ads/embarcados/esp32/led` para receber comandos.

O estado do LED embutido na placa é controlado da seguinte forma:
-   Ao receber a mensagem `1` no tópico, o LED acende.
-   Ao receber a mensagem `0` no tópico, o LED apaga.

O projeto foi desenvolvido utilizando o **ESP-IDF (Espressif IoT Development Framework)**, a partir do exemplo `mqtt5/tcp`.

## Hardware Utilizado

-   Placa de desenvolvimento ESP32 
-   Cabo USB 

## Software e Ferramentas

-   **ESP-IDF v5.4.1**: Framework de desenvolvimento da Espressif.
-   **Visual Studio Code**: Com a extensão oficial do ESP-IDF.
-   **Broker MQTT Público**: `mqtt://broker.hivemq.com`
-   **Cliente MQTT para Teste**: MQTT Explorer.
-   **Linguagem**: C

## Como Executar o Projeto

### 1. Pré-requisitos
- Ter o [ambiente ESP-IDF](https://docs.espressif.com/projects/esp-idf/pt_BR/stable/esp32/get-started/index.html) configurado no seu computador.
- Clonar este repositório: https://github.com/BonifacioGui/projeto-unidade-1---sistemas-embarcados

### 2. Configuração
- Conecte a placa ESP32 ao seu computador.
- Abra o projeto no Visual Studio Code.
- Clique no ícone de engrenagem (⚙️) na barra azul inferior para abrir o `menuconfig`.
- Navegue até `CONFIGURACOES DO PROJETO` -> `1. Conexao Wi-Fi e Broker`.
- Insira o **Nome da Rede (SSID)** e a **Senha da Rede Wi-Fi** (a rede deve ser de 2.4GHz).
- Salve as configurações.

### 3. Compilação e Gravação
- Com a configuração salva, use os ícones na barra azul inferior na seguinte ordem:
  1.  **Clean** (Ícone da Lixeira 🗑️): Limpa arquivos antigos.
  2.  **Build** (Ícone do 'Check' ✔️): Compila o projeto.
  3.  **Flash** (Ícone do Raio ⚡): Grava o programa na placa. (Pode ser necessário segurar o botão `BOOT` da placa durante este passo).

### 4. Teste
- Após gravar, clique no ícone da **Tomada (🔌 Monitor)** para ver os logs. Aguarde a mensagem `Inscrito com sucesso no tópico, aguardando mensagens...`.
- Use um cliente MQTT (como o MQTT Explorer) para se conectar ao broker `broker.hivemq.com`.
- Publique no tópico `/ifpe/ads/embarcados/esp32/led`:
    - Mensagem `1` para acender o LED.
    - Mensagem `0` para apagar o LED.