#ifndef STAMINA_H
#define STAMINA_H

class Stamina {
public:
    float atual;
    float maximo;

    float taxaConsumo;
    float taxaRecuperacao;

    Stamina();

    void consumir(float dt);
    void recuperar(float dt);
    void restaurarTotal();

    bool temEnergia() const;
};

#endif