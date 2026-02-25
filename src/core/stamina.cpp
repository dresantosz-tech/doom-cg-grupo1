#include "core/stamina.h"

Stamina::Stamina() {
    maximo = 100.0f;
    atual = maximo;

    taxaConsumo = 40.0f;      // energia por segundo correndo
    taxaRecuperacao = 10.0f;  // energia por segundo parado
}

void Stamina::consumir(float dt) {
    atual -= taxaConsumo * dt;
    if (atual < 0) atual = 0;
}

void Stamina::recuperar(float dt) {
    atual += taxaRecuperacao * dt;
    if (atual > maximo) atual = maximo;
}

void Stamina::restaurarTotal() {
    atual = maximo;
}

bool Stamina::temEnergia() const {
    return atual > 0.0f;
}