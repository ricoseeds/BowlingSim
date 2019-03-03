#My first makefile
all: compile

compile: main.o ShaderProgram.o Texture2D.o Camera.o Mesh.o
	g++ main.o ShaderProgram.o Texture2D.o Camera.o Mesh.o -o main 

main.o: src/main.cpp
	g++ -c src/main.cpp -framework OpenGl -I/usr/local/include -lglfw -lGLEW 

ShaderProgram.o: src/ShaderProgram.o 
	g++ -c src/ShaderProgram.cpp -framework OpenGl -I/usr/local/include -lglfw -lGLEW 

Texture2D.o: src/Texture2D.o 
	g++ -c src/Texture2D.cpp -framework OpenGl -I/usr/local/include -lglfw -lGLEW 

Camera.o: src/Camera.o 
	g++ -c src/Camera.cpp -framework OpenGl -I/usr/local/include -lglfw -lGLEW 

Mesh.o: src/Mesh.o 
	g++ -c src/Mesh.cpp -framework OpenGl -I/usr/local/include -lglfw -lGLEW 

