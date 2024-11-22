#ifndef MENU_H
#define MENU_H

#include "config.h"
#include <stdbool.h>

// Estados del juego
typedef enum {
    STATE_MENU,
    STATE_PLAYING,
    STATE_PAUSED
} GameState;

// Opciones del menú
typedef enum {
    MENU_NEW_GAME,
    MENU_LOAD_GAME,
    MENU_OPTIONS,
    MENU_EXIT,
    MENU_OPTION_COUNT
} MenuOption;

// Estructura del menú
typedef struct {
    MenuOption selected_option;
    bool is_active;
    float title_pos_y;
    float first_option_y;
    float option_spacing;
} Menu;

// Declaración de variables externas
extern Menu game_menu;
extern const char* menu_options[4];
extern GameState current_state;

// Prototipos de funciones del menú
void init_menu(void);
void update_menu(void);
void render_menu(void);
void menu_handle_input(unsigned char key);
void menu_handle_special_keys(int key);
void render_text(const char* text, float y, bool selected);

#endif // MENU_H