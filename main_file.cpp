/*
Niniejszy program jest wolnym oprogramowaniem; możesz go
rozprowadzać dalej i / lub modyfikować na warunkach Powszechnej
Licencji Publicznej GNU, wydanej przez Fundację Wolnego
Oprogramowania - według wersji 2 tej Licencji lub(według twojego
wyboru) którejś z późniejszych wersji.

Niniejszy program rozpowszechniany jest z nadzieją, iż będzie on
użyteczny - jednak BEZ JAKIEJKOLWIEK GWARANCJI, nawet domyślnej
gwarancji PRZYDATNOŚCI HANDLOWEJ albo PRZYDATNOŚCI DO OKREŚLONYCH
ZASTOSOWAŃ.W celu uzyskania bliższych informacji sięgnij do
Powszechnej Licencji Publicznej GNU.

Z pewnością wraz z niniejszym programem otrzymałeś też egzemplarz
Powszechnej Licencji Publicznej GNU(GNU General Public License);
jeśli nie - napisz do Free Software Foundation, Inc., 59 Temple
Place, Fifth Floor, Boston, MA  02110 - 1301  USA
*/

#define GLM_FORCE_RADIANS
#define GLM_FORCE_SWIZZLE

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stdlib.h>
#include <stdio.h>
#include "constants.h"
#include "lodepng.h"
#include "shaderprogram.h"
#include "loadFromOBJFile.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string.h>
using namespace std;

float speed_x = 0;
float speed_y = 0;
float aspectRatio = 1;

ShaderProgram* sp;
GLuint tex;
GLuint tex1;
GLuint texClock;
GLuint texClock1;
GLuint texMet;
GLuint texMet1;
GLuint texWsk1;
GLuint texWsk1_ciem;
GLuint texWsk2;
GLuint texWsk2_ciem;

GLuint readTexture(const char* filename) { //Deklaracja globalna
    GLuint tex;
    glActiveTexture(GL_TEXTURE0);
    //Wczytanie do pamięci komputera
    std::vector<unsigned char> image; //Alokuj wektor do wczytania obrazka
    unsigned width, height; //Zmienne do których wczytamy wymiary obrazka
    //Wczytaj obrazek
    unsigned error = lodepng::decode(image, width, height, filename);
    //Import do pamięci karty graficznej
    glGenTextures(1, &tex); //Zainicjuj jeden uchwyt
    glBindTexture(GL_TEXTURE_2D, tex); //Uaktywnij uchwyt
    //Wczytaj obrazek do pamięci KG skojarzonej z uchwytem
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0,
        GL_RGBA, GL_UNSIGNED_BYTE, (unsigned char*)image.data());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    return tex;
}

//Procedura obsługi błędów
void error_callback(int error, const char* description) {
    fputs(description, stderr);
}


void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS) {
        if (key == GLFW_KEY_LEFT) speed_x = -PI / 2;
        if (key == GLFW_KEY_RIGHT) speed_x = PI / 2;
        if (key == GLFW_KEY_UP) speed_y = PI / 2;
        if (key == GLFW_KEY_DOWN) speed_y = -PI / 2;
    }
    if (action == GLFW_RELEASE) {
        if (key == GLFW_KEY_LEFT) speed_x = 0;
        if (key == GLFW_KEY_RIGHT) speed_x = 0;
        if (key == GLFW_KEY_UP) speed_y = 0;
        if (key == GLFW_KEY_DOWN) speed_y = 0;
    }
}

void windowResizeCallback(GLFWwindow* window, int width, int height) {
    if (height == 0) return;
    aspectRatio = (float)width / (float)height;
    glViewport(0, 0, width, height);
}

//Procedura inicjująca
void initOpenGLProgram(GLFWwindow* window) {
    //************Tutaj umieszczaj kod, który należy wykonać raz, na początku programu************
    tex = readTexture("drewno1.png");
    tex1 = readTexture("drewno1_ciem.png");
    texClock = readTexture("clock2.png");
    texClock1 = readTexture("clock2_ciem.png");
    texMet = readTexture("metal.png");
    texMet1 = readTexture("metal_spec.png");
    texWsk1 = readTexture("wskazowka_czerwona2.png");
    texWsk1_ciem = readTexture("wskazowka_czerwona2_ciem.png");
    texWsk2 = readTexture("wskazowka_zolta2.png");
    texWsk2_ciem = readTexture("wskazowka_zolta2_ciem.png");

    glClearColor(0, 0, 0, 1);
    glEnable(GL_DEPTH_TEST);
    glfwSetWindowSizeCallback(window, windowResizeCallback);
    glfwSetKeyCallback(window, keyCallback);

    sp = new ShaderProgram("v_simplest.glsl", NULL, "f_simplest.glsl");
}


//Zwolnienie zasobów zajętych przez program
void freeOpenGLProgram(GLFWwindow* window) {
    //************Tutaj umieszczaj kod, który należy wykonać po zakończeniu pętli głównej************
    glDeleteTextures(1, &tex);
    glDeleteTextures(1, &tex1);
    glDeleteTextures(1, &texClock);
    glDeleteTextures(1, &texClock1);
    glDeleteTextures(1, &texMet);
    glDeleteTextures(1, &texMet1);
    glDeleteTextures(1, &texWsk1);
    glDeleteTextures(1, &texWsk2);
    glDeleteTextures(1, &texWsk1_ciem);
    glDeleteTextures(1, &texWsk2_ciem);
    delete sp;
}




//Procedura rysująca zawartość sceny
void drawScene(GLFWwindow* window, float angle, float angle_x, float angle_y, float anglePendulum, float* verticesGear, int numGear, float* texCoordsGear, float* normalCoordsGear, float* verticesPendulum, int numPendulum, float* texCoordsPendulum, float* normalCoordsPendulum, float* verticesChest, int numChest, float* texCoordsChest, float* normalCoordsChest, float* verticesHand, int numHand, float* texCoordsHand, float* normalCoordsHand) {
    //************Tutaj umieszczaj kod rysujący obraz******************l
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::mat4 V = glm::lookAt(
        glm::vec3(0, 0, -10),
        glm::vec3(0, 0, 0),
        glm::vec3(0.0f, 1.0f, 0.0f)); //Wylicz macierz widoku

    glm::mat4 P = glm::perspective(50.0f * PI / 180.0f, aspectRatio, 0.01f, 50.0f); //Wylicz macierz rzutowania
    glm::mat4 Mmain = glm::mat4(1.0f);
    glm::mat4 M = glm::mat4(1.0f);
    glm::mat4 M1 = glm::mat4(1.0f);
    glm::mat4 M2 = glm::mat4(1.0f);
    glm::mat4 M3 = glm::mat4(1.0f);
    glm::mat4 M4 = glm::mat4(1.0f);
    glm::mat4 M5 = glm::mat4(1.0f);
    glm::mat4 M6 = glm::mat4(1.0f);
    glm::mat4 Mclock = glm::mat4(1.0f);
    //główna macierz na środku
    Mmain = glm::rotate(Mmain, angle_y, glm::vec3(1.0f, 0.0f, 0.0f));
    Mmain = glm::rotate(Mmain, angle_x, glm::vec3(0.0f, 1.0f, 0.0f));
    //koło zębate z lewej
    M = glm::translate(Mmain, glm::vec3(0.5f, -0.5f, 0.0f));
    M = glm::rotate(M, PI / 2, glm::vec3(1.0f, 0.0f, 0.0f));
    M = glm::rotate(M, angle, glm::vec3(0.0f, 1.0f, 0.0f));
    M = glm::scale(M, glm::vec3(0.08f, 0.08f, 0.08f));
    //koło zebate z prawej
    M1 = glm::translate(Mmain, glm::vec3(-0.55f, 0.0f, 0.0f));
    M1 = glm::rotate(M1, PI / 2, glm::vec3(1.0f, 0.0f, 0.0f));
    M1 = glm::rotate(M1, -angle, glm::vec3(0.0f, 1.0f, 0.0f));
    M1 = glm::scale(M1, glm::vec3(0.08f, 0.08f, 0.08f));
    // wahadło
    M2 = glm::translate(Mmain, glm::vec3(0.0f, -0.6f, 0.0f));
    M2 = glm::rotate(M2, PI / 2, glm::vec3(1.0f, 0.0f, 0.0f));
    M2 = glm::rotate(M2, anglePendulum, glm::vec3(0.0f, 1.0f, 0.0f));
    M2 = glm::scale(M2, glm::vec3(0.05f, 0.05f, 0.05f));
    // skrzynia
    M3 = glm::translate(Mmain, glm::vec3(0.0f, 0.0f, 0.1f));
    M3 = glm::rotate(M3, PI / 2, glm::vec3(0.0f, 0.0f, 1.0f));
    M3 = glm::rotate(M3, PI / 2, glm::vec3(1.0f, 0.0f, 0.0f));
    M3 = glm::translate(M3, glm::vec3(0.0f, 0.0f, -2.0f));
    M3 = glm::translate(M3, glm::vec3(0.0f, 0.35f, 0.0f));
    M3 = glm::scale(M3, glm::vec3(4.0f, 4.0f, 4.0f));
    //macierz środka zegara
    Mclock = glm::translate(Mmain, glm::vec3(0.0f, 2.0f, 0.0f));
    // wskazówka 1
    M4 = glm::rotate(Mclock, angle, glm::vec3(0.0f, 0.0f, 1.0f));
    M4 = glm::translate(M4, glm::vec3(0.0f, 0.3f, -0.12f));
    M4 = glm::scale(M4, glm::vec3(0.2f, 0.2f, 0.2f));
    // wskazówka 2
    M5 = glm::rotate(Mclock, angle / 2, glm::vec3(0.0f, 0.0f, 1.0f));
    M5 = glm::translate(M5, glm::vec3(0.0f, 0.2f, -0.05f));
    M5 = glm::scale(M5, glm::vec3(0.2f, 0.15f, 0.2f));
    // zegar
    M6 = glm::translate(Mclock, glm::vec3(0.0f, 0.0f, 0.1f));
    M6 = glm::rotate(M6, PI / 2, glm::vec3(0.0f, 0.0f, 1.0f));
    M6 = glm::rotate(M6, PI / 2, glm::vec3(1.0f, 0.0f, 0.0f));
    M6 = glm::scale(M6, glm::vec3(1.0f, 1.2f, 2.0f));
    M6 = glm::translate(M6, glm::vec3(0.0f, 0.05f, -0.5f));

    sp->use();//Aktywacja programu cieniującego

    glUniformMatrix4fv(sp->u("P"), 1, false, glm::value_ptr(P));
    glUniformMatrix4fv(sp->u("V"), 1, false, glm::value_ptr(V));
    glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(Mmain));

    //rysowanie kola zebatego lewego
    glUniform3f(sp->u("objectColor"), 0.8f, 0.8f, 0.8f);
    glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(M));
    glUniform1i(sp->u("tex"), 0);
    glActiveTexture(GL_TEXTURE0); glBindTexture(GL_TEXTURE_2D, texMet);
    glUniform1i(sp->u("tex1"), 1);
    glActiveTexture(GL_TEXTURE1); glBindTexture(GL_TEXTURE_2D, texMet1);
    glUniform1i(sp->u("useTexture"), true);
    glEnableVertexAttribArray(sp->a("vertex"));
    glVertexAttribPointer(sp->a("vertex"), 4, GL_FLOAT, false, 0, verticesGear);
    glEnableVertexAttribArray(sp->a("texCoord"));
    glVertexAttribPointer(sp->a("texCoord"), 2, GL_FLOAT, false, 0, texCoordsGear);
    glEnableVertexAttribArray(sp->a("normalCoord"));
    glVertexAttribPointer(sp->a("normalCoord"), 4, GL_FLOAT, false, 0, normalCoordsGear);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glDrawArrays(GL_TRIANGLE_FAN, 0, numGear); //Narysuj obiekt
    glDisableVertexAttribArray(sp->a("vertex"));
    glDisableVertexAttribArray(sp->a("texCoord"));
    glDisableVertexAttribArray(sp->a("normalCoord"));

    //rysowanie kola zebatego prawego
    glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(M1));
    glUniform1i(sp->u("tex"), 0);
    glActiveTexture(GL_TEXTURE0); glBindTexture(GL_TEXTURE_2D, texMet);
    glUniform1i(sp->u("tex1"), 1);
    glActiveTexture(GL_TEXTURE1); glBindTexture(GL_TEXTURE_2D, texMet1);
    glUniform1i(sp->u("useTexture"), true);
    glEnableVertexAttribArray(sp->a("vertex"));
    glVertexAttribPointer(sp->a("vertex"), 4, GL_FLOAT, false, 0, verticesGear);
    glEnableVertexAttribArray(sp->a("texCoord"));
    glVertexAttribPointer(sp->a("texCoord"), 2, GL_FLOAT, false, 0, texCoordsGear);
    glEnableVertexAttribArray(sp->a("normalCoord"));
    glVertexAttribPointer(sp->a("normalCoord"), 4, GL_FLOAT, false, 0, normalCoordsGear);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glDrawArrays(GL_TRIANGLE_FAN, 0, numGear); //Narysuj obiekt
    glDisableVertexAttribArray(sp->a("vertex"));
    glDisableVertexAttribArray(sp->a("texCoord"));
    glDisableVertexAttribArray(sp->a("normalCoord"));

  //rysowanie wahadla
    glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(M2));
    glUniform1i(sp->u("tex"), 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texMet);
    glUniform1i(sp->u("tex1"), 1);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texMet1);
    glUniform1i(sp->u("useTexture"), true);
    glEnableVertexAttribArray(sp->a("vertex"));
    glVertexAttribPointer(sp->a("vertex"), 4, GL_FLOAT, false, 0, verticesPendulum);
    glEnableVertexAttribArray(sp->a("texCoord"));
    glVertexAttribPointer(sp->a("texCoord"), 2, GL_FLOAT, false, 0, texCoordsPendulum);
    glEnableVertexAttribArray(sp->a("normalCoord"));
    glVertexAttribPointer(sp->a("normalCoord"), 4, GL_FLOAT, false, 0, normalCoordsPendulum);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glDrawArrays(GL_TRIANGLE_FAN, 0, numPendulum); //Narysuj obiekt
    glDisableVertexAttribArray(sp->a("vertex"));
    glDisableVertexAttribArray(sp->a("texCoord"));
    glDisableVertexAttribArray(sp->a("normalCoord"));

    //rysowanie szafy
    glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(M3));
    glUniform1i(sp->u("tex"), 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex);
    glUniform1i(sp->u("tex1"), 1);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, tex1);
    glUniform1i(sp->u("useTexture"), true);
    glUniform3f(sp->u("objectColor"), 0.4f, 0.2f, 0.0f);
    glEnableVertexAttribArray(sp->a("vertex"));
    glVertexAttribPointer(sp->a("vertex"), 4, GL_FLOAT, false, 0, verticesChest);
    glEnableVertexAttribArray(sp->a("texCoord"));
    glVertexAttribPointer(sp->a("texCoord"), 2, GL_FLOAT, false, 0, texCoordsChest);
    glEnableVertexAttribArray(sp->a("normalCoord"));
    glVertexAttribPointer(sp->a("normalCoord"), 4, GL_FLOAT, false, 0, normalCoordsChest);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, numChest); //Narysuj obiekt
    glDisableVertexAttribArray(sp->a("vertex"));
    glDisableVertexAttribArray(sp->a("texCoord"));
    glDisableVertexAttribArray(sp->a("normalCoord"));

    //rysowanie wskazowki czerwonej
    glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(M4));
    glUniform1i(sp->u("tex"), 0);
    glActiveTexture(GL_TEXTURE0); glBindTexture(GL_TEXTURE_2D, texWsk1);
    glUniform1i(sp->u("tex1"), 1);
    glActiveTexture(GL_TEXTURE1); glBindTexture(GL_TEXTURE_2D, texWsk1_ciem);
    glUniform1i(sp->u("useTexture"), true);
    glUniform3f(sp->u("objectColor"), 0.6f, 0.0f, 0.0f);
    glEnableVertexAttribArray(sp->a("vertex"));
    glVertexAttribPointer(sp->a("vertex"), 4, GL_FLOAT, false, 0, verticesHand);
    glEnableVertexAttribArray(sp->a("texCoord"));
    glVertexAttribPointer(sp->a("texCoord"), 2, GL_FLOAT, false, 0, texCoordsHand);
    glEnableVertexAttribArray(sp->a("normalCoord"));
    glVertexAttribPointer(sp->a("normalCoord"), 4, GL_FLOAT, false, 0, normalCoordsHand);
    glActiveTexture(GL_TEXTURE0); glBindTexture(GL_TEXTURE_2D, texWsk1);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glDrawArrays(GL_LINE_STRIP, 0, numHand); //Narysuj obiekt
    glDisableVertexAttribArray(sp->a("vertex"));
    glDisableVertexAttribArray(sp->a("texCoord"));
    glDisableVertexAttribArray(sp->a("normalCoord"));

    //rysowanie wskazowki zoltej
    glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(M5));
    glUniform1i(sp->u("tex"), 0);
    glActiveTexture(GL_TEXTURE0); glBindTexture(GL_TEXTURE_2D, texWsk2);
    glUniform1i(sp->u("tex1"), 1);
    glActiveTexture(GL_TEXTURE1); glBindTexture(GL_TEXTURE_2D, texWsk2_ciem);
    glUniform1i(sp->u("useTexture"), true);
    glUniform3f(sp->u("objectColor"), 0.6f, 0.6f, 0.0f);
    glEnableVertexAttribArray(sp->a("vertex"));
    glVertexAttribPointer(sp->a("vertex"), 4, GL_FLOAT, false, 0, verticesHand);
    glEnableVertexAttribArray(sp->a("texCoord"));
    glVertexAttribPointer(sp->a("texCoord"), 2, GL_FLOAT, false, 0, texCoordsHand);
    glEnableVertexAttribArray(sp->a("normalCoord"));
    glVertexAttribPointer(sp->a("normalCoord"), 4, GL_FLOAT, false, 0, normalCoordsHand);
    glUniform1i(sp->u("tex"), 0);
    glUniform1i(sp->u("useTexture"), false);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glDrawArrays(GL_LINE_STRIP, 0, numHand); //Narysuj obiekt
    glDisableVertexAttribArray(sp->a("vertex"));
    glDisableVertexAttribArray(sp->a("texCoord"));
    glDisableVertexAttribArray(sp->a("normalCoord"));

    //rysowanie tarczy zegara
    glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(M6));
    glUniform1i(sp->u("tex"), 0);
    glActiveTexture(GL_TEXTURE0); glBindTexture(GL_TEXTURE_2D, texClock);
    glUniform1i(sp->u("tex1"), 1);
    glActiveTexture(GL_TEXTURE1); glBindTexture(GL_TEXTURE_2D, texClock1);
    glUniform1i(sp->u("useTexture"), true);
    glUniform3f(sp->u("objectColor"), 1.0f, 1.0f, 1.0f);
    glEnableVertexAttribArray(sp->a("vertex"));
    glVertexAttribPointer(sp->a("vertex"), 4, GL_FLOAT, false, 0, verticesChest);
    glEnableVertexAttribArray(sp->a("texCoord"));
    glVertexAttribPointer(sp->a("texCoord"), 2, GL_FLOAT, false, 0, texCoordsChest);
    glEnableVertexAttribArray(sp->a("normalCoord"));
    glVertexAttribPointer(sp->a("normalCoord"), 4, GL_FLOAT, false, 0, normalCoordsChest);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, numChest); //Narysuj obiekt
    glDisableVertexAttribArray(sp->a("vertex"));
    glDisableVertexAttribArray(sp->a("texCoord"));
    glDisableVertexAttribArray(sp->a("normalCoord"));

    glfwSwapBuffers(window); //Przerzuć tylny bufor na przedni
}

int main(void)
{
    // wczytywanie z plików, żeby potem przekazać jako argumenty do drawScene()
    pair<float*, int> gear = loadOBJ("./gear.obj");
    float* verticesGear = gear.first;
    int numGear = gear.second;

    pair<float*, int> pendulum = loadOBJ("./pendulum.obj");
    float* verticesPendulum = pendulum.first;
    int numPendulum = pendulum.second;

    pair<float*, int> chest = loadOBJ("./szafa.obj");
    float* verticesChest = chest.first;
    int numChest = chest.second;

    pair<float*, int> clockHand = loadOBJ("./clock_hand.obj");
    float* verticesHand = clockHand.first;
    int numHand = clockHand.second;

    float* texCoordsChest = loadTexCoordsOBJ("./szafa.obj");
    float* texCoordsHand = loadTexCoordsOBJ("./clock_hand.obj");
    float* texCoordsGear = loadTexCoordsOBJ("./gear.obj");
    float* texCoordsPendulum = generateTexCoords(verticesPendulum, numPendulum);


    float* normalCoordsChest = loadNormalCoordsOBJ("./szafa.obj");
    float* normalCoordsHand = loadNormalCoordsOBJ("./clock_hand.obj");
    float* normalCoordsGear = loadNormalCoordsOBJ("./gear.obj");
    float* normalCoordsPendulum = loadNormalCoordsOBJ("./pendulum.obj"); 

    GLFWwindow* window; //Wskaźnik na obiekt reprezentujący okno

    glfwSetErrorCallback(error_callback);//Zarejestruj procedurę obsługi błędów

    if (!glfwInit()) { //Zainicjuj bibliotekę GLFW
        fprintf(stderr, "Nie można zainicjować GLFW.\n");
        exit(EXIT_FAILURE);
    }

    window = glfwCreateWindow(800, 800, "OpenGL", NULL, NULL);  //Utwórz okno 500x500 o tytule "OpenGL" i kontekst OpenGL.

    if (!window) //Jeżeli okna nie udało się utworzyć, to zamknij program
    {
        fprintf(stderr, "Nie można utworzyć okna.\n");
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(window); //Od tego momentu kontekst okna staje się aktywny i polecenia OpenGL będą dotyczyć właśnie jego.
    glfwSwapInterval(1); //Czekaj na 1 powrót plamki przed pokazaniem ukrytego bufora

    if (glewInit() != GLEW_OK) { //Zainicjuj bibliotekę GLEW
        fprintf(stderr, "Nie można zainicjować GLEW.\n");
        exit(EXIT_FAILURE);
    }

    initOpenGLProgram(window); //Operacje inicjujące

    //Główna pętla

    //kąty obrotu dla całości
    float angle_x = 0;
    float angle_y = 0;
    //inne kąty obrotu (animacja zegara)
    float angle = 0;
    float anglePendulum = 0;
    bool direction = true;
    bool start = true;
    int i = 0;
    glfwSetTime(0); //Zeruj timer
    while (!glfwWindowShouldClose(window)) //Tak długo jak okno nie powinno zostać zamknięte
    {
        angle_x += speed_x * glfwGetTime(); //Zwiększ/zmniejsz kąt obrotu na podstawie prędkości i czasu jaki upłynał od poprzedniej klatki
        angle_y += speed_y * glfwGetTime();
        angle += PI / 10 * glfwGetTime();
        if (direction) {
            anglePendulum += PI / 24 * glfwGetTime();
        }
        else {
            anglePendulum -= PI / 24 * glfwGetTime();
        }
        glfwSetTime(0); //Zeruj timer
        drawScene(window, angle, angle_x, angle_y, anglePendulum, verticesGear, numGear, texCoordsGear, normalCoordsGear, verticesPendulum, numPendulum, texCoordsPendulum, normalCoordsPendulum, verticesChest, numChest, texCoordsChest, normalCoordsChest, verticesHand, numHand, texCoordsHand, normalCoordsHand); //Wykonaj procedurę rysującą
        glfwPollEvents(); //Wykonaj procedury callback w zalezności od zdarzeń jakie zaszły.
        i++;
        if (i == 100) {
            if (start) {
                direction = !direction;
                start = false;
                i = 0;
            }
        }
        if (i == 200) {
            direction = !direction;
            i = 0;
        }
    }

    freeOpenGLProgram(window);

    glfwDestroyWindow(window); //Usuń kontekst OpenGL i okno
    glfwTerminate(); //Zwolnij zasoby zajęte przez GLFW
    exit(EXIT_SUCCESS);
}
