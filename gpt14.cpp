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
void calculateBallColor(float ballX, float ballY, float& r, float& g, float& b) {
    // Posição da luz (ajuste conforme necessário)
    float lightX1 =  0.5f;
    float lightY1 =  -1.0f;

    float lightX2 =  0.5f;
    float lightY2 = 1.0f;

    // float lightX3 = -1.0f;
    // float lightY3 = 1.0f;

    // float lightX4 = 1.0f;
    // float lightY4 = 1.0f;

    // Vetor do observador (ajuste conforme necessário)
    float viewerX = 0.0f;
    float viewerY = 0.0f;

    // Vetores normais para as superfícies da bola (ajuste conforme necessário)
    float normalX = ballX;
    float normalY = ballY;

    // Coeficiente de especularidade (ajuste conforme necessário)
    float shininess = 32.0f;

    // Calcule a intensidade difusa e especular
    float diffuseIntensity1 = calculateDiffuseIntensity(lightX1, lightY1, normalX, normalY);
    float diffuseIntensity2 = calculateDiffuseIntensity(lightX2, lightY2, normalX, normalY);
    // float diffuseIntensity3 = calculateDiffuseIntensity(lightX3, lightY3, normalX, normalY);
    // float diffuseIntensity4 = calculateDiffuseIntensity(lightX4, lightY4, normalX, normalY);
    float specularIntensity1 = calculateSpecularIntensity(lightX1, lightY1, viewerX, viewerY, normalX, normalY, shininess);
    float specularIntensity2 = calculateSpecularIntensity(lightX2, lightY2, viewerX, viewerY, normalX, normalY, shininess);
    // float specularIntensity3 = calculateSpecularIntensity(lightX3, lightY3, viewerX, viewerY, normalX, normalY, shininess);
    // float specularIntensity4 = calculateSpecularIntensity(lightX4, lightY4, viewerX, viewerY, normalX, normalY, shininess);

    // Cores da luz (ajuste conforme necessário)
    float lightR = 1.0f;
    float lightG = 1.0f;
    float lightB = 1.0f;



        // Cores do material da bola (ajuste conforme necessário)
    float materialR = 1.2f;
    float materialG = 1.0f;
    float materialB = 1.5f;

    float ambLight = 0.2f;

    // Calcule a cor final da bola com base na iluminação

    float diffuseTimes1R = diffuseIntensity1 * lightR;
    float diffuseTimes2R = diffuseIntensity2 * lightR;
    // float diffuseTimes3R = diffuseIntensity3 * lightR;
    // float diffuseTimes4R = diffuseIntensity4 * lightR;

    float diffuseTimes1G = diffuseIntensity1 * lightG;
    float diffuseTimes2G = diffuseIntensity2 * lightG;
    // float diffuseTimes3G = diffuseIntensity3 * lightG;
    // float diffuseTimes4G = diffuseIntensity4 * lightG;

    float diffuseTimes1B = diffuseIntensity1 * lightB;
    float diffuseTimes2B = diffuseIntensity2 * lightB;
    // float diffuseTimes3B = diffuseIntensity3 * lightB;
    // float diffuseTimes4B = diffuseIntensity4 * lightB;

    r = ((diffuseTimes1R + diffuseTimes2R + specularIntensity1 + specularIntensity2 ) * materialR) + ambLight;
    g = ((diffuseTimes1G + diffuseTimes2G + specularIntensity1 + specularIntensity2 ) * materialG) + ambLight;
    b = ((diffuseTimes1B + diffuseTimes2B + specularIntensity1 + specularIntensity2 ) * materialB) + ambLight;
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
        // float lightX = 1.0f; // Posição X da luz (ajuste conforme necessário)
        // float lightY = 5.0f; // Posição Y da luz (ajuste conforme necessário)
        // float diffuseIntensity = calculateDiffuseIntensity(lightX, lightY, ballX, ballY);

        // // Cor da bola
        // float ballColor = 1.0f - diffuseIntensity; // Ajuste conforme necessário

        // // Aplicar a cor da bola
        // glColor3f(ballColor, ballColor, ballColor);

        // glBegin(GL_TRIANGLE_FAN);
        // for (int i = 0; i < 360; i++) {
        //     // Calcular intensidade de luz difusa
        //     float lightX = 1.0f; // Posição X da luz (ajuste conforme necessário)
        //     float lightY = -5.0f; // Posição Y da luz (ajuste conforme necessário)
            
        //     float angle = i * M_PI / 180.0f;
        //     float x = ballX + ballRadius * cos(angle);
        //     float y = ballY + ballRadius * sin(angle);
        //     float diffuseIntensity = calculateDiffuseIntensity(lightX, lightY, x, y);

        //     // Cor da bola
        //     float ballColor = 1.0f - diffuseIntensity; // Ajuste conforme necessário
        //     glColor3f(ballColor, ballColor, ballColor);
        //     glVertex2f(x, y);
        // }
        // glEnd();

         // Calcule a cor da bola com base na iluminação
        // float ballColorR, ballColorG, ballColorB;
        // calculateBallColor(ballX, ballY, ballColorR, ballColorG, ballColorB);

        // // Aplicar a cor da bola
        // glColor3f(ballColorR, ballColorG, ballColorB);

        glBegin(GL_TRIANGLE_FAN);
        for (int i = 0; i < 360; i++) {
            float angle = i * M_PI / 180.0f;
            float x = ballX + ballRadius * cos(angle);
            float y = ballY + ballRadius * sin(angle);

            // Calcule a cor da bola com base na iluminação
            float ballColorR, ballColorG, ballColorB;
            calculateBallColor(x, y, ballColorR, ballColorG, ballColorB);

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
