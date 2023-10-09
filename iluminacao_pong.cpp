#include <iostream>
#include <cmath>

// Função para calcular a intensidade da luz difusa
float calculaLuzDifusa(float luzX, float luzY, float normalX, float normalY) {
    // Vetor da luz
    float luzVectorX = luzX;
    float luzVectorY = luzY;

    // Normalizando o vetor da luz
    float length = sqrt(luzVectorX * luzVectorX + luzVectorY * luzVectorY);
    luzVectorX /= length;
    luzVectorY /= length;

    // Normalizando o vetor normal
    length = sqrt(normalX * normalX + normalY * normalY);
    normalX /= length;
    normalY /= length;

    // Produto escalar entre o vetor da luz e o vetor normal
    float produtoEscalar = luzVectorX * normalX + luzVectorY * normalY;

    // Mantém o resultado positivo
    produtoEscalar = std::max(0.0f, produtoEscalar);

    return produtoEscalar;
}

// Função para calcular a intensidade da luz especular
float calculaLuzEspecular(float luzX, float luzY, float observadorX, float observadorY, float normalX, float normalY, float brilho) {
    // Vetor da luz
    float luzVectorX = luzX;
    float luzVectorY = luzY;

    // Normalizando o vetor da luz
    float length = sqrt(luzVectorX * luzVectorX + luzVectorY * luzVectorY);
    luzVectorX /= length;
    luzVectorY /= length;

    // Vetor de reflexão da luz
    float reflectionVectorX = 2.0f * (normalX * luzVectorX + normalY * luzVectorY) * normalX - luzVectorX;
    float reflectionVectorY = 2.0f * (normalX * luzVectorX + normalY * luzVectorY) * normalY - luzVectorY;

    // Vetor do observador
    float observadorVectorX = observadorX;
    float observadorVectorY = observadorY;

    // Normalizando o vetor do observador
    length = sqrt(observadorVectorX * observadorVectorX + observadorVectorY * observadorVectorY);
    observadorVectorX /= length;
    observadorVectorY /= length;

    // Produto escalar entre o vetor de reflexão da luz e o vetor do observador
    float produtoEscalar = reflectionVectorX * observadorVectorX + reflectionVectorY * observadorVectorY;

    // Elevar o resultado à potência de brilho (fator de espalhamento da luz)
    float especula = pow(std::max(0.0f, produtoEscalar), brilho);

    return especula;
}

// Função para calcular a cor da bola com base na iluminação
void calculaCorBola(float bolaX, float bolaY, float& r, float& g, float& b, float ambRandom, float luzXRandom, float luzYRandom) {
    // Posição da luz 
    float luzX = luzXRandom;
    float luzY = luzYRandom;

    // Vetor do observador 
    float observadorX = 0.0f;
    float observadorY = 0.0f;

    // Vetores normais para as superfícies da bola 
    float normalX = bolaX;
    float normalY = bolaY;

    // Coeficiente de especularidade 
    float brilho = 32.0f;

    // Calcula a intensidade difusa e especular
    float intensidadeDifusa = calculaLuzDifusa(luzX, luzY, normalX, normalY);
    float intensidadeEspecular = calculaLuzEspecular(luzX, luzY, observadorX, observadorY, normalX, normalY, brilho);

    // Cores da luz 
    float luzR = 1.0f;
    float luzG = 1.0f;
    float luzB = 1.0f;

    // Cores do material da bola 
    float corBolaR = 1.2f;
    float corBolaG = 1.0f;
    float corBolaB = 1.5f;

    // Luz ambiente (Recebe um número aleatório)
    float ambluz = ambRandom;

    // Calcula a cor final da bola com base na iluminação
    r = ((intensidadeDifusa * luzR + intensidadeEspecular) * corBolaR) + ambluz;
    g = ((intensidadeDifusa * luzG + intensidadeEspecular) * corBolaG) + ambluz;
    b = ((intensidadeDifusa * luzB + intensidadeEspecular) * corBolaB) + ambluz;
}