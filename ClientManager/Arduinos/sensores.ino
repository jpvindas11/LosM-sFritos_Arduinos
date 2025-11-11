#include <WiFi.h>
#include "DHT.h"

#define DHTTYPE DHT11

// Credenciales WiFi
const char* ssid = "Lab-3-5";
const char* password = "Cata2960!";

const char* host = "10.232.69.56";
const uint16_t port = 8080;

// Pines
const int trigPin = 14;
const int echoPin = 27;
const int ledPin  = 2;
const int dhtPin  = 16;
const int uvPin   = 39;

long duration;
int distance;
WiFiClient client;
DHT dht(dhtPin, DHTTYPE);

// Tiempos de envío
unsigned long lastDistSend = 0;
unsigned long lastHumSend  = 0;
unsigned long lastUVSend   = 0;

// Intervalos de envío
const unsigned long distInterval = 30000;
const unsigned long humInterval  = 32000;
const unsigned long uvInterval   = 35000;

void setup() {
  Serial.begin(115200); // Inicializa la comunicación serial

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(ledPin, OUTPUT);

  dht.begin();

  Serial.print("Conectando a ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi conectado");
  Serial.print("IP asignada: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  unsigned long now = millis();

  if (now - lastDistSend >= distInterval) {
    lastDistSend = now;

    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);

    duration = pulseIn(echoPin, HIGH);
    distance = duration * 0.0343 / 2;

    digitalWrite(ledPin, (distance < 30) ? HIGH : LOW);

    Serial.print("Distancia: ");
    Serial.print(distance);
    Serial.println(" cm");

    if (client.connect(host, port)) {
      String mensaje = "Distancia: " + String(distance) + " cm\n";
      client.print(mensaje);
      Serial.println("Distancia enviada al servidor");
      client.stop();
    } else {
      Serial.println("No se pudo conectar al servidor (Distancia)");
    }
  }

  if (now - lastHumSend >= humInterval) {
    lastHumSend = now;

    float h = dht.readHumidity();
    if (isnan(h)) {
      Serial.println("Error leyendo DHT11");
    } else {
      Serial.print("Humedad: ");
      Serial.print(h);
      Serial.println(" %");

      if (client.connect(host, port)) {
        String mensaje = "Humedad: " + String(h, 1) + " %\n";
        client.print(mensaje);
        Serial.println("Humedad enviada al servidor");
        client.stop();
      } else {
        Serial.println("No se pudo conectar al servidor (Humedad)");
      }
    }
  }

  if (now - lastUVSend >= uvInterval) {
    lastUVSend = now;

    int sensorValue = analogRead(uvPin);
    float voltage = sensorValue * (3.3 / 4095.0);
    float uvIntensity = voltage * 307.0;

    Serial.print("UV: ");
    Serial.print(uvIntensity, 2);
    Serial.println(" mW/m²");

    if (client.connect(host, port)) {
      String mensaje = "UV: " + String(uvIntensity, 2) + " mW/m²\n";
      client.print(mensaje);
      Serial.println("UV enviado al servidor");
      client.stop();
    } else {
      Serial.println("No se pudo conectar al servidor (UV)");
    }
  }

  delay(100);
}