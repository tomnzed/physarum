# Physarum simulation using shaders

![image](physarum.jpg?raw=true)

A GPU simulation of Physarum polycephalum using shaders. Simulates 256k agents at 60fps on a 2018 MacbookPro. [Video](https://youtu.be/tuJdSjn4xFU)

## Setup
- Install [openFrameworks](https://openframeworks.cc/)
- Install [ofxBufferObject](https://github.com/tobiasebsen/ofxBufferObject) addon to openFrameworks/addons/ofxBufferObject
- Clone this repository to be located at openFrameworks/apps/myApps/app_with_shader
- Open the project file `app_with_shader.xcodeproj` in Xcode, I've used Xcode 13.3.1

## References
- Sage Jenson who has done highly detailed simulations and has a handy diagram to refer to https://cargocollective.com/sagejenson/physarum


## Todo
- [Performance] Make use of a PBO to store agents instead of requiring transfer between the CPU and the GPU