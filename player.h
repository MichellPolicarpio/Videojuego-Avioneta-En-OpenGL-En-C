#ifndef PLAYER_H
#define PLAYER_H

#include "config.h"

// Estructura para el estado del jugador (avioneta)
typedef struct {
    // Posición y movimiento
    float y_pos;           // Posición vertical
    float y_velocity;      // Velocidad vertical
    float normal_speed;    // Velocidad normal (NUEVO)
    float boost_speed;     // Velocidad con boost (NUEVO)
    float current_speed;   // Velocidad actual (NUEVO)
    float rotation;        // Rotación de la avioneta al subir/bajar

    // Estado de la hélice
    struct {
        float angle;
        float speed;
    } propeller;

    // Límites de movimiento
    float max_y;          // Límite superior
    float min_y;          // Límite inferior
} Player;

// Prototipos de funciones
void init_player(void);
void update_player(float delta_time);
void render_player(void);
void handle_player_input(unsigned char key);
void handle_player_key_up(unsigned char key);  // NUEVO: Para manejar cuando se suelta una tecla
void update_propeller(void);

// Variables externas
extern Player player;

#endif // PLAYER_H