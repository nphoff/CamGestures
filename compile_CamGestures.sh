#!/bin/bash

g++ -o CamGestures CamGestures.cpp colourslidebar.cpp `pkg-config opencv --libs --cflags`
