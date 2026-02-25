# DoomLike Makefile (Windows + Linux)
# - Build em build/DoomLike(.exe)
# - Stage de assets/maps/shaders em build/

CXX      := g++
CXXFLAGS := -g -O0 -Wall -Wextra -Iinclude
LDFLAGS  :=
TARGET   := DoomLike
BUILD_DIR := build
SRC_DIR  := src
MAIN     := main.cpp

ifeq ($(OS),Windows_NT)
EXE_EXT := .exe
CXX := C:/msys64/mingw64/bin/g++.exe
LDLIBS  := -lglew32 -lfreeglut -lopengl32 -lglu32 -lopenal
SRCS    := $(shell powershell -NoProfile -Command "Get-ChildItem -Path $(SRC_DIR) -Recurse -Filter *.cpp | % { $_.FullName.Replace('\\','/') }")
MKDIR_P := powershell -NoProfile -Command "New-Item -ItemType Directory -Force -Path \"$(1)\" | Out-Null"
RM_RF   := powershell -NoProfile -Command "if (Test-Path \"$(1)\") { Remove-Item -Recurse -Force \"$(1)\" }"
CP_R    := powershell -NoProfile -Command "Copy-Item -Recurse -Force \"$(1)\" \"$(2)\""
CP_F    := powershell -NoProfile -Command "if (Test-Path \"$(1)\") { Copy-Item -Force \"$(1)\" \"$(2)\" }"
RUN_CMD := .\\$(TARGET)$(EXE_EXT)
else
EXE_EXT :=
LDLIBS  := -lGLEW -lGL -lGLU -lglut -lopenal
SRCS    := $(shell find $(SRC_DIR) -name '*.cpp' | sort)
MKDIR_P := mkdir -p $(1)
RM_RF   := rm -rf $(1)
CP_R    := cp -r $(1) $(2)
CP_F    := cp -f $(1) $(2)
RUN_CMD := ./$(TARGET)$(EXE_EXT)
endif

SRCS += $(MAIN)
OBJS := $(patsubst %.cpp,$(BUILD_DIR)/%.o,$(SRCS))

.PHONY: all clean run dirs stage

all: $(BUILD_DIR)/$(TARGET)$(EXE_EXT) stage

$(BUILD_DIR)/$(TARGET)$(EXE_EXT): dirs $(OBJS)
	$(CXX) $(LDFLAGS) $(OBJS) -o $@ $(LDLIBS)

$(BUILD_DIR)/%.o: %.cpp
	$(call MKDIR_P,$(dir $@))
	$(CXX) $(CXXFLAGS) -c $< -o $@

dirs:
	$(call MKDIR_P,$(BUILD_DIR))

stage: dirs
	$(call RM_RF,$(BUILD_DIR)/assets)
	$(call RM_RF,$(BUILD_DIR)/maps)
	$(call RM_RF,$(BUILD_DIR)/shaders)
	$(call CP_R,assets,$(BUILD_DIR)/)
	$(call CP_R,maps,$(BUILD_DIR)/)
	$(call CP_R,shaders,$(BUILD_DIR)/)
ifeq ($(OS),Windows_NT)
	$(call CP_F,C:/msys64/mingw64/bin/libopenal-1.dll,$(BUILD_DIR)/)
	$(call CP_F,C:/msys64/mingw64/bin/OpenAL32.dll,$(BUILD_DIR)/)
	$(call CP_F,C:/msys64/mingw64/bin/libstdc++-6.dll,$(BUILD_DIR)/)
	$(call CP_F,C:/msys64/mingw64/bin/libgcc_s_seh-1.dll,$(BUILD_DIR)/)
	$(call CP_F,C:/msys64/mingw64/bin/libwinpthread-1.dll,$(BUILD_DIR)/)
endif

run: all
	cd $(BUILD_DIR) && $(RUN_CMD)

clean:
	$(call RM_RF,$(BUILD_DIR))

