#include <iostream>
#include <GL/glut.h>
#include <map>
#include <cmath>
#include <thread> // Para usar a função sleep_for
#include <future>
#include <chrono>
// sudo apt-get install libopenal-dev

#include <AL/al.h>
#include <AL/alc.h>
// #include <AL/alut.h>
#include "stb_vorbis.c" // Certifique-se de incluir o arquivo de código-fonte stb_vorbis.c na mesma pasta do seu projeto.

ALuint bounceSound; // ID do som de rebatida
ALuint pointSound;  // ID do som de ponto
ALuint winnerSound;  // ID do som de ponto
ALuint gameStartSound;  // ID do som de ponto
ALuint gamingSound;  // ID do som de ponto
ALCdevice* audioDevice;
ALCcontext* audioContext;


// Variáveis globais
static char score_1[20], score_2[20];
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
bool startButtonVisible = true; // Flag para verificar se o botão "Finish Game" deve ser exibido
int resultTextVisible = -1;
float initialBallSpeedX = ballSpeedX; // Velocidade inicial da bola em X
float ballSpeedIncrease = 0.002f; // Fator de aumento da velocidade da bola
bool varStart = true;
bool player1Scored = false;
bool player2Scored = false;
bool chooseBallPosition = false;
float launchPositionY = 0.0f;
ALuint bounceSource; // Variável para a fonte de som de rebatida
ALuint pointSource;  // Variável para a fonte de som de ponto
ALuint winnerSource;  // Variável para a fonte de som de winner
ALuint gameStartSource;  // Variável para a fonte de som de gameStart
ALuint gamingSource;  // Variável para a fonte de som de gaming

const float paddleWidth = 0.02f;
const float paddleHeight = 0.3f;

static GLint orthoSizeX = 600, orthoSizeY = 400;



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
    chooseBallPosition = false;
    launchPositionY = 0.0f;
    resultTextVisible = -1;
}

void initAudio() {
    // Inicializa o dispositivo de áudio
    audioDevice = alcOpenDevice(nullptr);
    audioContext = alcCreateContext(audioDevice, nullptr);
    alcMakeContextCurrent(audioContext);

    // Crie uma fonte de som para a rebatida
    alGenSources(1, &bounceSource);
    
    // Carrega o som de rebatida
    alGenBuffers(1, &bounceSound);
    {
        int channels, sampleRate;
        short* soundData;
        int samples = stb_vorbis_decode_filename("bounce_sound.ogg", &channels, &sampleRate, &soundData);
        if (samples != -1) {
            alBufferData(bounceSound, AL_FORMAT_MONO16, soundData, samples * sizeof(short), sampleRate);
            free(soundData);
            ALenum error = alGetError();
            if (error != AL_NO_ERROR) {
                std::cout << "Erro ao carregar som de rebatida: " << alGetString(error) << std::endl;
                // Trate o erro de carregamento do som aqui
            } else {
                std::cout << "Som de rebatida carregado com sucesso." << std::endl;
            }
        }
    }

    // Crie uma fonte de som para a rebatida
    alGenSources(1, &winnerSource);
    
    // Carrega o som de rebatida
    alGenBuffers(1, &winnerSound);
    {
        int channels, sampleRate;
        short* soundData;
        int samples = stb_vorbis_decode_filename("winner.ogg", &channels, &sampleRate, &soundData);
        if (samples != -1) {
            alBufferData(winnerSound, AL_FORMAT_MONO16, soundData, samples * sizeof(short), sampleRate);
            free(soundData);
            ALenum error = alGetError();
            if (error != AL_NO_ERROR) {
                std::cout << "Erro ao carregar som de winner: " << alGetString(error) << std::endl;
                // Trate o erro de carregamento do som aqui
            } else {
                std::cout << "Som de winner carregado com sucesso." << std::endl;
            }
        }
    }

    // Crie uma fonte de som para o ponto
    alGenSources(1, &pointSource);
    
    // Carrega o som de ponto
    alGenBuffers(1, &pointSound);
    {
        int channels, sampleRate;
        short* soundData;
        int samples = stb_vorbis_decode_filename("point_sound.ogg", &channels, &sampleRate, &soundData);
        if (samples != -1) {
            alBufferData(pointSound, AL_FORMAT_MONO16, soundData, samples * sizeof(short), sampleRate);
            free(soundData);
            ALenum error = alGetError();
            if (error != AL_NO_ERROR) {
                std::cout << "Erro ao carregar som de ponto: " << alGetString(error) << std::endl;
                // Trate o erro de carregamento do som aqui
            } else {
                std::cout << "Som de ponto carregado com sucesso." << std::endl;
            }
        }
    }

    // Crie uma fonte de som para a gameStart
    alGenSources(1, &gameStartSource);
    
    // Carrega o som de gameStart
    alGenBuffers(1, &gameStartSound);
    {
        int channels, sampleRate;
        short* soundData;
        int samples = stb_vorbis_decode_filename("gameStart.ogg", &channels, &sampleRate, &soundData);
        if (samples != -1) {
            alBufferData(gameStartSound, AL_FORMAT_MONO16, soundData, samples * sizeof(short), sampleRate);
            free(soundData);
            ALenum error = alGetError();
            if (error != AL_NO_ERROR) {
                std::cout << "Erro ao carregar som de gameStart: " << alGetString(error) << std::endl;
                // Trate o erro de carregamento do som aqui
            } else {
                std::cout << "Som de gameStart carregado com sucesso." << std::endl;
            }
        }
    }

    // Crie uma fonte de som para a gaming
    alGenSources(1, &gamingSource);
    
    // Carrega o som de gaming
    alGenBuffers(1, &gamingSound);
    {
        int channels, sampleRate;
        short* soundData;
        int samples = stb_vorbis_decode_filename("music-game.ogg", &channels, &sampleRate, &soundData);
        if (samples != -1) {
            alBufferData(gamingSound, AL_FORMAT_MONO16, soundData, samples * sizeof(short), sampleRate);
            free(soundData);
            ALenum error = alGetError();
            if (error != AL_NO_ERROR) {
                std::cout << "Erro ao carregar som de gaming: " << alGetString(error) << std::endl;
                // Trate o erro de carregamento do som aqui
            } else {
                std::cout << "Som de gaming carregado com sucesso." << std::endl;
            }
        }
    }

    // Configure a fonte de som para o som de rebatida
    alSourcei(bounceSource, AL_BUFFER, bounceSound);
    alSourcef(bounceSource, AL_GAIN, 1.0f); // Defina o volume (ajuste conforme necessário)

    // Configure a fonte de som para o som de ponto
    alSourcei(pointSource, AL_BUFFER, pointSound);
    alSourcef(pointSource, AL_GAIN, 1.0f); // Defina o volume (ajuste conforme necessário)

    // Configure a fonte de som para o som de ponto
    alSourcei(winnerSource, AL_BUFFER, winnerSound);
    alSourcef(winnerSource, AL_GAIN, 1.0f); // Defina o volume (ajuste conforme necessário)

    // Configure a fonte de som para o som de ponto
    alSourcei(gameStartSource, AL_BUFFER, gameStartSound);
    alSourcef(gameStartSource, AL_GAIN, 1.0f); // Defina o volume (ajuste conforme necessário)

    // Configure a fonte de som para o som de ponto
    alSourcei(gamingSource, AL_BUFFER, gamingSound);
    alSourcef(gamingSource, AL_GAIN, 1.0f); // Defina o volume (ajuste conforme necessário)

    alSourcef(bounceSource, AL_PITCH, 2.0f);
    alSourcef(pointSource, AL_PITCH, 2.0f);
    alSourcef(winnerSource, AL_PITCH, 2.0f);
    alSourcef(gameStartSource, AL_PITCH, 2.0f);
    alSourcef(gamingSource, AL_PITCH, 1.0f);

   
}

// draw text on screen
void drawStrokeText(char*string, int x, int y, int z)
{
    char *c;
    glColor3f(1.0f, 1.0f, 1.0f);
    glPushMatrix();
    glTranslatef(x, y+8,z);

    // glScalef(0.09f,-0.08f,z);
    for (c=string; *c != '\0'; c++)
    {
        glutStrokeCharacter(GLUT_STROKE_ROMAN , *c);
    }
    glPopMatrix();
}

// Função de desenho
void draw() {
    glClear(GL_COLOR_BUFFER_BIT);

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

    // Desenha o placar na parte superior
    // glColor3f(1.0f, 1.0f, 1.0f);
    // glRasterPos2f(-0.15f, 0.9f);
    // std::string scoreText = "Player 1: " + std::to_string(player1Score) + "  Player 2: " + std::to_string(player2Score);
    // for (char c : scoreText) {
    //     glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
    // }

    
    
    // glColor3f(1.0f, 1.0f, 1.0f); // Cor branca
    // glRasterPos2f(-0.1f, 0.5f); // Posição para o placar do jogador 1
    // std::string player1ScoreText = std::to_string(player1Score);
    // for (char c : player1ScoreText) {
    //     glutBitmapCharacter(GLUT_BITMAP_, c);
    // }

    // glRasterPos2f(0.07f, 0.5f); // Posição para o placar do jogador 2
    // std::string player2ScoreText = std::to_string(player2Score);
    // for (char c : player2ScoreText) {
    //     glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
    // }

    if(resultTextVisible == 1) {
        glRasterPos2f(-0.15f, 0.6f);
        glColor3f(1.0f, 1.0f, 1.0f); // Define a cor do texto (branco)

        std::string resultText = "Player 1 Won!";
        // Percorre a string e desenha cada caractere
        for (char c : resultText) {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c); // Use uma fonte específica, neste caso, Times Roman 24
        }
    } else if(resultTextVisible == 2) {
        glRasterPos2f(-0.15f, 0.6f);
        glColor3f(1.0f, 1.0f, 1.0f); // Define a cor do texto (branco)

        std::string resultText = "Player 2 Won!";
        // Percorre a string e desenha cada caractere
        for (char c : resultText) {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c); // Use uma fonte específica, neste caso, Times Roman 24
        }
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
        glRasterPos2f(-0.11f, 0.28f);
        
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
        //alSourcePlay(gameStartSound);
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

    // draw the score on the left for player 1
    snprintf (score_1, sizeof(score_1), "%d", player1Score);
    drawStrokeText(score_1, -300, 200, 0);

    // draw the score on the left for player 1
    snprintf (score_2, sizeof(score_2), "%d", player2Score);
    drawStrokeText(score_2, 200, 200, 0);
    

    glutSwapBuffers();
}

// Função de atualização
void update(int value) {

    // if (!gameOver) {
    if (!gamePaused && !gameOver && !varStart) {
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
            alSourcePlay(bounceSound); 
        }

        if ((ballX + ballRadius) > (0.93f) && (ballY >= paddle2Y - paddleHeight / 2) && (ballY <= paddle2Y + paddleHeight / 2)) {
            ballSpeedX = -ballSpeedX;
            ballSpeedX -= ballSpeedIncrease; // Aumenta a velocidade da bola
            alSourcePlay(bounceSound); 
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
            if (player2Score >= 11) {
                alSourceStop(gamingSound);
                alSourcePlay(winnerSound); 
                std::future<void> result = std::async(std::launch::async, []() {
                    std::this_thread::sleep_for(std::chrono::seconds(3));
                    alSourcePlay(gameStartSound); 
                });
                resultTextVisible = 2;
                gameOver = true;
                finishButtonVisible = true; // Exibe o botão "Finish Game"
                startButtonVisible = true;
            } else {
                alSourcePlay(pointSound); 
            }
        } else if (ballX + ballRadius > 1.0f) {
            player1Score++;
            player1Scored = true;
            chooseBallPosition = true; // O jogador 1 escolherá a posição de lançamento
            ballSpeedX = -initialBallSpeedX; // Restaurar a velocidade inicial
            ballSpeedY = 0.003f;
            if (player1Score >= 11) {
                alSourceStop(gamingSound);
                alSourcePlay(winnerSound); 
                std::future<void> result = std::async(std::launch::async, []() {
                    std::this_thread::sleep_for(std::chrono::seconds(3));
                    alSourcePlay(gameStartSound); 
                });
                gameOver = true;
                resultTextVisible = 1;
                finishButtonVisible = true; // Exibe o botão "Finish Game"
                startButtonVisible = true;
            } else {
                alSourcePlay(pointSound); 
            }
        }

        // Movimento das raquetes
        if (keyState['w'] && (paddle1Y + paddleHeight / 2) < 1.0f) {
            paddle1Y += 0.01f;
        }
        if (keyState['s'] && (paddle1Y - paddleHeight / 2) > -1.0f) {
            paddle1Y -= 0.01f;
        }
        if (keyState[101] && (paddle2Y + paddleHeight / 2) < 1.0f) {
            paddle2Y += 0.01f;
        }
        if (keyState[103] && (paddle2Y - paddleHeight / 2) > -1.0f) {
            paddle2Y -= 0.01f;
        }
    }

    glutPostRedisplay();
    glutTimerFunc(16, update, 0);
}

// Função para lidar com cliques do mouse
void mouseClick(int button, int state, int x, int y) {
    
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        alSourceStop(gameStartSound);
        alSourcePlay(gamingSound);
        // Verifica se o clique do mouse ocorreu dentro do botão "Start Game"
        if (startButtonVisible && ((x >= 320 && x <= 481 && y >= 180 && y <= 241) || (x >= 745 && x <= 1115 && y >= 305 && y <= 408)))  {
            startButtonVisible = false;
            if (gamePaused || gameOver || startButtonClicked) {
                resetGame();
                gamePaused = false;
                varStart = false;
                startButtonClicked = true; // Define o botão "Start Game" como clicado
            }
        }

        // Verifica se o clique do mouse ocorreu dentro do botão "Finish Game"
        if (finishButtonVisible && ((x >= 320 && x <= 480 && y >= 360 && y <= 420) || (x >= 745 && x <= 1115 && y >= 612 && y <= 712))) {
            exit(0); // Fecha o jogo
        }
    }
}

void handle_SpecialFunc(GLint key, GLint x, GLint y){
    keyState[key] = true;
}

// Função de teclado para escolher a posição de lançamento da bola
void keyPressed(unsigned char key, int x, int y) {
    // std::cout << key << std::endl;
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

// reshape the display
void reshape(int w, int h) {
    glViewport (0, 0, (GLsizei) w, (GLsizei) h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-orthoSizeX, orthoSizeX, -orthoSizeY, orthoSizeY, -0, 100);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}


int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Ping Pong Game");
    initAudio();
    
    alSourcePlay(gameStartSound);
     
    glutReshapeFunc(reshape);
    glutDisplayFunc(draw);
    glutTimerFunc(16, update, 0);
    glutKeyboardFunc(keyPressed);
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
