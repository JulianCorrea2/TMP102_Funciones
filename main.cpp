#include "I2C.h"
#include "ThisThread.h"
#include "mbed.h"
#include "Adafruit_GFX.h"
#include "Adafruit_SSD1306.h" 
#include <cstring>

#define tiempo_muestreo 1s
#define TMP102_ADDRESS 0x90

// Pines y puertos
BufferedSerial serial(USBTX, USBRX);
I2C i2c(D2, D3);
Adafruit_SSD1306_I2c oled(i2c, D0);
AnalogIn ain(A0);

// Variables globales 
float Vin = 0.0;
int ent = 0;
int dec = 0;
char men[40];

char comando[3] = {0x01, 0x60, 0xA0};
char data[2];

const char* mensaje_inicio = "Arranque del programa\n\r";

// Función para inicializar la pantalla OLED
void iniciar_oled() {
    oled.begin();
    oled.setTextSize(1);
    oled.setTextColor(1);
    oled.display();
    ThisThread::sleep_for(3000ms);
    oled.clearDisplay();
    oled.display();
    oled.printf("Test\r\n");
    oled.display();
}

// Función para leer el valor del sensor analógico
void leer_sensor_analogico() {
    Vin = ain * 3.3;
    ent = int(Vin);
    dec = int((Vin - ent) * 10000);
}

// Función para visualizar el valor en la pantalla OLED y enviarlo por serial
void visualizar_voltaje() {
    oled.clearDisplay();
    oled.display();
    sprintf(men, "El voltaje es:\n\r %01u.%04u volts \n\r", ent, dec);
    oled.setTextCursor(0, 2);
    oled.printf(men);
    oled.display();
    serial.write(men, strlen(men));
}

// Función para leer la temperatura desde el TMP102
void leer_temperatura() {
    comando[0] = 0;  // Registro de temperatura
    i2c.write(TMP102_ADDRESS, comando, 1);  // Enviar el comando para leer
    i2c.read(TMP102_ADDRESS, data, 2);  // Leer 2 bytes
    int16_t temp = (data[0] << 4) | (data[1] >> 4);
    float Temperatura = temp * 0.0625;
    ent = int(Temperatura);
    dec = int((Temperatura - ent) * 10000);
}

// Función para visualizar la temperatura en la pantalla OLED y enviarla por serial
void visualizar_temperatura() {
    oled.clearDisplay();
    oled.display();
    sprintf(men, "La Temperatura es:\n\r %01u.%04u Celsius\n\r", ent, dec);
    oled.setTextCursor(0, 2);
    oled.printf(men);
    oled.display();
    serial.write(men, strlen(men));
}

int main() {
    // Inicialización del OLED y saludo por serial
    iniciar_oled();
    i2c.write(TMP102_ADDRESS, comando, 3);
    serial.write(mensaje_inicio, strlen(mensaje_inicio));

    while (true) {
        // Leer sensor analógico y mostrar voltaje
        leer_sensor_analogico();
        visualizar_voltaje();

        // Leer sensor de temperatura y mostrar temperatura
        leer_temperatura();
        visualizar_temperatura();

        // Pausa para el siguiente ciclo
        ThisThread::sleep_for(tiempo_muestreo);
    }
}