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
#define INITIAL_DELAY 10000   // Retardo inicial en el bucle principal
#define SPEED_INCREMENT 400   // Incremento de velocidad al comer una manzana
#define MIN_DELAY 2000        // Retardo mínimo para la máxima velocidad

typedef struct {
    int x, y;  // Coordenadas del segmento de la serpiente
} Segment;

Segment snake[MAX_SNAKE_LENGTH];  // Arreglo que representa la serpiente
int snakeLength = 2;              // Longitud inicial de la serpiente
int appleX, appleY;               // Coordenadas de la manzana
int juegoActivo = 0;              
int gameDelay = INITIAL_DELAY;    // Control de la velocidad del juego

void clearTail();
void drawHead();
void drawApple();
void spawnApple();
void startGame();
void updateSnake(int dx, int dy);
void checkCollisionWithBorders();
void checkCollisionWithSelf();
void gameOver();

void clearTail() {
    Segment tail = snake[snakeLength - 1];  // Último segmento de la cola
    int tail_index = tail.y * LED_MATRIX_0_WIDTH + tail.x;
    led_base[tail_index] = 0;
    led_base[tail_index + 1] = 0;
    led_base[tail_index + LED_MATRIX_0_WIDTH] = 0;
    led_base[tail_index + LED_MATRIX_0_WIDTH + 1] = 0;
}

void drawHead() {
    Segment head = snake[0];  // Cabeza de la serpiente
    int head_index = head.y * LED_MATRIX_0_WIDTH + head.x;
    led_base[head_index] = 0xFF0000;  // Color rojo
    led_base[head_index + 1] = 0xFF0000;
    led_base[head_index + LED_MATRIX_0_WIDTH] = 0xFF0000;
    led_base[head_index + LED_MATRIX_0_WIDTH + 1] = 0xFF0000;
}

void drawApple() {
    int apple_index = appleY * LED_MATRIX_0_WIDTH + appleX;
    led_base[apple_index] = 0x00FF00;  // Color verde
    led_base[apple_index + 1] = 0x00FF00;
    led_base[apple_index + LED_MATRIX_0_WIDTH] = 0x00FF00;
    led_base[apple_index + LED_MATRIX_0_WIDTH + 1] = 0x00FF00;
}

void spawnApple() {
    appleX = rand() % (LED_MATRIX_0_WIDTH - 1);
    appleY = rand() % (LED_MATRIX_0_HEIGHT - 1);

    // Ajustar coordenadas a valores pares
    if (appleX % 2 != 0) { appleX--; }
    if (appleY % 2 != 0) { appleY--; }

    // Verificar que la manzana no aparezca sobre la serpiente
    for (int i = 0; i < snakeLength; i++) {
        if (appleX == snake[i].x && appleY == snake[i].y) {
            spawnApple();  // Reintentar si hay colisión
            return;
        }
    }

    drawApple();
}

void checkCollisionWithBorders() {
    Segment head = snake[0];  // Cabeza de la serpiente
    if (head.x < 0 || head.y < 0 || 
        head.x >= LED_MATRIX_0_WIDTH || 
        head.y >= LED_MATRIX_0_HEIGHT) {
        gameOver();  // Detener el juego si hay colisión con los bordes
    }
}

void checkCollisionWithSelf() {
    Segment head = snake[0];  // Cabeza de la serpiente
    for (int i = 1; i < snakeLength; i++) {  // Ignorar la cabeza (índice 0)
        if (head.x == snake[i].x && head.y == snake[i].y) {
            gameOver();  // Detener el juego si hay colisión con la cola
        }
    }
}

void gameOver() {
    juegoActivo = 0;  // Desactivar el juego
    for (int i = 0; i < LED_MATRIX_0_WIDTH * LED_MATRIX_0_HEIGHT; i++) {
        led_base[i] = 0;  // Apagar todos los LEDs
    }
}

void startGame() {
    snake[0].x = rand() % (LED_MATRIX_0_WIDTH - 1);
    snake[0].y = rand() % (LED_MATRIX_0_HEIGHT - 1);

    // Ajustar coordenadas a valores pares
    if (snake[0].x % 2 != 0) { snake[0].x--; }
    if (snake[0].y % 2 != 0) { snake[0].y--; }

    snakeLength = 2;        // Iniciar longitud de la serpiente
    gameDelay = INITIAL_DELAY;  // Iniciar velocidad del juego
    spawnApple();
    juegoActivo = 1;        
}

void updateSnake(int dx, int dy) {
    clearTail();  // Limpiar la posición anterior de la cola

    // Mover segmentos de la cola hacia adelante
    for (int i = snakeLength - 1; i > 0; i--) {
        snake[i] = snake[i - 1];
    }

    // Mover la cabeza en la dirección especificada
    snake[0].x += dx;
    snake[0].y += dy;

    // Verificar colisiones
    checkCollisionWithBorders();
    if (juegoActivo) {
        checkCollisionWithSelf();
    }

    // Verificar si la serpiente come la manzana
    if (juegoActivo && snake[0].x == appleX && snake[0].y == appleY) {
        if (snakeLength < MAX_SNAKE_LENGTH) {
            snakeLength++;  // Aumentar longitud de la serpiente
        }

        // Incrementar velocidad reduciendo el retardo
        if (gameDelay > MIN_DELAY) {
            gameDelay -= SPEED_INCREMENT;
        }

        spawnApple();
    }

    if (juegoActivo) {
        drawHead();  // Dibujar la nueva cabeza
    }
}

void main() {
    int dx = 2, dy = 0;  // Dirección inicial de la serpiente

    while (1) {
        if (*switch_0 == 1) {
            if (!juegoActivo) {
                startGame();  // Iniciar el juego si está detenido
            }
        } else {
            if (juegoActivo) {
                gameOver();  // Detener el juego si el switch se desactiva
            }
        }

        if (juegoActivo) {
            if (*d_pad_up && dy == 0) { dx = 0; dy = -2; }
            else if (*d_pad_do && dy == 0) { dx = 0; dy = 2; }
            else if (*d_pad_le && dx == 0) { dx = -2; dy = 0; }
            else if (*d_pad_ri && dx == 0) { dx = 2; dy = 0; }

            updateSnake(dx, dy);  // Actualizar la posición de la serpiente
        }

        // Retardo controlado por la velocidad del juego
        for (int i = 0; i <= gameDelay; i++);
    }
}