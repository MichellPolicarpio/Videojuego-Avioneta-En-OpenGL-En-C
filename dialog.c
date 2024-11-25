#include "dialog.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

DialogNode* mission_dialog;
DialogNode* current_dialog;
MissionStatus mission_status;

// Función para crear un nuevo nodo de diálogo
DialogNode* create_dialog_node(const char* speaker, const char* text, 
                             const char* left_choice, const char* right_choice,
                             MissionPhase phase) {
    DialogNode* node = (DialogNode*)malloc(sizeof(DialogNode));
    node->speaker = strdup(speaker);
    node->text = strdup(text);
    node->left_choice = left_choice ? strdup(left_choice) : NULL;
    node->right_choice = right_choice ? strdup(right_choice) : NULL;
    node->left = NULL;
    node->right = NULL;
    node->action = NULL;
    node->phase = phase;
    return node;
}

// Acciones de la misión
void action_start_mission(void) {
    mission_status.current_phase = PHASE_INFILTRATION;
    printf("Iniciando infiltración...\n");
}

void action_start_approach(void) {
    mission_status.current_phase = PHASE_APPROACH;
    printf("Iniciando aproximación al objetivo...\n");
}

void action_start_planting(void) {
    mission_status.current_phase = PHASE_PLANT_EXPLOSIVES;
    printf("Preparando colocación de explosivos...\n");
}

void action_start_escape(void) {
    mission_status.current_phase = PHASE_ESCAPE;
    mission_status.time_remaining = 60; // 60 segundos para escapar
    printf("¡Explosivos colocados! ¡Inicie escape!\n");
}

// Inicializar diálogos
void init_dialogs(void) {
    // Inicializar estado de la misión
    mission_status.target_x = 1000.0f;
    mission_status.target_y = 0.0f;
    mission_status.distance_to_target = 1000.0f;
    mission_status.defenses_alerted = false;
    mission_status.time_remaining = 0;
    mission_status.explosives_planted = false;
    mission_status.escaping = false;
    mission_status.current_phase = PHASE_BRIEFING;

    // Briefing inicial
    mission_dialog = create_dialog_node(
        "Comandante Eagle",
        "¡Atención piloto! Hemos identificado un almacén de armas enemigo.\nSu misión: infiltrarse y destruirlo. Es vital para la operación.",
        "Entendido, señor",
        "Detalles de la misión",
        PHASE_BRIEFING
    );

    // Rama de aceptación directa
    DialogNode* start_node = create_dialog_node(
        "Comandante Eagle",
        "Excelente actitud, piloto. Las defensas están activas.\nUse el boost para atravesar zonas de alto riesgo.",
        "Iniciar infiltración",
        NULL,
        PHASE_BRIEFING
    );
    start_node->action = action_start_mission;
    mission_dialog->left = start_node;

    // Rama de detalles
    DialogNode* details_node = create_dialog_node(
        "Oficial de Inteligencia",
        "Defensas identificadas:\n- Torres anti-aéreas\n- Sistemas de radar\n- Campos de minas\nEvite detección a toda costa.",
        "Entendido, proceder",
        "Más información",
        PHASE_BRIEFING
    );
    mission_dialog->right = details_node;

    // Información adicional
    DialogNode* extra_details = create_dialog_node(
        "Oficial de Inteligencia",
        "Datos adicionales:\n- Objetivo a 1000m\n- Defensas automatizadas\n- Sin apoyo aéreo disponible",
        "Iniciar misión",
        "Ruta de escape",
        PHASE_BRIEFING
    );
    details_node->right = extra_details;

    // Información de escape
    DialogNode* escape_info = create_dialog_node(
        "Oficial de Operaciones",
        "Post-detonación: 60 segundos para evacuar.\nRuta de escape estará marcada.\nUse el boost estratégicamente.",
        "Entendido, iniciando operación",
        NULL,
        PHASE_BRIEFING
    );
    extra_details->right = escape_info;

    // Conectar todos los endpoints al inicio de la misión
    details_node->left = start_node;
    extra_details->left = start_node;
    escape_info->left = start_node;

    current_dialog = mission_dialog;
}

// Renderizar diálogo actual
void render_dialog(DialogNode* node) {
    if (!node) return;

    // Configurar vista ortográfica para el diálogo
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(-1.0, 1.0, -1.0, 1.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Fondo semi-transparente
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.0f, 0.0f, 0.0f, 0.7f);
    glBegin(GL_QUADS);
        glVertex2f(-1.0f, -0.3f);
        glVertex2f( 1.0f, -0.3f);
        glVertex2f( 1.0f, -1.0f);
        glVertex2f(-1.0f, -1.0f);
    glEnd();

    // Renderizar nombre del hablante
    glColor3f(0.0f, 1.0f, 0.0f);  // Verde militar
    glRasterPos2f(-0.95f, -0.4f);
    const char* text = node->speaker;
    while (*text) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *text++);
    }

    // Renderizar texto principal
    glColor3f(1.0f, 1.0f, 1.0f);
    glRasterPos2f(-0.95f, -0.6f);
    text = node->text;
    float y_offset = -0.6f;
    while (*text) {
        if (*text == '\n') {
            y_offset -= 0.1f;
            glRasterPos2f(-0.95f, y_offset);
        } else {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *text);
        }
        text++;
    }

    // Renderizar opciones
    if (node->left_choice) {
        glColor3f(0.0f, 0.8f, 0.0f);  // Verde más claro
        glRasterPos2f(-0.95f, -0.8f);
        const char* prefix = "1: ";
        text = prefix;
        while (*text) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *text++);
        text = node->left_choice;
        while (*text) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *text++);
    }

    if (node->right_choice) {
        glColor3f(0.0f, 0.8f, 0.0f);
        glRasterPos2f(-0.95f, -0.9f);
        const char* prefix = "2: ";
        text = prefix;
        while (*text) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *text++);
        text = node->right_choice;
        while (*text) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *text++);
    }

    glDisable(GL_BLEND);

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}

// Manejar input del diálogo
void handle_dialog_input(unsigned char key) {
    if (!current_dialog) return;

    switch(key) {
        case '1':
            if (current_dialog->left) {
                if (current_dialog->action) {
                    current_dialog->action();
                }
                current_dialog = current_dialog->left;
            }
            break;

        case '2':
            if (current_dialog->right) {
                if (current_dialog->action) {
                    current_dialog->action();
                }
                current_dialog = current_dialog->right;
            }
            break;
    }
}

// Actualizar estado de la misión
void update_mission_status(void) {
    if (mission_status.current_phase == PHASE_ESCAPE && mission_status.time_remaining > 0) {
        mission_status.time_remaining--;
    }
}

// Renderizar información de la misión
void render_mission_info(void) {
    if (mission_status.current_phase == PHASE_BRIEFING) return;

    char info[128];
    sprintf(info, "Distancia al objetivo: %.0fm", mission_status.distance_to_target);
    
    if (mission_status.current_phase == PHASE_ESCAPE) {
        sprintf(info, "¡ESCAPE! Tiempo restante: %ds", mission_status.time_remaining);
    }

    // TODO: Renderizar información en pantalla
}

// Liberar memoria
void cleanup_dialogs(void) {
    // TODO: Implementar liberación recursiva del árbol de diálogos
}