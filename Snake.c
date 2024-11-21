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
    int x, y;  // Coordenadas (x, y) del segmento de serpiente
} Segment;


Segment snake[MAX_SNAKE_LENGTH];  // Arreglo para almacenar los segmentos de la serpiente
int snakeLength = 1;              // Longitud inicial de la serpiente
int appleX, appleY;               // Coordenadas de la manzana
int juegoActivo = 0;              // Estado del juego

void clearTail();
void drawHead();
void drawApple();
void spawnApple();
void startGame();
void updateSnake(int dx, int dy);

// Borra la cola de la serpiente en la matriz LED
void clearTail() {
    Segment tail = snake[snakeLength - 1];  // Obtener el último segmento de la serpiente
    int tail_index = tail.y * LED_MATRIX_0_WIDTH + tail.x;
    // Apagar los LEDs que corresponden a la cola
    led_base[tail_index] = 0;
    led_base[tail_index + 1] = 0;
    led_base[tail_index + LED_MATRIX_0_WIDTH] = 0;
    led_base[tail_index + LED_MATRIX_0_WIDTH + 1] = 0;
}

// Dibuja la cabeza de la serpiente en la matriz LED
void drawHead() {
    Segment head = snake[0];  // Obtener la cabeza de la serpiente
    int head_index = head.y * LED_MATRIX_0_WIDTH + head.x;
    // Encender los LEDs que corresponden a la cabeza (color rojo)
    led_base[head_index] = 0xFF0000;
    led_base[head_index + 1] = 0xFF0000;
    led_base[head_index + LED_MATRIX_0_WIDTH] = 0xFF0000;
    led_base[head_index + LED_MATRIX_0_WIDTH + 1] = 0xFF0000;
}

// Dibuja la manzana en la matriz LED
void drawApple() {
    int apple_index = appleY * LED_MATRIX_0_WIDTH + appleX;
    // Encender los LEDs que corresponden a la manzana (color verde)
    led_base[apple_index] = 0x00FF00;
    led_base[apple_index + 1] = 0x00FF00;
    led_base[apple_index + LED_MATRIX_0_WIDTH] = 0x00FF00;
    led_base[apple_index + LED_MATRIX_0_WIDTH + 1] = 0x00FF00;
}

// Genera una nueva manzana en una posición aleatoria
void spawnApple() {
    appleX = rand() % (LED_MATRIX_0_WIDTH - 1);
    appleY = rand() % (LED_MATRIX_0_HEIGHT - 1);

    // Ajustar las coordenadas a valores pares para un tamaño 2x2
    if (appleX % 2 != 0) { appleX--; }
    if (appleY % 2 != 0) { appleY--; }

    // Verificar que la manzana no aparezca sobre la serpiente
    for (int i = 0; i < snakeLength; i++) {
        if (appleX == snake[i].x && appleY == snake[i].y) {
            spawnApple();  // Reintentar si si aparece sobre la serpiente
            return;
        }
    }

    drawApple();  // Dibujar la manzana en su nueva posición
}

void checkCollisionWithBorders() {
    Segment head = snake[0];
    if (head.x < 0 || head.y < 0 || 
        head.x >= LED_MATRIX_0_WIDTH || 
        head.y >= LED_MATRIX_0_HEIGHT) {
        gameOver();  // Detener el juego si la serpiente choca
    }
}

void gameOver() {
    juegoActivo = 0;  // Detener el juego
    for (int i = 0; i < LED_MATRIX_0_WIDTH * LED_MATRIX_0_HEIGHT; i++) {
        led_base[i] = 0;  // Apagar todos los LEDs
    }
}


void startGame() {
    // Generar posición inicial de la cabeza de la serpiente
    snake[0].x = rand() % (LED_MATRIX_0_WIDTH - 1);
    snake[0].y = rand() % (LED_MATRIX_0_HEIGHT - 1);

    // Ajustar las coordenadas iniciales a valores pares
    if (snake[0].x % 2 != 0) { snake[0].x--; }
    if (snake[0].y % 2 != 0) { snake[0].y--; }

    snakeLength = 2;  // Iniciar la longitud de la serpiente
    spawnApple();     // Generar la  manzana
    juegoActivo = 1;  
}

void updateSnake(int dx, int dy) {
    clearTail();  // Borrar la cola de la serpiente en la matriz LED

    // Mover todos los segmentos hacia adelante (cola sigue a la cabeza)
    for (int i = snakeLength - 1; i > 0; i--) {
        snake[i] = snake[i - 1];
    }

    // Mover la cabeza en la dirección especificada
    snake[0].x += dx;
    snake[0].y += dy;
    
    checkCollisionWithBorders();  // Verificar si choca con los bordes

    // Verificar si la serpiente come la manzana
    if (snake[0].x == appleX && snake[0].y == appleY) {
        if (snakeLength < MAX_SNAKE_LENGTH) {
            snakeLength++;  // Aumentar la longitud de la serpiente
        }
        spawnApple();  // Generar una nueva manzana
    }

    drawHead();  // Dibujar la nueva posición de la cabeza
}

void main() {
    int dx = 2, dy = 0;  // Dirección inicial de movimiento (derecha)

    while (1) {
        if (*switch_0 == 1) {
            juegoActivo = 0;  
            continue;         // Esperar a que el switch se desactive
        } else if (juegoActivo == 0) {
            startGame();  // Iniciar el juego si está detenido
        }

        if (juegoActivo) {
            if (*d_pad_up && dy == 0) { dx = 0; dy = -2; }
            else if (*d_pad_do && dy == 0) { dx = 0; dy = 2; }
            else if (*d_pad_le && dx == 0) { dx = -2; dy = 0; }
            else if (*d_pad_ri && dx == 0) { dx = 2; dy = 0; }

            updateSnake(dx, dy); 
        }

        // Retardo para controlar la velocidad del juego
        for (int i = 0; i <= 10000; i++);
    }
}
