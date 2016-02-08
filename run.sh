#!/usr/bin/env bash

em++ main.cpp -std=c++11 -o main.html -Wall -D_REENTRANT -Wextra -O3 -s USE_GLFW=3 \
--preload-file fShader \
--preload-file vShader \
--preload-file uvmap.DDS \
--preload-file suzanne.obj
