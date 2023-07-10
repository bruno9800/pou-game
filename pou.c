#include <GL/glut.h>
#include <math.h>
#include <stdio.h>
#include <time.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <stdlib.h>
#include <stdbool.h>

int gameState = 0; // 0 para menu, 1 para jogo em execução

int width, height, numChannels;
unsigned char* image;

// Variaveis do pou;
float pouY = -0.4f;
float pouX = 0.0f;

// Variveis da gravidade;
float gravity = 0.008f;
float globalY = 0.0f;

// variaveis do pulo do pou
float jumpVelocity = 0.12f;
bool isJumping = false;

bool collision = false;

// Variaveis do piso;
float P_DISTANCE = 0.6f;

float altura_tela = 0.0f;
float altura_pou = -0.6f; // estado;

float piso_Y[] = {-0.6, 0.0, 0.6, 1.2};
float piso_X[] = {-0.0, 0.4, -0.2, 0.2};

int ind_piso_atual = 0;
int ind_piso_prox = 1;

bool checkCollision = false;
GLuint textureID;

void loadTexture() {
    image = stbi_load("background.jpg", &width, &height, &numChannels, 0);
    
    if (image == NULL) {
        printf("Falha ao carregar a imagem de fundo.\n");
        exit(1);
    }

    // Verificar se a imagem possui formato RGB de 24 bits (8 bits por canal)
    if (numChannels != 3) {
        printf("A imagem não está no formato RGB de 24 bits.\n");
        stbi_image_free(image);
        exit(1);
    }

    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    // Configurar parâmetros de filtragem e repetição da textura
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // Carregar a imagem como textura
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);

}


void textureEnable() {
    glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, textureID);
        glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 0.0f);
        glVertex2f(-1.0f, -1.0f);
        glTexCoord2f(1.0f, 0.0f);
        glVertex2f(1.0f, -1.0f);
        glTexCoord2f(1.0f, 1.0f);
        glVertex2f(1.0f, 1.0f);
        glTexCoord2f(0.0f, 1.0f);
        glVertex2f(-1.0f, 1.0f);
        glEnd();
        glDisable(GL_TEXTURE_2D);
}

double random_x_position() {
    // Define a semente para a função rand() com base no tempo atual
    srand(time(NULL));

    // Gera um número aleatório entre 0 e RAND_MAX
    int randInt = rand();

    // Converte o número aleatório para um valor entre -1.0 e 1.0
    double randDouble = ((double)randInt / (double)RAND_MAX) * 2.0 - 1.0;

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


int ind_menor(int atual) {
    float menor = piso_Y[atual];
    int ind_menor;

    for(int i = 0 ; i < 4; i++) {
        if(piso_Y[0] < menor) {
            menor = piso_Y[i];
            ind_menor = i;
        }
    }

    return ind_menor;
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

bool verificar_descida = false;

void atualiza_atual(int atual){
    piso_Y[atual] += 2.4f;
    piso_X[atual] = random_x_position();
    printf("valor atualizado: %.2f\n", piso_Y[atual]);
}

void calcular_indices(int *atual, int *prox) {
    float prox_value = piso_Y[*prox] + P_DISTANCE;
    *atual = *prox;
    *prox = (*prox + 1) % 4;
    printf("prox_value: %.2f", prox_value);
    printf("\natual: %d\nprox: %d\n", *atual, *prox);
}


void collisionEvent() {
   if(pou_no_piso_atual()) {
        collision = true;
   }else {
        collision = false;
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
// Função de renderização
void renderScene() {
    if (gameState == 0) { // Estado do menu
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluOrtho2D(0, width, 0, height);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        glColor3f(1.0f, 1.0f, 1.0f);

        glRasterPos2f(10.0f, 10.0f);

        const char* menuText = "Pressione Enter para iniciar o jogo";

        for (int i = 0; menuText[i] != '\0'; i++) {
            glutBitmapCharacter(GLUT_BITMAP_9_BY_15, menuText[i]);
        }
    } else {
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        gluLookAt(0.0f, altura_tela, 1.0f,
                  0.0f, altura_tela, 0.0f,
                  0.0f, 1.0f, 0.0f);

        glTranslatef(0.0f, globalY, 0.0f);
        glPushMatrix();
        glTranslatef(pouX, pouY, 0.0f);
        pou();
        glPopMatrix();
        glPushMatrix();
        glTranslatef(piso_X[0], piso_Y[0], 0.0f);
        piso();
        glPopMatrix();
        glPushMatrix();
        glTranslatef(piso_X[1], piso_Y[1], 0.0f);
        piso();
        glPopMatrix();
        glPushMatrix();
        glTranslatef(piso_X[2], piso_Y[2], 0.0f);
        piso();
        glPopMatrix();
        glPushMatrix();
        glTranslatef(piso_X[3], piso_Y[3], 0.0f);
        piso();
        glPopMatrix();
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

void gameEventLoop() {
    gravityAction();
    pouJump();
    collisionEvent();

    glutPostRedisplay();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Oswald");

    glutDisplayFunc(renderScene);
    glutKeyboardFunc(keyboardFunc);

    glutIdleFunc(gameEventLoop);

    //loadTexture();

    glutMainLoop();

    //stbi_image_free(image);

    return 0;
}
