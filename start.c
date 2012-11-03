#define NEUTRO 3
#include <allegro.h>
#include "estruturas.h"
/**
    Convenções do mapa:

    Construções que pertencem a outros jogadores:
        C = cidade
        H = HeadQuarters
        B = Base
        A = Aeroporto
        P = Porto (port)
    C1 == cidade do jogador 1
    C2 == cidade do jogador 2
    CN == Cidade neutra
    E assim vai

    L = Planície (plains)
    O = Mar (Ocean)
    S = Praia (shoal)
    T = Floresta
    F = Reefs(recifes)
    M = Montanha
    R = Rio (river)
    E = Estrada
*/
/**
    IN = INFANTRY
    ME = MECH
    RE = RECON
    TA = TANK
    MT = MEDIUM_TANK
    NT = NEO_TANK
    AP = APC
    AR = ARTILLERY
    RO = ROCKETS
    AA = A_AIR
    MI = MISSILES
    FI = FIGHTER
    BO = BOMBER
    BC = B_COPTER
    TC = T_COPTER
    BS = B_SHIP
    CR = CRUISER
    LA = LANDER
    SU = SUBMARINE

*/
void timerLuz(){
    timeLuz = (timeLuz+1) % 6;
}

void timerCursor(){
    if (timeCursor > 0) timeCursor--;
}

void timerUnit(){
    if (timeUnit > 0) timeUnit--;
}

//Caso o segundo parâmetro seja zero, ele verifica se aquele terreno precisa ser de propriedade de alguém, se não, ele checa se o jogador especificado é dono daquela propriedade

int verificaPropriedade(Tile* square, int jogador)
{
    char buffer = square->terrain->tipo;
    if (jogador == 0)
    {
        return (buffer == 'C' || buffer == 'H' || buffer == 'B' || buffer == 'A' || buffer == 'P');
    }

    if(square->terrain->propriedade == jogador) /* Para ser usado em momentos futuros */
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

void unidadesIniciais(Jogo* jogo){
    int i,j;
    TipoUnidade tipo;
    char string[4];

    for (i = 0; i < 25; i++){
        for (j = 0; j < 30; j++){
            scanf(" %s",string);
            if (string[0] != '0'){
                if (string[0] == 'I') tipo = INFANTRY;
                else if (string[0] == 'N') tipo = NEO_TANK;
                else if (string[0] == 'F') tipo = FIGHTER;
                else if (string[0] == 'C') tipo = CRUISER;
                else if (string[0] == 'L') tipo = LANDER;
                else if (string[0] == 'S') tipo = SUBMARINE;
                else if (string[0] == 'M'){
                    if (string[1] == 'E') tipo = MECH;
                    else if (string[1] == 'T') tipo = MEDIUM_TANK;
                    else if (string[1] == 'I') tipo = MISSILES;
                }
                else if (string[0] == 'R'){
                    if (string[1] == 'E') tipo = RECON;
                    else if (string[1] == 'O') tipo = ROCKETS;
                }
                else if (string[0] == 'B'){
                    if (string[1] == 'O') tipo = BOMBER;
                    else if (string[1] == 'S') tipo = B_SHIP;
                    else if (string[1] == 'C') tipo = B_COPTER;
                }
                else if (string[0] == 'A'){
                    if (string[1] == 'A') tipo = A_AIR;
                    else if (string[1] == 'P') tipo = APC;
                    else if (string[1] == 'R') tipo = ARTILLERY;
                }
                else if (string[0] == 'T'){
                    if (string[1] == 'A') tipo = TANK;
                    else if (string[1] == 'C') tipo = T_COPTER;
                }
                jogo->matriz[i][j]->unit = NovaUnidade(tipo,string[2]-'0');
            }
        }
    }
}
void preencheMatriz(Jogo* jogo)
{
    int i = 0, j = 0;
    char buffer[3];

    for( i = 0; i < 25; i++ )
    {
        for ( j = 0; j < 30; j++ )
        {
            scanf (" %s", buffer);
            jogo->matriz[i][j]->terrain->tipo = buffer[0];

            //Calcula Defesa
            jogo->matriz[i][j]->terrain->defesa = 0;

            switch(buffer[0])
            {
                case 'H':
                case 'M':
                    jogo->matriz[i][j]->terrain->defesa++;
                case 'C':
                case 'B':
                case 'A':
                case 'P':
                    jogo->matriz[i][j]->terrain->defesa++;
                case 'T':
                    jogo->matriz[i][j]->terrain->defesa++;
                case 'L':
                case 'F':
                    jogo->matriz[i][j]->terrain->defesa++;
            }

            //Define a propriedade
            if( verificaPropriedade(jogo->matriz[i][j], 0) ) /*Verifica se o tile é do tipo que deve pertencer a algum jogador */
            {
                if (buffer[1] == '1' || buffer[1] == '2')  /*Verifica a que jogador pertence */
                {
                    jogo->matriz[i][j]->terrain->propriedade = buffer[1] - '0' ; /*E atribui o jogador correto*/
                }
                else /*Ou se é um terreno neutro */
                {
                    jogo->matriz[i][j]->terrain->propriedade = NEUTRO;
                }
            }
            else /* Se não for terreno que precise ser propriedade, a propriedade é 0 */
            {
                jogo->matriz[i][j]->terrain->propriedade = 0;
            }
        }
    }

    unidadesIniciais(jogo);
}

void preencheDC(Jogo* jogo)
{
    FILE* DC = fopen ("damagechart.txt","r");
    int i, j;

    for ( i = 0; i < 25; i++)
    {
        for ( j = 0; j < 25; j++)
        {
            fscanf (DC, "%d", &jogo->damageChart[i][j]);
        }
    }

    fclose(DC);
}

char auxContorno (char c)
{
    char retorno;
    if (c == 'S' || c == 'R' || c == 'O' || c == 'F') retorno = 'A';
    else if (c == 'E') retorno = 'E';
    else retorno = 'O';

    return retorno;
}

void mudaConstrucao(Jogo* jogo,int i, int j, int player)
{
    /*  1- Player 1
        2- Player 2
        3-  Neutro   */
    BITMAP* buildings = load_bitmap("sprites/buildings (32x16).bmp",NULL);
    BITMAP* blitAux = create_bitmap(16,32);
    Terreno* tipo;
    int aux, luz, k = i;
    if (jogo->matriz[i][j]->terrain->propriedade != 0)
    {
        jogo->matriz[i][j]->terrain->propriedade = player;

        if (i > 0) i--;
        for (;i < 25 && (i < k || verificaPropriedade(jogo->matriz[i][j], 0));i++){
            tipo = jogo->matriz[i][j]->terrain;

            if (tipo->propriedade != 0){  /*Verificando o tipo de tile que se trata pertence a algum jogador */
                if(tipo->tipo == 'H'){
                    aux = 0;
                }
                else if(tipo->tipo == 'C'){
                    aux = 1;
                }
                else if(tipo->tipo == 'B'){
                    aux = 2;
                }
                else if(tipo->tipo == 'A'){
                    aux = 3;
                }
                else aux = 4; //Porto

                clear_to_color(blitAux,makecol(255,0,255));
                if (tipo->propriedade == NEUTRO){
                    blit(buildings , blitAux, 128, aux*32, 0, 0, 16, 32);
                    draw_sprite(jogo->constucoes[0],blitAux,j*16,(i-1)*16);
                    draw_sprite(jogo->constucoes[1],blitAux,j*16,(i-1)*16);
                }
                else{
                    for (luz = 0; luz < 2; luz++){
                        blit(buildings , blitAux, 32*(jogo->cor[(tipo->propriedade) - 1]) + 16*luz, aux*32 , 0, 0, 16, 32);
                        draw_sprite(jogo->constucoes[luz],blitAux,j*16,(i-1)*16);
                    }
                }
            }
            else{
                rectfill(jogo->constucoes[0],j*16,i*16,(j + 1)*16,(i + 1)*16, makecol(255,0,255));
                rectfill(jogo->constucoes[1],j*16,i*16,(j + 1)*16,(i + 1)*16, makecol(255,0,255));
            }
        }
    }
    destroy_bitmap(buildings);
    destroy_bitmap(blitAux);
}

void montaConstrucoes(Jogo* jogo)
{
    int i,j, aux, luz;
    BITMAP* blitAux = create_bitmap(16,32);
    jogo->constucoes[0] = create_bitmap(480,400);
    jogo->constucoes[1] = create_bitmap(480,400);
    BITMAP* buildings = load_bitmap("sprites/buildings (32x16).bmp",NULL);

    clear_to_color(jogo->constucoes[0],makecol(255,0,255));
    clear_to_color(jogo->constucoes[1],makecol(255,0,255));

    Terreno* tipo;

    for (i = 0; i < 25; i++){
        for (j = 0; j < 30; j++){
            tipo = jogo->matriz[i][j]->terrain;

            if (tipo->propriedade != 0){  /*Verificando o tipo de tile que se trata pertence a algum jogador */
                if(tipo->tipo == 'H'){
                    aux = 0;
                }
                else if(tipo->tipo == 'C'){
                    aux = 1;
                }
                else if(tipo->tipo == 'B'){
                    aux = 2;
                }
                else if(tipo->tipo == 'A'){
                    aux = 3;
                }
                else aux = 4; //Porto

                clear_to_color(blitAux,makecol(255,0,255));
                if (tipo->propriedade == NEUTRO){
                     blit(buildings , blitAux, 128, aux*32, 0, 0, 16, 32);
                     draw_sprite(jogo->constucoes[0],blitAux,j*16,(i-1)*16);
                     draw_sprite(jogo->constucoes[1],blitAux,j*16,(i-1)*16);
                }
                else{
                    for (luz = 0; luz < 2; luz++){
                        blit(buildings , blitAux, 32*(jogo->cor[(tipo->propriedade) - 1]) + 16*luz, aux*32 , 0, 0, 16, 32);
                        draw_sprite(jogo->constucoes[luz],blitAux,j*16,(i-1)*16);
                    }
                }
            }
        }
    }
    destroy_bitmap(buildings);
    destroy_bitmap(blitAux);
}

BITMAP* retornaTile(int i, int j, Jogo* jogo, BITMAP* lands)
{
    int aux, aux2;
    char auxC;
    char contorno[4];
    BITMAP* parte = create_bitmap(16,32); /* Esse bitmap será retornado, depois de ter sido desenhado bonitinho, pra poder ser desenhado sobre o mapa na outra função */
    BITMAP* blitAux = create_bitmap(16,32);

    clear_to_color(parte, makecol(255, 0, 255));
    clear_to_color(blitAux, makecol(255, 0, 255));
    blit(lands , parte, 16, 128, 0, 16, 16, 16);

    Terreno* tipo = jogo->matriz[i][j]->terrain;

    /* Tá, aqui tem uns bagulhos que eu não vou fingir que entendo, mas é basicamente pra poder desenhar bonitinho, dependendo do jogador ao qual pertence a construção */
       /* Certo, a partir desse ponto é completamente magia negra, não tente entender */

    if (tipo->propriedade == 0){
        if (tipo->tipo == 'T'){
            blit(lands , parte, 16, 144, 0, 16, 16, 16);
        }
        else if (tipo->tipo == 'F'){
            blit(lands , parte, 48, 128, 0, 16, 16, 16);
        }
        else if (tipo->tipo == 'M'){
            if (i == 0 || jogo->matriz[i-1][j]->terrain->propriedade != 0 || auxContorno(jogo->matriz[i-1][j]->terrain->tipo) != 'O') blit(lands , blitAux, 48, 144, 0, 16, 16, 16);
            else blit(lands , blitAux, 0, 128, 0, 0, 16, 32);
            draw_sprite(parte,blitAux,0,0);
        }
        else{
            if (i == 0){
                contorno[0] = 0;
                auxC = jogo->matriz[i + 1][j]->terrain->tipo;
                contorno[2] = auxContorno(auxC);
            }
            else if (i == 24){
                contorno[2] = 0;
                auxC = jogo->matriz[i - 1][j]->terrain->tipo;
                contorno[0] = auxContorno(auxC);
            }
            else{
                auxC = jogo->matriz[i - 1][j]->terrain->tipo;
                contorno[0] = auxContorno(auxC);
                auxC = jogo->matriz[i + 1][j]->terrain->tipo;
                contorno[2] = auxContorno(auxC);
            }

            if (j == 0){
                contorno[1] = 0;
                auxC = jogo->matriz[i][j + 1]->terrain->tipo;
                contorno[3] = auxContorno(auxC);
            }
            else if (j == 29){
                contorno[3] = 0;
                auxC = jogo->matriz[i][j - 1]->terrain->tipo;
                contorno[1] = auxContorno(auxC);
            }
            else{
                auxC = jogo->matriz[i][j - 1]->terrain->tipo;
                contorno[1] = auxContorno(auxC);
                auxC = jogo->matriz[i][j + 1]->terrain->tipo;
                contorno[3] = auxContorno(auxC);
            }

            if (tipo->tipo == 'E'){
                if (contorno[0] == 'A' && contorno[2] == 'A')  blit(lands , parte, 32, 128, 0, 16, 16, 16);
                else if (contorno[1] == 'A' && contorno[3] == 'A')  blit(lands , parte, 32, 144, 0, 16, 16, 16);
                else{
                    if (contorno[0] != 'E' && contorno[2] != 'E') aux = 0;
                    else if (contorno[0] != 'E') aux = 1;
                    else if (contorno[2] != 'E') aux = 3;
                    else aux = 2;

                    if (contorno[1] != 'E' && contorno[3] != 'E') aux2 = 0;
                    else if (contorno[1] != 'E') aux2 = 1;
                    else if (contorno[3] != 'E') aux2 = 3;
                    else aux2 = 2;

                     blit(lands , parte, aux2*16, aux*16, 0, 16, 16, 16);
                }
            }
            else if (tipo->tipo == 'R'){
                if ( contorno[0] == 'O' && contorno[2] == 'O') aux = 0;
                else if (contorno[0] == 'O'){
                    if (contorno[2] == 'E' && (i > 22 || auxContorno(jogo->matriz[i + 2][j]->terrain->tipo) != 'A') ) aux = 0;
                    else aux = 1;
                }
                else if (contorno[2] == 'O'){
                    if (contorno[0] == 'E' && (i < 2 || auxContorno(jogo->matriz[i - 2][j]->terrain->tipo) != 'A') ) aux = 0;
                    else aux = 3;
                }
                else{
                    aux = 2;
                    if (contorno[2] == 'E' && (i > 22 || auxContorno(jogo->matriz[i + 2][j]->terrain->tipo) != 'A') ) aux = 3;
                    if (contorno[0] == 'E' && (i < 2 || auxContorno(jogo->matriz[i - 2][j]->terrain->tipo) != 'A') ){
                        if (aux == 2) aux = 1;
                        else aux = 0;
                    }
                }

                if (contorno[1] == 'O' && contorno[3]  == 'O') aux2 = 0;
                else if (contorno[1] == 'O'){
                    if (contorno[3] == 'E' && (j > 27 || auxContorno(jogo->matriz[i][j + 2]->terrain->tipo) != 'A') ) aux2 = 0;
                    else aux2 = 1;
                }
                else if (contorno[3] == 'O'){
                    if (contorno[1] == 'E' && (j < 2 || auxContorno(jogo->matriz[i][j - 2]->terrain->tipo) != 'A') ) aux2 = 0;
                    else aux2 = 3;
                }
                else{
                    aux2 = 2;
                    if (contorno[3] == 'E' && (j > 27 || auxContorno(jogo->matriz[i][j + 2]->terrain->tipo) != 'A') ) aux2 = 3;
                    if (contorno[1] == 'E' && (j < 2 || auxContorno(jogo->matriz[i][j - 2]->terrain->tipo) != 'A') ){
                        if (aux2 == 2) aux2 = 1;
                        else aux2 = 0;
                    }
                }

                blit(lands , parte, 64 + aux2*16, aux*16, 0, 16, 16, 16);
            }
            else{
                if (contorno[0] != 'A' && contorno[0] != 0 && contorno[2] != 'A' && contorno[2] != 0) aux = 0;
                else if (contorno[0] != 'A' && contorno[0] != 0) aux = 1;
                else if (contorno[2] != 'A' && contorno[2] != 0) aux = 3;
                else aux = 2;

                if (contorno[1] != 'A' && contorno[1] != 0 && contorno[3] != 'A' && contorno[3] != 0) aux2 = 0;
                else if (contorno[1] != 'A' && contorno[1] != 0) aux2 = 1;
                else if (contorno[3] != 'A' && contorno[3] != 0) aux2 = 3;
                else aux2 = 2;

                if (tipo->tipo == 'O'){
                    blit(lands , parte, aux2*16, 64 + aux*16, 0, 16, 16, 16);
                    aux = 64;
                }
                else if (tipo->tipo == 'S'){
                    blit(lands , parte, 64 + aux2*16, 64 + aux*16, 0, 16, 16, 16);
                    aux = 96;
                }

                if (contorno[0] == 'A' && contorno[1] == 'A' && auxContorno(jogo->matriz[i - 1][j - 1]->terrain->tipo) != 'A'){
                    if (jogo->matriz[i - 1][j]->terrain->tipo != 'R' && jogo->matriz[i][j - 1]->terrain->tipo != 'R'){ //evita por uma beirada na foz
                        blit(lands, blitAux, aux, 128, 0, 16, 16, 16);
                        draw_sprite(parte,blitAux,0,0);
                    }
                }
                if (contorno[1] == 'A' && contorno[2] == 'A' && auxContorno(jogo->matriz[i + 1][j - 1]->terrain->tipo) != 'A'){
                    if (jogo->matriz[i][j - 1]->terrain->tipo != 'R' && jogo->matriz[i + 1][j]->terrain->tipo != 'R'){
                        blit(lands, blitAux, aux, 144, 0, 16, 16, 16);
                        draw_sprite(parte,blitAux,0,0);
                    }
                }
                if (contorno[2] == 'A' && contorno[3] == 'A' && auxContorno(jogo->matriz[i + 1][j + 1]->terrain->tipo) != 'A'){
                    if (jogo->matriz[i + 1][j]->terrain->tipo != 'R' && jogo->matriz[i][j + 1]->terrain->tipo != 'R'){
                        blit(lands, blitAux, aux + 16, 144, 0, 16, 16, 16);
                        draw_sprite(parte,blitAux,0,0);
                    }
                }
                if (contorno[3] == 'A' && contorno[0] == 'A' && auxContorno(jogo->matriz[i - 1][j + 1]->terrain->tipo) != 'A'){
                    if (jogo->matriz[i][j + 1]->terrain->tipo != 'R' && jogo->matriz[i - 1][j]->terrain->tipo != 'R'){
                        blit(lands, blitAux, aux + 16, 128, 0, 16, 16, 16);
                        draw_sprite(parte,blitAux,0,0);
                    }
                }
            }
        }
    }
    destroy_bitmap(blitAux);
    return parte; /* O tile tendo sido desenhado de acordo com as checagens, é retornado */
}

void geraTerreno(Jogo* jogo)
{
    int i,j;
     /*Os sprites são guardados em uma única imagem, Eduardo faz magia negra pra saber que pedaço pegar*/
    BITMAP* lands = load_bitmap("sprites/Land.bmp",NULL);

    jogo->mapa = create_bitmap(480,400);
    BITMAP* parte;

    for (i = 0;i < 25; i++)
    {
        for (j = 0;j < 30; j++)
        {
            parte = retornaTile(i,j,jogo, lands);

            /* A função retornaTile cria o bitmap correspondente ao tipo de tile que será desenhado, pra poder desenhar ele em cima do mapa */

            draw_sprite(jogo->mapa, parte, j * 16, (i-1) * 16);

            destroy_bitmap(parte);
        }
    }
    destroy_bitmap(lands);
}

Jogo* inicializaJogo()
{
    int i, j;

    Jogo* jogo = (Jogo*) malloc (sizeof(Jogo));
    for(i = 0; i < 25; i++)
    {
        for (j = 0; j < 30; j++)
        {
            jogo->matriz[i][j] = (Tile*) malloc (sizeof(Tile));
            jogo->matriz[i][j]->terrain = (Terreno*) malloc (sizeof (Terreno));
            jogo->matriz[i][j]->unit = NULL;
        }
    }

    preencheMatriz(jogo);

    for (i = 0; i < 25; i++)
    {
        for (j = 0; j < 30; j++)
        {
            jogo->range[i][j] = 0;
        }
    }

    jogo->cor[0] = -1;
    jogo->cor[1] = -1;

    preencheDC(jogo);

    jogo->buffer = create_bitmap(SCREEN_W,SCREEN_H);
    clear_to_color(jogo->buffer,0);
    Jogador* jogador1 = (Jogador*) malloc (sizeof(Jogador));
    Jogador* jogador2 = (Jogador*) malloc (sizeof(Jogador));
    Terreno* ter;

    jogador1->unitList = NULL;
    jogador2->unitList = NULL;
    jogador1->numJogador = 1;
    jogador2->numJogador = 2;
    jogador1->numPropriedades = 0;
    jogador2->numPropriedades = 0;

    for (i = 0; i < 25; i++)
    {
        for(j = 0; j < 30; j++)
        {
            ter = jogo->matriz[i][j]->terrain;
            if(ter->propriedade == 1)
            {
                jogador1->numPropriedades++;
                if (ter->tipo == HQ) jogador1->HQ = ter;
            }
            else if (ter->propriedade == 2)
            {
                jogador2->numPropriedades++;
                if (ter->tipo == HQ) jogador2->HQ = ter;
            }
            jogo->matriz[i][j]->terrain->capture = 20;
        }
    }

    jogador1->money = jogador1->numPropriedades * 1000; //só deve ser o 1º player, o 2º vai ganhar na função passaTurno

    jogador1->prox = jogador2;
    jogador2->prox = jogador1;

    jogo->jogadores = jogador1;

    geraTerreno(jogo);
    jogo->cursor = (Cursor*) malloc (sizeof(Cursor));
    jogo->selecionado = (Ponto*) malloc (sizeof(Cursor));
    jogo->cursor->x = 0;
    jogo->cursor->y = 0;
    jogo->selecionado->x = -1;
    jogo->selecionado->y = -1;
    jogo->UnidadeLoja = -1;
    jogo->cursor->imagem = load_bitmap("sprites/cursor.bmp", NULL);

    FILE* unitValue = fopen ("unitValue.txt","r");

    for (i = 0; i < 25; i++)
    {
        fscanf(unitValue, "%d", &jogo->unitValue[i]);
    }

    fclose(unitValue);

    FILE* movCost = fopen("movcost.txt","r");

    for(i = 0; i < 7; i++)
    {
        for(j = 0; j < 13; j++)
        {
            fscanf(movCost, " %d", &jogo->custoMovimento[i][j]);
        }
    }
    fclose(movCost);

    jogo->imgs = (Imgs*) malloc (sizeof(Imgs));

    jogo->imgs->hud = load_bitmap("sprites/HUD.bmp",NULL);
    jogo->imgs->unitsON = load_bitmap("sprites/color tanks.bmp",NULL);
    jogo->imgs->unitsOFF = load_bitmap("sprites/uncolor tanks.bmp",NULL);
    jogo->imgs->UnitHUD = load_bitmap("sprites/LargeInfo Unit (272x400).bmp",NULL);
    jogo->imgs->TerHUD = load_bitmap("sprites/LargeInfo Ter (272x400).bmp",NULL);
    jogo->imgs->fotoTer = load_bitmap("sprites/TerPhoto (96x128).bmp",NULL);
    jogo->imgs->fotoUnit = load_bitmap("sprites/UnitPhoto (70x63).bmp",NULL);
    jogo->imgs->cash = load_bitmap("sprites/dinheiro (273x68).bmp",NULL);
    jogo->imgs->alvo = load_bitmap("sprites/alvos (50x62).bmp",NULL);
    return jogo;
}

void menuEscolhaPlayers (Jogo* jogo)
{
    short int selecao = 0,i,player = 0;

    BITMAP* tela = load_bitmap("sprites/Menu_personagem.bmp",NULL);
    BITMAP* telaBW = load_bitmap("sprites/personagem_selecionado (180x440).bmp",NULL);
    BITMAP* cinza = create_bitmap(180,440);

    clear_to_color(cinza,makecol(126,126,126));

    set_trans_blender(0,0,0,200);

    while (!key[KEY_ESC] && jogo->cor[1] == -1)
    {
        draw_sprite(jogo->buffer,tela,0,0);
        for (i = 0;i < 4;i++)
        {
            if (jogo->cor[0] == i) blit(telaBW,jogo->buffer,i*180,0,16 + i*196,128,180,440);
            if (selecao != i) draw_trans_sprite(jogo->buffer,cinza,16 + i*196,128);
        }


        if (timeCursor == 0)
        {
            if (key[KEY_RIGHT])
            {
                if (selecao <= 2) selecao++;
                else selecao = 0;
            }

            else if (key[KEY_LEFT])
            {
                if (selecao >= 1) selecao--;
                else selecao = 3;
            }

            timeCursor = 8;
        }

        if (key[KEY_Z])
        {
            if (player == 0)
            {
                jogo->cor[0] = selecao;
                player++;
            }
            else if (selecao != jogo->cor[0])
            {
                jogo->cor[1] = selecao;
            }
        }
        if (key[KEY_X]){
            player = 0;
            jogo->cor[0] = -1;
        }
        draw_sprite(screen,jogo->buffer,0,0);
    }

    set_trans_blender(0,0,0,160); ///trasparencia usada na Range do movimento

    destroy_bitmap(tela);
    destroy_bitmap(telaBW);
    destroy_bitmap(cinza);
    timeCursor = 8;
}
