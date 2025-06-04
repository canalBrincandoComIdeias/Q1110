#include <WiFi.h>
#include <MQTT.h>
#include "user.h"  //Informar a rede WiFi, a senha, usuário MQTT e a senha

//const char ssid[] = "xxx";
//const char pass[] = "xxx";

#define MEU_DISPOSITIVO "bi_q1110"
#define TOPICO_ALERTA "bi_q1110/alerta"

//Servidor MQTT
const char servidor[] = "54.233.221.233";
//const char servidor[] = "mqtt.monttudo.com";  //(caso o IP não funcione, use a identificação pelo domínio)

//Usuário MQTT
const char deviceName[] = MEU_DISPOSITIVO;
const char mqttUser[] = "todos";
const char mqttPass[] = "cortesi@BrincandoComIdeias";

#define pinLEDPlaca 8
#define pinSensorDC 4      //Arduino=Qualquer porta Analógica   ESP=Qualquer porta exceto 0,1,2,3,12,13 e 15
#define maxSensor 16.5     //volts  (Arduino Uno=25  Esp32=16.5)
#define maxAnalogica 4095  //valor máximo de leitura da porta analógica (Arduino Uno=1023  Esp32=4095)

#define tensaoDeCorte 2.00  //tensão mínima para considerar que a bateria esta sendo carregada

int leituraSensorDC;
unsigned long delayDisplay;
bool estadoAlerta = false;
bool estadoAlertaAnt = false;

WiFiClient net;
MQTTClient client;

void connect() {
  Serial.print("->wifi...");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }
  Serial.println(";");

  Serial.print("->mqtt...");
  while (!client.connect(deviceName, mqttUser, mqttPass)) {
    Serial.print(".");
    delay(1000);
  }
  Serial.println(";");
  Serial.println("->conectado;");
}

void messageReceived(String &topic, String &payload) {
  Serial.println(topic + ":" + payload);

  //if (topic == TOPICO_ALERTA) {
  //  estado = payload.toInt();
  //}

  // Nota: Não use o "client" nessa função para publicar, subscrever ou
  // dessubscrever porque pode causar problemas quando outras mensagens são recebidas
  // enquanto são feitos envios ou recebimentos. Então, altere uma variavel global,
  // coloque numa fila e trate essa fila no loop depois de chamar `client.loop()`.
}

void setup() {
  pinMode(pinLEDPlaca, OUTPUT);
  Serial.begin(9600);

  WiFi.begin(ssid, pass);

  // Observação: Nomes de domínio locais (por exemplo, "Computer.local" no OSX)
  // não são suportados pelo Arduino. Você precisa definir o endereço IP diretamente.
  client.begin(servidor, net);
  client.onMessage(messageReceived);

  connect();
  digitalWrite(pinLEDPlaca, HIGH);
}

void loop() {
  client.loop();
  delay(10);  //corrige alguns problemas com a estabilidade do WiFi

  if (!client.connected()) {
    digitalWrite(pinLEDPlaca, LOW);
    connect();
    digitalWrite(pinLEDPlaca, HIGH);
  }

  leituraSensorDC = analogRead(pinSensorDC);
  float tensaoSensor = (float(leituraSensorDC) * float(maxSensor)) / float(maxAnalogica);

  if ((millis() - delayDisplay) > 1000) {
    delayDisplay = millis();

    Serial.print("Leitura:");
    Serial.print(leituraSensorDC);

    Serial.print("  Tensao:");
    Serial.print(tensaoSensor);

    if (tensaoSensor > tensaoDeCorte) {
      estadoAlerta = false;
      if (estadoAlertaAnt) {
        Serial.print("  [ok]");
        client.publish(TOPICO_ALERTA, "0");
      }
    } else {
      estadoAlerta = true;
      if (!estadoAlertaAnt) {
        Serial.print("  [BATERIA!!!]");
        client.publish(TOPICO_ALERTA, "2");
      }
    }
    Serial.println();

    estadoAlertaAnt = estadoAlerta;
  }

  delay(20);
}
