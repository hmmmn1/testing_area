#!/usr/bin/env bash

g++ `pkg-config --cflags glfw3` main.cpp -std=c++11 -o cmain.out `pkg-config --static --libs glfw3` -lGLEW
