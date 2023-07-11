#include <GL/glut.h>
#include <math.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

int gameState = 0; // 0 para menu, 1 para jogo em execução;

int score = 0;

// image texture;
int imageWidth, imageHeight, numChannels;
unsigned char* image;
GLuint textureID;

// Variaveis do pou;
float pouY = -0.4f;
float pouX = 0.0f;

// Variveis da gravidade;
float gravity = 0.008f;

// variaveis do pulo do pou;
float jumpVelocity = 0.12f;
bool isJumping = false;

// variaveis de colisão;
bool collision = false;
float altura_tela = 0.0f;
bool checkCollision = false;
bool verificar_descida = false;

// Variaveis do piso;
float P_DISTANCE = 0.6f;

float piso_Y[] = {-0.6, 0.0, 0.6, 1.2};
float piso_X[] = {-0.0, 0.4, -0.2, 0.2};

int ind_piso_atual = 0;
int ind_piso_prox = 1;


double random_x_position() {
    srand(time(NULL));
    int randInt = rand();

    // Converte o número aleatório para um valor entre -1.0 e 1.0
    double randDouble = ((double)randInt / (double)RAND_MAX) * 1.6 - 0.8;

    return randDouble;
}

void pou() {
    glColor3f(0.71f, 0.61f, 0.52f); // Define a cor de preenchimento como marrom claro
    glBegin(GL_POLYGON); // Inicia o desenho de um polígono preenchido
    glVertex2f(0.0f, 0.0f); // base
    glVertex2f(0.06, 0.010f);
    glVertex2f(0.116f, 0.032f); // direta
    glVertex2f(0.118f, 0.036f); // direta
    glVertex2f(0.12f, 0.04f); // direta
    glVertex2f(0.12, 0.08f);
    glVertex2f(0.06, 0.24f);
    glVertex2f(0.044, 0.272f);
    glVertex2f(-0.00f, 0.28f); // topo
    glVertex2f(-0.044, 0.272f);
    glVertex2f(-0.06, 0.24f);
    glVertex2f(-0.12, 0.08f);
    glVertex2f(-0.12f, 0.02f); // esquerda
    glVertex2f(-0.118f, 0.04f); // esquerda
    glVertex2f(-0.116f, 0.036f); // esquerda
    glVertex2f(-0.06f, 0.010f);
    glEnd(); // Finaliza o desenho do polígono

    glFlush(); // Força a renderização da cena
}

void piso() {

    glColor3f(0.0f, 1.0f, 0.0f);
    glBegin(GL_POLYGON); // Inicia o desenho de um polígono preenchido
    glVertex2f(-0.18f, 0.0f); // base
    glVertex2f(0.18f, 0.0f);
    glVertex2f(0.18f, 0.08f);
    glVertex2f(-0.18f, 0.08f);
    glEnd(); // Finaliza o desenho do polígono
    glFlush();
}

void gravityAction() {
    if(!collision) {
        pouY -= gravity;
    }
}

bool pou_no_piso_atual() {
    bool emX =  pouX >= piso_X[ind_piso_atual]-0.25 && pouX <= piso_X[ind_piso_atual] + 0.25;
    bool emY =  pouY <= piso_Y[ind_piso_atual] + 0.08 && pouY >= piso_Y[ind_piso_atual];
    return emX && emY;
}

bool pou_passando_na_descida() {
    bool emY = pouY >= piso_Y[ind_piso_prox] && pouY <= piso_Y[ind_piso_prox] + 0.08;
    bool emX = pouX >= piso_X[ind_piso_prox]-0.25 && pouX <= piso_X[ind_piso_prox] + 0.25;
    return emY && emX;
}

bool pou_passou_do_proximo_piso() {
    return pouY >= piso_Y[ind_piso_prox] + 0.08;
}

void atualiza_atual(int atual){
    piso_Y[atual] += 2.4f;
    piso_X[atual] = random_x_position();
}

void calcular_indices(int *atual, int *prox) {
    float prox_value = piso_Y[*prox] + P_DISTANCE;
    *atual = *prox;
    *prox = (*prox + 1) % 4;
}

void collisionEvent() {
    collision = pou_no_piso_atual();

   if(pouY < altura_tela - 1.4) {
    printf("game over\n");
    printf("score: %d\n", score);
    gameState = 3;
   }

   if(pou_passou_do_proximo_piso()) {
        verificar_descida = true;
   }
   if(verificar_descida) {
        if(pouY < piso_Y[ind_piso_prox]){
            verificar_descida = false;
        }
        if(pou_passando_na_descida()) {
            float prox_value =  piso_Y[ind_piso_prox];
            atualiza_atual(ind_piso_atual);
            calcular_indices(&ind_piso_atual, &ind_piso_prox);
            verificar_descida = false;
            altura_tela += P_DISTANCE;
            score++;
        }
   }
}

void pouJump() {
    if (isJumping) {
        pouY += jumpVelocity; // Atualiza a posição do círculo com base na velocidade do salto
        jumpVelocity -= gravity; // Aplica a força da gravidade na velocidade do salto

        // Verifica se o círculo atingiu o chão
        if (jumpVelocity <= 0.0f) {
            jumpVelocity = 0.12f; // Anula a velocidade do salto
            isJumping = false; // Finaliza o salto
        }
    }
}
// Funções de renderização

void desenha_piso(int indice) {
    glPushMatrix();
        glTranslatef(piso_X[indice], piso_Y[indice], 0.0f);
        piso();
        glPopMatrix();
}


void reset() {
    piso_Y[0] = -0.6f;
    piso_X[0] = random_x_position();
    for(int i = 1 ; i < 4; i++) {
        piso_Y[i] = piso_Y[i-1] + P_DISTANCE;
        piso_X[i] = random_x_position();
    }
    pouY = -0.4f;
    pouX = piso_X[0]+0.2;
    ind_piso_atual = 0;
    ind_piso_prox = 1;


    altura_tela = 0.0f;
}

void menu_item(const char *label, float textTranslateX) {
    glColor3f(0.0f, 1.0f, 1.0f);
        glBegin(GL_POLYGON); // Inicia o desenho de um polígono preenchido
        glVertex2f(-0.18f, -0.02f); // base
        glVertex2f(0.18f, -0.02f);
        glVertex2f(0.18f, 0.08f);
        glVertex2f(-0.18f, 0.08f);
        glEnd(); // Finaliza o desenho do polígono
        glFlush();    
        glColor3f(0.0, 0.0, 0.0); // Define a cor do texto como branco
        glTranslatef(textTranslateX, 0.0f, 0.0f);
        glRasterPos2f(-0.1, 0.0); // Define a posição inicial do texto
        //const char *text = "JOGAR"; // Texto a ser exibido
        
        int len = strlen(label);
        for (int i = 0; i < len; i++) {
            glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, label[i]); // Renderiza cada caractere do texto
        }
}

void game_over() {
    char scoreStr[20];
        glPushMatrix();
        glTranslatef(0.0f, 0.2f, 0.0f);
        menu_item("REINICIAR", -0.05f);
        glPopMatrix();
        glPushMatrix();
        glTranslatef(0.0f, 0.05f, 0.0f);
        menu_item("SAIR",0.0f);
        glPopMatrix();
        // score
        glColor3f(1.0f, 0.0f, 1.0f);
        glTranslatef(0.0f, 0.5f, 0.0f);
        glBegin(GL_POLYGON); // Inicia o desenho de um polígono preenchido
        glVertex2f(-0.2f, -0.04f); // base
        glVertex2f(0.2f, -0.04f);
        glVertex2f(0.2f, 0.1f);
        glVertex2f(-0.2f, 0.1f);
        glEnd(); // Finaliza o desenho do polígono
        glFlush();
        sprintf(scoreStr, "Score: %d", score);
        glColor3f(0.0, 0.0, 0.0);
        glTranslatef(-0.1f, 0.0f, 0.0f);
        glRasterPos2i(0.0, 0.0);
        for(int i = 0; i < strlen(scoreStr); i++){
            glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, scoreStr[i]);
        }
        glColor3f(1.0f, 0.0f, 0.0f);
        glTranslatef(0.1f, 0.2f, 0.0f);
        glBegin(GL_POLYGON); // Inicia o desenho de um polígono preenchido
        glVertex2f(-0.4f, -0.04f); // base
        glVertex2f(0.4f, -0.04f);
        glVertex2f(0.4f, 0.1f);
        glVertex2f(-0.4f, 0.1f);
        glEnd();
        glFlush();
        const char *label = "GAME OVER";
        glColor3f(0.0, 0.0, 0.0);
        glTranslatef(-0.18f, 0.0f, 0.0f);
        glRasterPos2i(0.0, 0.0);
        for(int i = 0; i < strlen(label); i++){
            glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, label[i]);
        }
        
        reset();

}


void renderScene() {
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        
    if (gameState == 0) { // Estado do menu

        glPushMatrix();
        glTranslatef(0.0f, 0.2f, 0.0f);
        menu_item("JOGAR", 0.0f);
        glPopMatrix();
        glPushMatrix();
        glTranslatef(0.0f, 0.05f, 0.0f);
        menu_item("SAIR", 0.0f);
        glPopMatrix();

    } else if(gameState == 1) {
        gluLookAt(0.0f, altura_tela, 1.0f,
                  0.0f, altura_tela, 0.0f,
                  0.0f, 1.0f, 0.0f);

        glPushMatrix();
        glTranslatef(pouX, pouY, 0.0f);
        pou();
        glPopMatrix();
        desenha_piso(0);
        desenha_piso(1);
        desenha_piso(2);
        desenha_piso(3);
    } else if (gameState == 2) {
        exit(1);
    } else if (gameState == 3) {
        game_over();
    }
    glutSwapBuffers();
}

// Função para lidar com o pressionamento das teclas
void keyboardFunc(unsigned char key, int x, int y) {
     if (gameState == 0) { // Verificar se está no estado do menu
        switch (key) {
            case 13: // Tecla Enter
                gameState = 1; // Mudar para o estado do jogo em execução
                break;
            default:
                break;
        }
    } else {
        switch (key) {
            case 'w':
                if(collision){
                    isJumping = true; // Mover para cima
                    collision = false; 
                }
                break;
            case 'a':
                pouX -= 0.1f; // Mover para esquerda
                break;
            case 'd':
                pouX += 0.1f;
                break;
            default:
                break;
        }
    }

    glutPostRedisplay(); // Atualiza a tela
}

void mouseClick(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        // Converter as coordenadas da janela para as coordenadas do OpenGL
        float windowX = ((float)x / glutGet(GLUT_WINDOW_WIDTH)) * 2 - 1;
        float windowY = 1 - ((float)y / glutGet(GLUT_WINDOW_HEIGHT)) * 2;

        // Verificar se o clique ocorreu dentro do retângulo do item do menu "JOGAR"
        if (windowX >= -0.18 && windowX <= 0.18 && windowY >= 0.18 && windowY <= 0.28) {
            gameState = 1;
        }

        // Verificar se o clique ocorreu dentro do retângulo do item do menu "SAIR"
        if (windowX >= -0.18 && windowX <= 0.18 && windowY >= 0.02 && windowY <= 0.12) {
            gameState = 2;
        }
    }
}

void gameEventLoop() {
    gravityAction();
    pouJump();
    collisionEvent();

    glutPostRedisplay();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Oswald");

    glutDisplayFunc(renderScene);
    glutKeyboardFunc(keyboardFunc);
    glutMouseFunc(mouseClick);
    glutIdleFunc(gameEventLoop);

    glutMainLoop();

    return 0;
}
