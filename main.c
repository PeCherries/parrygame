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
               // break;
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
    const unsigned char blank_tile[16] = {
    0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00
    };

    // Ensure blank tile 0 exists
    set_bkg_data(0, 1, blank_tile);

    // Clear screen with blank tile
    fill_bkg_rect(0, 0, 32, 32, 0);

    // Load menu graphics
    set_bkg_data(1, menu_TILE_COUNT, menu_tiles); // shift menu tiles up by 1
    set_bkg_tiles(0, 0, 8, 6, menu_map);

    SHOW_BKG;
    move_bkg(5, 5);
}

void update_menu(void) {
    uint8_t keys = joypad();

    // Move cursor up
    if(keys & J_UP) {
        if(menu_index > 0) menu_index--;
        show_menu();
        delay(150); // debounce
    }

    // Move cursor down
    if(keys & J_DOWN) {
        if(menu_index < MENU_COUNT-1) menu_index++;
        show_menu();
        delay(150);
    }

    // Select option
    if(keys & J_START || keys & J_A) {
        // For now just print to screen
        fill_bkg_rect(0,0,20,18,0);
        gotoxy(3,8);
        printf("You chose: %s", menu_items[menu_index]);
        delay(500);
    }
}
