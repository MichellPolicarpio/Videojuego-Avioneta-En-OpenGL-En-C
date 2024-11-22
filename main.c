#include "config.h"
#include "menu.h"
#include <stdio.h>
#include <stdbool.h>

// gcc -Wall -framework OpenGL -framework GLUT menu.c main.c -o main

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

// Prototipos de funciones
void init(void);
void display(void);
void update(void);
void reshape(int w, int h);
void keyboard(unsigned char key, int x, int y);
void special_keys(int key, int x, int y);
void cleanup(void);
void timer(int value);

// Función de inicialización
void init(void) {
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
    
    // Inicializar menú
    init_menu();
}

// Función de actualización
void update(void) {
    // Calcular delta time
    int current_time = glutGet(GLUT_ELAPSED_TIME);
    delta_time = (current_time - last_time) / 1000.0f;
    last_time = current_time;
    
    // Actualizar según el estado
    switch(current_state) {
        case STATE_MENU:
            update_menu();
            break;
            
        case STATE_PLAYING:
            // TODO: Actualizar juego
            break;
            
        case STATE_PAUSED:
            // TODO: Actualizar pausa
            break;
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
            // TODO: Renderizar juego
            glTranslatef(0.0f, 0.0f, -5.0f);
            glColor3f(0.0f, 1.0f, 0.0f);
            glutWireCube(1.0f);  // Cubo temporal para prpbar que cargue opcion del juego
            break;
            
        case STATE_PAUSED:
            // Primero renderizar el juego pausado
            glTranslatef(0.0f, 0.0f, -5.0f);
            glColor3f(0.0f, 1.0f, 0.0f);
            glutWireCube(1.0f);
            
            // Luego renderizar el texto de pausa
            glMatrixMode(GL_PROJECTION);
            glPushMatrix();
            glLoadIdentity();
            gluOrtho2D(-1.0, 1.0, -1.0, 1.0);
            glMatrixMode(GL_MODELVIEW);
            glLoadIdentity();
            
            glColor3f(1.0f, 1.0f, 1.0f);
            //AQUI IBA ALGO DE PAUSED
            
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
            switch(key) {
                case 27: // ESC
                    current_state = STATE_PAUSED;
                    break;
                    
                case 'w':
                case 'W':
                    // TODO: Mover adelante
                    break;
                    
                case 's':
                case 'S':
                    // TODO: Mover atrás
                    break;
                    
                case 'a':
                case 'A':
                    // TODO: Mover izquierda
                    break;
                    
                case 'd':
                case 'D':
                    // TODO: Mover derecha
                    break;
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
                    // TODO: Mover adelante
                    break;
                case GLUT_KEY_DOWN:
                    // TODO: Mover atrás
                    break;
                case GLUT_KEY_LEFT:
                    // TODO: Girar izquierda
                    break;
                case GLUT_KEY_RIGHT:
                    // TODO: Girar derecha
                    break;
            }
            break;
            
        case STATE_PAUSED:
            // No procesar teclas especiales en pausa
            break;
    }
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
    
    // Mensaje de inicio
    printf("Iniciando juego...\n");
    
    // Iniciar el loop principal
    glutMainLoop();
    
    // Limpieza
    cleanup();
    
    return 0;
}