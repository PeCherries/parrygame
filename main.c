#include <gb/gb.h>
#include <gbdk/font.h>
#include <stdio.h>
#include "assetts/splash.h"
#include "assetts/splash.c"
#include "assetts/menu.h"
#include "assetts/menu.c"


// Game states
typedef enum {
    STATE_SPLASH,
    STATE_MENU
} GameState;

GameState current_state = STATE_SPLASH;

// Menu data
const char *menu_items[] = {
    "NEW GAME",
    "LOAD GAME",
    "HIGH SCORES",
    "ABOUT"
};
#define MENU_COUNT 4
uint8_t menu_index = 0;

// Function declarations
void show_splash(void);
void update_splash(void);
void show_menu(void);
void update_menu(void);
void gotoxy(int x, int y);

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
    if(joypad() & J_START) {  
        current_state = STATE_MENU; 
        show_menu();
    }
}


// -------- MENU --------
void show_menu(void) {
    // Blank tile 0
    const unsigned char blank_tile[16] = {
    0x00,0x00,
    0x00,0x00,  
    0x00,0x00,  
    0x00,0x00,  
    0x00,0x00,  
    0x00,0x00,  
    0x00,0x00,  
    0x00,0x00   
};

    set_bkg_data(0, 1, blank_tile);

    // Clear screen
    fill_bkg_rect(0, 0, 32, 32, 0);
    uint8_t shifted_map[8*6];
    // this is to offset stuff by 1 because then it can align ewith memory the right way?
for (uint8_t i = 0; i < 8*6; i++) {
    shifted_map[i] = menu_map[i] + 1;
}

    // Load menu graphics
    set_bkg_data(1, menu_TILE_COUNT, menu_tiles);
    set_bkg_tiles(6, 6, 8, 6, shifted_map);

    // Load cursor sprite
    set_sprite_data(0, 1, cursor_tile);
    set_sprite_tile(0, 0);  // sprite 0 = cursor
    SHOW_SPRITES;

    // Position cursor at first item
    menu_index = 0;
    move_sprite(0, 16, 48 + menu_index * 16); // x=16, y=48 start
    move_bkg(5, 5);
    SHOW_BKG;
    

    current_state = STATE_MENU;
}

void update_menu(void) {
    uint8_t keys = joypad();

    if(keys & J_UP) {
        if(menu_index > 0) menu_index--;
        delay(150);
    }
    if(keys & J_DOWN) {
        if(menu_index < MENU_COUNT-1) menu_index++;
        delay(150);
    }

    // Move cursor sprite instead of redrawing background
    move_sprite(0, 16, 48 + menu_index * 16);

    if(keys & (J_START | J_A)) {
        // Just as a placeholder
        printf("You chose: %s", menu_items[menu_index]);
        delay(500);
    }
}
