#include <GL/glew.h>
#include <GL/freeglut.h>
#include <GL/glu.h>
#include <SOIL/SOIL.h>
#include <SDL/SDL.h>
#include <SDL/SDL_mixer.h>
#include <math.h>
#include <string>
#include <iostream>

using namespace std;

#define PI 3.14
#define WINDOW_WIDTH 2000
#define WINDOW_HEIGHT 1000

void init();
void reshapeAndSetProjection(int w, int h);
void mouseButtonHandler(int button, int state, int x, int y);
void mouseMovementHandler(int x, int y);
void inputHandler(unsigned char key, int x, int y);
void specialInputHandler(int key, int x, int y);
void drawText(float x, float y, float z, string text);
GLuint loadTexture(const char *file);

float eyeX, eyeY, eyeZ;

void drawSphere(float radius, int rows, int columns);

typedef struct Planet
{

    float x, y, z;
    float translate, rotate;

} Planet;

typedef struct Satelite
{

    Planet home;
    float x, y, z;
    float rotate;

} Satelite;

typedef struct lesserSatelite
{

    Satelite home;
    float x, y, z;
    float rotate;

} lesserSatelite;

float nirnRotation = 0;
float nirnMoonsRotation = 0;
float nirnMoonsTranslate = 1;
Planet pNirn = {0, 0, 0, 0, 0};
Planet pAkatosh = {4, 0, -5, 1, 1};
Planet pJulianos = {-4, 0, 5, 1, 1};
Planet pKynareth = {5, 0, -2.5, 1, 1};
Planet pArkay = {-1, 3, -2, 1, 1};
Planet pZenithar = {5, -3, 3, 1, 1};

Satelite sMara = {pZenithar, sMara.home.x, sMara.home.y + 0.5f, sMara.home.z, 1};
lesserSatelite sDibella = {sMara, sDibella.home.x, sDibella.home.y + 0.5f, sDibella.home.z, 1};
Satelite sStendarr = {pJulianos, 1, sStendarr.home.y + 1, 1, 1};

void objectsMovement();
void drawPlanetOrbits(Planet planet, float angle);

int camera = 1;
int showNirnAt = -1;
int showOrbitalPlane = -1;
int showOrbits = -1;
int lightEnabled = 1;

// funcao de carregamento de texturas
GLuint loadTexture(const char *file)
{
    GLuint textureID = SOIL_load_OGL_texture(
        file,
        SOIL_LOAD_AUTO,
        SOIL_CREATE_NEW_ID,
        SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT);

    if (textureID == 0)
    {
        printf("Erro do SOIL: '%s'\n", SOIL_last_result());
    }

    return textureID;
}

GLuint backgroundTex;
GLuint nirnTex;
GLuint masserTex;
GLuint secundaTex;
GLuint zenitharTex;
GLuint maraTex;
GLuint dibellaTex;
GLuint akatoshTex;
GLuint julianosTex;
GLuint stendarrTex;
GLuint kynarethTex;
GLuint arkayTex;

int light = 1;
GLfloat magnusAmbientLight[] = {0.4, 0.4, 0.4, 1.0};
GLfloat magnusDiffuseLight[] = {1.0, 1.0, 1.0, 1.0};
GLfloat magnusSpecularLight[] = {1.0, 1.0, 1.0, 1.0};
GLfloat magnusLightPosition[] = {6.0, 12.0, 0, 1.0};
GLfloat materialAmbientV[] = {0.5, 0.5, 0.5, 1.0};
GLfloat materialDiffuseV[] = {0.8, 0.8, 0.8, 1.0};
GLfloat materialSpecularV[] = {0.8, 0.8, 0.8, 1.0};
GLint materialSpecularI = 80;

// funcao de desenho geral
void draw()
{
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glEnable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    if (camera == 1)
        gluLookAt(0, 2, 12, 0, 0, 0, 0, 1, 0);
    if (camera == -1)
        gluLookAt(0, 10, 0, 0, 0, 0, 0, 0, -1);

    glEnable(GL_TEXTURE_2D);

    // Galaxia
    glBindTexture(GL_TEXTURE_2D, backgroundTex);
    glPushMatrix();
    glBegin(GL_TRIANGLE_STRIP);

    glNormal3f(0, 0, -1);
    glTexCoord2f(0, 0);
    glVertex3f(-50, -25, -10);
    glNormal3f(0, 0, -1);
    glTexCoord2f(0, 1);
    glVertex3f(-50, 25, -15);
    glNormal3f(0, 0, -1);
    glTexCoord2f(1, 0);
    glVertex3f(50, -25, -10);
    glNormal3f(0, 0, -1);
    glTexCoord2f(1, 1);
    glVertex3f(50, 25, -15);

    glEnd();
    glPopMatrix();

    glPushMatrix();
    glBegin(GL_TRIANGLE_STRIP);

    glNormal3f(0, -1, 0);
    glTexCoord2f(0, 0);
    glVertex3f(-65, -15, -15);
    glNormal3f(0, -1, 0);
    glTexCoord2f(0, 1);
    glVertex3f(-65, -15, 30);
    glNormal3f(0, -1, 0);
    glTexCoord2f(1, 0);
    glVertex3f(65, -15, -15);
    glNormal3f(0, -1, 0);
    glTexCoord2f(1, 1);
    glVertex3f(65, -15, 30);

    glEnd();
    glPopMatrix();

    if(light == 1){
        glEnable(GL_LIGHTING);
    } else if (light == -1){
        glDisable(GL_LIGHTING);
    }

    // Nirn
    glBindTexture(GL_TEXTURE_2D, nirnTex);
    glPushMatrix();
    glTranslatef(pNirn.x, pNirn.y, pNirn.z);
    glRotatef(nirnRotation, 0, 1, 0);
    drawSphere(1, 50, 50);
    glPopMatrix();

    // Masser
    glBindTexture(GL_TEXTURE_2D, masserTex);
    glPushMatrix();
    glTranslatef(2 * cos(2 * PI * nirnMoonsTranslate), 1, 2 * sin(2 * PI * nirnMoonsTranslate));
    glRotatef(nirnMoonsRotation, 0, 1, 0);
    drawSphere(0.2, 50, 50);
    glPopMatrix();

    // Secunda
    glBindTexture(GL_TEXTURE_2D, secundaTex);
    glPushMatrix();
    glTranslatef(2 * cos(2 * PI * nirnMoonsTranslate), 0.5, 2 * sin(2 * PI * nirnMoonsTranslate));
    glRotatef(nirnMoonsRotation, 0, 1, 0);
    glTranslatef(0.2, 0, 0.5);
    drawSphere(0.05, 50, 50);
    glPopMatrix();

    // Zenithar
    glBindTexture(GL_TEXTURE_2D, zenitharTex);
    glPushMatrix();
    glTranslatef(pZenithar.x * cos(2 * PI * pZenithar.translate), pZenithar.y, pZenithar.z * sin(2 * PI * pZenithar.translate));
    glRotatef(pZenithar.rotate, 0, 1, 0);
    drawSphere(0.6, 50, 50);
    glPopMatrix();

    // Mara
    glBindTexture(GL_TEXTURE_2D, maraTex);
    glPushMatrix();
    glTranslatef(sMara.home.x * cos(2 * PI * pZenithar.translate), sMara.home.y, sMara.home.z * sin(2 * PI * pZenithar.translate));
    glRotatef(45, 0, 0, 1);
    glRotatef(sMara.rotate, 0, 1, 0);
    glTranslatef(0, 0, 1);
    drawSphere(0.3, 50, 50);
    glPopMatrix();

    // Dibella
    glBindTexture(GL_TEXTURE_2D, dibellaTex);
    glPushMatrix();
    glTranslatef(sDibella.home.x * cos(2 * PI * pZenithar.translate), sDibella.home.y, sDibella.home.z * sin(2 * PI * pZenithar.translate));
    glRotatef(sDibella.rotate, 0, 1, 0);
    glTranslatef(0, -0.8f, -1.2);
    drawSphere(0.1, 50, 50);
    glPopMatrix();

    // Akatosh
    glBindTexture(GL_TEXTURE_2D, akatoshTex);
    glPushMatrix();
    glRotatef(-15, 0, 0, 1);
    glTranslatef(pAkatosh.x * cos(2 * PI * pAkatosh.translate), pAkatosh.y, pAkatosh.z * sin(2 * PI * pAkatosh.translate));
    glRotatef(pAkatosh.rotate, 0, 1, 0);
    drawSphere(0.6, 50, 50);
    glPopMatrix();

    // Julianos
    glBindTexture(GL_TEXTURE_2D, julianosTex);
    glPushMatrix();
    glRotatef(-15, 0, 0, 1);
    glTranslatef(pJulianos.x * cos(2 * PI * pJulianos.translate), pJulianos.y, pJulianos.z * sin(2 * PI * pJulianos.translate));
    glRotatef(pJulianos.rotate, 0, 1, 0);
    drawSphere(0.6, 50, 50);
    glPopMatrix();

    // Stendarr
    glBindTexture(GL_TEXTURE_2D, stendarrTex);
    glPushMatrix();
    glRotatef(-15, 0, 0, 1);
    glTranslatef(sStendarr.home.x * cos(2 * PI * pJulianos.translate), sStendarr.home.y, sStendarr.home.z * sin(2 * PI * pJulianos.translate));
    glRotatef(sStendarr.rotate, 0, 1, 0);
    glRotatef(120, 0, 0.5, 0);
    glTranslatef(0, 0, 1);
    drawSphere(0.3, 50, 50);
    glPopMatrix();

    // Kynareth
    glBindTexture(GL_TEXTURE_2D, kynarethTex);
    glPushMatrix();
    glRotatef(25, 0, 0, 1);
    glTranslatef(pKynareth.x * cos(2 * PI * pKynareth.translate), pKynareth.y, pKynareth.z * sin(2 * PI * pKynareth.translate));
    glRotatef(pKynareth.rotate, 0, 1, 0);
    drawSphere(0.6, 50, 50);
    glPopMatrix();

    // Kynareth's Ring
    glColor3f(0, 0, 1);
    glPushMatrix();
    glRotatef(25, 0, 0, 1);
    glTranslatef(pKynareth.x * cos(2 * PI * pKynareth.translate), pKynareth.y, pKynareth.z * sin(2 * PI * pKynareth.translate));
    glRotatef(-90, 1, 0, 0);
    glutSolidTorus(0.05, 0.8, 50, 50);
    glPopMatrix();
    glColor3f(1, 1, 1);

    // Arkay
    glBindTexture(GL_TEXTURE_2D, arkayTex);
    glPushMatrix();
    glRotatef(20, 0, 0, 1);
    glTranslatef(pArkay.x * cos(2 * PI * pArkay.translate), pArkay.y, pArkay.z * sin(2 * PI * pArkay.translate));
    glRotatef(pArkay.rotate, 0, 1, 0);
    drawSphere(0.6, 50, 50);
    glPopMatrix();

    glDisable(GL_TEXTURE_2D);


    // Plano orbital e atmosfera de Nirn
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glColor4f(1, 1, 1, 0.5);

    if(showNirnAt == 1){
        glShadeModel(GL_SMOOTH);
        glutSolidSphere(1.1, 50, 50);
        glShadeModel(GL_FLAT);
    }

    if(showOrbits == 1){
        drawPlanetOrbits(pZenithar, 0);
        drawPlanetOrbits(pAkatosh, -15);
        drawPlanetOrbits(pKynareth, 25);
        drawPlanetOrbits(pArkay, 20);    
    }

    if(showOrbitalPlane == 1){
        glPushMatrix();
        glBegin(GL_TRIANGLE_STRIP);

        glNormal3f(0, -1, 0);
        glVertex3f(-65, 0, -15);
        glNormal3f(0, -1, 0);
        glVertex3f(-65, 0, 30);
        glNormal3f(0, -1, 0);
        glVertex3f(65, 0, -15);
        glNormal3f(0, -1, 0);
        glVertex3f(65, 0, 30);

        glEnd();
        glPopMatrix();
    }

    glDisable(GL_BLEND); 

    glDisable(GL_DEPTH_TEST);

    glutSwapBuffers();
}

// funcao que desenha orbita dos planetas

void drawPlanetOrbits(Planet planet, float angle){

    glDisable(GL_TEXTURE_2D);
    glColor3f(1, 1, 1);
    glPushMatrix();
    glRotatef(angle, 0, 0, 1);
    glLineWidth(3);
    glBegin(GL_LINE_LOOP);
    for(float i = 0; i < 2 * PI; i += 0.01){
        float x = planet.x * cos(i);
        float y = planet.y;
        float z = planet.z * sin(i);
        glVertex3f(x, y, z);
    }
    glEnd();
    glPopMatrix();
    glEnable(GL_TEXTURE_2D);
    if(lightEnabled == 1)
        glEnable(GL_LIGHTING);

    glutPostRedisplay();
}

// funcao que desenha esfera solida pronta para textura
void drawSphere(float radius, int rows, int columns)
{
    GLUquadric *quadObj = gluNewQuadric();
    gluQuadricDrawStyle(quadObj, GLU_FILL);
    gluQuadricNormals(quadObj, GLU_SMOOTH);
    gluQuadricTexture(quadObj, GL_TRUE);
    gluSphere(quadObj, radius, rows, columns);
    gluDeleteQuadric(quadObj);
}

// funcao de movimento dos planetas e satelites
void objectsMovement()
{
    nirnMoonsTranslate += 6;
    if (cos(2 * PI * nirnMoonsTranslate) >= cos(2 * PI))
    // {
        // cout << "nirn moons reset" << endl;
        nirnMoonsTranslate = 1;
    // }
    nirnRotation += 1;
    if (nirnRotation >= 360)
        nirnRotation = 1;
    nirnMoonsRotation += 2;
    if (nirnMoonsRotation >= 360)
        nirnMoonsRotation = 1;

    pZenithar.translate += 2;
    if (cos(2 * PI * pZenithar.translate) >= cos(2 * PI))
    // {
        // cout << "zenithar reset" << endl;
        pZenithar.translate = 1;
    // }
    pZenithar.rotate += 1.5;
    if (pZenithar.rotate >= 360)
        pZenithar.rotate = 1;
    sMara.rotate += 2.5;
    if (sMara.rotate >= 360)
        sMara.rotate = 1;
    sDibella.rotate += 3;
    if (sDibella.rotate >= 360)
        sDibella.rotate = 1;

    pJulianos.translate += 1;
    if (cos(2 * PI * pJulianos.translate) >= cos(2 * PI))
    // {
        // cout << "julianos reset" << endl;
        pJulianos.translate = 1;
    // }
    pJulianos.rotate += 1;
    if (pJulianos.rotate >= 360)
        pJulianos.rotate = 1;
    sStendarr.rotate += 2.5;
    if (sStendarr.rotate >= 360)
        sStendarr.rotate = 1;

    pAkatosh.translate += 1;
    if (cos(2 * PI * pAkatosh.translate) >= cos(2 * PI))
    // {
        // cout << "akatosh reset" << endl;
        pAkatosh.translate = 1;
    // }
    pAkatosh.rotate += 1;
    if (pAkatosh.rotate >= 360)
        pAkatosh.rotate = 1;

    pKynareth.translate += 2;
    if (cos(2 * PI * pKynareth.translate) >= cos(2 * PI))
    // {
        // cout << "kynareth reset" << endl;
        pKynareth.translate = 1;
    // }
    pKynareth.rotate += 2;
    if (pKynareth.rotate >= 360)
        pKynareth.rotate = 1;

    pArkay.translate += 5;
    if (cos(2 * PI * pArkay.translate) >= cos(2 * PI))
    // {
        // cout << "arkay reset" << endl;
        pArkay.translate = 1;
    // }
    pArkay.rotate += 3;
    if (pArkay.rotate >= 360)
        pArkay.rotate = 1;

    glutPostRedisplay();
}

// inicializa algumas coisas importantes
void init()
{
    backgroundTex = loadTexture("pngs/background.png");
    nirnTex = loadTexture("pngs/nirn.png");
    masserTex = loadTexture("pngs/masser.png");
    secundaTex = loadTexture("pngs/secunda.png");
    zenitharTex = loadTexture("pngs/zenithar.png");
    maraTex = loadTexture("pngs/mara.png");
    dibellaTex = loadTexture("pngs/dibella.png");
    akatoshTex = loadTexture("pngs/akatosh.png");
    julianosTex = loadTexture("pngs/julianos.png");
    stendarrTex = loadTexture("pngs/stendarr.png");
    kynarethTex = loadTexture("pngs/kynareth.png");
    arkayTex = loadTexture("pngs/arkay.png");

    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);
    Mix_Chunk *nirnSound;
    nirnSound = Mix_LoadWAV("sound/theme.wav");
    Mix_PlayChannel(-1, nirnSound, -1);

    glShadeModel(GL_FLAT);

    glLightfv(GL_LIGHT0, GL_AMBIENT, magnusAmbientLight);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, magnusDiffuseLight);
    glLightfv(GL_LIGHT0, GL_SPECULAR, magnusSpecularLight);
    glLightfv(GL_LIGHT0, GL_POSITION, magnusLightPosition);
    glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 2);

    glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbientV);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuseV);
    glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecularV);
    glMateriali(GL_FRONT, GL_SHININESS, materialSpecularI);

    glEnable(GL_LIGHT0);
}

// funcao principal
int main(int argc, char *argv[])
{
    glutInit(&argc, argv);
    SDL_Init(SDL_INIT_AUDIO);

    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutCreateWindow("Mundus");
    glClearColor(1, 1, 1, 1);

    glutDisplayFunc(draw);
    glutReshapeFunc(reshapeAndSetProjection);
    glutIdleFunc(objectsMovement);
    glutKeyboardFunc(inputHandler);

    init();
    glutMainLoop();

    return 0;
}

// callback de redimensionamento
void reshapeAndSetProjection(int w, int h)
{
    if(w != WINDOW_WIDTH || h != WINDOW_HEIGHT){
        w = WINDOW_WIDTH;
        h = WINDOW_HEIGHT;
    }
    
    glViewport(0, 0, w, h);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60, 2, 1, 100);
}

// callback do teclado
void inputHandler(unsigned char key, int x, int y)
{
    switch (key)
    {

    case 27:
        exit(0);
        break;

    case 'l':
        light *= -1;
        lightEnabled *= -1;
        break;
    case 'L':
        light *= -1;
        lightEnabled *= -1;
        break;

    case 'k':
        showNirnAt *= -1;
        break;
    case 'K':
        showNirnAt *= -1;
        break;

    case 'o':
        showOrbits *= -1;
        break;
    case 'O':
        showOrbits *= -1;
        break;    

    case 'p':
        showOrbitalPlane *= -1;
        break;
    case 'P':
        showOrbitalPlane *= -1;
        break;

    case 'c':
        camera *= -1;
        break;
    case 'C':
        camera *= -1;
        break;

    case '1':
        camera = 1;
        break;
    case '2':
        camera = -1;
        break;

    default:
        break;
    }
    glutPostRedisplay();
}

// funcao de desenho de texto
void drawText(float x, float y, float z, string text)
{
    glRasterPos3f(x, y, z);
    for (int i = 0; i < text.length(); i++)
    {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, text[i]);
    }
}