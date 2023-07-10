#include <GL/glut.h>
#include <math.h>
#include <stdio.h>
#include <time.h>

// Variaveis do pou;
float pouY = 0.0f;
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
float altura_pou = 0.0f; // estado;

float piso_Y[] = {-0.4, 0.2, 0.8, 1.4};

void background() {
    glColor3f(0.5f, 0.5f, 0.5f); // Cor cinza para as linhas

}
double gerarNumeroAleatorio() {
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
    glVertex2f(0.0f, 0.0f); // base
    glVertex2f(0.36f, 0.0f);
    glVertex2f(0.36f, 0.08f);
    glVertex2f(0.0f, 0.08f);
    glEnd(); // Finaliza o desenho do polígono
    glFlush();
}


void gravityAction() {
  if(!collision) {
    pouY -= gravity;
  }
  
}

void collisionEvent() {
    if(pouY < -1.0){
        collision = true;
    }

    if(collision && altura_tela != altura_pou) {
      altura_tela = altura_pou;
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
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0.0f, altura_tela, 1.0f,  // Posição da câmera
              0.0f, altura_tela, 0.0f,          // Ponto de interesse
              0.0f, 1.0f, 0.0f); 

    background();
    glTranslatef(0.0f, globalY, 0.0f);
    glPushMatrix();
    glTranslatef(pouX, pouY, 0.0f);
    pou();
    glPopMatrix();
    glPushMatrix();
    glTranslatef(0.5f,piso_Y[0], 0.0f);
    piso();
    glPopMatrix();
    glPushMatrix();
    glTranslatef(0.1f,piso_Y[1],0.0f);
    piso();
    glPopMatrix();
    glPushMatrix();
    glTranslatef(-0.2f,piso_Y[2],0.0f);
    piso();
    glPopMatrix();
    glPushMatrix();
    glTranslatef(0.0f,piso_Y[3],0.0f);
    piso();
    glPopMatrix();

    glutSwapBuffers();
}


// Função para lidar com o pressionamento das teclas
void keyboardFunc(unsigned char key, int x, int y) {
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

    glutMainLoop();

    return 0;
}
