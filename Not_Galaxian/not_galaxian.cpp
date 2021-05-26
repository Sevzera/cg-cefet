#include <GL/glew.h>
#include <GL/freeglut.h>
#include <SOIL/SOIL.h>
#include <time.h>
#include <string>
#include <iostream>

using namespace std;

// constantes de contagem de inimigos
#define FRONT_ROW 8
#define MID_ROW 5
#define BACK_ROW 3

// iteradores de posicao dos inimigos na contagem geral
enum
{
    FRONT_ROW_ITERATOR = 0
};
enum
{
    MID_ROW_ITERATOR = (FRONT_ROW)
};
enum
{
    BACK_ROW_ITERATOR = (FRONT_ROW + MID_ROW)
};
enum
{
    MAX_ENEMIES = (FRONT_ROW + MID_ROW + BACK_ROW)
};

// modelo de objetos de cenario
typedef struct background
{

    float dx, dy, w, h;

} background;

// declaracao de objetos de cenario
background scenery = {450, 450, 900, 900};
background lives = {50, 50, 75, 50};
background endgame = {450, 600, 600, 300};
background score = {800, 820, 130, 100};
background rulesBackground = {450, 450, 900, 900};
background rulesBack = {450, 450, 900, 900};

// modelo de personagens
typedef struct human
{

    float dx, dy, w, h;
    bool shooting, melee;

} human;

// declaracao de personagens
human player = {(scenery.w / 2), 60, 60, 100, false, false};

human enemies[MAX_ENEMIES];

human boss = {(scenery.w / 2), (scenery.h - 100), 100, 150};

// modelo de projeteis
typedef struct bullet
{

    float currentX, dy, w, h;

} bullet;

// declaracao de projeteis
bullet playerBullet = {.dy = player.dy, .w = 6, .h = 12};

bullet enemyBullet = {.w = 6, .h = 12};

// variaveis de estado do jogo
int paused = 1; // paused = 1 -> jogo pausado
                // paused = -1 -> jogo rodando
int game = 2;   // game = 2 -> estado inicial
                // game = -1 -> estado de derrota
                // game = 1 -> estado de vitoria
                // game = 0 -> jogo em andamento
int deadEnemies = 0;
bool reset = false;

// variveis e funcoes do tiro do jogador
void playerBulletAnimation(int v);
int playerBulletInc = 12;
int enemyBulletInc = (-12);

// variveis e funcoes do ataque corpo a corpo do jogador
void playerMeleeAnimation(int v);
int playerMeleeAnimationController = 0;

// variveis e funcoes do projetil inimigo
int n;
int contEnemyBullet = 0;
bool bulletFlying = false;
void enemyBulletAnimation(int v);

// variaveis e funcoes do movimento dos inimigos
int enemiesXInc = 2;
int enemiesYInc = (-20);
void enemyApproach();
int enemyApproachController = 450;

// variveis de vida
int bossLives = 3;
int playerLives = 3;
bool aliveEnemies[MAX_ENEMIES];
int backRowLives[BACK_ROW] = {2, 2, 2};

// variaveis de pontuacao
float playerPoints = 0;
string playerPointsStr; // pontuacao convertida em string para desenho
int bossBonus = 1;
string bossBonusStr; // multiplicador convertido em string para desenho

// variaveis e funcoes de sistema
GLuint loadTexture(const char *file);
void drawPoints(float x, float y, string text);
void reshapeAndSetOrtho(int w, int h);
void mouseButtonHandler(int button, int state, int x, int y);
void mouseMovementHandler(int x, int y);
void inputHandler(unsigned char key, int x, int y);
void specialInputHandler(int key, int x, int y);

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
GLuint enemy1DeadTex;
GLuint enemy2DeadTex;
GLuint enemy3DeadTex;
GLuint enemy1IdleTex;
GLuint enemy2ShootingTex;
GLuint enemy2IdleTex;
GLuint enemy32HPShootingTex;
GLuint enemy31HPShootingTex;
GLuint enemy32HPTex;
GLuint enemy31HPTex;
GLuint player3HPTex;
GLuint player2HPTex;
GLuint player1HPTex;
GLuint player3HPMeleeTex;
GLuint player2HPMeleeTex;
GLuint player1HPMeleeTex;
GLuint player3HPShootingTex;
GLuint player2HPShootingTex;
GLuint player1HPShootingTex;
GLuint player0HPTex;
GLuint playerBulletTex;
GLuint enemyBulletTex;
GLuint boss3HPTex;
GLuint boss2HPTex;
GLuint boss1HPTex;
GLuint boss0HPTex;
GLuint lifeTex;
GLuint rulesTex;
GLuint gameOverTex;
GLuint victoryTex;

// funcao de desenho geral
void draw()
{

    glClear(GL_COLOR_BUFFER_BIT);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    // reset das variaveis iniciais do jogo
    if (reset == true)
    {
        for (int i = 0; i < MAX_ENEMIES; i++)
        {
            aliveEnemies[i] = true;
            enemies[i].shooting = false;
            if (i >= FRONT_ROW_ITERATOR && i < MID_ROW_ITERATOR)
            {
                enemies[i].dx = 100 + (i * 100);
                enemies[i].dy = 400;
                enemies[i].w = 100;
                enemies[i].h = 100;
            }
            else if (i >= MID_ROW_ITERATOR && i < BACK_ROW_ITERATOR)
            {
                enemies[i].dx = 150 + ((i - MID_ROW_ITERATOR) * 150);
                enemies[i].dy = 550;
                enemies[i].w = 100;
                enemies[i].h = 100;
            }
            else if (i >= BACK_ROW_ITERATOR && i < MAX_ENEMIES)
            {
                enemies[i].dx = 225 + ((i - BACK_ROW_ITERATOR) * 225);
                enemies[i].dy = 700;
                enemies[i].w = 100;
                enemies[i].h = 100;
            }
        }
        for (int i = 0; i < BACK_ROW; i++)
        {
            backRowLives[i] = 2;
        }
        bulletFlying = false;
        playerLives = 3;
        player.dx = scenery.w / 2;
        player.shooting = false;
        playerBullet.dy = player.dy;
        player.melee = false;

        bossLives = 3;
        boss.dx = scenery.w / 2;
        boss.dy = scenery.h - 100;

        enemyApproachController = 450;
        playerMeleeAnimationController = 0;

        playerPoints = 0;
        bossBonus = 1;
        game = 0;
        paused = (-1);

        reset = false;
    }
    if (game == 2)
    {
        for (int i = 0; i < MAX_ENEMIES; i++)
        {
            aliveEnemies[i] = true;
            enemies[i].shooting = false;
            if (i >= FRONT_ROW_ITERATOR && i < MID_ROW_ITERATOR)
            {
                enemies[i].dx = 100 + (i * 100);
                enemies[i].dy = 400;
                enemies[i].w = 100;
                enemies[i].h = 100;
            }
            else if (i >= MID_ROW_ITERATOR && i < BACK_ROW_ITERATOR)
            {
                enemies[i].dx = 150 + ((i - MID_ROW_ITERATOR) * 150);
                enemies[i].dy = 550;
                enemies[i].w = 100;
                enemies[i].h = 100;
            }
            else if (i >= BACK_ROW_ITERATOR && i < MAX_ENEMIES)
            {
                enemies[i].dx = 225 + ((i - BACK_ROW_ITERATOR) * 225);
                enemies[i].dy = 700;
                enemies[i].w = 100;
                enemies[i].h = 100;
            }
        }
        for (int i = 0; i < BACK_ROW; i++)
        {
            backRowLives[i] = 2;
        }
        paused = 1;
    }

    glColor3f(1, 1, 1);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_TEXTURE_2D);

    // desenho do cenario de fundo
    glBindTexture(GL_TEXTURE_2D, backgroundTex);
    glPushMatrix();
    glTranslatef(scenery.dx, scenery.dy, 0);
    glBegin(GL_TRIANGLE_STRIP);

    glTexCoord2f(0, 0);
    glVertex3f(-scenery.w / 2, -scenery.h / 2, 0);
    glTexCoord2f(0, 1);
    glVertex3f(-scenery.w / 2, scenery.h / 2, 0);
    glTexCoord2f(1, 0);
    glVertex3f(scenery.w / 2, -scenery.h / 2, 0);
    glTexCoord2f(1, 1);
    glVertex3f(scenery.w / 2, scenery.h / 2, 0);

    glEnd();
    glPopMatrix();

    // desenho dos inimigos abatidos
    for (int i = FRONT_ROW_ITERATOR; i < MAX_ENEMIES; i++)
    {
        if (aliveEnemies[i] == false)
        {
            if (i >= FRONT_ROW_ITERATOR && i < MID_ROW_ITERATOR)
                glBindTexture(GL_TEXTURE_2D, enemy1DeadTex);
            if (i >= MID_ROW_ITERATOR && i < BACK_ROW_ITERATOR)
                glBindTexture(GL_TEXTURE_2D, enemy2DeadTex);
            if (i >= BACK_ROW_ITERATOR && i < MAX_ENEMIES)
                glBindTexture(GL_TEXTURE_2D, enemy3DeadTex);

            glPushMatrix();
            glTranslatef(enemies[i].dx, enemies[i].dy, 0);
            glBegin(GL_TRIANGLE_STRIP);

            glTexCoord2f(0, 0);
            glVertex3f(-enemies[i].w / 2, -enemies[i].h / 2, 0);
            glTexCoord2f(1, 0);
            glVertex3f(enemies[i].w / 2, -enemies[i].h / 2, 0);
            glTexCoord2f(0, 1);
            glVertex3f(-enemies[i].w / 2, enemies[i].h / 2, 0);
            glTexCoord2f(1, 1);
            glVertex3f(enemies[i].w / 2, enemies[i].h / 2, 0);

            glEnd();
            glPopMatrix();
        }
    }

    enemyApproach();

    // desenho da primeira linha de inimigos
    for (int i = FRONT_ROW_ITERATOR; i < MID_ROW_ITERATOR; i++)
    {

        if (aliveEnemies[i] == true)
        {
            glBindTexture(GL_TEXTURE_2D, enemy1IdleTex);

            glPushMatrix();
            glTranslatef(enemies[i].dx, enemies[i].dy, 0);
            glBegin(GL_TRIANGLE_STRIP);

            glTexCoord2f(0, 0);
            glVertex3f(-enemies[i].w / 2, -enemies[i].h / 2, 0);
            glTexCoord2f(1, 0);
            glVertex3f(enemies[i].w / 2, -enemies[i].h / 2, 0);
            glTexCoord2f(0, 1);
            glVertex3f(-enemies[i].w / 2, enemies[i].h / 2, 0);
            glTexCoord2f(1, 1);
            glVertex3f(enemies[i].w / 2, enemies[i].h / 2, 0);

            glEnd();
            glPopMatrix();
        }
    }

    // desenho da segunda linha de inimigos
    for (int i = MID_ROW_ITERATOR; i < BACK_ROW_ITERATOR; i++)
    {
        if (aliveEnemies[i] == true)
        {
            if (enemies[i].shooting == true)
            {
                glBindTexture(GL_TEXTURE_2D, enemy2ShootingTex);
            }
            else
            {
                glBindTexture(GL_TEXTURE_2D, enemy2IdleTex);
            }

            glPushMatrix();
            glTranslatef(enemies[i].dx, enemies[i].dy, 0);
            glBegin(GL_TRIANGLE_STRIP);

            glTexCoord2f(0, 0);
            glVertex3f(-enemies[i].w / 2, -enemies[i].h / 2, 0);
            glTexCoord2f(1, 0);
            glVertex3f(enemies[i].w / 2, -enemies[i].h / 2, 0);
            glTexCoord2f(0, 1);
            glVertex3f(-enemies[i].w / 2, enemies[i].h / 2, 0);
            glTexCoord2f(1, 1);
            glVertex3f(enemies[i].w / 2, enemies[i].h / 2, 0);

            glEnd();
            glPopMatrix();
        }
    }

    // desenho da terceira linha de inimigos
    for (int i = BACK_ROW_ITERATOR; i < MAX_ENEMIES; i++)
    {
        if (aliveEnemies[i] == true)
        {
            if (enemies[i].shooting == true)
            {
                if (backRowLives[i - BACK_ROW_ITERATOR] == 2)
                    glBindTexture(GL_TEXTURE_2D, enemy32HPShootingTex);

                else if (backRowLives[i - BACK_ROW_ITERATOR] == 1)
                    glBindTexture(GL_TEXTURE_2D, enemy31HPShootingTex);
            }
            else if (enemies[i].shooting == false)
            {
                if (backRowLives[i - BACK_ROW_ITERATOR] == 2)
                    glBindTexture(GL_TEXTURE_2D, enemy32HPTex);

                else if (backRowLives[i - BACK_ROW_ITERATOR] == 1)
                    glBindTexture(GL_TEXTURE_2D, enemy31HPTex);
            }

            glPushMatrix();
            glTranslatef(enemies[i].dx, enemies[i].dy, 0);
            glBegin(GL_TRIANGLE_STRIP);

            glTexCoord2f(0, 0);
            glVertex3f(-enemies[i].w / 2, -enemies[i].h / 2, 0);
            glTexCoord2f(1, 0);
            glVertex3f(enemies[i].w / 2, -enemies[i].h / 2, 0);
            glTexCoord2f(0, 1);
            glVertex3f(-enemies[i].w / 2, enemies[i].h / 2, 0);
            glTexCoord2f(1, 1);
            glVertex3f(enemies[i].w / 2, enemies[i].h / 2, 0);

            glEnd();
            glPopMatrix();
        }
    }

    // desenho do jogador
    if (player.shooting == false && player.melee == false && playerLives > 0)
    {
        if (playerLives == 3)
            glBindTexture(GL_TEXTURE_2D, player3HPTex);
        if (playerLives == 2)
            glBindTexture(GL_TEXTURE_2D, player2HPTex);
        if (playerLives == 1)
            glBindTexture(GL_TEXTURE_2D, player1HPTex);
    }
    else if (player.melee == true && playerLives > 0)
    {
        if (playerLives == 3)
            glBindTexture(GL_TEXTURE_2D, player3HPMeleeTex);
        if (playerLives == 2)
            glBindTexture(GL_TEXTURE_2D, player2HPMeleeTex);
        if (playerLives == 1)
            glBindTexture(GL_TEXTURE_2D, player1HPMeleeTex);
    }
    else if (player.shooting == true && playerLives > 0)
    {
        if (playerLives == 3)
            glBindTexture(GL_TEXTURE_2D, player3HPShootingTex);
        if (playerLives == 2)
            glBindTexture(GL_TEXTURE_2D, player2HPShootingTex);
        if (playerLives == 1)
            glBindTexture(GL_TEXTURE_2D, player1HPShootingTex);
    }
    else if (playerLives == 0)
    {
        glBindTexture(GL_TEXTURE_2D, player0HPTex);
    }

    glPushMatrix();
    glTranslatef(player.dx, player.dy, 0);
    glBegin(GL_TRIANGLE_STRIP);

    glTexCoord2f(0, 0);
    glVertex3f(-player.w / 2, -player.h / 2, 0);
    glTexCoord2f(0, 1);
    glVertex3f(-player.w / 2, player.h / 2, 0);
    glTexCoord2f(1, 0);
    glVertex3f(player.w / 2, -player.h / 2, 0);
    glTexCoord2f(1, 1);
    glVertex3f(player.w / 2, player.h / 2, 0);

    glEnd();
    glPopMatrix();

    // desenho e tratamento do hitbox do projetil do jogador
    if (player.shooting == true)
    {
        glBindTexture(GL_TEXTURE_2D, playerBulletTex);

        glPushMatrix();
        glTranslatef(playerBullet.currentX, playerBullet.dy, 0);
        glBegin(GL_TRIANGLE_STRIP);

        glTexCoord2f(0, 0);
        glVertex3f(-playerBullet.w / 2, -playerBullet.h / 2, 0);
        glTexCoord2f(1, 0);
        glVertex3f(playerBullet.w / 2, -playerBullet.h / 2, 0);
        glTexCoord2f(0, 1);
        glVertex3f(-playerBullet.w / 2, playerBullet.h / 2, 0);
        glTexCoord2f(1, 1);
        glVertex3f(playerBullet.w / 2, playerBullet.h / 2, 0);

        glEnd();
        glPopMatrix();

        for (int i = 0; i < MAX_ENEMIES; i++)
        {
            if (aliveEnemies[i] == true)
            {
                if (playerBullet.dy + playerBullet.h / 2 <= enemies[i].dy + enemies[i].h / 2 && playerBullet.dy - playerBullet.h / 2 >= enemies[i].dy - enemies[i].h / 2)
                {
                    if (playerBullet.currentX + playerBullet.w / 2 <= enemies[i].dx + enemies[i].w / 2 && playerBullet.currentX - playerBullet.w / 2 >= enemies[i].dx - enemies[i].w / 2)
                    {
                        // trata inimigos da terceira linha
                        if (i >= BACK_ROW_ITERATOR)
                        {
                            backRowLives[i - BACK_ROW_ITERATOR]--;

                            if (backRowLives[i - BACK_ROW_ITERATOR] == 0)
                            {
                                aliveEnemies[i] = false;
                                playerPoints += 1 * bossBonus;
                            }
                        }
                        // trata inimigos da primeira e segunda linha
                        else
                        {
                            aliveEnemies[i] = false;
                            playerPoints += 0.5 * bossBonus;
                        }
                        playerBullet.dy = player.dy;
                        player.shooting = false;
                    }
                }
            }
        }
        // trata o chefe
        if (playerBullet.dy + playerBullet.h / 2 <= boss.dy + boss.h / 2 && playerBullet.dy - playerBullet.h / 2 >= boss.dy - boss.h / 2)
            if (playerBullet.currentX >= boss.dx - boss.w / 2 && playerBullet.currentX <= boss.dx + boss.w / 2)
            {
                if (bossLives > 0)
                {
                    bossBonus++;
                    bossLives--;
                }
                playerBullet.dy = player.dy;
                player.shooting = false;
            }
    }

    // tratamento dos hitboxes corpo a corpo
    playerMeleeAnimation(0);
    if (player.melee == true)
    {
        for (int i = 0; i < MAX_ENEMIES; i++)
        {
            if (aliveEnemies[i] == true)
            {
                if (player.dy + 25 >= enemies[i].dy - enemies[i].w / 2)
                {
                    if (player.dx + player.w / 2 + 15 >= enemies[i].dx - enemies[i].w / 2 && player.dx - player.w / 2 - 15 <= enemies[i].dx + enemies[i].w / 2)
                    {

                        // trata inimigos da terceira linha
                        if (i >= BACK_ROW_ITERATOR)
                        {
                            backRowLives[i - BACK_ROW_ITERATOR]--;

                            if (backRowLives[i - BACK_ROW_ITERATOR] == 0)
                            {
                                aliveEnemies[i] = false;
                                playerPoints += 1 * bossBonus;
                            }
                        }
                        // trata inimigos da primeira e segunda linha
                        else
                        {
                            aliveEnemies[i] = false;
                            playerPoints += 0.5 * bossBonus;
                        }
                        player.melee = false;
                    }
                }
            }
        }
    }

    // sorteio do atirador
    contEnemyBullet++;
    if (bulletFlying == false)                  // trata conflitos entre os tiros inimigos
        n = (rand() % MAX_ENEMIES) + FRONT_ROW; // sorteia index do inimigo que atira entre os presentes na segunda e terceira linha
    if (contEnemyBullet % 10 == 0)              // define a frequencia dos tiros
    {
        if (enemies[n].shooting == false && bulletFlying == false)
        {
            if (aliveEnemies[n] == true)
            {
                enemies[n].shooting = true;
                bulletFlying = true;
                enemyBullet.currentX = enemies[n].dx;
                enemyBullet.dy = enemies[n].dy;

                enemyBulletAnimation(0);
            }
        }
    }

    if (contEnemyBullet > 100)
        contEnemyBullet = 0;

    // desenho e tratamento do hitbox do projetil do inimigo
    if (enemies[n].shooting == true && bulletFlying == true)
    {
        glBindTexture(GL_TEXTURE_2D, enemyBulletTex);

        glPushMatrix();
        glTranslatef(enemyBullet.currentX, enemyBullet.dy, 0);
        glBegin(GL_TRIANGLE_STRIP);

        glTexCoord2f(0, 0);
        glVertex3f(-enemyBullet.w / 2, -enemyBullet.h / 2, 0);
        glTexCoord2f(1, 0);
        glVertex3f(enemyBullet.w / 2, -enemyBullet.h / 2, 0);
        glTexCoord2f(0, 1);
        glVertex3f(-enemyBullet.w / 2, enemyBullet.h / 2, 0);
        glTexCoord2f(1, 1);
        glVertex3f(enemyBullet.w / 2, enemyBullet.h / 2, 0);

        glEnd();
        glPopMatrix();

        if (enemyBullet.dy + enemyBullet.h / 2 <= player.dy + player.h / 2 && enemyBullet.dy - enemyBullet.h / 2 >= player.dy - player.h / 2)
        {
            if (enemyBullet.currentX + enemyBullet.w / 2 <= player.dx + player.w / 2 && enemyBullet.currentX - enemyBullet.w / 2 >= player.dx - player.w / 2)
            {
                if (playerLives > 0)
                    playerLives--;

                enemies[n].shooting = false;
                bulletFlying = false;
            }
        }
    }

    // desenho do chefe
    if (bossLives > 0)
    {
        if (bossLives == 3)
        {
            glBindTexture(GL_TEXTURE_2D, boss3HPTex);
        }
        else if (bossLives == 2)
        {
            glBindTexture(GL_TEXTURE_2D, boss2HPTex);
        }
        else if (bossLives == 1)
        {
            glBindTexture(GL_TEXTURE_2D, boss1HPTex);
        }
    }
    else if (bossLives == 0)
    {
        glBindTexture(GL_TEXTURE_2D, boss0HPTex);
    }

    glPushMatrix();
    glTranslatef(boss.dx, boss.dy, 0);
    glBegin(GL_TRIANGLE_STRIP);

    glTexCoord2f(0, 0);
    glVertex3f(-boss.w / 2, -boss.h / 2, 0);
    glTexCoord2f(0, 1);
    glVertex3f(-boss.w / 2, boss.h / 2, 0);
    glTexCoord2f(1, 0);
    glVertex3f(boss.w / 2, -boss.h / 2, 0);
    glTexCoord2f(1, 1);
    glVertex3f(boss.w / 2, boss.h / 2, 0);

    glEnd();
    glPopMatrix();

    // desenho das vidas
    glBindTexture(GL_TEXTURE_2D, lifeTex);

    for (int i = 0; i < playerLives; i++)
    {
        glPushMatrix();
        glTranslatef(lives.dx + (i * 60), lives.dy, 0);
        glBegin(GL_TRIANGLE_STRIP);

        glTexCoord2f(0, 0);
        glVertex3f(-lives.w / 2, -lives.h / 2, 0);
        glTexCoord2f(0, 1);
        glVertex3f(-lives.w / 2, lives.h / 2, 0);
        glTexCoord2f(1, 0);
        glVertex3f(lives.w / 2, -lives.h / 2, 0);
        glTexCoord2f(1, 1);
        glVertex3f(lives.w / 2, lives.h / 2, 0);

        glEnd();
        glPopMatrix();
    }

    // desenho das regras
    if (paused == 1 && game != 1 && game != -1)
    {
        glDisable(GL_TEXTURE_2D);

        glColor4f(1, 1, 1, 0.8);

        glPushMatrix();
        glTranslatef(rulesBack.dx, rulesBack.dy, 0);
        glBegin(GL_TRIANGLE_STRIP);

        glTexCoord2f(0, 0);
        glVertex3f(-rulesBack.w / 2, -rulesBack.h / 2, 0);
        glTexCoord2f(0, 1);
        glVertex3f(-rulesBack.w / 2, rulesBack.h / 2, 0);
        glTexCoord2f(1, 0);
        glVertex3f(rulesBack.w / 2, -rulesBack.h / 2, 0);
        glTexCoord2f(1, 1);
        glVertex3f(rulesBack.w / 2, rulesBack.h / 2, 0);

        glEnd();
        glPopMatrix();

        glColor4f(1, 1, 1, 1);

        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, rulesTex);
        glPushMatrix();
        glTranslatef(rulesBack.dx, rulesBack.dy, 0);
        glBegin(GL_TRIANGLE_STRIP);

        glTexCoord2f(0, 0);
        glVertex3f(-rulesBack.w / 2, -rulesBack.h / 2, 0);
        glTexCoord2f(0, 1);
        glVertex3f(-rulesBack.w / 2, rulesBack.h / 2, 0);
        glTexCoord2f(1, 0);
        glVertex3f(rulesBack.w / 2, -rulesBack.h / 2, 0);
        glTexCoord2f(1, 1);
        glVertex3f(rulesBack.w / 2, rulesBack.h / 2, 0);

        glEnd();
        glPopMatrix();

        glEnable(GL_TEXTURE_2D);
    }

    // condicoes de vitoria/derrota
    deadEnemies = 0;
    for (int i = 0; i < MAX_ENEMIES; i++)
    {
        if (aliveEnemies[i] == false)
            deadEnemies++;
    }
    if (deadEnemies == MAX_ENEMIES)
    {
        game = 1;
    }

    if (playerLives == 0)
        game = (-1);

    // desenho do estado de derrota
    if (game == (-1))
    {
        glBindTexture(GL_TEXTURE_2D, gameOverTex);
        glPushMatrix();
        glTranslatef(endgame.dx, endgame.dy, 0);
        glBegin(GL_TRIANGLE_STRIP);

        glTexCoord2f(0, 0);
        glVertex3f(-endgame.w / 2, -endgame.h / 2, 0);
        glTexCoord2f(0, 1);
        glVertex3f(-endgame.w / 2, endgame.h / 2, 0);
        glTexCoord2f(1, 0);
        glVertex3f(endgame.w / 2, -endgame.h / 2, 0);
        glTexCoord2f(1, 1);
        glVertex3f(endgame.w / 2, endgame.h / 2, 0);

        glEnd();
        glPopMatrix();
        paused = 1;
    }

    // desenho do estado de vitoria
    else if (game == 1)
    {
        glBindTexture(GL_TEXTURE_2D, victoryTex);
        glPushMatrix();
        glTranslatef(endgame.dx, endgame.dy, 0);
        glBegin(GL_TRIANGLE_STRIP);

        glTexCoord2f(0, 0);
        glVertex3f(-endgame.w / 2, -endgame.h / 2, 0);
        glTexCoord2f(0, 1);
        glVertex3f(-endgame.w / 2, endgame.h / 2, 0);
        glTexCoord2f(1, 0);
        glVertex3f(endgame.w / 2, -endgame.h / 2, 0);
        glTexCoord2f(1, 1);
        glVertex3f(endgame.w / 2, endgame.h / 2, 0);

        glEnd();
        glPopMatrix();
        paused = 1;
    }

    glDisable(GL_TEXTURE_2D);

    // desenho do placar
    if (paused == (-1) || game == 1 || game == (-1))
    {
        glColor4f(1, 1, 1, 0.5);

        glPushMatrix();
        glTranslatef(score.dx, score.dy, 0);
        glBegin(GL_TRIANGLE_STRIP);

        glTexCoord2f(0, 0);
        glVertex3f(-score.w / 2, -score.h / 2, 0);
        glTexCoord2f(0, 1);
        glVertex3f(-score.w / 2, score.h / 2, 0);
        glTexCoord2f(1, 0);
        glVertex3f(score.w / 2, -score.h / 2, 0);
        glTexCoord2f(1, 1);
        glVertex3f(score.w / 2, score.h / 2, 0);

        glEnd();
        glPopMatrix();

        glColor3f(0, 0, 0);

        playerPointsStr = to_string(playerPoints);
        for (int i = playerPointsStr.length(); i >= 0; i--)
        {
            if (playerPointsStr[i] == '0')
                playerPointsStr[i] = ' ';
            if (playerPointsStr[i] == '.')
            {
                i = 0;
            }
        }

        bossBonusStr = to_string(bossBonus);
        bossBonusStr = bossBonusStr + "x";

        drawPoints(750, 850, "Pontuacao:");
        drawPoints(750, 825, playerPointsStr);
        drawPoints(750, 800, "Multiplicador:");
        drawPoints(750, 775, bossBonusStr);
    }

    glutSwapBuffers();
}

// funcao de movimentacao do projetil do jogador
void playerBulletAnimation(int v)
{
    if (player.shooting == true && player.melee == false)
    {
        if (paused == (-1))
        {
            playerBullet.dy = playerBullet.dy + playerBulletInc;
        }
        if (playerBullet.dy >= scenery.h)
        {
            playerBullet.dy = player.dy;
            player.shooting = false;
        }
        glutPostRedisplay();
        glutTimerFunc(33, playerBulletAnimation, 0);
    }
}

// funcao de controle do ataque corpo a corpo do jogador
void playerMeleeAnimation(int v)
{
    if (player.melee == true)
    {
        if (paused == (-1))
        {
            playerMeleeAnimationController++;
            if (playerMeleeAnimationController > 25)
            {
                player.melee = false;
                playerMeleeAnimationController = 0;
            }
        }
        glutPostRedisplay();
        glutTimerFunc(33, playerBulletAnimation, 0);
    }
}

// funcao de movimentacao do projetil inimigo
void enemyBulletAnimation(int v)
{
    if (enemies[n].shooting == true && bulletFlying == true)
    {
        if (paused == (-1))
        {
            enemyBullet.dy += enemyBulletInc;
        }
        if (enemyBullet.dy <= 0)
        {
            bulletFlying = false;
            enemies[n].shooting = false;
        }
        glutPostRedisplay();
        glutTimerFunc(33, enemyBulletAnimation, 0);
    }
}

// funcao de movimentacao dos inimigos
void enemyApproach()
{
    if (paused == (-1))
    {
        for (int i = 0; i < MAX_ENEMIES; i++)
        {
            enemyApproachController += enemiesXInc / 2;

            if (aliveEnemies[i] == true)
            {
                enemies[i].dx += enemiesXInc * 1.5;
            }
            if (enemyApproachController <= 0 || enemyApproachController >= scenery.w)
            {
                enemiesXInc *= (-1);
                for (int i = 0; i < MAX_ENEMIES; i++)
                    if (aliveEnemies[i] == true)
                    {
                        enemies[i].dy += enemiesYInc;

                        if (enemies[i].dy <= 0)
                        {
                            game = (-1);
                        }
                    }
            }
        }

        glutPostRedisplay();
    }
}

// inicializa algumas coisas importantes
void init()
{
    backgroundTex = loadTexture("pngs/background.png");
    enemy1DeadTex = loadTexture("pngs/enemy1_dead.png");
    enemy2DeadTex = loadTexture("pngs/enemy2_dead.png");
    enemy3DeadTex = loadTexture("pngs/enemy3_0hp.png");
    enemy1IdleTex = loadTexture("pngs/enemy1_idle.png");
    enemy2ShootingTex = loadTexture("pngs/enemy2_shooting.png");
    enemy2IdleTex = loadTexture("pngs/enemy2_idle.png");
    enemy32HPShootingTex = loadTexture("pngs/enemy3_2hpshooting.png");
    enemy31HPShootingTex = loadTexture("pngs/enemy3_1hpshooting.png");
    enemy32HPTex = loadTexture("pngs/enemy3_2hp.png");
    enemy31HPTex = loadTexture("pngs/enemy3_1hp.png");
    player3HPTex = loadTexture("pngs/player_3hp.png");
    player2HPTex = loadTexture("pngs/player_2hp.png");
    player1HPTex = loadTexture("pngs/player_1hp.png");
    player3HPMeleeTex = loadTexture("pngs/player_3hpmelee.png");
    player2HPMeleeTex = loadTexture("pngs/player_2hpmelee.png");
    player1HPMeleeTex = loadTexture("pngs/player_1hpmelee.png");
    player3HPShootingTex = loadTexture("pngs/player_3hpshooting.png");
    player2HPShootingTex = loadTexture("pngs/player_2hpshooting.png");
    player1HPShootingTex = loadTexture("pngs/player_1hpshooting.png");
    player0HPTex = loadTexture("pngs/player_0hp.png");
    playerBulletTex = loadTexture("pngs/playerBullet.png");
    enemyBulletTex = loadTexture("pngs/enemyBullet.png");
    boss3HPTex = loadTexture("pngs/boss_3hp.png");
    boss2HPTex = loadTexture("pngs/boss_2hp.png");
    boss1HPTex = loadTexture("pngs/boss_1hp.png");
    boss0HPTex = loadTexture("pngs/boss_0hp.png");
    lifeTex = loadTexture("pngs/life.png");
    rulesTex = loadTexture("pngs/rules.png");
    gameOverTex = loadTexture("pngs/gameover.png");
    victoryTex = loadTexture("pngs/victory.png");
}

// funcao principal
int main(int argc, char *argv[])
{
    srand(time(0));

    glutInit(&argc, argv);

    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(scenery.w, scenery.h);
    glutCreateWindow("not Galaxian");
    glClearColor(1, 1, 1, 1);

    glutDisplayFunc(draw);
    glutReshapeFunc(reshapeAndSetOrtho);
    glutKeyboardFunc(inputHandler);
    glutMouseFunc(mouseButtonHandler);
    glutPassiveMotionFunc(mouseMovementHandler);
    glutSpecialFunc(specialInputHandler);

    init();
    glutMainLoop();

    return 0;
}

// callback de redimensionamento
void reshapeAndSetOrtho(int w, int h)
{
    if (w != scenery.w || h != scenery.h)
    {
        w = scenery.w;
        h = scenery.h;
    }
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, scenery.w, 0, scenery.h, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

// callback de botoes do mouse
void mouseButtonHandler(int button, int state, int x, int y)
{
    if (paused == (-1))
    {
        switch (button)
        {

        case GLUT_LEFT_BUTTON:

            if (state == GLUT_DOWN)

                if (player.melee == false && player.shooting == false && playerBullet.dy == player.dy)
                {
                    player.shooting = true;
                    playerBullet.currentX = player.dx;
                    playerBulletAnimation(0);
                }

        case GLUT_RIGHT_BUTTON:
            if (state == GLUT_DOWN)
                if (player.melee == false && player.shooting == false)
                    player.melee = true;
            break;
        }
    }
    glutPostRedisplay();
}

// callback de movimentacao do ponteiro do mouse
void mouseMovementHandler(int x, int y)
{
    if (paused == (-1))
    {
        if (x > player.dx)
            if (player.dx > -50)
                player.dx += 20;
        if (x < player.dx)
            if (player.dx < scenery.w + 50)
                player.dx -= 20;
    }
    glutPostRedisplay();
}

// callback do teclado
void inputHandler(unsigned char key, int x, int y)
{
    switch (key)
    {

    case 27:
        exit(0);
        break;

    case ' ':
        if (paused == (-1))
        {
            if (player.melee == false && playerBullet.dy == player.dy)
            {
                player.shooting = true;
                playerBullet.currentX = player.dx;
                playerBulletAnimation(0);
            }
        }
        break;

    case 'r':
        reset = true;
        break;

    case 'p':

        if (game != 2)
            paused *= (-1);
        break;

        // case 'a':

        //     if (paused == (-1))
        //         if (player.dx > 0)
        //             player.dx -= 50;
        //     break;

        // case 'd':

        //     if (paused == (-1))
        //         if (player.dx < scenery.w)
        //             player.dx += 50;
        //     break;

    default:
        break;
    }
    glutPostRedisplay();
}

// callback de teclas especiais
void specialInputHandler(int key, int x, int y)
{
    if (paused == (-1))
    {
        if (key == GLUT_KEY_LEFT)
            if (player.dx > 0)
                player.dx -= 50;

        if (key == GLUT_KEY_RIGHT)
            if (player.dx < scenery.w)
                player.dx += 50;

        if (key == GLUT_KEY_CTRL_L)
            if (player.melee == false && player.shooting == false)
                player.melee = true;
    }
    glutPostRedisplay();
}

// funcao de desenho de texto (no caso, desenha o placar)
void drawPoints(float x, float y, string text)
{
    glRasterPos2f(x, y);
    for (int i = 0; i < text.length(); i++)
    {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, text[i]);
    }
}