#pragma once

#include "md407/types.h"
#include "md407/keypad.h"
#include "md407/display.h"
#include "md407/gpio.h"
#include "md407/time.h"
#include "md407/lcd.h"

#define PADDLE_SPEED 1
#define BALL_SPEED (float) 0.3
#define MAX_SCORE 9
#define DELAY_TIME 10

static LCD lcd;
static Keypad keypad_player1, keypad_player2;

typedef struct {
    uint8_t x;
    uint8_t y;
}   Point;

typedef struct {
	float dx;
	float dy;
} Speed;

typedef struct {
	uint32_t width;
	uint32_t height;
} Hitbox;

typedef struct {
	Point *points;
	size_t size;
    Hitbox hitbox;
} Model;


typedef struct{
	Point position;
	Model model; // Constant
	Speed speed;
} Entity, *EntityPtr;


typedef struct {
    Keypad* keypad;
    Entity paddle;
} Controller, *ControllerPtr;

typedef struct {
    Controller controller;
    uint16_t score;
} Player, *PlayerPtr;


typedef struct {
	Entity ball;
    Player player1;
    Player player2;
	boolean latest_hit;
} Game, *GamePtr;


static const Model ball_model = {
    .points =
        (Point[]){
            {1, 0},
            {2, 0},
            {0, 1},
            {1, 1},
            {2, 1},
            {3, 1},
            {0, 2},
            {1, 2},
            {2, 2},
            {3, 2},
            {1, 3},
            {2, 3},

        },
    .size = 12,
    .hitbox = {4, 4}
};

static const Point DISPLAY_SIZE = 
    (Point){
        .x = 128,
        .y = 64
    };
static const Model paddle_model = {
    .points =
        (Point[]){
            {0, 0}, {1, 0}, {2, 0}, {3, 0}, {4, 0}, {0, 1}, {4, 1},
            {0, 2}, {4, 2}, {0, 3}, {2, 3}, {4, 3}, {0, 4}, {2, 4},
            {4, 4}, {0, 5}, {2, 5}, {4, 5}, {0, 6}, {4, 6}, {0, 7},
            {4, 7}, {0, 8}, {1, 8}, {2, 8}, {3, 8}, {4, 8},
        },
    .size = 27,
    .hitbox = {5, 9}
};