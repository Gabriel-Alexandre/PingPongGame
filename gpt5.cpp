#include <iostream>
#include <GL/glut.h>
#include <map>
#include <cmath>

// Variáveis globais
std::map<int, bool> keyState;
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
float ballSpeedIncrease = 0.002f; // Fator de aumento da velocidade da bola
bool varStart = true;
bool player1Scored = false;
bool player2Scored = false;
bool chooseBallPosition = false;
float launchPositionY = 0.0f;

const float paddleWidth = 0.02f;
const float paddleHeight = 0.2f;



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
    if (varStart || gameOver) {
        
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

    // Se o jogo estiver em execução, desenha as raquetes e a bola
    if (!gameOver) {
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

        glColor3f(1.0f, 1.0f, 1.0f); // Cor branca
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
        if (!chooseBallPosition) {
            ballX += ballSpeedX;
            ballY += ballSpeedY;
        } else {
            if(player1Scored) {
                ballSpeedX = -initialBallSpeedX;
                ballX = 0.90f; // Posiciona a bola no centro
                ballY = paddle2Y;
            } else {
                ballSpeedX = initialBallSpeedX;
                ballX = -0.90f;
                ballY = paddle1Y;
            }
        }

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
            player2Scored = true;
            chooseBallPosition = true; // O jogador 2 escolherá a posição de lançamento
            ballSpeedX = initialBallSpeedX; // Restaurar a velocidade inicial
            ballSpeedY = 0.003f;

            if (player2Score >= 4) {
                gameOver = true;
                finishButtonVisible = true; // Exibe o botão "Finish Game"
            }
        } else if (ballX + ballRadius > 1.0f) {
            player1Score++;
            player1Scored = true;
            chooseBallPosition = true; // O jogador 1 escolherá a posição de lançamento
            ballSpeedX = -initialBallSpeedX; // Restaurar a velocidade inicial
            ballSpeedY = 0.003f;

            if (player1Score >= 4) {
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

// Função para lidar com cliques do mouse
void mouseClick(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        // Verifica se o clique do mouse ocorreu dentro do botão "Start Game"
        if (x >= 320 && x <= 481 && y >= 180 && y <= 241)  {
            if (gamePaused || gameOver || startButtonClicked) {
                resetGame();
                gamePaused = false;
                varStart = false;
                startButtonClicked = true; // Define o botão "Start Game" como clicado
            }
        }

        // Verifica se o clique do mouse ocorreu dentro do botão "Finish Game"
        if (finishButtonVisible && x >= 320 && x <= 480 && y >= 360 && y <= 420) {
            exit(0); // Fecha o jogo
        }
    }
}

// Função de teclado para escolher a posição de lançamento da bola
void keyPressed(unsigned char key, int x, int y) {
    keyState[key] = true;
    if (key == 32) {
        gamePaused = !gamePaused;
    }
    if (key == 'r' && gameOver) {
        resetGame();
    }
    if (chooseBallPosition) {
        if (key == 13) { // 13 é o valor ASCII da tecla "Enter"
            // A bola será lançada na posição atual do jogador que marcou pontos
            chooseBallPosition = false;
            if(player1Scored) {
                ballSpeedX = -initialBallSpeedX;
            } else if (player2Scored) {
                ballSpeedX = initialBallSpeedX;
            }
            player1Scored = false;
            player2Scored = false;
        }
        
        if (key == 'w' && (launchPositionY + ballRadius) < 1.0f) {
            launchPositionY += 0.04f; // Mova a posição de lançamento para cima
        }
        if (key == 's' && (launchPositionY - ballRadius) > -1.0f) {
            launchPositionY -= 0.04f; // Mova a posição de lançamento para baixo
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
    glutKeyboardFunc(keyPressed);
    glutKeyboardUpFunc([](unsigned char key, int x, int y) {
        keyState[key] = false;
    });
    glutMouseFunc(mouseClick);

    resetGame();

    glutMainLoop();
    return 0;
}
