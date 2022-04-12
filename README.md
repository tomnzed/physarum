# Physarum simulation using shaders

![image](physarum.png?raw=true)

A GPU simulation of Physarum polycephalum using shaders. Simulates 500k agents at 60fps on a 2018 MacbookPro. [Video](https://www.youtube.com/watch?v=UxGwVbBtk90)

## Setup
- Install [openFrameworks](https://openframeworks.cc/)
- Clone this repository to be located in openFrameworks/apps/myApps/


## References
- Sage Jenson who has done highly detailed simulations and has a handy diagram to refer to https://cargocollective.com/sagejenson/physarum


## Todo
- [Performance] Make use of a PBO to store agents instead of requiring transfer between the CPU and the GPU
- Fix whatever seems to be favouring vertical and horizontal movement, probably because it is aligned to pixel rows and columns.