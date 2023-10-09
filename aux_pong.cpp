#include <cmath>
#include <vector> 

// Função para retornar um número aleatório de acordo com o vetor que foi recebido como parâmetro
float randomDirection(std::vector<float> valueList) {
    if (valueList.empty()) {
        return 0.0f; // Retorna 0 se a lista estiver vazia
    }
    
    int index = rand() % valueList.size(); // Gere um índice aleatório
    return valueList[index]; // Retorne o valor correspondente ao índice gerado
}