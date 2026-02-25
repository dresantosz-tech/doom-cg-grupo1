# DoomLike Makefile
# - Compila .o em build/
# - Linka o executavel em build/DoomLike(.exe no Windows)
# - Copia assets/maps/shaders para dentro de build/ (build portatil)
# - make run ja prepara tudo e executa

CXX       := g++
CXXFLAGS  := -g -O0 -Wall -Wextra -Iinclude
LDFLAGS   :=
TARGET    := DoomLike

ifeq ($(OS),Windows_NT)
EXE_EXT   := .exe
LDLIBS    := -lglew32 -lfreeglut -lopengl32 -lglu32 -lopenal
RM_RF     := rm -rf
CP_R      := cp -r
RUN_CMD   := ./$(TARGET)$(EXE_EXT)
else
EXE_EXT   :=
LDLIBS    := -lGLEW -lGL -lGLU -lglut -lopenal
RM_RF     := rm -rf
CP_R      := cp -r
RUN_CMD   := ./$(TARGET)$(EXE_EXT)
endif

BUILD_DIR := build

SRC_DIR   := src
MAIN      := main.cpp

# Encontra todos os .cpp dentro de src (recursivo)
SRCS      := $(shell find $(SRC_DIR) -name '*.cpp' | sort)
SRCS      += $(MAIN)

# Converte paths em nomes de objetos dentro de build/
OBJS      := $(patsubst %.cpp,$(BUILD_DIR)/%.o,$(SRCS))

.PHONY: all clean run dirs stage

# Build padrao ja gera build pronta para rodar/entregar
all: $(BUILD_DIR)/$(TARGET)$(EXE_EXT) stage

$(BUILD_DIR)/$(TARGET)$(EXE_EXT): dirs $(OBJS)
	$(CXX) $(LDFLAGS) $(OBJS) -o $@ $(LDLIBS)

# Regra generica: compila qualquer .cpp para build/.../.o
$(BUILD_DIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

dirs:
	@mkdir -p $(BUILD_DIR)

# Copia recursos para dentro de build/ (self-contained)
stage: dirs
	@$(RM_RF) $(BUILD_DIR)/assets $(BUILD_DIR)/maps $(BUILD_DIR)/shaders
	@$(CP_R) assets  $(BUILD_DIR)/
	@$(CP_R) maps    $(BUILD_DIR)/
	@$(CP_R) shaders $(BUILD_DIR)/

# Roda SEM depender do cwd externo
run: all
	cd $(BUILD_DIR) && $(RUN_CMD)

clean:
	$(RM_RF) $(BUILD_DIR)

