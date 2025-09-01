#include <gb/gb.h>
#include <gbdk/font.h>
#include <stdio.h>
#include "assetts/splash.h"
#include "assetts/splash.c"
#include "assetts/menubg.h"
#include "assetts/menubg.c"
#include "assetts/spritemap-world.h"
#include "assetts/spritemap-world.c"
#include <gbdk/metasprites.h>


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
#define PLAYER_SPRITES 4
#define LAMP_SPRITES 8
#define MAP_HEIGHT 32
#define MAP_WIDTH 26
#define SCREEN_TILE_WIDTH  20
#define SCREEN_TILE_HEIGHT 18
#define SCREEN_WIDTH  (SCREEN_TILE_WIDTH * 8)
#define SCREEN_HEIGHT (SCREEN_TILE_HEIGHT * 8)

#define TILE_SIZE 8  // Game Boy tiles are 8x8


unsigned char collision_map[MAP_HEIGHT][MAP_WIDTH] = {
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
{1,1,0,0,0,0,0,0,0,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,1,1},
{1,1,0,0,0,0,0,0,0,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,1,1},
{1,1,0,0,1,1,1,0,0,1,1,1,1,1,1,1,0,0,1,1,1,1,0,0,1,1},
{1,1,0,0,1,1,1,0,0,1,1,1,1,1,1,1,0,0,1,1,1,1,0,0,1,1},
{1,1,0,0,1,1,1,0,0,1,1,1,1,1,1,1,0,0,1,1,1,1,0,0,1,1},
{1,1,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,0,0,1,1},
{1,1,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,0,0,1,1},
{1,1,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,0,0,1,1},
{1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1},
{1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1},
{1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1},
{1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1},
{1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1},
{1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1},
{1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1},
{1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1},
{1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1},
{1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1},
{1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1},
{1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1},
{1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1},
{1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,1,1},
{1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,1,1},
{1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,1,1},
{1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,1,1},
{1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1},
{1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1},
{1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1},
{ 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
};

uint8_t check_collision(uint8_t new_x, uint8_t new_y) {
    // Convert pixel coords to tile coords
    uint8_t tile_x = new_x / TILE_SIZE;
    uint8_t tile_y = new_y / TILE_SIZE;

    // Simple bounds check
    if(tile_x >= MAP_WIDTH || tile_y >= MAP_HEIGHT) return 1;

    // 1 = solid, 0 = empty
    return collision_map[tile_y][tile_x];
}


uint8_t menu_index = 0;
uint8_t x = 80;
uint8_t y = 130;
uint8_t player_direction;
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

#include <gbdk/metasprites.h>

// --- DOWN ---
const metasprite_t player_down_frame0[] = {
    {0, 0, 27, 0}, {8, 0, 35, 0},
    {-8, 8, 28, 0}, {8, 0, 36, 0},
    {metasprite_end}
};
const metasprite_t player_down_frame1[] = {
    {0, 0, 25, 0}, {8, 0, 33, 0},
    {-8, 8, 26, 0}, {8, 0, 34, 0},
    {metasprite_end}
};

// --- UP ---
// --- UP ---
const metasprite_t player_up_frame0[] = {
    {0, 0, 9, 0},   // top-left
    {8, 0, 17, 0},  // top-right
    {0, 8, 10, 0},  // bottom-left
    {8, 8, 18, 0},  // bottom-right
    {metasprite_end}
};

const metasprite_t player_up_frame1[] = {
    {0, 0, 11, 0},  // top-left
    {8, 0, 19, 0},  // top-right
    {0, 8, 12, 0},  // bottom-left
    {8, 8, 20, 0},  // bottom-right
    {metasprite_end}
};

// --- LEFT ---
const metasprite_t player_left_frame0[] = {
    {0, 0, 13, 0}, {8, 0, 21, 0},
    {-8, 8, 14, 0}, {8, 0, 22, 0},
    {metasprite_end}
};
const metasprite_t player_left_frame1[] = {
    {0, 0, 15, 0}, {8, 0, 23, 0},
    {-8, 8, 16, 0}, {8, 0, 24, 0},
    {metasprite_end}
};

// --- RIGHT ---
const metasprite_t player_right_frame0[] = {
    {0, 0, 29, 0}, {8, 0, 37, 0},
    {-8, 8, 30, 0}, {8, 0, 38, 0},
    {metasprite_end}
};
const metasprite_t player_right_frame1[] = {
    {0, 0, 31, 0}, {8, 0, 39, 0},
    {-8, 8, 32, 0}, {8, 0, 40, 0},
    {metasprite_end}
};





void main(void) {

    DISPLAY_ON;
    

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
            wait_vbl_done();
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
    delay(500);
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
        wait_vbl_done();
        init_level(); 
        current_state = STATE_LEVEL;
    }
    

    
}

void init_level(void) {
    // Load tiles
    set_bkg_data(0, spritemap_world_TILE_COUNT, spritemap_world_tiles);
    set_sprite_data(0, spritemap_world_TILE_COUNT, spritemap_world_tiles);

    // Fill map with 16x16 blocks
    for(uint8_t by = 0; by < MAP_HEIGHT; by += 2) {
        for(uint8_t bx = 0; bx < MAP_WIDTH; bx += 2) {
            put_16x16_block(bx, by, 0);
        }
    }

    // Set player bottom-middle
    x = (MAP_WIDTH * TILE_SIZE) / 2;
    y = (MAP_HEIGHT * TILE_SIZE) - 16;

    player_direction = PLAYER_DIRECTION_DOWN;
    player_frame = 0;

    

    SHOW_BKG;
    SHOW_SPRITES;

    // Camera: center player
    int cam_x = x - SCREEN_WIDTH / 2;
    int cam_y = y - SCREEN_HEIGHT / 2;

    // Clamp to map bounds
    if(cam_x < 0) cam_x = 0;
    if(cam_y < 0) cam_y = 0;
    if(cam_x > (MAP_WIDTH * TILE_SIZE - SCREEN_WIDTH)) cam_x = MAP_WIDTH * TILE_SIZE - SCREEN_WIDTH;
    if(cam_y > (MAP_HEIGHT * TILE_SIZE - SCREEN_HEIGHT)) cam_y = MAP_HEIGHT * TILE_SIZE - SCREEN_HEIGHT;
    move_metasprite(player_down_frame0, 0,0, x - cam_x, y - cam_y);
    move_bkg(cam_x, cam_y);
}

// Call this every frame
void update_level(void) {
    wait_vbl_done();
    uint8_t keys = joypad();
    is_player_walking = 0;

    int next_x = x;
    int next_y = y;

    int cam_x = x - SCREEN_WIDTH / 2;
    int cam_y = y - SCREEN_HEIGHT / 2;

    if(cam_x < 0) cam_x = 0;
    if(cam_y < 0) cam_y = 0;
    if(cam_x > (MAP_WIDTH * TILE_SIZE - SCREEN_WIDTH)) cam_x = MAP_WIDTH * TILE_SIZE - SCREEN_WIDTH;
    if(cam_y > (MAP_HEIGHT * TILE_SIZE - SCREEN_HEIGHT)) cam_y = MAP_HEIGHT * TILE_SIZE - SCREEN_HEIGHT;

    move_bkg(cam_x, cam_y);

    if(keys & J_UP)    { next_y--; player_direction = PLAYER_DIRECTION_UP;    is_player_walking = 1; }
    if(keys & J_DOWN)  { next_y++; player_direction = PLAYER_DIRECTION_DOWN;  is_player_walking = 1; }
    if(keys & J_LEFT)  { next_x--; player_direction = PLAYER_DIRECTION_LEFT;  is_player_walking = 1; }
    if(keys & J_RIGHT) { next_x++; player_direction = PLAYER_DIRECTION_RIGHT; is_player_walking = 1; }

    if(!check_collision(next_x, next_y)) {
        x = next_x;
        y = next_y;
    }

    if(is_player_walking) {
        frame_skip--;
        if(frame_skip < 1) {
            player_frame ^= 1;
            frame_skip = 6;
        }
    }

    switch(player_direction) {
        case PLAYER_DIRECTION_DOWN:
            move_metasprite(player_frame == 0 ? player_down_frame0 : player_down_frame1, 0, 0, x - cam_x, y - cam_y);
            break;
        case PLAYER_DIRECTION_UP:
            move_metasprite(player_frame == 0 ? player_up_frame0 : player_up_frame1, 0, 0, x - cam_x, y - cam_y);
            break;
        case PLAYER_DIRECTION_LEFT:
            move_metasprite(player_frame == 0 ? player_left_frame0 : player_left_frame1, 0, 0, x - cam_x, y - cam_y);
            break;
        case PLAYER_DIRECTION_RIGHT:
            move_metasprite(player_frame == 0 ? player_right_frame0 : player_right_frame1, 0, 0, x - cam_x, y - cam_y);
            break;
    }

    
}


void put_16x16_block(uint8_t x, uint8_t y, uint8_t logical_index) {
    uint8_t base_tile = logical_index * 4;

    unsigned char block[4] = {
        base_tile, base_tile + 1,
        base_tile + 6, base_tile + 7
    };

    set_bkg_tiles(x, y, 2, 2, block);
}
