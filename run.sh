#!/bin/sh
# Please s/0x9130/XXXX if binary shader format is different from former one
# Outputs to simple-egl-shader.fx
./simple-egl --compile-binary-shader

# Uses simple-egl-shader.fx
./simple-egl --use-binary-shader
