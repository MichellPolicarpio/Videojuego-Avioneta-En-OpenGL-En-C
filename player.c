#include "player.h"
#include <math.h>

// Variables globales del jugador
Player player;

// Dimensiones de la avioneta
static float BODY_LENGTH = 8.0f;
static float BODY_WIDTH = 2.0f;
static float BODY_HEIGHT = 2.0f;
static float WING_LENGTH = 10.0f;
static float WING_WIDTH = 0.3f;
static float WING_HEIGHT = 3.0f;
static float WING_ANGLE = 15.0f;
static float TAIL_LENGTH = 2.5f;
static float TAIL_WIDTH = 0.3f;
static float TAIL_HEIGHT = 2.0f;
static float PROPELLER_LENGTH = 1.2f;
static float PROPELLER_WIDTH = 0.2f;
static float PROPELLER_HEIGHT = 4.0f;
static float PROPELLER_HUB_RADIUS = 0.4f;

// Funciones auxiliares de dibujo
static void drawScaledCube(float width, float height, float length) {
    glPushMatrix();
        glScalef(width, height, length);
        glutSolidCube(1.0);
    glPopMatrix();
}

static void drawWing() {
    glPushMatrix();
        glColor3f(0.6f, 0.6f, 0.6f);
        glRotatef(WING_ANGLE, 0.0f, 0.0f, 0.0f);
        
        // Parte central del ala
        drawScaledCube(WING_LENGTH, WING_WIDTH, WING_HEIGHT);
        
        // Borde de ataque
        glPushMatrix();
            glTranslatef(0.0f, 0.0f, WING_HEIGHT/2);
            glRotatef(-30.0f, 1.0f, 0.0f, 0.0f);
            drawScaledCube(WING_LENGTH, WING_WIDTH, WING_HEIGHT * 0.3f);
        glPopMatrix();
        
        // Borde de fuga
        glPushMatrix();
            glTranslatef(0.0f, 0.0f, -WING_HEIGHT/2);
            glRotatef(15.0f, 1.0f, 0.0f, 0.0f);
            drawScaledCube(WING_LENGTH, WING_WIDTH, WING_HEIGHT * 0.2f);
        glPopMatrix();
    glPopMatrix();
}

static void drawPropeller() {
    glPushMatrix();
        // Hub de la hélice
        glColor3f(0.3f, 0.3f, 0.3f);
        GLUquadric *quadric = gluNewQuadric();
        gluQuadricDrawStyle(quadric, GLU_FILL);
        gluCylinder(quadric, PROPELLER_HUB_RADIUS, PROPELLER_HUB_RADIUS, 0.5f, 20, 1);
        gluDeleteQuadric(quadric);

        // Aspas rotando
        glTranslatef(0.0f, 0.0f, 0.25f);
        glRotatef(player.propeller.angle, 0.0f, 0.0f, 1.0f);
        
        // Primera aspa
        glPushMatrix();
            glColor3f(0.4f, 0.4f, 0.4f);
            drawScaledCube(PROPELLER_HEIGHT, PROPELLER_WIDTH, PROPELLER_LENGTH);
        glPopMatrix();
        
        // Segunda aspa
        glPushMatrix();
            glRotatef(90, 0.0f, 0.0f, 1.0f);
            glColor3f(0.4f, 0.4f, 0.4f);
            drawScaledCube(PROPELLER_HEIGHT, PROPELLER_WIDTH, PROPELLER_LENGTH);
        glPopMatrix();
    glPopMatrix();
}

void init_player(void) {
    player.y_pos = 0.0f;
    player.y_velocity = 0.0f;
    player.normal_speed = 30.0f;     // Velocidad normal base
    player.boost_speed = 60.0f;      // Velocidad con boost
    player.current_speed = player.normal_speed;  // Velocidad inicial
    player.rotation = 0.0f;
    player.propeller.angle = 0.0f;
    player.propeller.speed = 5.0f;
    player.max_y = 20.0f;
    player.min_y = -20.0f;
}

void update_player(float delta_time) {
    // Actualizar posición vertical
    player.y_pos += player.y_velocity * delta_time;
    
    // Limitar posición vertical
    if (player.y_pos > player.max_y) {
        player.y_pos = player.max_y;
        player.y_velocity = 0;
    }
    if (player.y_pos < player.min_y) {
        player.y_pos = player.min_y;
        player.y_velocity = 0;
    }
    
    // Actualizar rotación basada en la velocidad vertical
    player.rotation = -player.y_velocity * 2.0f;
    
    // Actualizar hélice
    update_propeller();
}

void render_player(void) {
    glPushMatrix();
        // Posicionar la avioneta
        glTranslatef(0.0f, player.y_pos, 0.0f);
        
        // Rotar 90 grados para mirar hacia la derecha
        glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
        
        // Rotar según el movimiento vertical
        glRotatef(player.rotation, 1.0f, 0.0f, 0.0f);
        
        // Fuselaje principal
        glColor3f(0.8f, 0.8f, 0.8f);
        drawScaledCube(BODY_WIDTH, BODY_HEIGHT, BODY_LENGTH);
        
        // Nariz y hélice
        glPushMatrix();
            glTranslatef(0.0f, 0.0f, BODY_LENGTH/2);
            
            // Nariz cónica
            glColor3f(0.7f, 0.7f, 0.7f);
            GLUquadric *quadric = gluNewQuadric();
            gluQuadricDrawStyle(quadric, GLU_FILL);
            gluCylinder(quadric, BODY_WIDTH/2, BODY_WIDTH/4, 2.0f, 20, 1);
            gluDeleteQuadric(quadric);
            
            // Hélice
            glTranslatef(0.0f, 0.0f, 1.0f);
            drawPropeller();
        glPopMatrix();
        
        // Alas y cola
        drawWing();
        
        // Cola vertical
        glPushMatrix();
            glTranslatef(0.0f, BODY_HEIGHT/2 + TAIL_HEIGHT/2, -BODY_LENGTH/2 + TAIL_LENGTH/2);
            glColor3f(0.7f, 0.7f, 0.7f);
            drawScaledCube(TAIL_WIDTH, TAIL_HEIGHT, TAIL_LENGTH);
        glPopMatrix();
        
        // Cola horizontal
        glPushMatrix();
            glTranslatef(0.0f, 0.0f, -BODY_LENGTH/2 + TAIL_LENGTH/2);
            glColor3f(0.7f, 0.7f, 0.7f);
            drawScaledCube(WING_LENGTH/2, WING_WIDTH, WING_HEIGHT/2);
        glPopMatrix();
    glPopMatrix();
}

void handle_player_input(unsigned char key) {
    switch(key) {
        case 'w':
        case 'W':
            player.y_velocity = 10.0f;
            break;
        case 's':
        case 'S':
            player.y_velocity = -10.0f;
            break;
        case ' ': // Tecla espacio
            player.current_speed = player.boost_speed;
            player.propeller.speed = 15.0f; // Aumentar velocidad de la hélice
            break;
        case 13: // Enter (acelerador permanente)
            player.normal_speed += 10.0f;
            if(player.normal_speed > player.boost_speed) {
                player.normal_speed = player.boost_speed;
            }
            player.current_speed = player.normal_speed;
            break;
    }
}

void handle_player_key_up(unsigned char key) {
    switch(key) {
        case ' ': // Cuando se suelta el espacio
            player.current_speed = player.normal_speed;
            player.propeller.speed = 7.0f;
            break;
        case 'w':
        case 'W':
        case 's':
        case 'S':
            player.y_velocity = 0.0f; // Detener movimiento vertical
            break;
    }
}

void update_propeller(void) {
    player.propeller.angle += player.propeller.speed;
    if (player.propeller.angle > 360.0f) {
        player.propeller.angle -= 360.0f;
    }
}