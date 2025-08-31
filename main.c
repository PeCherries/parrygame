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

typedef struct {
    int8_t dx;
    int8_t dy;
    uint8_t tile;
    uint8_t prop;
} metasprite_t;

#define MENU_COUNT 3
#define PLAYER_DIRECTION_DOWN  0
#define PLAYER_DIRECTION_UP    6
#define PLAYER_DIRECTION_RIGHT 12
#define PLAYER_DIRECTION_LEFT  18
#define PLAYER_SPRITES 4
#define LAMP_SPRITES 8
#define MAP_HEIGHT 32
#define MAP_WIDTH 26

#define TILE_SIZE 8  // Game Boy tiles are 8x8

uint8_t check_collision(uint8_t new_x, uint8_t new_y) {
    // Convert pixel coords to tile coords
    uint8_t tile_x = new_x / TILE_SIZE;
    uint8_t tile_y = new_y / TILE_SIZE;

    // Simple bounds check
    if(tile_x >= MAP_WIDTH || tile_y >= MAP_HEIGHT) return 1;

    // 1 = solid, 0 = empty
    return collision_map[tile_y][tile_x];
}

void move_metasprite(uint8_t sprite_index, const metasprite_t *ms, uint8_t x, uint8_t y, uint8_t flip) {
    for(uint8_t i=0; ms[i].tile != 0xFF; i++) {
        move_sprite(sprite_index+i, x + ms[i].dx, y + ms[i].dy);
        set_sprite_tile(sprite_index+i, ms[i].tile);
        set_sprite_prop(sprite_index+i, ms[i].prop | flip);
    }
}

#define metasprite_end {0,0,0,0xFF}

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

#include <gbdk/metasprites.h>

const metasprite_t player_down[] = {
    {0, 0, 27, 0},    // top-left
    {8, 0, 28, 0},    // top-right
    {0, 8, 35, 0},    // bottom-left
    {8, 8, 36, 0},    // bottom-right
    metasprite_end  // end marker
};

const metasprite_t player_up[] = {
    {0, 0, 9, 0},
    {8, 0, 10, 0},
    {0, 8, 17, 0},
    {8, 8, 18, 0},
    metasprite_end
};

const metasprite_t player_left[] = {
    {0, 0, 13, 0},
    {8, 0, 14, 0},
    {0, 8, 21, 0},
    {8, 8, 22, 0},
    metasprite_end
};

const metasprite_t player_right[] = {
    {0, 0, 29, 0},
    {8, 0, 30, 0},
    {0, 8, 37, 0},
    {8, 8, 38, 0},
    metasprite_end
};


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
    set_bkg_data(0, spritemap_world_TILE_COUNT, spritemap_world_tiles);
    set_sprite_data(0, spritemap_world_TILE_COUNT, spritemap_world_tiles);

    // Fill map with blocks
    for(uint8_t by = 0; by < 32; by += 2) {
        for(uint8_t bx = 0; bx < 32; bx += 2) {
            put_16x16_block(bx, by, 0);
        }
    }

    // Set initial player position
    x = 80;
    y = 130;
    player_direction = PLAYER_DIRECTION_DOWN;
    player_frame = 0;

    move_metasprite(player_down, 0, x, y,0);

    SHOW_SPRITES;
    SHOW_BKG;
    scroll_bkg(48, 112);
}


void update_level(void){
    wait_vbl_done();
    uint8_t keys = joypad();
    is_player_walking = 0;

    uint8_t next_x = x;
    uint8_t next_y = y;

    if(keys & J_UP)    { next_y--; player_direction = PLAYER_DIRECTION_UP; is_player_walking = 1; }
    if(keys & J_DOWN)  { next_y++; player_direction = PLAYER_DIRECTION_DOWN; is_player_walking = 1; }
    if(keys & J_LEFT)  { next_x--; player_direction = PLAYER_DIRECTION_LEFT; is_player_walking = 1; }
    if(keys & J_RIGHT) { next_x++; player_direction = PLAYER_DIRECTION_RIGHT; is_player_walking = 1; }

    // Collision check
    if(!check_collision(next_x, next_y)) {
        x = next_x;
        y = next_y;
    }

    // Animate and move metasprite
    if(is_player_walking) {
        frame_skip--;
        if(frame_skip < 1){
            player_frame = player_animation(player_direction, player_frame);
            frame_skip = 6;
        }
    }

    switch(player_direction){
        case PLAYER_DIRECTION_DOWN:  move_metasprite(player_down, 0, x, y,0); break;
        case PLAYER_DIRECTION_UP:    move_metasprite(player_up, 0, x, y,0); break;
        case PLAYER_DIRECTION_LEFT:  move_metasprite(player_left, 0, x, y,0); break;
        case PLAYER_DIRECTION_RIGHT: move_metasprite(player_right, 0, x, y,0); break;
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
