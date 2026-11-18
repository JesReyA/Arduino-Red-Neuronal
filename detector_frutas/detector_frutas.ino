#include <Wire.h>
#include "Adafruit_TCS34725.h"
#include "modelo_frutas.h"  

Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_154MS, TCS34725_GAIN_4X);

byte gammatable[256];

// Pines para los LEDs indicadores (puedes cambiarlos si usas otros)
const int PIN_LED_MANZANA = 8;
const int PIN_LED_PERA    = 9;

void setup(void) {
  Serial.begin(9600);

  pinMode(PIN_LED_MANZANA, OUTPUT);
  pinMode(PIN_LED_PERA, OUTPUT);

  if (!tcs.begin()) {
    Serial.println("Error al iniciar TCS34725");
    while (1) delay(1000);
  }

  // Tabla de corrección gamma (idéntica a la captura de datos)
  for (int i = 0; i < 256; i++) {
    float x = i / 255.0;
    x = pow(x, 2.5);
    x *= 255;
    gammatable[i] = (byte)x;
  }
}

void loop(void) {
  uint16_t r, g, b, c;
  tcs.getRawData(&r, &g, &b, &c);

  if (c == 0) {
    Serial.println("Sin luz");
    digitalWrite(PIN_LED_MANZANA, LOW);
    digitalWrite(PIN_LED_PERA, LOW);
    delay(500);
    return;
  }

  // Normalizar respecto al canal Clear -> 0 a 255
  float rf = (float)r / c * 255.0;
  float gf = (float)g / c * 255.0;
  float bf = (float)b / c * 255.0;

  int R = gammatable[constrain((int)rf, 0, 255)];
  int G = gammatable[constrain((int)gf, 0, 255)];
  int B = gammatable[constrain((int)bf, 0, 255)];

  // 1. Preparar el arreglo de entrada para la red neuronal
  float entrada[3] = {(float)R, (float)G, (float)B};

  // 2. Ejecutar la predicción (0 = MANZANA, 1 = PERA, 2 = SIN FRUTA)
  int fruta = modelo_frutas_predict(entrada, 3);

  // 3. Mostrar resultado en Monitor Serie y encender el LED correspondiente
  Serial.print("RGB: ");
  Serial.print(R); Serial.print(",");
  Serial.print(G); Serial.print(",");
  Serial.print(B);
  Serial.print(" -> ");

  if (fruta == 0) {
    Serial.println("MANZANA");
    digitalWrite(PIN_LED_MANZANA, HIGH);
    digitalWrite(PIN_LED_PERA, LOW);
  } else if (fruta == 1) {
    Serial.println("PERA");
    digitalWrite(PIN_LED_MANZANA, LOW);
    digitalWrite(PIN_LED_PERA, HIGH);
  } else {
    Serial.println("SIN FRUTA");
    digitalWrite(PIN_LED_MANZANA, LOW);
    digitalWrite(PIN_LED_PERA, LOW);
  }

  delay(500);
}