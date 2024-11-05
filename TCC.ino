#include <Arduino.h>

const int sensorPin = 34;  // Pino onde o sensor está conectado
const int threshold = 50;  // Variação mínima para detectar um novo pico ou vale
const unsigned long interval = 60000;  // Intervalo de 1 minuto em milissegundos
const int numReadings = 10;  // Número de leituras para a média móvel

int peakCount = 0;  // Contador de picos para calcular a frequência respiratória
int lastSensorValue = 0;
bool isPeakDetected = false;
unsigned long startTime;

// Array para armazenar as leituras para a média móvel
int readings[numReadings];  
int readIndex = 0;  // Índice da leitura atual
int total = 0;  // Soma das leituras
int average = 0;  // Média das leituras

void setup() {
  Serial.begin(9600);  // Inicializa a comunicação serial
  startTime = millis();  // Marca o início do intervalo de contagem
  
  // Inicializa todas as leituras com zero
  for (int i = 0; i < numReadings; i++) {
    readings[i] = 0;
  }
}

void loop() {
  // Lê o valor do sensor
  int sensorValue = analogRead(sensorPin);

  // Atualiza a soma total para a média móvel
  total -= readings[readIndex];  // Remove a leitura mais antiga
  readings[readIndex] = sensorValue;  // Adiciona a nova leitura
  total += readings[readIndex];  // Atualiza a soma total
  readIndex = (readIndex + 1) % numReadings;  // Avança o índice para a próxima leitura

  // Calcula a média
  average = total / numReadings;

  // Detecta pico se a média atual for consideravelmente maior que a anterior
  if (average > lastSensorValue + threshold && !isPeakDetected) {
    peakCount++;
    isPeakDetected = true;  // Marca o início do pico
  }

  // Detecta vale se a média atual for consideravelmente menor que a anterior
  if (average < lastSensorValue - threshold && isPeakDetected) {
    isPeakDetected = false;  // Reseta para detectar novo pico no próximo ciclo
  }

  lastSensorValue = average;  // Atualiza o último valor do sensor com a média

  // Envia o valor da média do sensor em tempo real
  Serial.print("VAL:");
  Serial.println(average);

  // Verifica se o intervalo de 1 minuto se passou para calcular a RPM
  if (millis() - startTime >= interval) {
    int rpm = peakCount;  // Cada pico representa uma respiração completa
    Serial.print("RPM:");
    Serial.println(rpm);

    // Reinicia o contador e o temporizador para o próximo minuto
    peakCount = 0;
    startTime = millis();
  }

  delay(100);  // Aguarda antes da próxima leitura
}
