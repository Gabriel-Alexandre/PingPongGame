#include <iostream>
#include <GL/glut.h>
#include <map>
#include <cmath>
#include <vector> 

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

float randomDirection(std::vector<float> valueList) {
    valueList = {-0.5f, -0.9f, -0.95f, -1.2f, -1.5f, -1.6f, -1.77f, -1.8f, -1.85f,  0.5f, 0.85f, 0.9f, 0.95f, 0.99f, 1.2f, 1.5f, 1.6f, 1.77f, 1.8f, 1.85f};
    if (valueList.empty()) {
        return 0.0f; // Retorna 0 se a lista estiver vazia
    }
    
    int index = rand() % valueList.size(); // Gere um índice aleatório
    return valueList[index]; // Retorne o valor correspondente ao índice gerado
}

/// Função para calcular a intensidade da luz difusa
float calculateDiffuseIntensity(float lightX, float lightY, float normalX, float normalY) {
    // Vetor da luz
    float lightVectorX = lightX;
    float lightVectorY = lightY;

    // Normalize o vetor da luz
    float length = sqrt(lightVectorX * lightVectorX + lightVectorY * lightVectorY);
    lightVectorX /= length;
    lightVectorY /= length;

    // Normalize o vetor normal
    length = sqrt(normalX * normalX + normalY * normalY);
    normalX /= length;
    normalY /= length;

    // Produto escalar entre o vetor da luz e o vetor normal
    float dotProduct = lightVectorX * normalX + lightVectorY * normalY;

    // Mantenha o resultado positivo apenas (não aplicaremos sombra agora)
    dotProduct = std::max(0.0f, dotProduct);

    return dotProduct;
}

float calculateSpecularIntensity(float lightX, float lightY, float viewerX, float viewerY, float normalX, float normalY, float shininess) {
// Vetor da luz
float lightVectorX = lightX;
float lightVectorY = lightY;

// Normalize o vetor da luz
float length = sqrt(lightVectorX * lightVectorX + lightVectorY * lightVectorY);
lightVectorX /= length;
lightVectorY /= length;

// Vetor de reflexão da luz
float reflectionVectorX = 2.0f * (normalX * lightVectorX + normalY * lightVectorY) * normalX - lightVectorX;
float reflectionVectorY = 2.0f * (normalX * lightVectorX + normalY * lightVectorY) * normalY - lightVectorY;

// Vetor do observador
float viewerVectorX = viewerX;
float viewerVectorY = viewerY;

// Normalize o vetor do observador
length = sqrt(viewerVectorX * viewerVectorX + viewerVectorY * viewerVectorY);
viewerVectorX /= length;
viewerVectorY /= length;

// Produto escalar entre o vetor de reflexão da luz e o vetor do observador
float dotProduct = reflectionVectorX * viewerVectorX + reflectionVectorY * viewerVectorY;

// Elevar o resultado à potência de shininess
float specular = pow(std::max(0.0f, dotProduct), shininess);

    return specular;
}

// Função para calcular a cor da bola com base na iluminação
void calculateBallColor(float ballX, float ballY, float& r, float& g, float& b, float ambRandom, float lightXRandom, float lightYRandom) {
    // Posição da luz (ajuste conforme necessário)
    float lightX = lightXRandom;
    float lightY = lightYRandom;

    // Vetor do observador (ajuste conforme necessário)
    float viewerX = 0.0f;
    float viewerY = 0.0f;

    // Vetores normais para as superfícies da bola (ajuste conforme necessário)
    float normalX = ballX;
    float normalY = ballY;

    // Coeficiente de especularidade (ajuste conforme necessário)
    float shininess = 32.0f;

    // Calcule a intensidade difusa e especular
    float diffuseIntensity = calculateDiffuseIntensity(lightX, lightY, normalX, normalY);
    float specularIntensity = calculateSpecularIntensity(lightX, lightY, viewerX, viewerY, normalX, normalY, shininess);

    // Cores da luz (ajuste conforme necessário)
    float lightR = 1.0f;
    float lightG = 1.0f;
    float lightB = 1.0f;

        // Cores do material da bola (ajuste conforme necessário)
    float materialR = 1.2f;
    float materialG = 1.0f;
    float materialB = 1.5f;

    float ambLight = ambRandom;

    // Calcule a cor final da bola com base na iluminação
    r = ((diffuseIntensity * lightR + specularIntensity) * materialR) + ambLight;
    g = ((diffuseIntensity * lightG + specularIntensity) * materialG) + ambLight;
    b = ((diffuseIntensity * lightB + specularIntensity) * materialB) + ambLight;
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

        std::vector<float> randAmbShadow = {0.0f, 0.02f, 0.05f, 0.07f, 0.08f, 0.09f};
        float ambRandom = randomDirection(randAmbShadow);
        std::vector<float> randLightX = {-0.5f, -0.9f, -0.95f,  0.5f, 0.85f, 0.9f, 0.95f, 0.99f};
        float lightXRandom = randomDirection(randLightX);
        std::vector<float> randLightY  = {-0.5f, -0.9f, -0.95f,  0.5f, 0.85f, 0.9f, 0.95f, 0.99f};
        float lightYRandom = randomDirection(randLightY);

        glBegin(GL_TRIANGLE_FAN);
        for (int i = 0; i < 360; i++) {
            float angle = i * M_PI / 180.0f;
            float x = ballX + ballRadius * cos(angle);
            float y = ballY + ballRadius * sin(angle);

            // Calcule a cor da bola com base na iluminação
            float ballColorR, ballColorG, ballColorB;
            calculateBallColor(x, y, ballColorR, ballColorG, ballColorB, ambRandom, lightXRandom, lightYRandom);

            // Aplicar a cor da bola
            glColor3f(ballColorR, ballColorG, ballColorB);
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
    // std::cout << "oiiiiiiiiii" << std :: endl;
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
    srand(time(nullptr));
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
