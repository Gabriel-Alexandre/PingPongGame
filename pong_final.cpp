#include <iostream>
#include <GL/glut.h>
#include <map>
#include <cmath>
#include <thread> // Para usar a função sleep_for
#include <future>
#include <chrono>
#include <vector> 

#include "audio_pong.cpp"
#include "iluminacao_pong.cpp"
#include "aux_pong.cpp"

// Alunos: Gabriel Alexandre Carvalho (20190021438), Laura Francine Aráujo Silva (20180025872)

// sudo apt-get install libopenal-dev (Comando para instalar biblioteca de áudio)
// g++ pong_final.cpp -o pong_final -lGL -lGLU -lglut -lopenal (Comando pra compilar código)

// Variáveis globais
std::map<int, bool> keyState; // Estado da tecla
const float raqueteHeight = 0.3f;
float raquete1Y = 0.0f; 
float raquete2Y = 0.0f;
float bolaX = 0.0f;
float bolaY = -0.4f; // Bola posicionada abaixo do botão
float bolaSpeedX = 0.005f; // Velocidade reduzida
float bolaSpeedY = 0.003f;  // Velocidade reduzida
float bolaRaio = 0.03f;
int player1Score = 0;
int player2Score = 0;
bool player1Scored = false;
bool player2Scored = false;
bool varStart = true;
bool escolhePosicaoBola = false;
bool gamePause = true; // Inicie o jogo pausado
bool gameOver = false;
bool botaoInicioClicked = false; // Flag para verificar se o botão "Start Game" foi clicado
bool botaoFimVisible = false; // Flag para verificar se o botão "Finish Game" deve ser exibido
bool botaoInicioVisible = true; // Flag para verificar se o botão "Finish Game" deve ser exibido
bool linhaVisible = true;
bool isHard = false;
int resultTextVisible = -1; // -1: texto não aparece, 1: aparece texto do jogador 1, 2: aparece texto do jogador 2
float bolaInicialSpeedX = bolaSpeedX; // Velocidade inicial da bola em X
float bolaSpeedIncrease = 0.002f; // Fator de aumento da velocidade da bola

// Inicando vetor de valores aleatórios para algumas variáveis usadas ao longo do programa
std::vector<float> valueList = {-0.5f, -0.9f, -0.95f, -1.2f, -1.5f, -1.6f, -1.77f, -1.8f, -1.85f,  0.5f, 0.85f, 0.9f, 0.95f, 0.99f, 1.2f, 1.5f, 1.6f, 1.77f, 1.8f, 1.85f};
float angleBall = randomDirection(valueList);
std::vector<float> randAmbShadow = {0.0f, 0.02f, 0.05f, 0.07f, 0.08f, 0.09f};
float ambRandom = randomDirection(randAmbShadow);
std::vector<float> randLightX = {-0.5f, -0.9f, -0.95f,  0.5f, 0.85f, 0.9f, 0.95f, 0.99f};
float luzXRandom = randomDirection(randLightX);
std::vector<float> randLightY  = {-0.5f, -0.9f, -0.95f,  0.5f, 0.85f, 0.9f, 0.95f, 0.99f};
float luzYRandom = randomDirection(randLightY);

// Função para reiniciar o jogo
void resetGame() {
    bolaX = 0.0f;
    bolaY = -0.4f;
    bolaSpeedX = bolaInicialSpeedX; // Restaurar a velocidade inicial
    bolaSpeedY = 0.003f;
    raquete1Y = 0.0f;
    raquete2Y = 0.0f;
    player1Score = 0;
    player2Score = 0;
    gamePause = true;
    gameOver = false;
    botaoInicioClicked = false; // Redefine os botões de início para não clicado
    botaoFimVisible = false; // Redefine o botão "Finish Game" para não visível
    escolhePosicaoBola = false;
    resultTextVisible = -1;
    linhaVisible = true;
}

// Função de desenho
void draw() {
    glClear(GL_COLOR_BUFFER_BIT);

    // Desenha linha central 
    if(linhaVisible) {
        // Desenha a linha vertical pontilhada no meio da tela
        glColor3f(1.0f, 1.0f, 1.0f); // Cor branca
        glLineWidth(2.0f); // Espessura da linha

        // Define o padrão de repetição dos pontos (pontilhado)
        glEnable(GL_LINE_STIPPLE);
        glLineStipple(10, 0xAAAA); // Padrão para linha pontilhada

        glBegin(GL_LINES);
        glVertex2f(0.0f, 1.0f); // Ponto inicial (meio superior)
        glVertex2f(0.0f, -1.0f); // Ponto final (meio inferior)
        glEnd();

        glDisable(GL_LINE_STIPPLE); // Desativa o padrão de linha pontilhada
    }


    // Desenha o placar na parte superior
    glColor3f(1.0f, 1.0f, 1.0f);
    glRasterPos2f(-0.30f, 0.87f);
    std::string scoreText = "Player 1: " + std::to_string(player1Score) + "     Player 2: " + std::to_string(player2Score);
    for (char c : scoreText) {
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, c);
    }
    
    // Desenha resultado de quem ganhou
    if(resultTextVisible == 1) {
        glRasterPos2f(-0.17f, 0.6f);
        glColor3f(1.0f, 1.0f, 1.0f); // Define a cor do texto (branco)

        std::string resultText = "Player 1 Won!";
        // Percorre a string e desenha cada caractere
        for (char c : resultText) {
            glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, c); // Use uma fonte específica, neste caso, Times Roman 24
        }
    } else if(resultTextVisible == 2) {
        glRasterPos2f(-0.17f, 0.6f);
        glColor3f(1.0f, 1.0f, 1.0f); // Define a cor do texto (branco)

        std::string resultText = "Player 2 Won!";
        // Percorre a string e desenha cada caractere
        for (char c : resultText) {
            glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, c); // Use uma fonte específica, neste caso, Times Roman 24
        }
     }

    // Exibe os botões de início se o jogo tiver terminado ou a primeira vez
    if (varStart || gameOver) {
        
        glColor3f(0.0f, 1.0f, 0.0f); // Cor verde para o botão "EASY MODE"
        glBegin(GL_QUADS);
        glVertex2f(-0.2f, 0.2f);
        glVertex2f(0.2f, 0.2f);
        glVertex2f(0.2f, 0.4f);
        glVertex2f(-0.2f, 0.4f);
        glEnd();

        // Texto "EASY MODE" no botão
        glColor3f(0.0f, 0.0f, 0.0f); // Cor preta
        glRasterPos2f(-0.13f, 0.28f);
        
        std::string buttonText = "EASY MODE";
        for (char c : buttonText) {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
        }

        glColor3f(255.0f, 165.0f, 0.0f); // Cor amarela para o botão "HARD MODE"
        glBegin(GL_QUADS);
        glVertex2f(-0.2f, 0.1f);
        glVertex2f(0.2f,  0.1f);
        glVertex2f(0.2f, -0.1f);
        glVertex2f(-0.2f, -0.1f);
        glEnd();

        // Texto "HARD MODE" no botão
        glColor3f(0.0f, 0.0f, 0.0f); // Cor preta
        glRasterPos2f(-0.13f, -0.02f);
        
        buttonText = "HARD MODE";
        for (char c : buttonText) {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
        }
        
        
    }
   
    // Exibe o botão "FINISH GAME" quando um jogador ganha
    if (botaoFimVisible) {
        glColor3f(1.0f, 0.0f, 0.0f); // Cor vermelha para o botão "FINISH GAME"
        glBegin(GL_QUADS);
        glVertex2f(-0.2f, -0.2f);
        glVertex2f(0.2f, -0.2f);
        glVertex2f(0.2f, -0.4f);
        glVertex2f(-0.2f, -0.4f);
        glEnd();

        // Texto "FINISH GAME" no botão
        glColor3f(0.0f, 0.0f, 0.0f); // Cor preta
        glRasterPos2f(-0.14f, -0.32f);
        std::string buttonText = "FINISH GAME";
        for (char c : buttonText) {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
        }
    }

    // Se o jogo estiver em execução, desenha as raquetes e a bola
    if (!gameOver) {

        // Desenha as raquetes
        glColor3f(1.0f, 1.0f, 1.0f); // Cor branca
        glBegin(GL_QUADS);
        glVertex2f(-0.95f, raquete1Y - raqueteHeight / 2);
        glVertex2f(-0.93f, raquete1Y - raqueteHeight / 2);
        glVertex2f(-0.93f, raquete1Y + raqueteHeight / 2);
        glVertex2f(-0.95f, raquete1Y + raqueteHeight / 2);

        glVertex2f(0.93f, raquete2Y - raqueteHeight / 2);
        glVertex2f(0.95f, raquete2Y - raqueteHeight / 2);
        glVertex2f(0.95f, raquete2Y + raqueteHeight / 2);
        glVertex2f(0.93f, raquete2Y + raqueteHeight / 2);
        glEnd();

        // Desenha bola de acordo com a dificuldade escolhida
        glBegin(GL_TRIANGLE_FAN);
        for (int i = 0; i < 360; i++) {
            float angle = i * M_PI / 180.0f;
            float x = bolaX + bolaRaio * cos(angle);
            float y = bolaY + bolaRaio * sin(angle);

            // Calcule a cor da bola com base na iluminação (se for HARD)
            float ballColorR, ballColorG, ballColorB;
            if(isHard && !varStart) {
                calculaCorBola(x, y, ballColorR, ballColorG, ballColorB, ambRandom, luzXRandom, luzYRandom);
            } else {
                // Inicia a bola como branca
                ballColorR = 1.0f;
                ballColorG = 1.0f;
                ballColorB = 1.0f;
            }

            // Inicia a bola como branca na raquete
            if(escolhePosicaoBola) {
                ballColorR = 1.0f;
                ballColorG = 1.0f;
                ballColorB = 1.0f;
            }

            // Aplicar a cor da bola
            glColor3f(ballColorR, ballColorG, ballColorB);
            glVertex2f(x, y);
        }
        glEnd();
    }

    // Double buffer
    glutSwapBuffers();
}

// Função de atualização
void renderGame(int value) {
    // Executa se o jogo não estiver pausado, não tiver sido finalizado e não tiver sido iniciado pela primeira vez
    if (!gamePause && !gameOver && !varStart) {
        // Trata se a bola está sendo lançado ou está se movendo
        if (!escolhePosicaoBola) {
            // Calcula movimento da bola em X e Y
            bolaX += bolaSpeedX;
            bolaY += (angleBall * bolaSpeedY);
        } else {
            // Calcula posição de lançamento da bola de acordo com quem marcou ponto
            if(player1Scored) {
                bolaSpeedX = -bolaInicialSpeedX;
                bolaX = 0.90f; // Posiciona a bola no centro
                bolaY = raquete2Y;
            } else {
                bolaSpeedX = bolaInicialSpeedX;
                bolaX = -0.90f; // Posiciona a bola no centro
                bolaY = raquete1Y;
            }
        }

        // Identifica colisão da bola na raquete do jogador 1
        if ((bolaX - bolaRaio) < (-0.93f) && (bolaY >= raquete1Y - raqueteHeight / 2) && (bolaY <= raquete1Y + raqueteHeight / 2)) {
            // Aumenta a velocidade da bola a cada colisão
            bolaSpeedX = -bolaSpeedX;
            bolaSpeedX += bolaSpeedIncrease; 

            // Verifica dificuldade
            if(isHard) {
                // HARD MODE
                // Gera valores aleatórios para ângulo, luz ambiente e vetor de iluminação
                std::vector<float> valueList = {-0.5f, -0.9f, -0.95f, -1.2f, -1.5f, -1.6f, -1.77f, -1.8f, -1.85f,  0.5f, 0.85f, 0.9f, 0.95f, 0.99f, 1.2f, 1.5f, 1.6f, 1.77f, 1.8f, 1.85f};
                angleBall = randomDirection(valueList);
                std::vector<float> randAmbShadow = {0.05f, 0.07f, 0.08f, 0.09f};
                ambRandom = randomDirection(randAmbShadow);
                std::vector<float> randLightX = {-0.9f, -0.95f, 0.9f, 0.95f};
                luzXRandom = randomDirection(randLightX);
                std::vector<float> randLightY  = {-0.5f, -0.9f, -0.95f,  0.5f, 0.85f, 0.9f, 0.95f, 0.99f};
                luzYRandom = randomDirection(randLightY);
            } else {
                // EASY MODE
                // Gera valor aleatório para ângulo
                std::vector<float> valueList = {-0.5f, -0.9f, -0.95f, -1.2f, -1.5f, -1.6f, -1.77f, 0.5f, 0.85f, 0.9f, 0.95f, 0.99f};
                angleBall = randomDirection(valueList);
            }
            // Executa som de rebatida
            alSourcePlay(bounceSound); 
        }


        // Identifica colisão da bola na raquete do jogador 2
        if ((bolaX + bolaRaio) > (0.93f) && (bolaY >= raquete2Y - raqueteHeight / 2) && (bolaY <= raquete2Y + raqueteHeight / 2)) {
            bolaSpeedX = -bolaSpeedX;
            bolaSpeedX -= bolaSpeedIncrease; 
            if(isHard) {
                std::vector<float> valueList = {-0.5f, -0.9f, -0.95f, -1.2f, -1.5f, -1.6f, -1.77f, -1.8f, -1.85f,  0.5f, 0.85f, 0.9f, 0.95f, 0.99f, 1.2f, 1.5f, 1.6f, 1.77f, 1.8f, 1.85f};
                angleBall = randomDirection(valueList);
                std::vector<float> randAmbShadow = {0.0f, 0.02f, 0.05f, 0.07f, 0.08f, 0.09f};
                ambRandom = randomDirection(randAmbShadow);
                std::vector<float> randLightX = {-0.5f, -0.9f, -0.95f,  0.5f, 0.85f, 0.9f, 0.95f, 0.99f};
                luzXRandom = randomDirection(randLightX);
                std::vector<float> randLightY  = {-0.5f, -0.9f, -0.95f,  0.5f, 0.85f, 0.9f, 0.95f, 0.99f};
                luzYRandom = randomDirection(randLightY);
            } else {
                std::vector<float> valueList = {-0.5f, -0.9f, -0.95f, -1.2f, -1.5f, -1.6f, -1.77f, 0.5f, 0.85f, 0.9f, 0.95f, 0.99f};
                angleBall = randomDirection(valueList);
            }
            alSourcePlay(bounceSound); 
        }

        // Identifica colisão nas laterais horizontais da tela
        if (bolaY + bolaRaio > 1.0f || bolaY - bolaRaio < -1.0f) {
            bolaSpeedY = -bolaSpeedY;
        }

        // Verifica se jogador 2 marcou ponto
        if (bolaX - bolaRaio < -1.0f) {
            player2Score++;
            player2Scored = true;
            escolhePosicaoBola = true; // O jogador 1 escolherá a posição de lançamento
            // Restaurar a velocidade inicial
            bolaSpeedX = bolaInicialSpeedX; 
            bolaSpeedY = 0.003f;
            // Verifica se jogador 2 ganhou
            if (player2Score >= 15) {
                // Para som do jogo
                alSourceStop(gamingSound);
                // Toca som do vencedor
                alSourcePlay(winnerSound); 
                // Espera 3 segundos e toca som do inicio de jogo
                std::future<void> result = std::async(std::launch::async, []() {
                    std::this_thread::sleep_for(std::chrono::seconds(3));
                    alSourcePlay(gameStartSound); 
                });
                resultTextVisible = 2; // aparece texto informando que o jogador 2 venceu
                gameOver = true; // Seta fim de jogo
                botaoFimVisible = true; // Exibe o botão "FINISH GAME"
                botaoInicioVisible = true; // Exibe os botões de início
                linhaVisible = false; // Esconde linha central
            } else {
                // Som de marcar ponto
                alSourcePlay(pointSound); 
            }
        // Verifica se jogador 1 marcou ponto
        } else if (bolaX + bolaRaio > 1.0f) {
            player1Score++;
            player1Scored = true;
            escolhePosicaoBola = true; 
            bolaSpeedX = -bolaInicialSpeedX; 
            bolaSpeedY = 0.003f;
            if (player1Score >= 15) {
                alSourceStop(gamingSound);
                alSourcePlay(winnerSound); 
                std::future<void> result = std::async(std::launch::async, []() {
                    std::this_thread::sleep_for(std::chrono::seconds(3));
                    alSourcePlay(gameStartSound); 
                });
                resultTextVisible = 1;
                gameOver = true;
                botaoFimVisible = true;
                botaoInicioVisible = true;
                linhaVisible = false;
            } else {
                alSourcePlay(pointSound); 
            }
        }

        // Movimento das raquetes
        if (keyState['w'] && (raquete1Y + raqueteHeight / 2) < 1.0f) {
            raquete1Y += 0.01f;
        }
        if (keyState['s'] && (raquete1Y - raqueteHeight / 2) > -1.0f) {
            raquete1Y -= 0.01f;
        }
        if (keyState[101] && (raquete2Y + raqueteHeight / 2) < 1.0f) {
            raquete2Y += 0.01f;
        }
        if (keyState[103] && (raquete2Y - raqueteHeight / 2) > -1.0f) {
            raquete2Y -= 0.01f;
        }
    }

    glutPostRedisplay();
    glutTimerFunc(16, renderGame, 0);
}

// Função para lidar com cliques do mouse
void mouseClick(int button, int state, int x, int y) {
    // Se for clicado com botão esquerdo do mouse nos botões que aparecem na tela
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        // Verifica se o clique do mouse ocorreu dentro do botão "EASY MODE"
        if (botaoInicioVisible && ((x >= 320 && x <= 481 && y >= 180 && y <= 241) || (x >= 745 && x <= 1115 && y >= 305 && y <= 408)))  {
            alSourceStop(gameStartSound); // Para o som da tela de início
            alSourcePlay(gamingSound); // Começa a tocar o som do jogo
            resetGame(); // Inicia jogo
            botaoInicioVisible = false; // Os botões de EASY MODE e HARD MODE não estão mais visíveis
            gamePause = false; // Despausa jogo
            varStart = false; // Define como falso a variável de controle dos botões de início para a primeira vez do jogo
            isHard = false; // Define dificuldade como EASY
            botaoInicioClicked = true; // Define o botão de inicio como clicado
        }

        // Verifica se o clique do mouse ocorreu dentro do botão "HARD MODE"
        if(botaoInicioVisible && ((x >= 320 && x <= 481 && y >= 270 && y <= 333) || (x >= 745 && x <= 1115 && y >= 457 && y <= 561))) {
            alSourceStop(gameStartSound);
            alSourcePlay(gamingSound);
            resetGame();
            botaoInicioVisible = false;
            gamePause = false;
            varStart = false;
            isHard = true;
            botaoInicioClicked = true;
        }

        // Verifica se o clique do mouse ocorreu dentro do botão "FINISH GAME"
        if (botaoFimVisible && ((x >= 320 && x <= 480 && y >= 360 && y <= 420) || (x >= 745 && x <= 1115 && y >= 612 && y <= 712))) {
            exit(0); // Fecha o jogo
        }
    }
}

// Função para lidar com as teclas especiais 
void handle_SpecialFunc(GLint key, GLint x, GLint y){
    // tecla cima e tecla baixo
    // Define tecla pressionada como true
    keyState[key] = true;
}

// Função para lidar com eventos do teclado
void handle_KeyboardFunc(unsigned char key, int x, int y) {
    // Define tecla pressionada como true
    keyState[key] = true;

    // 32 = Tecla "espaço" (Pausa e Despausa o jogo)
    if (key == 32) {
        gamePause = !gamePause;
    }

    // Lança bola na posição escolhida
    if(escolhePosicaoBola) {

        if (key == 13) { // 13 = tecla "Enter"
            // A bola será lançada na posição atual do jogador que marcou pontos
            escolhePosicaoBola = false;
            if(player1Scored) {
                bolaSpeedX = -bolaInicialSpeedX;
            } else if (player2Scored) {
                bolaSpeedX = bolaInicialSpeedX;
            }
            // Reseta variáveis de controle de scored dos jogadores
            player1Scored = false;
            player2Scored = false;
        }
    }
}

// Função main
int main(int argc, char** argv) {
    srand(time(nullptr)); 
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Ping Pong Game");
    initAudio();
    
    alSourcePlay(gameStartSound);
    glutDisplayFunc(draw);
    glutTimerFunc(16, renderGame, 0);
    glutKeyboardFunc(handle_KeyboardFunc);
    // Reseta tecla que foi clicada
    glutKeyboardUpFunc([](unsigned char key, int x, int y) {
        keyState[key] = false;
    });
    glutMouseFunc(mouseClick);
    glutSpecialFunc(handle_SpecialFunc);
    glutSpecialUpFunc([](int key, int x, int y) {
        keyState[key] = false;
    });
    resetGame();

    glutMainLoop();
    return 0;
}
