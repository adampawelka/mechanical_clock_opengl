#version 330

//Zmienne jednorodne
uniform mat4 P;
uniform mat4 V;
uniform mat4 M;

//Atrybuty
in vec4 vertex; //wspolrzedne wierzcholka w przestrzeni modelu
in vec2 texCoord; //kolor zwi¹zany z wierzcho³kiem
in vec4 normalCoord; //wektor normalny w przestrzeni modelu


//Zmienne interpolowane
out vec4 l1;
out vec4 l2;
out vec4 n;
out vec4 v;
out vec2 iTexCoord0;

void main(void) {
    vec4 lp1 = vec4(0, 2,0, 1);//pozcyja œwiat³a 1, przestrzeñ œwiata
    vec4 lp2 = vec4(-3, 0, 0, 1);//pozcyja œwiat³a 2, przestrzeñ œwiata
    
    l1 = normalize(V * lp1 - V*M*vertex); //wektor do œwiat³a w przestrzeni oka 
    l2 = normalize(V * lp2 - V*M*vertex); 
    v = normalize(vec4(0, 0, 0, 1) - V * M * vertex); //wektor do obserwatora w przestrzeni oka
    n = normalize(V * M * normalCoord); //wektor normalny w przestrzeni oka
    iTexCoord0=texCoord; //w main
    gl_Position=P*V*M*vertex;
}
