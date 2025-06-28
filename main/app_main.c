/**
 * @file mqtt5_led_control.c
 * @author Guilherme Bonifácio Feitosa (gbf1@discente.ifpe.edu.br)
 * @brief Exemplo de controle de um LED via MQTTv5 em um ESP32.
 * @date 2025-06-28
 * Basicamente, isso aqui conecta o ESP32 num broker MQTT, fica ouvindo o tópico
 * "/ifpe/ads/embarcados/esp32/led" e acende/apaga um LED quando chega "1" ou "0"
 *
 */

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "protocol_examples_common.h" // Para a função example_connect()
#include "esp_log.h"
#include "driver/gpio.h"             // Para controle dos pinos GPIO (General Purpose Input/Output)
#include "mqtt_client.h"             // Para funcionalidades do cliente MQTT

// O pino do LED vem do menuconfig. Assim não precisa mudar o código pra trocar de pino.
#define BLINK_GPIO CONFIG_BLINK_GPIO

// Tag pra facilitar o debug no Monitor Serial.
static const char *TAG = "PROJETO_LED_MQTT";

// Prepara o pino do LED para ser usado.
static void configure_led(void)
{
    ESP_LOGI(TAG, "Configurando o pino do LED como saida no GPIO %d", BLINK_GPIO);
    gpio_reset_pin(BLINK_GPIO);                  
    gpio_set_direction(BLINK_GPIO, GPIO_MODE_OUTPUT); 
}

/* Essa é a função principal que lida com tudo do MQTT. */
static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    esp_mqtt_event_handle_t event = event_data;
    esp_mqtt_client_handle_t client = event->client;
    int msg_id;

    // O switch-case trata os diferentes tipos de eventos MQTT que podem ocorrer.
    switch ((esp_mqtt_event_id_t)event_id) {
    case MQTT_EVENT_CONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
        // Uma vez conectado ao broker, inscreve-se no tópico para receber os comandos do LED.
        // O segundo parâmetro '1' define a Qualidade de Serviço (QoS) como 1.
        msg_id = esp_mqtt_client_subscribe(client, "/ifpe/ads/embarcados/esp32/led", 1);
        ESP_LOGI(TAG, "Inscrição enviada para o tópico /ifpe/ads/embarcados/esp32/led, msg_id=%d", msg_id);
        break;
    case MQTT_EVENT_DISCONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
        break;
    case MQTT_EVENT_SUBSCRIBED:
        ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
        ESP_LOGI(TAG, "Inscrito com sucesso no tópico, aguardando mensagens...");
        break;
    case MQTT_EVENT_UNSUBSCRIBED:
        ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_PUBLISHED:
        ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_DATA:
        ESP_LOGI(TAG, "MQTT_EVENT_DATA - Mensagem recebida!");
        printf("TOPICO: %.*s\r\n", event->topic_len, event->topic);
        printf("DADO: %.*s\r\n", event->data_len, event->data);

        // Verifica se a mensagem recebida é para o tópico específico do LED.
        if (strncmp(event->topic, "/ifpe/ads/embarcados/esp32/led", event->topic_len) == 0) {
            ESP_LOGI(TAG, "Mensagem recebida para o topico do LED.");
            
            // Compara o conteúdo da mensagem ("dado") para decidir a ação.
            if (strncmp(event->data, "1", event->data_len) == 0) {
                ESP_LOGI(TAG, "Comando: ACENDER o LED.");
                gpio_set_level(BLINK_GPIO, 1); // Envia  1 para o pino do LED.
            } else if (strncmp(event->data, "0", event->data_len) == 0) {
                ESP_LOGI(TAG, "Comando: APAGAR o LED.");
                gpio_set_level(BLINK_GPIO, 0); // Envia 0 para o pino do LED.
            }
        }
        break;
    case MQTT_EVENT_ERROR:
        ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
        if (event->error_handle->error_type == MQTT_ERROR_TYPE_TCP_TRANSPORT) {
            
        }
        break;
    default:
        ESP_LOGI(TAG, "Other event id: %d", (int)event_id);
        break;
    }
}

/**
 * @brief Inicializa e inicia o cliente MQTT.
 * * Configura o cliente MQTT com a URL do broker e
 * registra a função `mqtt_event_handler` para tratar todos os eventos.
 */
static void mqtt_app_start(void)
{
    esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.uri = CONFIG_BROKER_URL,
    };

    esp_mqtt_client_handle_t client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);
    esp_mqtt_client_start(client);
}

/* conecta-se à rede Wi-Fi, configura o hardware (LED) e inicia a aplicação MQTT. */
void app_main(void)
{
    ESP_LOGI(TAG, "[APP] Startup..");
    ESP_LOGI(TAG, "[APP] Free memory: %" PRIu32 " bytes", esp_get_free_heap_size());
    ESP_LOGI(TAG, "[APP] IDF version: %s", esp_get_idf_version());

    // Inicialização do NVS (Non-Volatile Storage), necessário para o Wi-Fi.
    ESP_ERROR_CHECK(nvs_flash_init());
    // Inicialização da pilha de rede TCP/IP.
    ESP_ERROR_CHECK(esp_netif_init());
    // Criação do loop de eventos padrão do sistema.
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    // Conecta-se à rede Wi-Fi configurada no menuconfig.
    ESP_ERROR_CHECK(example_connect());

    // Chama as nossas funções pra iniciliar o programa.
    configure_led();
    mqtt_app_start();
}