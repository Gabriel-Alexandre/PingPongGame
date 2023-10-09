#include <AL/al.h>
#include <AL/alc.h>
#include "stb_vorbis.c" // Arquivo que é necessário incluir na pasta do projeto para poder utilizar funções de áudio (Encontramos esse arquivo no github)

ALuint rebateSound; // ID do som de rebatida
ALuint pointSound;  // ID do som de ponto
ALuint winnerSound;  // ID do som de vencedor
ALuint gameStartSound;  // ID do som de inicio de jogo
ALuint gamingSound;  // ID do som de jogo
ALCdevice* audioDevice;
ALCcontext* audioContext;

ALuint rebateSource; // Variável para a fonte de som de rebatida
ALuint pointSource;  // Variável para a fonte de som de ponto
ALuint winnerSource;  // Variável para a fonte de som de vencedor
ALuint gameStartSource;  // Variável para a fonte de som de inicio de jogo
ALuint gamingSource;  // Variável para a fonte de som de jogo

// Função para inicializar configurações de áudio
void initAudio() {
    // Inicializa o dispositivo de áudio
    audioDevice = alcOpenDevice(nullptr);
    audioContext = alcCreateContext(audioDevice, nullptr);
    alcMakeContextCurrent(audioContext);

    // Crie uma fonte de som para a rebatida
    alGenSources(1, &rebateSource);
    
    // Carrega o som de rebatida
    alGenBuffers(1, &rebateSound);
    {
        int canais, amostraRate;
        short* soundData;
        int amostras = stb_vorbis_decode_filename("bounce_sound.ogg", &canais, &amostraRate, &soundData);
        if (amostras != -1) {
            alBufferData(rebateSound, AL_FORMAT_MONO16, soundData, amostras * sizeof(short), amostraRate);
            free(soundData);
        }
    }

    // Crie uma fonte de som para o vencedor
    alGenSources(1, &winnerSource);
    
    // Carrega o som de vencedor
    alGenBuffers(1, &winnerSound);
    {
        int canais, amostraRate;
        short* soundData;
        int amostras = stb_vorbis_decode_filename("winner.ogg", &canais, &amostraRate, &soundData);
        if (amostras != -1) {
            alBufferData(winnerSound, AL_FORMAT_MONO16, soundData, amostras * sizeof(short), amostraRate);
            free(soundData);
        }
    }

    // Crie uma fonte de som para o ponto
    alGenSources(1, &pointSource);
    
    // Carrega o som de ponto
    alGenBuffers(1, &pointSound);
    {
        int canais, amostraRate;
        short* soundData;
        int amostras = stb_vorbis_decode_filename("point_sound.ogg", &canais, &amostraRate, &soundData);
        if (amostras != -1) {
            alBufferData(pointSound, AL_FORMAT_MONO16, soundData, amostras * sizeof(short), amostraRate);
            free(soundData);
        }
    }

    // Crie uma fonte de som para o início de jogo
    alGenSources(1, &gameStartSource);
    
    // Carrega o som de início de jogo
    alGenBuffers(1, &gameStartSound);
    {
        int canais, amostraRate;
        short* soundData;
        int amostras = stb_vorbis_decode_filename("gameStart.ogg", &canais, &amostraRate, &soundData);
        if (amostras != -1) {
            alBufferData(gameStartSound, AL_FORMAT_MONO16, soundData, amostras * sizeof(short), amostraRate);
            free(soundData);
        }
    }

    // Crie uma fonte de som para o jogo
    alGenSources(1, &gamingSource);
    
    // Carrega o som de jogo
    alGenBuffers(1, &gamingSound);
    {
        int canais, amostraRate;
        short* soundData;
        int amostras = stb_vorbis_decode_filename("music-game.ogg", &canais, &amostraRate, &soundData);
        if (amostras != -1) {
            alBufferData(gamingSound, AL_FORMAT_MONO16, soundData, amostras * sizeof(short), amostraRate);
            free(soundData);
        }
    }

    // Configure a fonte de som para o som de rebatida
    alSourcei(rebateSource, AL_BUFFER, rebateSound);
    alSourcef(rebateSource, AL_GAIN, 1.0f); // Define o volume 
    alSourcef(rebateSource, AL_PITCH, 2.0f); // Definie velocidade

    // Configure a fonte de som para o som de ponto
    alSourcei(pointSource, AL_BUFFER, pointSound);
    alSourcef(pointSource, AL_GAIN, 1.0f); // Define o volume 
    alSourcef(pointSource, AL_PITCH, 2.0f); // Definie velocidade

    // Configure a fonte de som para o som de vencedor
    alSourcei(winnerSource, AL_BUFFER, winnerSound);
    alSourcef(winnerSource, AL_GAIN, 1.0f); // Define o volume 
    alSourcef(winnerSource, AL_PITCH, 2.0f); // Definie velocidade

    // Configure a fonte de som para o som de inicio de jogo
    alSourcei(gameStartSource, AL_BUFFER, gameStartSound);
    alSourcef(gameStartSource, AL_GAIN, 1.0f); // Define o volume 
    alSourcef(gameStartSource, AL_PITCH, 2.0f); // Definie velocidade
    alSourcei(gameStartSource, AL_LOOPING, AL_TRUE); // Definie o loop (repete a música)

    // Configure a fonte de som para o som de jogo
    alSourcei(gamingSource, AL_BUFFER, gamingSound);
    alSourcef(gamingSource, AL_GAIN, 1.0f); // Define o volume 
    alSourcef(gamingSource, AL_PITCH, 1.0f); // Definie velocidade    
    alSourcei(gamingSource, AL_LOOPING, AL_TRUE); // Definie o loop (repete a música)
   
}
