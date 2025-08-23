#include <gb/gb.h>
#include <gbdk/font.h>
#include <stdio.h>
#include "assetts/splash.h"
#include "assetts/splash.c"
#include "assetts/menubg.h"
#include "assetts/menubg.c"
#include "assetts/spritemap-world.h"
#include "assetts/spritemap-world.c"


// Game states
typedef enum {
    STATE_SPLASH,
    STATE_MENU,
    STATE_LEVEL
} GameState;

GameState current_state = STATE_SPLASH;

// Menu data
const char *menu_items[] = {
    "NEW GAME",
    "LOAD GAME",
    "HIGH SCORES",
    "ABOUT"
};
#define MENU_COUNT 3
uint8_t menu_index = 0;

// Function declarations
void show_splash(void);
void update_splash(void);
void show_menu(void);
void update_menu(void);
void init_level(void);
void gotoxy(int x, int y);
void update_level(void);
void put_16x16_block(uint8_t x, uint8_t y, uint8_t base_tile);

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

// -------- MAIN LOOP --------
void main(void) {

    DISPLAY_ON;
    SHOW_BKG;

    // Init fonts ONCE here
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


// -------- SPLASH SCREEN --------
void show_splash(void) {
    // Load splash background tiles
    set_bkg_data(0, splash_TILE_COUNT, splash_tiles);
    set_bkg_tiles(0, 0, 20, 18, splash_map);  // 20x18 tiles for 160x144
    SHOW_BKG;
    // Position the background
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


// -------- MENU --------
void show_menu(void) {

    // Load menu graphics
    set_bkg_data(0, menubg_TILE_COUNT, menubg_tiles);
    set_bkg_tiles(0, 0, 20,18, menubg_map);
    SHOW_BKG;
    // Load cursor sprite
    set_sprite_data(0, 1, cursor_tile);
    set_sprite_tile(0, 0);  // sprite 0 = cursor
    

    // Position cursor at first item
    menu_index = 0;
    //move_sprite(0, 150, 120); // x=16, y=48 start
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
    // Load tileset for world
    set_bkg_data(0, 16, spritemap_world_tiles);
    set_sprite_data(0, spritemap_world_TILE_COUNT, spritemap_world_tiles);

    // Fill background with 16x16 block #0
    for(uint8_t y = 0; y < 18; y += 2) {
        for(uint8_t x = 0; x < 20; x += 2) {
            put_16x16_block(x, y, 0); // base tile index = 0
        }
    }

    // Example: set sprite 0 to use tile 3 (top-left corner of second 16x16 block)
    set_sprite_tile(0, 25);
    set_sprite_tile(1, 26);
    set_sprite_tile(2, 34);
    set_sprite_tile(3, 35);
    uint8_t x = 80;
    uint8_t y = 130;
    move_sprite(0, x, y);       
    move_sprite(1, x+8, y);     
    move_sprite(2, x, y+8);     
    move_sprite(3, x+8, y+8);   

    SHOW_SPRITES;
    SHOW_BKG;
}

void update_level(void){

}

void put_16x16_block(uint8_t x, uint8_t y, uint8_t logical_index) {
    uint8_t base_tile = logical_index * 4;

    unsigned char block[4] = {
        base_tile, base_tile + 1,
        base_tile + 6, base_tile + 7
    };

    set_bkg_tiles(x, y, 2, 2, block);
}