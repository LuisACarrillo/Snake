#include "ripes_system.h"
#include <stdio.h>
#include <stdlib.h>

volatile unsigned int *led_base = (unsigned int *)LED_MATRIX_0_BASE;
volatile unsigned int *d_pad_up = (unsigned int *)D_PAD_0_UP;
volatile unsigned int *d_pad_do = (unsigned int *)D_PAD_0_DOWN;
volatile unsigned int *d_pad_le = (unsigned int *)D_PAD_0_LEFT;
volatile unsigned int *d_pad_ri = (unsigned int *)D_PAD_0_RIGHT;
volatile unsigned int *switch_0 = (unsigned int *)SWITCHES_0_BASE;

#define MAX_SNAKE_LENGTH 100  // Longitud máxima de la serpiente

typedef struct {
    int x, y;
} Segment;

Segment snake[MAX_SNAKE_LENGTH];
int snakeLength = 1;  // Longitud inicial de la serpiente
int appleX, appleY;
int juegoActivo = 0;  // 0 = juego detenido, 1 = juego activo

void apagarLEDs() {
    for (int i = 0; i < LED_MATRIX_0_WIDTH * LED_MATRIX_0_HEIGHT; i++) {
        led_base[i] = 0;  // Apagar LED
    }
}

void startGame() {
    apagarLEDs();  // Limpiar la pantalla antes de iniciar
    snake[0].x = rand() % (LED_MATRIX_0_WIDTH - 1);
    snake[0].y = rand() % (LED_MATRIX_0_HEIGHT - 1);

    // Asegurarnos de que las coordenadas sean válidas (pares)
    if (snake[0].x % 2 != 0) { snake[0].x--; }
    if (snake[0].y % 2 != 0) { snake[0].y--; }

    snakeLength = 1;  // Reiniciar la longitud de la serpiente
    spawnApple();
    juegoActivo = 1;  // Activar el juego
}

void drawSnake() {
    apagarLEDs();  // Limpiar LEDs antes de dibujar

    for (int i = 0; i < snakeLength; i++) {
        int index = snake[i].y * LED_MATRIX_0_WIDTH + snake[i].x;
        led_base[index] = 0xFF0000;  // Color de la serpiente
        led_base[index + 1] = 0xFF0000;
        led_base[index + LED_MATRIX_0_WIDTH] = 0xFF0000;
        led_base[index + LED_MATRIX_0_WIDTH + 1] = 0xFF0000;
    }

    // Dibujar la manzana
    int apple_index = appleY * LED_MATRIX_0_WIDTH + appleX;
    led_base[apple_index] = 0x00FF00;  // Color de la manzana
    led_base[apple_index + 1] = 0x00FF00;
    led_base[apple_index + LED_MATRIX_0_WIDTH] = 0x00FF00;
    led_base[apple_index + LED_MATRIX_0_WIDTH + 1] = 0x00FF00;
}

void updateSnake(int dx, int dy) {
    // Mover segmentos de la cola hacia adelante
    for (int i = snakeLength - 1; i > 0; i--) {
        snake[i] = snake[i - 1];
    }

    // Mover la cabeza
    snake[0].x += dx;
    snake[0].y += dy;

    // Verificar si la serpiente come la manzana
    if (snake[0].x == appleX && snake[0].y == appleY) {
        if (snakeLength < MAX_SNAKE_LENGTH) {
            snakeLength++;  // Aumentar la longitud de la serpiente
        }
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
    for (int i = 0; i < snakeLength; i++) {
        if (appleX == snake[i].x && appleY == snake[i].y) {
            spawnApple();  // Reintentar si hay conflicto
            return;
        }
    }
}

void main() {
    int dx = 2, dy = 0;  // Dirección inicial

    while (1) {
        if (*switch_0 == 1) {  // Si el switch está activado
            juegoActivo = 0;   // Detener el juego
            apagarLEDs();      // Apagar todos los LEDs
            continue;          // Saltar a la siguiente iteración
        } else if (juegoActivo == 0) {  // Si el juego estaba detenido y el switch se apagó
            startGame();  // Reiniciar el juego
        }

        if (juegoActivo) {  // Solo actualizar si el juego está activo
            if (*d_pad_up && dy == 0) { dx = 0; dy = -2; }
            else if (*d_pad_do && dy == 0) { dx = 0; dy = 2; }
            else if (*d_pad_le && dx == 0) { dx = -2; dy = 0; }
            else if (*d_pad_ri && dx == 0) { dx = 2; dy = 0; }

            updateSnake(dx, dy);  // Actualizar posiciones de la serpiente
            drawSnake();          // Dibujar la serpiente y la manzana
        }

        for (int i = 0; i <= 10000; i++);  // Retardo
    }
}
