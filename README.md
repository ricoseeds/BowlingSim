### Bowling Simulation
Assignment 2 on on computer graphics where we have to render scene and implement basic collision detection and possibly physics.


###### Run in Mac
```
$ g++ src/main.cpp src/ShaderProgram.cpp src/Texture2D.cpp src/Camera.cpp src/Mesh.cpp  -framework OpenGl -I/usr/local/include -lglfw -lGLEW -I/usr/local/include/SDL2 -L/usr/local/lib -lSDL2

$ ./a.out
```

###### Run in Linux
```
g++ -std=c++11 src/main.cpp src/ShaderProgram.cpp src/Texture2D.cpp src/Camera.cpp src/Mesh.cpp -lglfw -lGL -lm -lXrandr -lXi -lX11 -lXxf86vm -lpthread -lGLEW
```
###### Sample output

![Screenshot 2019-03-18 at 5 10 48 AM](https://user-images.githubusercontent.com/17334660/54499890-ed52c200-493c-11e9-8ebf-cc09bcfd1fe8.png)


###### LINKS
https://github.com/FakeTruth/SkeletalAnimation (immediate mode)

https://github.com/RagnarrIvarssen/Assimp-Tutorial-LWJGL-3

