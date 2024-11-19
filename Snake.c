#include "ripes_system.h"
#include <stdio.h>
#include <stdlib.h>

volatile unsigned int *led_base = (unsigned int *)LED_MATRIX_0_BASE;
volatile unsigned int *d_pad_up = (unsigned int *)D_PAD_0_UP;
volatile unsigned int *d_pad_do = (unsigned int *)D_PAD_0_DOWN;
volatile unsigned int *d_pad_le = (unsigned int *)D_PAD_0_LEFT;
volatile unsigned int *d_pad_ri = (unsigned int *)D_PAD_0_RIGHT;

int snakeHeadx, snakeHeady;
int prevsnakex, prevsnakey;

void startGame() {
    snakeHeadx = rand() % (LED_MATRIX_0_WIDTH - 1);
    snakeHeady = rand() % (LED_MATRIX_0_HEIGHT - 1);
    prevsnakex = snakeHeadx;
    prevsnakey = snakeHeady;

    createSnake();
}

void createSnake() {
    int prev_index = prevsnakey * LED_MATRIX_0_WIDTH + prevsnakex;
    led_base[prev_index] = 0;
    led_base[prev_index + 1] = 0;
    led_base[prev_index + LED_MATRIX_0_WIDTH] = 0;
    led_base[prev_index + LED_MATRIX_0_WIDTH + 1] = 0;

    int head_index = snakeHeady * LED_MATRIX_0_WIDTH + snakeHeadx;
    led_base[head_index] = 0xFF0000;
    led_base[head_index + 1] = 0xFF0000;
    led_base[head_index + LED_MATRIX_0_WIDTH] = 0xFF0000;
    led_base[head_index + LED_MATRIX_0_WIDTH + 1] = 0xFF0000;

    prevsnakex = snakeHeadx;
    prevsnakey = snakeHeady;
}


void updateSnake(int x, int y) {
    snakeHeadx += x;
    snakeHeady += y;
}

void main() {
    int x = 2, y = 0;  
    startGame();
    
    while (1) {
        if (*d_pad_up && y == 0) { x = 0; y = -2; }  
        else if (*d_pad_do && y == 0) { x = 0; y = 2; } 
        else if (*d_pad_le && x == 0) { x = -2; y = 0; } 
        else if (*d_pad_ri && x == 0) { x = 2; y = 0; }  

        updateSnake(x, y);
        createSnake();

        for (int i = 0; i <= 10000; i++);
    }
}
