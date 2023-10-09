#include <iostream>
#include <GL/glut.h>
#include <map>
#include <cmath>

// Variáveis globais
std::map<unsigned char, bool> keyState;
float paddle1Y = 0.0f;
float paddle2Y = 0.0f;
float ballX = 0.0f;
float ballY = -0.4f; // Bola posicionada abaixo do botão
float ballSpeedX = 0.005f; // Velocidade reduzida
float ballSpeedY = 0.003f;  // Velocidade reduzida
float ballRadius = 0.03f;
int player1Score = 0;
int player2Score = 0;
bool gamePaused = true; // Inicie o jogo pausado
bool gameOver = false;
bool startButtonClicked = false; // Flag para verificar se o botão "Start Game" foi clicado
bool finishButtonVisible = false; // Flag para verificar se o botão "Finish Game" deve ser exibido
float initialBallSpeedX = ballSpeedX; // Velocidade inicial da bola em X
float ballSpeedIncrease = 0.0002f; // Fator de aumento da velocidade da bola

const float paddleWidth = 0.02f;
const float paddleHeight = 0.2f;

float lightPositionX = ballX;
float lightPositionY = 1.0f; // Altura da fonte de luz acima da bola
GLuint shadowTexture;


// Função para reiniciar o jogo
void resetGame() {
    ballX = 0.0f;
    ballY = -0.4f;
    ballSpeedX = initialBallSpeedX; // Restaurar a velocidade inicial
    ballSpeedY = 0.003f;
    paddle1Y = 0.0f;
    paddle2Y = 0.0f;
    player1Score = 0;
    player2Score = 0;
    gamePaused = true;
    gameOver = false;
    startButtonClicked = false; // Redefine o botão "Start Game" para não clicado
    finishButtonVisible = false; // Redefine o botão "Finish Game" para não visível
}

// Função para calcular a intensidade da luz difusa
float calculateDiffuseIntensity(float lightX, float lightY, float ballX, float ballY) {
    // Vetor da luz
    float lightVectorX = lightX - ballX;
    float lightVectorY = lightY - ballY;

    // Normalize o vetor da luz
    float length = sqrt(lightVectorX * lightVectorX + lightVectorY * lightVectorY);
    lightVectorX /= length;
    lightVectorY /= length;

    // Vetor normal da superfície da bola (apontando para fora da bola)
    float normalVectorX = ballX;
    float normalVectorY = ballY;

    // Normalize o vetor normal
    length = sqrt(normalVectorX * normalVectorX + normalVectorY * normalVectorY);
    normalVectorX /= length;
    normalVectorY /= length;

    // Produto escalar entre o vetor da luz e o vetor normal
    float dotProduct = lightVectorX * normalVectorX + lightVectorY * normalVectorY;

    // Mantenha o resultado positivo apenas (não aplicaremos sombra agora)
    dotProduct = std::max(0.0f, dotProduct);

    return dotProduct;
}


// Função de desenho
void draw() {
    glClear(GL_COLOR_BUFFER_BIT);

    // Desenha o placar na parte superior
    glColor3f(1.0f, 1.0f, 1.0f);
    glRasterPos2f(-0.15f, 0.9f);
    std::string scoreText = "Player 1: " + std::to_string(player1Score) + "  Player 2: " + std::to_string(player2Score);
    for (char c : scoreText) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
    }

    // Exibe o botão "Start Game" se o jogo estiver pausado ou tiver terminado
    if (gamePaused || gameOver) {
        glColor3f(0.0f, 1.0f, 0.0f); // Cor verde para o botão "Start Game"
        glBegin(GL_QUADS);
        glVertex2f(-0.2f, 0.2f);
        glVertex2f(0.2f, 0.2f);
        glVertex2f(0.2f, 0.4f);
        glVertex2f(-0.2f, 0.4f);
        glEnd();

        // Texto "Start Game" no botão
        glColor3f(0.0f, 0.0f, 0.0f); // Cor preta
        glRasterPos2f(-0.08f, 0.28f);
        std::string buttonText = "Start Game";
        for (char c : buttonText) {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
        }
    }

    // Exibe o botão "Finish Game" quando um jogador ganha
    if (finishButtonVisible) {
        glColor3f(1.0f, 0.0f, 0.0f); // Cor vermelha para o botão "Finish Game"
        glBegin(GL_QUADS);
        glVertex2f(-0.2f, -0.2f);
        glVertex2f(0.2f, -0.2f);
        glVertex2f(0.2f, -0.4f);
        glVertex2f(-0.2f, -0.4f);
        glEnd();

        // Texto "Finish Game" no botão
        glColor3f(0.0f, 0.0f, 0.0f); // Cor preta
        glRasterPos2f(-0.12f, -0.32f);
        std::string buttonText = "Finish Game";
        for (char c : buttonText) {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
        }
    }

    // Se o jogo estiver em execução e não estiver pausado, desenha as raquetes e a bola
    if (!gamePaused && !gameOver) {
        glColor3f(1.0f, 1.0f, 1.0f); // Cor branca
        glBegin(GL_QUADS);
        glVertex2f(-0.95f, paddle1Y - paddleHeight / 2);
        glVertex2f(-0.93f, paddle1Y - paddleHeight / 2);
        glVertex2f(-0.93f, paddle1Y + paddleHeight / 2);
        glVertex2f(-0.95f, paddle1Y + paddleHeight / 2);

        glVertex2f(0.93f, paddle2Y - paddleHeight / 2);
        glVertex2f(0.95f, paddle2Y - paddleHeight / 2);
        glVertex2f(0.95f, paddle2Y + paddleHeight / 2);
        glVertex2f(0.93f, paddle2Y + paddleHeight / 2);
        glEnd();

        // glColor3f(1.0f, 1.0f, 1.0f); // Cor branca
        // glBegin(GL_TRIANGLE_FAN);
        // for (int i = 0; i < 360; i++) {
        //     float angle = i * M_PI / 180.0f;
        //     float x = ballX + ballRadius * cos(angle);
        //     float y = ballY + ballRadius * sin(angle);
        //     glVertex2f(x, y);
        // }
        // glEnd();


                // Calcular intensidade de luz difusa
        float lightX = 1.0f; // Posição X da luz (ajuste conforme necessário)
        float lightY = 5.0f; // Posição Y da luz (ajuste conforme necessário)
        float diffuseIntensity = calculateDiffuseIntensity(lightX, lightY, ballX, ballY);

        // Cor da bola
        float ballColor = 1.0f - diffuseIntensity; // Ajuste conforme necessário

        // Aplicar a cor da bola
        glColor3f(ballColor, ballColor, ballColor);

        glBegin(GL_TRIANGLE_FAN);
        for (int i = 0; i < 360; i++) {
            float angle = i * M_PI / 180.0f;
            float x = ballX + ballRadius * cos(angle);
            float y = ballY + ballRadius * sin(angle);
            glVertex2f(x, y);
        }
        glEnd();
     }

    glutSwapBuffers();
}

// Função de atualização
void update(int value) {
    if (!gamePaused && !gameOver) {
        ballX += ballSpeedX;
        ballY += ballSpeedY;

        if ((ballX - ballRadius) < (-0.93f) && (ballY >= paddle1Y - paddleHeight / 2) && (ballY <= paddle1Y + paddleHeight / 2)) {
            ballSpeedX = -ballSpeedX;
            ballSpeedX += ballSpeedIncrease; // Aumenta a velocidade da bola
        }

        if ((ballX + ballRadius) > (0.93f) && (ballY >= paddle2Y - paddleHeight / 2) && (ballY <= paddle2Y + paddleHeight / 2)) {
            ballSpeedX = -ballSpeedX;
            ballSpeedX -= ballSpeedIncrease; // Aumenta a velocidade da bola
        }

        if (ballY + ballRadius > 1.0f || ballY - ballRadius < -1.0f) {
            ballSpeedY = -ballSpeedY;
        }

        if (ballX - ballRadius < -1.0f) {
            player2Score++;
            ballX = 0.0f;
            ballY = -0.4f;
            ballSpeedX = initialBallSpeedX; // Restaurar a velocidade inicial
            ballSpeedY = 0.003f;

            if (player2Score >= 15) {
                gameOver = true;
                finishButtonVisible = true; // Exibe o botão "Finish Game"
            }
        } else if (ballX + ballRadius > 1.0f) {
            player1Score++;
            ballX = 0.0f;
            ballY = -0.4f;
            ballSpeedX = -initialBallSpeedX; // Restaurar a velocidade inicial
            ballSpeedY = 0.003f;

            if (player1Score >= 15) {
                gameOver = true;
                finishButtonVisible = true; // Exibe o botão "Finish Game"
            }
        }

        // Movimento das raquetes
        if (keyState['w'] && (paddle1Y + paddleHeight / 2) < 1.0f) {
            paddle1Y += 0.01f;
        }
        if (keyState['s'] && (paddle1Y - paddleHeight / 2) > -1.0f) {
            paddle1Y -= 0.01f;
        }
        if (keyState['i'] && (paddle2Y + paddleHeight / 2) < 1.0f) {
            paddle2Y += 0.01f;
        }
        if (keyState['k'] && (paddle2Y - paddleHeight / 2) > -1.0f) {
            paddle2Y -= 0.01f;
        }
    }

    glutPostRedisplay();
    glutTimerFunc(16, update, 0);
}
// Função para lidar com teclas especiais (tecla espaço)
void specialKeyPressed(GLint key, GLint x, GLint y) {
    std::cout << "oiiiiiiiiii" << std :: endl;
    if (key == GLUT_KEY_DOWN) {
        std::cout << "oiii" << std :: endl;
        gamePaused = !gamePaused; // Inverte o estado de pausa do jogo
    }
}


// Função para lidar com cliques do mouse
void mouseClick(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        // Verifica se o clique do mouse ocorreu dentro do botão "Start Game"
        if (x >= 300 && x <= 500 && y >= 300 && y <= 400) {
            if (gamePaused || gameOver || startButtonClicked) {
                resetGame();
                gamePaused = false;
                startButtonClicked = true; // Define o botão "Start Game" como clicado
            }
        }

        // Verifica se o clique do mouse ocorreu dentro do botão "Finish Game"
        if (finishButtonVisible && x >= 300 && x <= 500 && y >= 200 && y <= 300) {
            exit(0); // Fecha o jogo
        }
    }
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Ping Pong Game");

    glutDisplayFunc(draw);
    glutTimerFunc(16, update, 0);
    glutKeyboardFunc([](unsigned char key, int x, int y) {
        keyState[key] = true;
        if (key == 'r' && gameOver) {
            resetGame();
        }
    });
    glutKeyboardUpFunc([](unsigned char key, int x, int y) {
        keyState[key] = false;
    });
    glutSpecialFunc(specialKeyPressed);
    glutMouseFunc(mouseClick);

    resetGame();

    glutMainLoop();
    return 0;
}
