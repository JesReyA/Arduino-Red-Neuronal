#include <Wire.h>
#include "Adafruit_TCS34725.h"

Adafruit_TCS34725 
tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_154MS, TCS34725_GAIN_4X);

byte gammatable[256];

void setup(void) {
  Serial.begin(9600);

  if (!tcs.begin()) {
    Serial.println("Error al iniciar TCS34725");
    while (1) delay(1000);
  }

  // Tabla de corrección gamma
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

  Serial.print(R); Serial.print(",");
  Serial.print(G); Serial.print(",");
  Serial.println(B);

  delay(1000);
}