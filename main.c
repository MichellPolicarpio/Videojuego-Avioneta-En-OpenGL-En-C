#include "config.h"
#include "menu.h"
#include "player.h"
#include "obstacle.h" 
#include <stdio.h>
#include <stdbool.h>
#include <math.h> 


// gcc -Wall -framework OpenGL -framework GLUT menu.c player.c obstacle.c main.c -o game

// Constantes
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define WINDOW_TITLE "MI VIDEOJUEGO"
#define FPS 60

// Variables globales
int window_id;
GameState current_state = STATE_MENU;
bool running = true;
float delta_time = 0.0f;
int last_time = 0;

int score = 0;  // Variable para almacenar la puntuación

// Prototipos de funciones
void init(void);
void display(void);
void update(void);
void reshape(int w, int h);
void keyboard(unsigned char key, int x, int y);
void special_keys(int key, int x, int y);
void cleanup(void);
void timer(int value);
void render_game(void);
void keyboard_up(unsigned char key, int x, int y);

// Función de inicialización
void init(void) {
    init_obstacles();

    // Configurar color de fondo
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    
    // Configurar proyección
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f, (float)WINDOW_WIDTH/WINDOW_HEIGHT, 0.1f, 100.0f);
    
    // Configurar vista
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    // Habilitar características
    glEnable(GL_DEPTH_TEST);
    
    // Inicializar tiempo
    last_time = glutGet(GLUT_ELAPSED_TIME);
    
    // Inicializar menú y jugador
    init_menu();
    init_player();
}

// Función de actualización
void update(void) {
    // Calcular delta time
    int current_time = glutGet(GLUT_ELAPSED_TIME);
    delta_time = (current_time - last_time) / 1000.0f;
    last_time = current_time;

    // Incrementar la puntuación
    if (current_state == STATE_PLAYING) {
        score += (int)(delta_time * 10);  // Incrementa 10 puntos por segundo
    }

    // Actualizar según el estado
    switch(current_state) {
        case STATE_MENU:
            update_menu();
            break;

        case STATE_PLAYING:
            update_player(delta_time);
            update_obstacles(delta_time);
            // Verificar colisiones
            if (check_collision(0.0f, player.y_pos, 2.0f)) {
                printf("¡Colisión detectada!\n");
                // TODO: Manejar colisión
            }
            break;

        case STATE_PAUSED:
            // No actualizar durante pausa
            break;
    }
}

// Función para renderizar el juego
void render_game(void) {
    glLoadIdentity();

    // Cambiar la vista para que mire desde el lado
    gluLookAt(0.0f, 0.0f, 70.0f,    // Posición de la cámara (ahora en Z)
              20.0f, 0.0f, 0.0f,      // Punto al que mira (centro)
              0.0f, 1.0f, 0.0f);     // Vector "arriba"

    // Renderizar obstáculos
    render_obstacles();

    // Renderizar avioneta
    render_player();

    // Renderizar puntuación
    glColor3f(1.0f, 1.0f, 1.0f); // Color blanco para el texto
    char score_text[20];
    sprintf(score_text, "Score: %d", score);
    float text_length = (float)glutBitmapLength(GLUT_BITMAP_HELVETICA_18, (const unsigned char*)score_text);
    glRasterPos2f(-15.0f + text_length / 2000.0f, 20.1f); // Ajusta aquí para mover el texto más arriba

    const char* text = score_text;
    while (*text) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *text++);
    }
}

// Función de renderizado
void display(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    
    // Renderizar según el estado
    switch(current_state) {
        case STATE_MENU:
            render_menu();
            break;
            
        case STATE_PLAYING:
            render_game();
            break;
            
        case STATE_PAUSED:
            // Renderizar el juego pausado
            render_game();
            
            // Superponer texto de pausa
            glMatrixMode(GL_PROJECTION);
            glPushMatrix();
            glLoadIdentity();
            gluOrtho2D(-1.0, 1.0, -1.0, 1.0);
            glMatrixMode(GL_MODELVIEW);
            glLoadIdentity();
            
            glColor3f(1.0f, 1.0f, 1.0f);
            const char* pause_text = "PAUSED";
            float text_length = (float)glutBitmapLength(GLUT_BITMAP_HELVETICA_18, 
                                                      (const unsigned char*)pause_text);
            float scale = 1000.0f;
            float x = -text_length / scale;
            glRasterPos2f(x, 0.0f);
            while (*pause_text) {
                glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *pause_text++);
            }
            
            glMatrixMode(GL_PROJECTION);
            glPopMatrix();
            glMatrixMode(GL_MODELVIEW);
            break;
    }
    
    glutSwapBuffers();
}

// Función de reshape
void reshape(int w, int h) {
    if (h == 0) h = 1;
    float aspect = (float)w / h;
    
    glViewport(0, 0, w, h);
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f, aspect, 0.1f, 100.0f);
    
    glMatrixMode(GL_MODELVIEW);
}

// Función de teclado normal
void keyboard(unsigned char key, int x, int y) {
    switch(current_state) {
        case STATE_MENU:
            menu_handle_input(key);
            break;
            
        case STATE_PLAYING:
            if(key == 27) { // ESC
                current_state = STATE_PAUSED;
            } else {
                handle_player_input(key);
            }
            break;
            
        case STATE_PAUSED:
            if (key == 27) { // ESC
                current_state = STATE_PLAYING;
            }
            break;
    }
    
    glutPostRedisplay();
}

// Función de teclas especiales
void special_keys(int key, int x, int y) {
    switch(current_state) {
        case STATE_MENU:
            menu_handle_special_keys(key);
            break;
            
        case STATE_PLAYING:
            switch(key) {
                case GLUT_KEY_UP:
                    handle_player_input('w');
                    break;
                case GLUT_KEY_DOWN:
                    handle_player_input('s');
                    break;
            }
            break;
            
        case STATE_PAUSED:
            // No procesar teclas especiales en pausa
            break;
    }
    
    glutPostRedisplay();
}

// Función de timer para FPS constantes
void timer(int value) {
    update();
    glutPostRedisplay();
    glutTimerFunc(1000/FPS, timer, 0);
}

// Función de limpieza
void cleanup(void) {
    printf("Limpiando recursos...\n");
    // TODO: Liberar recursos
}

// Implementar la función
void keyboard_up(unsigned char key, int x, int y) {
    if(current_state == STATE_PLAYING) {
        handle_player_key_up(key);
    }
    glutPostRedisplay();
}

// Función principal
int main(int argc, char** argv) {
    // Inicializar GLUT
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    
    // Crear ventana
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutInitWindowPosition(100, 100);
    window_id = glutCreateWindow(WINDOW_TITLE);
    
    // Inicializar
    init();
    
    // Registrar callbacks
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(special_keys);
    glutTimerFunc(0, timer, 0);

    glutKeyboardFunc(keyboard);
    glutKeyboardUpFunc(keyboard_up);  // Añadir esta línea
    
    // Mensaje de inicio
    printf("Iniciando juego...\n");
    
    // Iniciar el loop principal
    glutMainLoop();
    
    // Limpieza
    cleanup();
    
    return 0;
}