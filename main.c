/*COMO COMPILARLO:
ABRIR TERMINAL: 
cd /archivos/Proyecto/src/ 
coloca: gcc -Wall -framework OpenGL -framework GLUT menu.c player.c obstacle.c main.c -o game
*/

#include "config.h"
#include "menu.h"
#include "player.h"
#include "obstacle.h" 
#include <stdio.h>
#include <stdbool.h>
#include <math.h> 

// Constantes
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define WINDOW_TITLE "MI VIDEOJUEGO"
#define FPS 60
#define POINTS_PER_SECOND 100  // Aumentado de 10 a 100
#define POINTS_FOR_BOOST 50    // Aumentado de 5 a 50

// Variables globales
int window_id;
GameState current_state = STATE_MENU;
bool running = true;
float delta_time = 0.0f;
int last_time = 0;
int score = 0; 

// Con las variables globales
int high_score = 0;

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
void render_game_over(void);

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
    int current_time = glutGet(GLUT_ELAPSED_TIME);
    delta_time = (current_time - last_time) / 1000.0f;
    last_time = current_time;

    switch(current_state) {
        case STATE_MENU:
            update_menu();
            break;
            
        case STATE_PLAYING:
            update_player(delta_time);
            update_obstacles(delta_time);
            
            score += (int)(delta_time * POINTS_PER_SECOND);
            if (player.current_speed > player.normal_speed) {
                score += (int)(delta_time * POINTS_FOR_BOOST);
            }
            
            if (check_collision(0.0f, player.y_pos, 2.0f)) {
                printf("¡Colisión detectada! Puntuación final: %d\n", score);
                if (score > high_score) {
                    high_score = score;
                }
                current_state = STATE_GAME_OVER;
            }
            break;
            
        case STATE_PAUSED:
            break;

        case STATE_GAME_OVER:
            // No actualizar nada en game over
            break;
    }
}

// Función para renderizar el juego
void render_game(void) {
    glPushMatrix();
    
    // Vista del juego
    glLoadIdentity();
    gluLookAt(0.0f, 0.0f, 70.0f,
              20.0f, 0.0f, 0.0f,
              0.0f, 1.0f, 0.0f);

    render_obstacles();
    render_player();

    // Configurar vista para HUD
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(-1.0, 1.0, -1.0, 1.0);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Renderizar puntuación
    // En la parte donde se renderiza el score
    glColor3f(1.0f, 1.0f, 1.0f);
    char score_text[32];
    sprintf(score_text, "Score: %d  High Score: %d", score, high_score);
    glRasterPos2f(-0.9f, 0.85f);  // Ajustado ligeramente la posición
    
    const char* text = score_text;
    while (*text) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *text++);
    }

    // Restaurar matrices
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}

void render_game_over(void) {
    // Primero renderizar el juego en el fondo
    render_game();
    
    // Ahora superponer la pantalla de Game Over
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(-1.0, 1.0, -1.0, 1.0);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    // Fondo semi-transparente negro
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.0f, 0.0f, 0.0f, 0.7f);  // El último valor (0.7f) controla la transparencia
    glBegin(GL_QUADS);
        glVertex2f(-1.0f, -1.0f);
        glVertex2f( 1.0f, -1.0f);
        glVertex2f( 1.0f,  1.0f);
        glVertex2f(-1.0f,  1.0f);
    glEnd();
    
    // Título "GAME OVER" grande
    glColor3f(1.0f, 0.0f, 0.0f);  // Rojo
    const char* game_over_text = "GAME OVER";
    glRasterPos2f(-0.4f, 0.6f);
    while (*game_over_text) {
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, *game_over_text++);
    }
    
    // Estadísticas
    glColor3f(1.0f, 1.0f, 1.0f);  // Blanco
    char stats_text[128];
    
    // Puntuación final
    sprintf(stats_text, "Final Score: %d", score);
    glRasterPos2f(-0.3f, 0.3f);
    const char* text = stats_text;
    while (*text) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *text++);
    }
    
    // High Score
    sprintf(stats_text, "High Score: %d", high_score);
    glRasterPos2f(-0.3f, 0.2f);
    text = stats_text;
    while (*text) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *text++);
    }
    
    // Instrucciones
    glColor3f(0.7f, 0.7f, 1.0f);  // Azul claro
    const char* restart_text = "Press ENTER to Play Again";
    glRasterPos2f(-0.3f, -0.2f);
    text = restart_text;
    while (*text) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *text++);
    }
    
    const char* menu_text = "Press ESC for Main Menu";
    glRasterPos2f(-0.3f, -0.3f);
    text = menu_text;
    while (*text) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *text++);
    }
    
    glDisable(GL_BLEND);
    
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
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
        case STATE_GAME_OVER:
            render_game_over();
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

        case STATE_GAME_OVER:
            if (key == 13) {  // ENTER
                // Reiniciar juego
                score = 0;
                init_player();
                init_obstacles();
                current_state = STATE_PLAYING;
            } else if (key == 27) {  // ESC
                // Volver al menú
                score = 0;
                current_state = STATE_MENU;
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
            break;

        case STATE_GAME_OVER:
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