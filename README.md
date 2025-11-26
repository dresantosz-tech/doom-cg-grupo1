# ODEIOS
- ODEIO DÁRIO
- ODEIO HELOYSA
- ODEIO AUZIER
- ODEIO KELVIN

# DoomLike OpenGL Project

Este projeto utiliza **OpenGL (pipeline fixo + GLSL 1.20)** para renderização,  
**GLUT** para gerenciamento de janela/entrada e **GLEW** para carregar funções modernas  
como shaders, VBOs e extensões necessárias.

## 🎥 Demonstração
https://github.com/user-attachments/assets/e42eb186-d471-4aed-8c69-9c8bc67b3ab3

---

## 📦 Dependências

Certifique-se de ter instalados os seguintes pacotes no seu sistema Linux:

### 🛠️ Compilação
- `g++`
- `make`

### 🖥️ Bibliotecas OpenGL
- `freeglut`
- `glew` (NOVA BIBLIOTECA QUE PRECISA INSTALAR)
- `mesa`
- `glu`

### 🖼️ Carregamento de Texturas  
- `stb_image.h` (arquivo de cabeçalho incluso no projeto)

---

## 🚀 Compilar e Executar

Use o comando abaixo para compilar o projeto e executá-lo imediatamente:

### 🐧 Linux
```bash
g++ main.cpp draw.cpp input.cpp scene.cpp texture.cpp shader.cpp \
    -o DoomLike \
    -lGLEW -lGL -lGLU -lglut && ./DoomLike
```

### 🪟 Windows
```bash
g++ main.cpp draw.cpp input.cpp scene.cpp texture.cpp shader.cpp ^
    -o DoomLike.exe ^
    -lglew32 -lfreeglut -lopengl32 -lglu32 && DoomLike.exe
```
## 🎮 Como Jogar

A cena pode ser explorada em primeira pessoa, com movimentação típica de FPS clássico.

---

## ⌨️ Controles

### 🧭 Movimento
| Tecla | Ação |
|-------|------|
| **W** | Avançar |
| **A** | Mover para a esquerda (strafe) |
| **S** | Recuar |
| **D** | Mover para a direita (strafe) |

---

### 🖱️ Visão
| Ação | Resultado |
|------|-----------|
| **Mover o mouse** | Olhar em qualquer direção |

---

### 🪟 Janelas e Sistema
| Tecla | Ação |
|-------|------|
| **Alt + Enter** | Alterna entre tela cheia e modo janela |
| **ESC** | Encerra o programa |
