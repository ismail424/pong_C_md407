#include "startup.h"
#include "pong.h"

void draw_entity(Model model, Point position) {
	for (int i = 0; i < model.size; i++) {
		display_set_pixel(model.points[i].x + position.x,
		                  model.points[i].y + position.y, true);
	}
}

void clear_entity(Model model, Point position) {
	for (int i = 0; i < model.size; i++) {
		display_set_pixel(model.points[i].x + position.x,
		                  model.points[i].y + position.y, false);
	}
}

void check_bounds_ball(Entity *entity) {
	uint32_t new_x_pos = entity->position.x + (uint32_t) entity->speed.dx;
	uint32_t new_y_pos = entity->position.y + (uint32_t) entity->speed.dy;

	if (new_x_pos > DISPLAY_SIZE.x - 1 || new_x_pos < 1) {
		entity->speed.dx = -entity->speed.dx;
		new_x_pos        = entity->position.x + entity->speed.dx;
	}

	if (new_y_pos > DISPLAY_SIZE.y - 2 || new_y_pos < 1) {
		entity->speed.dy = -entity->speed.dy;
		new_y_pos        = entity->position.y + entity->speed.dy;
	}
}

void check_ball_paddle_collision(Entity *ball, Entity *paddle1, Entity *paddle2, boolean *latest_hit) {
	uint32_t new_yball = ball->position.y + ball->speed.dy;
	uint32_t new_xball = ball->position.x + ball->speed.dy;

	uint32_t paddle1_top_pos = paddle1->position.y;
	uint32_t paddle1_bot_pos = paddle1->position.y + paddle1->model.hitbox.height;
	
	uint32_t paddle2_top_pos = paddle2->position.y;
	uint32_t paddle2_bot_pos = paddle2->position.y + paddle2->model.hitbox.height;


	// Logic for paddle 1
	if (new_yball + ball->model.hitbox.height >= paddle1_top_pos
	    && new_yball <= paddle1_bot_pos
	    && new_xball <= paddle1->position.x + paddle1->model.hitbox.width
	    && *latest_hit == true) {
		ball->speed.dx = -ball->speed.dx;
		ball->speed.dx += BALL_SPEED;
		*latest_hit = false;
	}

	// Logic for paddle 2
	if (new_yball + ball->model.hitbox.height >= paddle2_top_pos
	    && new_yball <= paddle2_bot_pos
	    && new_xball + ball->model.hitbox.width >= paddle2->position.x
		&& *latest_hit == false) {		
		ball->speed.dx = -ball->speed.dx;
		ball->speed.dx -= BALL_SPEED;
		*latest_hit = true;
	}
}

void check_bounds_paddle(Entity *entity) {
	uint32_t new_y_pos = entity->position.y + entity->speed.dy;
	if (new_y_pos + entity->model.hitbox.height >= DISPLAY_SIZE.y) {
		entity->position.y = DISPLAY_SIZE.y - entity->model.hitbox.height - 2;
	}
	if (new_y_pos <= 1) {
		entity->position.y = 3;
	}
}

boolean check_if_scored(Game *game) {

	// check if player 1 scored
	if( game->ball.position.x > game->player2.controller.paddle.position.x + game->player2.controller.paddle.model.hitbox.width) {
		game->player1.score++;
		return true;
	}
	// check if player 2 scored
	if (game->ball.position.x < game->player1.controller.paddle.position.x) {
		game->player2.score++;
		return true;
	}

	return false;
}

boolean check_lose(Game *game) {
	if (game->player1.score >= MAX_SCORE ){
		lcd_puts(&lcd, 0,1, "              ");
		lcd_puts(&lcd, 0,0, "Player 1 wins!");
		return true;
	}
	
	if (game->player2.score >= MAX_SCORE ){
		lcd_puts(&lcd, 0,1, "              ");
		lcd_puts(&lcd, 0,0, "Player 2 wins!");
		return true;
	}
	return false;
}


void new_round(Game *game) {

	game->ball.position 					 = (Point) {(uint8_t) 64, 32};
	game->latest_hit   						 = false;
	game->ball.speed   						 = (Speed) {1, 1};
	game->player1.controller.paddle.position = (Point) {(uint8_t) 10, (uint8_t) 32};
	game->player2.controller.paddle.position = (Point) {(uint8_t) 115, (uint8_t) 32};
}

void render_entity(Entity *entity) {
	clear_entity(entity->model, entity->position);
	entity->position.x = entity->position.x + entity->speed.dx;
	entity->position.y = entity->position.y + entity->speed.dy;
	draw_entity(entity->model, entity->position);
}

void pong_update(Game *game) {
	// Check bounds and render ball
	check_bounds_ball(&game->ball);
	check_bounds_paddle(&game->player1.controller.paddle);
	check_bounds_paddle(&game->player2.controller.paddle);

	// Check paddle collision
	check_ball_paddle_collision(&game->ball, 
								&game->player1.controller.paddle,
	                            &game->player2.controller.paddle,
								&game->latest_hit);

	//Check if scored
	if (check_if_scored(game)) {
		display_clear( &lcd );
		new_round( game );
		display_score(game->player1.score, game->player2.score);
	}

	
	// Render entities
	render_entity(&game->ball);
	render_entity(&game->player1.controller.paddle);
	render_entity(&game->player2.controller.paddle);
}



void pong_init(Game *game) {
	Entity paddle1 = {
	    .model    = paddle_model,
	    .position = {10, 32},
	    .speed    = {0, 0},
	};

	Entity paddle2 = {
	    .model    = paddle_model,
	    .position = {115, 32},
	    .speed    = {0, 0},
	};

	Entity ball = {
	    .model    = ball_model,
	    .position = {64, 32},
	    .speed    = {1, 1},
	};
	
	lcd_connect(&lcd, GPIO_E);
	lcd_puts(&lcd, 0, 0, "Loading...");  // fixes a weird bug
	
	/* init keypad */
	keypad_connect(&keypad_player1, GPIO_D, false);
	keypad_connect(&keypad_player2, GPIO_D, true);
	
	game->ball    					= ball;
	game->player1.controller.paddle = paddle1;
	game->player2.controller.paddle = paddle2;
	game->player1.controller.keypad = &keypad_player1;
	game->player2.controller.keypad = &keypad_player2;
	game->player1.score             = 0;
	game->player2.score             = 0;
	game->latest_hit 			  	= 0;
	
	draw_entity(ball.model, ball.position);
	draw_entity(paddle1.model, paddle1.position);
	draw_entity(paddle2.model, paddle2.position);

	display_score( game->player1.score, game->player2.score );
}


void display_score(uint16_t player1_score, uint16_t player2_score ) {
	char player1_score_str[12] =  "Player 1:  ";
	char player2_score_str[12] =  "Player 2:  ";	
	player1_score_str[10] = player1_score + '0';
	player2_score_str[10] = player2_score + '0';
	lcd_puts(&lcd, 0, 0, player1_score_str);
	lcd_puts(&lcd, 0, 1, player2_score_str);
}

void player_movement(Player *player) {
	char key = keypad_read(player->controller.keypad);
	switch (key) {
	case '2':
		player->controller.paddle.speed.dy = -1;
		break;
	case '8':
		player->controller.paddle.speed.dy = 1;
		break;
	default:
		player->controller.paddle.speed.dy = 0;
		break;
	}
}

int main(void) {
	    while (true) {
        Game game;
        boolean isRunning = true;
        display_clear();
        pong_init(&game);
        while (isRunning) {
            player_movement(&game.player1);
            player_movement(&game.player2);
            if (check_lose(&game)) {
                delay_milli(1);
                isRunning = false;
                break;
            }
            pong_update(&game);
            delay_micro(DELAY_TIME);
        }
    }
}


