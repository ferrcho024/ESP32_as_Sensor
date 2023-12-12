#include <Arduino.h>

#include "parameters.h"
#include "file_func.h"
#include "mqtt.h"

extern PubSubClient client;
extern String in_txt;
extern bool callback;

int listSize;  // Tamaño de la lista que almacena los values
int startline; // Inicializa la lectura desde la línea 0
int siataValue; // Contador para extraer el valor de la estación SIATA
bool ban = true;
int frec = 1000; // Frecuencia de envío de los mensajes que se envian (en milisegundos)

void send_data() {

  startline = 0;
  siataValue = 0;
  
  while(true){
     printf("************ Free Memory: %u bytes ************\n", esp_get_free_heap_size());

      listSize = 60;
      
      float* myList_df = read_file(df, startline, &listSize);
      float* myList_nova = read_file(nova, startline, &listSize);
      startline = startline + listSize; // Aumenta la el número de la línea para la siguiente iteracción
      //Serial.println(startline);

      int siataList = 1;
      float* value_siata = read_file(siata, siataValue, &siataList);
      siataValue++;
      
      if (myList_df != NULL) {
            printf("\nList of values:\n");
            printf("List size: %d\n", listSize);
            for (int i = 0; i < listSize; i++) {
                char mqtt_msg[30];
                sprintf(mqtt_msg, "%.5f,%.5f,%.5f",myList_df[i],myList_nova[i],value_siata[0]);
                client.publish(TOPIC.c_str(), mqtt_msg);
                delay(frec);
            }

            // Liberar memoria después de su uso
            free(myList_df);
            free(myList_nova);
        }

        //esp_vfs_spiffs_unregister(NULL); // Desmonta el sistema de archivo SPIFFS
        Serial.println("**** Finished ******");

  }

}

void setup() {
  Serial.begin(115200);

  // Configurar y conectar WiFi
  ConnectToWiFi();
  
  initialize_spiffs();
  createMQTTClient();
  send_data();
}

void loop() {
  reconnectMQTTClient();
  client.loop();
  delay(1000);
}