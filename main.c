#include <gb/gb.h>
#include <gbdk/font.h>
#include <stdio.h>
#include "assetts/splash.h"
#include "assetts/splash.c"
#include "assetts/menubg.h"
#include "assetts/menubg.c"
#include "assetts/spritemap-world.h"
#include "assetts/spritemap-world.c"



typedef enum {
    STATE_SPLASH,
    STATE_MENU,
    STATE_LEVEL
} GameState;

GameState current_state = STATE_SPLASH;


const char *menu_items[] = {
    "NEW GAME",
    "LOAD GAME",
    "HIGH SCORES",
    "ABOUT"
};
#define MENU_COUNT 3
#define PLAYER_DIRECTION_DOWN  0
#define PLAYER_DIRECTION_UP    6
#define PLAYER_DIRECTION_RIGHT 12
#define PLAYER_DIRECTION_LEFT  18

uint8_t menu_index = 0;
uint8_t x = 80;
uint8_t y = 130;
uint8_t player_direction;
uint8_t player_animation_frame;
uint8_t is_player_walking;
uint8_t frame_skip = 8;
uint8_t player_frame = 0;


void show_splash(void);
void update_splash(void);
void show_menu(void);
void update_menu(void);
void init_level(void);
void gotoxy(int x, int y);
void update_level(void);
void put_16x16_block(uint8_t x, uint8_t y, uint8_t base_tile);
uint8_t player_animation(uint8_t player_direction, uint8_t player_frame);

const unsigned char cursor_tile[16] = {
    0x18,0x18,
    0x1C,0x1C,
    0x1E,0x1E,
    0x1C,0x1C,
    0x18,0x18,
    0x00,0x00,
    0x00,0x00,
    0x00,0x00
};

void main(void) {

    DISPLAY_ON;
    SHOW_BKG;

    font_init();
    font_set(font_load(font_ibm));

    current_state = STATE_SPLASH;
    show_splash();

    while(1) {
        switch(current_state) {
            case STATE_SPLASH:
                update_splash();
                break;
            case STATE_MENU:
                update_menu();
                break;
            case STATE_LEVEL:
                update_level();
                break;
        }
        wait_vbl_done(); // sync with display
    }
}



void show_splash(void) {
    set_bkg_data(0, splash_TILE_COUNT, splash_tiles);
    set_bkg_tiles(0, 0, 20, 18, splash_map);  
    SHOW_BKG;

    move_bkg(0, 0);
}

void update_splash(void) {
    if(joypad() & J_START ) {  
        if (current_state==STATE_SPLASH){
            delay(100);
            show_menu();
        current_state = STATE_MENU; 
        }
    }
}



void show_menu(void) {


    set_bkg_data(0, menubg_TILE_COUNT, menubg_tiles);
    set_bkg_tiles(0, 0, 20,18, menubg_map);
    SHOW_BKG;

    set_sprite_data(0, 1, cursor_tile);
    set_sprite_tile(0, 0);  
    
    menu_index = 0;
    move_bkg(0, 0);
    move_sprite(0, 50, 67 + menu_index * 16);
    SHOW_SPRITES;
    

    current_state = STATE_MENU;
}

void update_menu(void) {

    uint8_t keys = joypad();

    if(keys & J_UP) {
        if(menu_index > 0) menu_index--;
        move_sprite(0, 50, 67 + menu_index * 16);
        delay(150);
    }
    if(keys & J_DOWN) {
        if(menu_index < MENU_COUNT-1) menu_index++;
        move_sprite(0, 50, 67 + menu_index * 16);
        delay(150);
    }
    if(keys & (J_START | J_A)) {
        init_level(); 
        current_state = STATE_LEVEL;
    }
    

    
}

void init_level(void) {

    set_bkg_data(0, 16, spritemap_world_tiles);
    set_sprite_data(0, spritemap_world_TILE_COUNT, spritemap_world_tiles);


    for(uint8_t y = 0; y < 18; y += 2) {
        for(uint8_t x = 0; x < 20; x += 2) {
            put_16x16_block(x, y, 0); 
        }
    }

    set_sprite_tile(0, 25);
    set_sprite_tile(1, 26);
    set_sprite_tile(2, 34);
    set_sprite_tile(3, 35);
    move_sprite(0, x, y);       
    move_sprite(1, x+8, y);     
    move_sprite(2, x, y+8);     
    move_sprite(3, x+8, y+8);   
    wait_vbl_done();
    SHOW_SPRITES;
    SHOW_BKG;
}

void update_level(void){
    wait_vbl_done();
    uint8_t keys = joypad();
        if (keys & J_UP) {
            player_direction = PLAYER_DIRECTION_UP;
            is_player_walking = 1;
        } else if (keys & J_DOWN) {
            player_direction = PLAYER_DIRECTION_DOWN;
            is_player_walking = 1;
        } else if (keys & J_LEFT) {
            player_direction = PLAYER_DIRECTION_LEFT;
            is_player_walking = 1;
        } else if (keys & J_RIGHT) {
            player_direction = PLAYER_DIRECTION_RIGHT;
            is_player_walking = 1;
        } else {
            is_player_walking = 0;
            frame_skip = 1; 
        }

        // Update the player position if it is walking
        if (is_player_walking) {
            if (player_direction == PLAYER_DIRECTION_RIGHT) x += 1;
            else if (player_direction == PLAYER_DIRECTION_LEFT) x -= 1;
            else if (player_direction == PLAYER_DIRECTION_UP) y -= 1;
            else if (player_direction == PLAYER_DIRECTION_DOWN) y += 1;
                move_sprite(0, x, y);       
                move_sprite(1, x+8, y);     
                move_sprite(2, x, y+8);     
                move_sprite(3, x+8, y+8);  

                frame_skip -= 1;
            if (frame_skip < 1){
                player_frame = player_animation(player_direction,player_frame);
                frame_skip = 6;
            }
            
}}

void put_16x16_block(uint8_t x, uint8_t y, uint8_t logical_index) {
    uint8_t base_tile = logical_index * 4;

    unsigned char block[4] = {
        base_tile, base_tile + 1,
        base_tile + 6, base_tile + 7
    };

    set_bkg_tiles(x, y, 2, 2, block);
}

uint8_t player_animation(uint8_t player_direction,uint8_t player_frame){
    if(player_direction==PLAYER_DIRECTION_LEFT){
        if(player_frame==1){
            player_frame = 0;
            // tile index = column + row * witdth of tilemap
                set_sprite_tile(0,13);
                set_sprite_tile(1, 14);
                set_sprite_tile(2, 21);
                set_sprite_tile(3, 22);
        }
        else{player_frame = 1;
                set_sprite_tile(0, 15);
                set_sprite_tile(1, 16);
                set_sprite_tile(2, 23);
                set_sprite_tile(3, 24);
    }

}
if(player_direction==PLAYER_DIRECTION_RIGHT){
        if(player_frame==1){
            player_frame = 0;
                set_sprite_tile(0,29);
                set_sprite_tile(1, 30);
                set_sprite_tile(2, 37);
                set_sprite_tile(3, 38);
        }
        else{player_frame = 1;
                set_sprite_tile(0, 31);
                set_sprite_tile(1, 32);
                set_sprite_tile(2, 39);
                set_sprite_tile(3, 40);
    }

}
if(player_direction==PLAYER_DIRECTION_UP){
        if(player_frame==1){
            player_frame = 0;
                set_sprite_tile(0,9);
                set_sprite_tile(1, 10);
                set_sprite_tile(2, 17);
                set_sprite_tile(3, 18);
        }
        else{player_frame = 1;
                set_sprite_tile(0, 11);
                set_sprite_tile(1, 12);
                set_sprite_tile(2, 19);
                set_sprite_tile(3, 20);
    }

}
if(player_direction==PLAYER_DIRECTION_DOWN){
        if(player_frame==1){
            player_frame = 0;
                set_sprite_tile(0,25);
                set_sprite_tile(1, 26);
                set_sprite_tile(2, 33);
                set_sprite_tile(3, 34);
        }
        else{player_frame = 1;
                set_sprite_tile(0, 27);
                set_sprite_tile(1, 28);
                set_sprite_tile(2, 35);
                set_sprite_tile(3, 36);
    }

}
return player_frame;
}