#include "ripes_system.h"
#include <stdio.h>
#include <stdlib.h>

volatile unsigned int *led_base = (unsigned int *)LED_MATRIX_0_BASE;
volatile unsigned int *d_pad_up = (unsigned int *)D_PAD_0_UP;
volatile unsigned int *d_pad_do = (unsigned int *)D_PAD_0_DOWN;
volatile unsigned int *d_pad_le = (unsigned int *)D_PAD_0_LEFT;
volatile unsigned int *d_pad_ri = (unsigned int *)D_PAD_0_RIGHT;
volatile unsigned int *switch_0 = (unsigned int *)SWITCHES_0_BASE;

int snakeHeadx, snakeHeady;
int prevsnakex, prevsnakey;
int appleX, appleY;
int juegoActivo = 0;  // 0 = juego detenido, 1 = juego activo

void apagarLEDs() {
    for (int i = 0; i < LED_MATRIX_0_WIDTH * LED_MATRIX_0_HEIGHT; i++) {
        led_base[i] = 0;  // Apagar LED
    }
}

void startGame() {
    apagarLEDs();  // Limpiar la pantalla antes de iniciar
    snakeHeadx = rand() % (LED_MATRIX_0_WIDTH - 1);
    snakeHeady = rand() % (LED_MATRIX_0_HEIGHT - 1);

    // Asegurarnos de que las coordenadas sean válidas (pares)
    if (snakeHeadx % 2 != 0) { snakeHeadx--; }
    if (snakeHeady % 2 != 0) { snakeHeady--; }

    prevsnakex = snakeHeadx;
    prevsnakey = snakeHeady;

    createSnake();
    spawnApple();
    juegoActivo = 1;  // Activar el juego
}

void createSnake() {
    int prev_index = prevsnakey * LED_MATRIX_0_WIDTH + prevsnakex;
    led_base[prev_index] = 0;
    led_base[prev_index + 1] = 0;
    led_base[prev_index + LED_MATRIX_0_WIDTH] = 0;
    led_base[prev_index + LED_MATRIX_0_WIDTH + 1] = 0;

    int head_index = snakeHeady * LED_MATRIX_0_WIDTH + snakeHeadx;
    led_base[head_index] = 0xFF0000;  // Color de la serpiente
    led_base[head_index + 1] = 0xFF0000;
    led_base[head_index + LED_MATRIX_0_WIDTH] = 0xFF0000;
    led_base[head_index + LED_MATRIX_0_WIDTH + 1] = 0xFF0000;

    prevsnakex = snakeHeadx;
    prevsnakey = snakeHeady;
}

void updateSnake(int x, int y) {
    snakeHeadx += x;
    snakeHeady += y;

    // Comprobar si la serpiente come la manzana
    if (snakeHeadx == appleX && snakeHeady == appleY) {
        spawnApple();  // Generar una nueva manzana
    }
}

void spawnApple() {
    appleX = rand() % (LED_MATRIX_0_WIDTH - 1);
    appleY = rand() % (LED_MATRIX_0_HEIGHT - 1);

    // Asegurarnos de que las coordenadas sean válidas (pares)
    if (appleX % 2 != 0) { appleX--; }
    if (appleY % 2 != 0) { appleY--; }

    // Asegurarnos de que no aparezca sobre la serpiente
    if (appleX == snakeHeadx && appleY == snakeHeady) {
        spawnApple();
        return;
    }

    int apple_index = appleY * LED_MATRIX_0_WIDTH + appleX;
    led_base[apple_index] = 0x00FF00;  // Color de la manzana
    led_base[apple_index + 1] = 0x00FF00;
    led_base[apple_index + LED_MATRIX_0_WIDTH] = 0x00FF00;
    led_base[apple_index + LED_MATRIX_0_WIDTH + 1] = 0x00FF00;
}

void main() {
    int x = 2, y = 0;

    while (1) {
        if (*switch_0 == 1) {  // Si el switch está activado
            juegoActivo = 0;   // Detener el juego
            apagarLEDs();
            continue;        
        } else if (juegoActivo == 0) {  // Si el juego estaba detenido y el switch se apagó
            startGame();  // Reiniciar el juego
        }

        if (juegoActivo) {  // Solo actualizar si el juego está activo
            if (*d_pad_up && y == 0) { x = 0; y = -2; }
            else if (*d_pad_do && y == 0) { x = 0; y = 2; }
            else if (*d_pad_le && x == 0) { x = -2; y = 0; }
            else if (*d_pad_ri && x == 0) { x = 2; y = 0; }

            updateSnake(x, y);
            createSnake();
        }

        for (int i = 0; i <= 10000; i++);  // Retardo
    }
}
