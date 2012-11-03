#include <allegro.h>
#include "estruturas.h"


void segueCaminho(Jogo* jogo, int x, int y);

void verificaDerrota(Jogo* jogo)
{
    Jogador* aux = jogo->jogadores;

    if(aux->unitList == NULL) //Vencedor é jogo->jogadores->prox
    {
        printf("Perdeu preiba!!\n");

        //Sugiro fazer algo do tipo uma tela entrando mostrando a imagem do jogador vencedor e a voz do cara lá "WINNER".
        //Eu vou ver se faço isso.
        exit(1); // Na verdade, o certo era fazer um timer pra esperar um pouco depois de mostrar a animação da vitória, e atribuir ESC ao teclado, pro allegro sair.
    }
    else if (aux->prox->unitList == NULL) //Vencedor é jogo->jogadores
    {
        printf("Perdeu preiba!!\n");
        exit(1);
    }
    if(aux->prox->HQ->propriedade != aux->prox->numJogador)
    {
        printf("Perdeu preiba!!\n");
        exit(1);
    }
}

TipoTerreno numTerreno(char tipo)
{
    if (tipo == 'H') return HQ;
    if (tipo == 'C') return CITY;
    if (tipo == 'B') return BASE;
    if (tipo == 'A') return AIRPORT;
    if (tipo == 'P') return PORT;
    if (tipo == 'L') return PLAIN;
    if (tipo == 'O') return OCEAN;
    if (tipo == 'S') return SHOAL;
    if (tipo == 'T') return FOREST;
    if (tipo == 'M') return MOUNT;
    if (tipo == 'R') return RIVER;
    if (tipo == 'F') return REEFS;
    else return ROAD; //  tipo == 'E'
}

int pontoFora(Ponto p)
{
    if( p.x < 0 || p.x >= 30 || p.y < 0 || p.y >= 25 ) return 1;
    else return 0;
}

#define min(a,b) ( (a) < (b) ? (a) : (b) )
#define mod(a) ( (a) < 0 ? -(a) : (a) )

Caminho* criaCaminho (int x, int y)
{
    Caminho* caminho = (Caminho*) malloc (sizeof(Caminho));
    caminho->ponto.x = x;
    caminho->ponto.y = y;
    caminho->prox = NULL;

    return caminho;
}

void destroiCaminho(Caminho* c)
{
    if (c)
    {
        destroiCaminho(c->prox);
        free(c);
    }
}

Caminho* addCaminho (Caminho* list, Caminho* termo)
{
    Caminho* aux;
    if (!list){
        destroiCaminho(termo);
        return NULL;
    }
    else{
        for (aux = list; aux->prox != NULL; aux = aux->prox);
        aux->prox = termo;
    }
    return list;
}

Caminho* menorCaminho(Caminho* opcoes[4])
{
    int i,j = -1;
    Caminho* menor = NULL;
    for (i = 0; i < 4; i++){
        if (opcoes[i]){
            if (menor == NULL || menor->distancia > opcoes[i]->distancia){
                menor = opcoes[i];
                j = i;
            }
        }
    }

    for (i = 0; i < 4; i++){
        if (i != j) destroiCaminho(opcoes[i]);
    }

    return menor;
}

Caminho* caminhoAux(Jogo* jogo, int i, int j, int passos, Unidade* unit, int gas)
{
    Caminho* caminho = criaCaminho (j,i);
    caminho->distancia = passos;
    Caminho* opcoes[4] = {NULL,NULL,NULL,NULL};
    int custo;

    if (caminho->ponto.x == jogo->cursor->x && caminho->ponto.y == jogo->cursor->y ){
        return caminho;
    }
    else if (gas)
    {
        if(i > 0 && jogo->range[i-1][j])
        {
            custo = jogo->custoMovimento[unit->tipoMov][numTerreno(jogo->matriz[i-1][j]->terrain->tipo)];
            if (custo <= gas) opcoes[0] = caminhoAux(jogo, i-1, j, passos+1, unit, gas - custo);
        }
        if(i < 24 && jogo->range[i+1][j])
        {
            custo = jogo->custoMovimento[unit->tipoMov][numTerreno(jogo->matriz[i+1][j]->terrain->tipo)];
            if (custo <= gas) opcoes[1] = caminhoAux(jogo, i+1, j, passos+1, unit, gas - custo);
        }
        if(j > 0 && jogo->range[i][j-1])
        {
            custo = jogo->custoMovimento[unit->tipoMov][numTerreno(jogo->matriz[i][j-1]->terrain->tipo)];
            if (custo <= gas) opcoes[2] = caminhoAux(jogo, i, j-1, passos+1, unit, gas - custo);
        }
        if(j < 29 && jogo->range[i][j+1])
        {
            custo = jogo->custoMovimento[unit->tipoMov][numTerreno(jogo->matriz[i][j+1]->terrain->tipo)];
            if (custo <= gas) opcoes[3] = caminhoAux(jogo, i, j+1, passos+1, unit, gas - custo);
        }

        caminho = addCaminho(menorCaminho(opcoes),caminho); //devolve o menor caminho e destroi os outros;
        return caminho;
    }
    else{
        destroiCaminho(caminho);
        return NULL;
    }
}

void montacaminho(Jogo* jogo)
{
    int i,j;
    Caminho *aux, *aux2;
    Unidade* unit = jogo->matriz[jogo->selecionado->y][jogo->selecionado->x]->unit;
    Caminho* caminho = caminhoAux(jogo,jogo->selecionado->y,jogo->selecionado->x,0, unit,unit->movimento);

    for (i = 0; i < 25; i++){ //limpa a range (trocando todos os locais possiveis por 6)
        for (j = 0; j < 30; j++){
            if (jogo->range[i][j]) jogo->range[i][j] = 6;
        }
    }

    for (aux = caminho; aux->prox != NULL; aux = aux2){
        aux2 = aux->prox;
        if (aux2->ponto.y > aux->ponto.y) jogo->range[aux2->ponto.y][aux2->ponto.x] = UP;
        else if (aux2->ponto.y < aux->ponto.y) jogo->range[aux2->ponto.y][aux2->ponto.x] = DOWN;
        else if (aux2->ponto.x > aux->ponto.x) jogo->range[aux2->ponto.y][aux2->ponto.x] = LEFT;
        else jogo->range[aux2->ponto.y][aux2->ponto.x] = RIGHT;
    }
    jogo->range[caminho->ponto.y][caminho->ponto.x] = HERE;

    destroiCaminho(caminho);
}

void swap( int* a, int* b)
{
    *a ^= *b;
    *b ^= *a;
    *a ^= *b;
}

int PontoIgual( Ponto* a, Ponto* b)
{
    return (a->x == b->x) && (a->y == b->y);
}

int terrenoCursor( Jogo* jogo, Cursor* c)
{
    return numTerreno(jogo->matriz[c->y][c->x]->terrain->tipo);
}

int terrenoPonto( Jogo* jogo, Ponto* p)
{
    return numTerreno(jogo->matriz[p->y][p->x]->terrain->tipo);
}

int CursorIgual( Cursor* a, Cursor* b)
{
    return (a->x == b->x) && (a->y == b->y);
}

int combustivelCursor( Jogo* jogo, Cursor* c)
{
    return jogo->matriz[c->y][c->x]->unit->fuel;
}

int combustivelPonto( Jogo* jogo, Ponto* p)
{
    return jogo->matriz[p->y][p->x]->unit->fuel;
}

int movimentoCursor( Jogo* jogo, Cursor* c)
{
    return jogo->matriz[c->y][c->x]->unit->movimento;
}

int movimentoPonto( Jogo* jogo, Ponto* p)
{
    return jogo->matriz[p->y][p->x]->unit->movimento;
}

TipoMovimento tipoMovimentoPonto(Jogo* jogo, Ponto* p)
{
    return jogo->matriz[p->y][p->x]->unit->tipoMov;
}

TipoMovimento tipoMovimentoCursor(Jogo* jogo, Cursor* c)
{
    return jogo->matriz[c->y][c->x]->unit->tipoMov;
}

Ponto pontoProximo(Ponto p, Direcao d)
{
    if( d == UP) p.y --;
    else if( d == DOWN) p.y++;
    else if( d == LEFT ) p.x--;
    else if( d == RIGHT ) p.x++;
    return p;
}

int custoPassoCursor(Jogo* jogo, Cursor* c, TipoMovimento movimento )
{
    return jogo -> custoMovimento[ movimento  ][ terrenoCursor(jogo,c)  ];
    //bruxaria que pega o quanto custa essa casa em passos.
}

int custoPassoPonto(Jogo* jogo, Ponto* p, TipoMovimento movimento )
{
    if( pontoFora(*p) ) return 99;
    return jogo -> custoMovimento[ movimento  ][ terrenoPonto(jogo,p)  ];
    //bruxaria que pega o quanto custa essa casa em passos.
}

void auxRange (Jogo* jogo, int dist, TipoMovimento unit, int i, int j, int jogador)
{
    int custo;

    if (jogo->range[i][j] == 0) jogo->range[i][j] = 6; //"if" para não sobreescrever o caminho de Djeefther

    if (dist > 0)
    {
        if (i > 0 && (!jogo->matriz[i-1][j]->unit || jogo->matriz[i-1][j]->unit->jogador == jogador))
        {
            custo = jogo->custoMovimento[unit][numTerreno(jogo->matriz[i-1][j]->terrain->tipo)];
            if (custo <= dist)
            {
                auxRange (jogo, dist - custo, unit, i-1, j, jogador);
            }
        }
        if (i < 24 && (!jogo->matriz[i+1][j]->unit || jogo->matriz[i+1][j]->unit->jogador == jogador))
        {
            custo = jogo->custoMovimento[unit][numTerreno(jogo->matriz[i+1][j]->terrain->tipo)];
            if (custo <= dist)
            {
                auxRange (jogo, dist - custo, unit, i+1, j, jogador);
            }
        }
        if (j > 0 && (!jogo->matriz[i][j-1]->unit || jogo->matriz[i][j-1]->unit->jogador == jogador))
        {
            custo = jogo->custoMovimento[unit][numTerreno(jogo->matriz[i][j-1]->terrain->tipo)];
            if (custo <= dist)
            {
                auxRange (jogo, dist - custo, unit, i, j-1, jogador);
            }
        }
        if (j < 29 && (!jogo->matriz[i][j+1]->unit || jogo->matriz[i][j+1]->unit->jogador == jogador))
        {
            custo = jogo->custoMovimento[unit][numTerreno(jogo->matriz[i][j+1]->terrain->tipo)];
            if (custo <= dist)
            {
                auxRange (jogo, dist - custo, unit, i, j+1, jogador);
            }
        }
    }
}

void Range (Jogo* jogo, int x, int y)
{
    int i,j;
    for (i = 0; i < 25; i++)
    {
        for (j = 0; j < 30; j++)
        {
            jogo->range[i][j] = 0;
        }
    }
    if (x != -1)
    {
        Unidade* unit = jogo->matriz[y][x]->unit;
        if (unit) auxRange (jogo, unit->movimento,unit->tipoMov,y,x,unit->jogador);
    }
}

void auxRangeAtk (int range[][30], int x, int y, int i, int j, int cont, Unidade* unit)
{
    if (cont >= unit->range && cont <= unit->rangeMax) range[i][j] = 7;

    if (cont < unit->rangeMax){
        if (i >  0 && i <= y) auxRangeAtk (range, x, y, i-1, j, cont+1, unit);
        if (i < 24 && i >= y) auxRangeAtk (range, x, y, i+1, j, cont+1, unit);
        if (j >  0 && j <= x) auxRangeAtk (range, x, y, i, j-1, cont+1, unit);
        if (j < 29 && j >= x) auxRangeAtk (range, x, y, i, j+1, cont+1, unit);
    }
}

void RangeAtk (Jogo* jogo, int x, int y)
{
    int i,j;
    int range[25][30] = {};
    Unidade* unit = jogo->matriz[y][x]->unit;

    range[y][x] = 8; //determina que o RangeAtk para unidades de longo alcance tá ativado

    if (unit->range){
        if (unit->rangeMax){
            auxRangeAtk (range, x, y, y, x, 0, unit);
        }
        else{
            Range(jogo,x,y);
            for (i = 0; i < 25; i++){
                for (j = 0; j < 30; j++){
                    if (jogo->range[i][j]){
                        if (i >  0) range[i-1][j] = 7;
                        if (i < 24) range[i+1][j] = 7;
                        if (j >  0) range[i][j-1] = 7;
                        if (j < 29) range[i][j+1] = 7;
                    }
                }
            }
        }
    }


    for (i = 0; i < 25; i++){
        for (j = 0; j < 30; j++){
            jogo->range[i][j] = range[i][j];
        }
    }
}

void printaCursor(Jogo* jogo)
{
    if (timeCursor < 4) jogo->cursor->dir = HERE;

    if (jogo->cursor->dir == HERE)
    {
        draw_sprite(jogo->buffer, jogo->cursor->imagem, 12 + jogo->cursor->x*16 , 13 + jogo->cursor->y*16);
    }
    else if (jogo->cursor->dir == UP)
    {
        draw_sprite(jogo->buffer, jogo->cursor->imagem, 12 + jogo->cursor->x*16 , 13 + jogo->cursor->y*16 + (timeCursor-4)*2);
    }
    else if (jogo->cursor->dir == DOWN)
    {
        draw_sprite(jogo->buffer, jogo->cursor->imagem, 12 + jogo->cursor->x*16 , 13 + jogo->cursor->y*16 - (timeCursor-4)*2);
    }
    else if (jogo->cursor->dir == LEFT)
    {
        draw_sprite(jogo->buffer, jogo->cursor->imagem, 12 + jogo->cursor->x*16 + (timeCursor-4)*2, 13 + jogo->cursor->y*16);
    }
    else //RIGHT
    {
        draw_sprite(jogo->buffer, jogo->cursor->imagem, 12 + jogo->cursor->x*16 - (timeCursor-4)*2, 13 + jogo->cursor->y*16);
    }
}

int checkMovCursor()
{
    if(key[KEY_RIGHT] || key[KEY_LEFT] || key[KEY_UP] || key[KEY_DOWN] || key[KEY_Z] || key[KEY_X])
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

void moveUnit(Jogo* jogo)
{
    int i,j;
    Unidade* unit = jogo->matriz[jogo->selecionado->y][jogo->selecionado->x]->unit;

    if (jogo->range[jogo->cursor->y][jogo->cursor->x] == HERE){
        timeUnit = 0;
        for (i = 0; i < 25; i++){
            for (j = 0; j < 30; j++){
                jogo->range[i][j] = 0;
            }
        }
        jogo->matriz[jogo->cursor->y][jogo->cursor->x]->unit = unit;
        jogo->matriz[jogo->selecionado->y][jogo->selecionado->x]->unit = NULL;
        jogo->selecionado->x = -1;
        jogo->selecionado->y = -1;
        unit->jogou = 1;
    }
    else{
        Unidade* unit = jogo->matriz[jogo->selecionado->y][jogo->selecionado->x]->unit;
        BITMAP* blitAux = create_bitmap(16,16);
        int aux = (int)unit->tipo;
        if (unit->tipo > 22) aux -= 6;
        else if (unit->tipo > 18) aux -= 5;
        else if (unit->tipo > 9) aux -= 4;
        else if (unit->tipo > 7) aux -= 3;
        else if (unit->tipo > 5) aux -= 2;
        else if (unit->tipo > 2) aux -= 1;

        int frames = 2;
        switch (unit->tipo)
        {
            case INFANTRY:
            case RECON:
            case TANK:
            case MEDIUM_TANK:
            case NEO_TANK:
            case APC:
            case ARTILLERY:
            case ROCKETS:
            case A_AIR:
            case FIGHTER:
            case B_COPTER:
            case T_COPTER:
            case SUBMARINE:
                frames++;
            default: ///default para não dar Warning
                break;
        }
        i = jogo->cursor->y;
        j = jogo->cursor->x;

        blit(jogo->imgs->unitsON , blitAux, 48*jogo->cor[unit->jogador-1] + 16*(timeLuz % frames), aux*16, 0, 0, 16, 16);

        if (jogo->range[jogo->cursor->y][jogo->cursor->x] == UP){
            if (jogo->inverterSprite) draw_sprite_h_flip(jogo->buffer,blitAux, (j+1)*16, (i)*16 +(2*(timeUnit-1)));
            else draw_sprite(jogo->buffer,blitAux, (j+1)*16, (i)*16 +(2*(timeUnit-1)));
        }
        else if (jogo->range[jogo->cursor->y][jogo->cursor->x] == DOWN){
            if (jogo->inverterSprite) draw_sprite_h_flip(jogo->buffer,blitAux, (j+1)*16, (i+2)*16 -(2*(timeUnit-1)));
            else draw_sprite(jogo->buffer,blitAux, (j+1)*16, (i+2)*16 -(2*(timeUnit-1)));
        }
        else if (jogo->range[jogo->cursor->y][jogo->cursor->x] == LEFT){
            draw_sprite_h_flip(jogo->buffer,blitAux, (j)*16+(2*(timeUnit-1)), (i+1)*16);
            jogo->inverterSprite = 1;
        }
        else{
            draw_sprite(jogo->buffer,blitAux, (j+2)*16-(2*(timeUnit-1)), (i+1)*16);
            jogo->inverterSprite = 0;
        }


        if (timeUnit <= 2){
            timeUnit = 9;
            unit->fuel--;
            if (jogo->range[jogo->cursor->y][jogo->cursor->x] == UP) jogo->cursor->y--;
            else if (jogo->range[jogo->cursor->y][jogo->cursor->x] == DOWN) jogo->cursor->y++;
            else if (jogo->range[jogo->cursor->y][jogo->cursor->x] == LEFT) jogo->cursor->x--;
            else jogo->cursor->x++;
        }

        destroy_bitmap(blitAux);
    }
}

void moveCursor(Jogo* jogo)
{
    Unidade* unit;
    Terreno* ter;
    Cursor* cursor = jogo->cursor;
    int auxH = !key[KEY_LEFT] - !key[KEY_RIGHT]; ///Trava o cursor se clicar direções opostas!
    int auxV = !key[KEY_DOWN] - !key[KEY_UP];
    if (jogo->selecionado->x != -1) unit = jogo->matriz[jogo->selecionado->y][jogo->selecionado->x]->unit;

    if (timeCursor == 0)
    {
        if (auxH != 0 && auxV != 0)//faz diagonal variando Vert e Hor
        {
            if((cursor->x + cursor->y)%2 == 0){
                if (!(cursor->y == 0 && auxV == 1) && !(cursor->y == 24 && auxV == -1)) auxH = 0;
            }
            else if (!(cursor->x == 0 && auxH == -1) && !(cursor->x == 29 && auxH == 1)) auxV = 0;
        }

        if (auxH == 1 && cursor->x < 29)
        {
            cursor->x++;
            cursor->dir = RIGHT;
            timeCursor = 11;
        }
        else if (auxH == -1 && cursor->x > 0)
        {
            cursor->x--;
            cursor->dir = LEFT;
            timeCursor = 11;
        }
        else if (auxV == -1 && cursor->y < 24)
        {
            cursor->y++;
            cursor->dir = DOWN;
            timeCursor = 11;
        }
        else if (auxV == 1 && cursor->y > 0)
        {
            cursor->y--;
            cursor->dir = UP;
            timeCursor = 11;
        }
        else if (key[KEY_Z])
        {
            if (jogo->matriz[cursor->y][cursor->x]->unit)
            {
                jogo->selecionado->x = cursor->x;
                jogo->selecionado->y = cursor->y;
                Range (jogo, cursor->x, cursor->y);
                timeCursor = 10;
            }
            else if (jogo->range[cursor->y][cursor->x] == HERE && !unit->jogou && (unit->jogador-1) == jogo->dia%2)
            {
                cursor->x = jogo->selecionado->x;
                cursor->y = jogo->selecionado->y;
                jogo->inverterSprite = unit->jogador - 1;
                timeUnit = 8;
            }
            else if (jogo->range[cursor->y][cursor->x] != 0 && jogo->range[cursor->y][cursor->x] < 7)
            {
                montacaminho(jogo);
                timeCursor = 15;
            }
            else if (!jogo->matriz[cursor->y][cursor->x]->unit)
            {
                if (jogo->selecionado->x == -1){
                    ter = jogo->matriz[cursor->y][cursor->x]->terrain;
                    if (ter->propriedade == jogo->jogadores->numJogador){
                        if (ter->tipo == 'B'){
                            jogo->UnidadeLoja = 0;
                            timeCursor = 8;
                        }
                        else if (ter->tipo == 'A'){
                            jogo->UnidadeLoja = 15;
                            timeCursor = 8;
                        }
                        else if (ter->tipo == 'P'){
                            jogo->UnidadeLoja = 20;
                            timeCursor = 8;
                        }
                    }
                }
                else{
                    jogo->selecionado->x = -1;
                    Range(jogo,-1,-1);
                }
            }
        }
        else if (key[KEY_X])
        {
            if (jogo->selecionado->x != -1){
                if (jogo->range[jogo->selecionado->y][jogo->selecionado->x] >= 6){
                    jogo->selecionado->x = -1;
                    Range(jogo,-1,-1);
                    timeCursor = 15;
                }
                else{
                    Range(jogo,jogo->selecionado->x,jogo->selecionado->y);
                    timeCursor = 15;
                }
            }
            else{
                if (jogo->matriz[cursor->y][cursor->x]->unit){
                    RangeAtk(jogo,cursor->x,cursor->y);
                    jogo->selecionado->x = cursor->x;
                    jogo->selecionado->y = cursor->y;
                    timeCursor = 15;
                }
            }
        }

    }
}

void imprimeUnidades(Jogo* jogo)
{
    BITMAP* range = load_bitmap("sprites/range.bmp",NULL);
    BITMAP* rangeAtk = load_bitmap("sprites/rangeAtk.bmp",NULL);
    BITMAP* seta = load_bitmap("sprites/seta.bmp",NULL);
    BITMAP* blitAux = create_bitmap(16,16);
    int i,j, frames, aux;
    Unidade* unit;

    if (!timeUnit){
        for (i = 0; i < 25; i++) //imprime range
        {
            for (j = 0; j < 30; j++)
            {
                if (jogo->range[i][j] != 0 && jogo->range[i][j] < 7)
                {
                    blit(range,blitAux,0,16*(timeLuz%3),0,0,16,16);
                    draw_trans_sprite(jogo->buffer,blitAux, (j + 1) * 16, (i + 1) * 16);
                }
                else if (jogo->range[i][j] == 7)
                {
                    blit(rangeAtk,jogo->buffer,0,16*(timeLuz%3),(j + 1) * 16, (i + 1) * 16,16,16);
                }
            }
        }
        destroy_bitmap(range);
        destroy_bitmap(rangeAtk);

        for (i = 0; i < 25; i++) //imprime Seta
        {
            for (j = 0; j < 30; j++)
            {
                aux = jogo->range[i][j];

                if (aux == UP){
                    masked_blit(seta,jogo->buffer,0,0,(j+1)*16,(i+1)*16,16,16);
                    masked_blit(seta,jogo->buffer,0,16,(j+1)*16,(i)*16,16,16);
                }
                else if (aux == DOWN){
                    masked_blit(seta,jogo->buffer,0,16,(j+1)*16,(i+1)*16,16,16);
                    masked_blit(seta,jogo->buffer,0,0,(j+1)*16,(i+2)*16,16,16);
                }
                else if (aux == LEFT){
                    masked_blit(seta,jogo->buffer,0,32,(j+1)*16,(i+1)*16,16,16);
                    masked_blit(seta,jogo->buffer,0,48,(j)*16,(i+1)*16,16,16);
                }
                else if (aux == RIGHT){
                    masked_blit(seta,jogo->buffer,0,48,(j+1)*16,(i+1)*16,16,16);
                    masked_blit(seta,jogo->buffer,0,32,(j+2)*16,(i+1)*16,16,16);
                }
                else if (aux == HERE){
                    if (i > 0 && jogo->range[i-1][j] == DOWN){
                        masked_blit(seta,jogo->buffer,0,72,(j+1)*16,8 + (i+1)*16,16,8);
                    }
                    else if (i < 24 && jogo->range[i+1][j] == UP){
                        masked_blit(seta,jogo->buffer,0,64,(j+1)*16,(i+1)*16,16,8);
                    }
                    else if (j > 0 && jogo->range[i][j-1] == RIGHT){
                        masked_blit(seta,jogo->buffer,8,64,8 + (j+1)*16,(i+1)*16,8,16);
                    }
                    else{
                        masked_blit(seta,jogo->buffer,0,64,(j+1)*16,(i+1)*16,8,16);
                    }
                }
            }
        }
        destroy_bitmap(seta);
    }
    for (i = 0; i < 25; i++)
    {
        for (j = 0; j < 30; j++)
        {
            unit = jogo->matriz[i][j]->unit;
            if (unit && !(timeUnit && i == jogo->selecionado->y && j == jogo->selecionado->x))
            {
                clear_to_color (blitAux,makecol(255,0,255));

                aux = (int)unit->tipo;
                if (unit->tipo > 22) aux -= 6;
                else if (unit->tipo > 18) aux -= 5;
                else if (unit->tipo > 9) aux -= 4;
                else if (unit->tipo > 7) aux -= 3;
                else if (unit->tipo > 5) aux -= 2;
                else if (unit->tipo > 2) aux -= 1;

                frames = 2;
                switch (unit->tipo)
                {
                case INFANTRY:
                case RECON:
                case TANK:
                case MEDIUM_TANK:
                case NEO_TANK:
                case APC:
                case ARTILLERY:
                case ROCKETS:
                case A_AIR:
                case FIGHTER:
                case B_COPTER:
                case T_COPTER:
                case SUBMARINE:
                    frames++;
                default: ///default para não dar Warning
                    break;
                }
                if (unit->jogou) blit(jogo->imgs->unitsOFF , blitAux, 48*jogo->cor[unit->jogador-1] + 16*(timeLuz % frames), aux*16, 0, 0, 16, 16);
                else blit(jogo->imgs->unitsON , blitAux, 48*jogo->cor[unit->jogador-1] + 16*(timeLuz % frames), aux*16, 0, 0, 16, 16);

                if (unit->jogador == 1) draw_sprite (jogo->buffer, blitAux, (j+1) * 16, (i + 1) * 16);
                else  draw_sprite_h_flip (jogo->buffer, blitAux, (j+1) * 16, (i + 1) * 16); //o jogador 2 é invertido

            }
        }
    }
    if (timeUnit){
        moveUnit(jogo);
    }

    destroy_bitmap(blitAux);
}

void printaTexto(BITMAP* destino, int x, int y, const char* texto)
{
    BITMAP* fonte = load_bitmap("sprites/alfabeto (30x27).bmp",NULL);
    BITMAP* numero = load_bitmap("sprites/numero (22x27).bmp",NULL);
    BITMAP* blitAux = create_bitmap(30,27);

    int n,pos;
    char atual = '.';

    for (n = 0,pos = 0; atual != '\0'; n++)
    {
        clear_to_color(blitAux,makecol(255,0,255));
        atual = texto[n];

        if (atual >= 'a' && atual <= 'z')
        {
            blit(fonte,blitAux,0,27*(atual - 'a'),0,0,30,27);
            draw_sprite(destino,blitAux,x+pos,y);
            pos += 27;
        }
        else if (atual >= 'A' && atual <= 'Z')
        {
            blit(fonte,blitAux,0,27*(atual - 'A'),0,0,30,27);
            draw_sprite(destino,blitAux,x+pos,y);
            pos += 27;
        }
        else if (atual >= '0' && atual <= '9')
        {
            blit(numero,blitAux,0,27*(atual - '0'),0,0,22,27);
            draw_sprite(destino,blitAux,x+pos,y);
            pos += 22;
        }
        else if (atual == '-')
        {
            blit(fonte,blitAux,0,27*26,0,0,30,27);
            draw_sprite(destino,blitAux,x+pos,y);
            pos += 22;
        }
        else if (atual == ' ')
            pos += 18;
    }

    destroy_bitmap(blitAux);
    destroy_bitmap(fonte);
    destroy_bitmap(numero);
}

void imprimirLargeInfo_Alvos(Jogo* jogo, int a1, int a2, int b1, int b2){
    int i,j = 0;

    for (i = 0;i < 6;i++){
        if (a1%2){
            masked_blit(jogo->imgs->alvo,jogo->buffer,50*(a2%2),62*i,570 + 52*j,236,50,62);
            j++;
            a2 /= 2;
        }
        a1 /= 2;
    }
    j = 0;
    for (i = 0;i < 6;i++){
        if (b1%2){
            masked_blit(jogo->imgs->alvo,jogo->buffer,50*(b2%2),62*i,570 + 52*j,332,50,62);
            j++;
            b2 /= 2;
        }
        b1 /= 2;
    }
}

void imprimirInfo_Alvos(Jogo* jogo, TipoUnidade unit){
    BITMAP* NameAtk = load_bitmap("sprites/NameAtk (102x22).bmp",NULL);
    int A1 = 0, A2 = 0;
    if (unit == INFANTRY){
        imprimirLargeInfo_Alvos(jogo, 0,0, 19, 6);
        A2 = 1;
    }
    else if (unit == MECH){
        imprimirLargeInfo_Alvos(jogo, 2,0, 19, 6);
        A1 = 2;
        A2 = 1;
    }
    else if (unit == RECON){
        imprimirLargeInfo_Alvos(jogo, 0,0, 19, 6);
        A2 = 1;
    }
    else if (unit == TANK){
        imprimirLargeInfo_Alvos(jogo, 14,6, 19, 6);
        A1 = 3;
        A2 = 1;
    }
    else if (unit == MEDIUM_TANK){
        imprimirLargeInfo_Alvos(jogo, 14,6, 19, 6);
        A1 = 3;
        A2 = 1;
    }
    else if (unit == NEO_TANK){
        imprimirLargeInfo_Alvos(jogo, 14,6, 19, 6);
        A1 = 4;
        A2 = 1;
    }
    else if (unit == ARTILLERY){
        imprimirLargeInfo_Alvos(jogo, 15,12, 0, 0);
        A1 = 3;
    }
    else if (unit == ROCKETS){
        imprimirLargeInfo_Alvos(jogo, 15,0, 0, 0);
        A1 = 5;
    }
    else if (unit == A_AIR){
        imprimirLargeInfo_Alvos(jogo, 51,2, 0, 0);
        A1 = 6;
    }
    else if (unit == MISSILES){
        imprimirLargeInfo_Alvos(jogo, 48,0, 0, 0);
        A1 = 7;
    }
    else if (unit == FIGHTER){
        imprimirLargeInfo_Alvos(jogo, 48,0, 0, 0);
        A1 = 7;
    }
    else if (unit == BOMBER){
        imprimirLargeInfo_Alvos(jogo, 15,0, 0, 0);
        A1 = 8;
    }
    else if (unit == B_COPTER){
        imprimirLargeInfo_Alvos(jogo, 14,0, 19, 2);
        A1 = 7;
        A2 = 1;
    }
    else if (unit == B_SHIP){
        imprimirLargeInfo_Alvos(jogo, 15,0, 0, 0);
        A1 = 3;
    }
    else if (unit == CRUISER){
        imprimirLargeInfo_Alvos(jogo, 8,0, 48, 0);
        A1 = 7;
        A2 = 9;
    }
    else if (unit == SUBMARINE){
        imprimirLargeInfo_Alvos(jogo, 12,0, 0, 0);
        A1 = 10;
    }

    masked_blit(NameAtk,jogo->buffer,0,A1*22,576,212,102,22);
    masked_blit(NameAtk,jogo->buffer,0,A2*22,576,306,102,22);

    destroy_bitmap(NameAtk);
}

void littleNumbers(BITMAP* dest, int a, int x, int y){
    BITMAP* num = load_bitmap("sprites/numero (12x10).bmp",NULL);

    if (a > 10){
        masked_blit(num,dest,0,(a/10)*10,x,y,12,10);
        masked_blit(num,dest,0,(a%10)*10,x+13,y,12,10);
    }
    else masked_blit(num,dest,0,a*10,x+8,y,12,10);

    destroy_bitmap(num);
}

void bigNumbers(BITMAP* dest, int a, int x, int y){
    BITMAP* num = load_bitmap("sprites/numero (12x20).bmp",NULL);

    masked_blit(num,dest,0,(a%10)*20,x,y,12,20);

    destroy_bitmap(num);
}

void whiteNumbers(BITMAP* dest, int a, int x, int y){
    BITMAP* num = load_bitmap("sprites/numero (16x16).bmp",NULL);
    int i;
    if (a == 0){
        masked_blit(num,dest,0,0,x - 14,y,16,16);
    }
    else{
        for (i = 0; a > 0; i++){
            masked_blit(num,dest,0,(a%10)*16,x - (i+1)*14,y,16,16);
            a /= 10;
        }
    }
    destroy_bitmap(num);
}

void imprimirLargeInfo_Unit (Jogo* jogo, Unidade* unit, short int espacoPreco)
{
    blit(jogo->imgs->UnitHUD,jogo->buffer,0,400*jogo->cor[jogo->dia%2],512,16,272,400);
    BITMAP* move = load_bitmap("sprites/tipo movimento (64x30).bmp",NULL);
    BITMAP* fotoAux = create_bitmap(70,63);
    clear_to_color(fotoAux,makecol(255,0,255));

    int tipo = unit->tipo;
    if (unit->tipo > 22) tipo -= 6;
    else if (unit->tipo > 18) tipo -= 5;
    else if (unit->tipo > 9) tipo -= 4;
    else if (unit->tipo > 7) tipo -= 3;
    else if (unit->tipo > 5) tipo -= 2;
    else if (unit->tipo > 2) tipo -= 1;

    blit(jogo->imgs->fotoUnit,fotoAux,70*jogo->cor[unit->jogador - 1],63*tipo,0,0,70,63);
    stretch_sprite(jogo->buffer,fotoAux,555,80,140,126);

    imprimirInfo_Alvos(jogo, unit->tipo);

    //tipo de movimento:
    if (unit->tipo == SUBMARINE) masked_blit(move,jogo->buffer,0,180,676,120,64,30);
    else if (unit->tipoMov == TRA) masked_blit(move,jogo->buffer,0,150,676,120,64,30);
    else masked_blit(move,jogo->buffer,0,unit->tipoMov*30,676,120,64,30);
    bigNumbers(jogo->buffer, unit->movimento, 754, 124);

    //fuel
    littleNumbers(jogo->buffer, unit->fuel, 724, 176);
    littleNumbers(jogo->buffer, unit->maxFuel, 748, 192);

    //Range
    if (unit->rangeMax){
        BITMAP* ranged = load_bitmap("sprites/rangeTiro.bmp",NULL);

        littleNumbers(ranged, unit->range, -6, 20);
        littleNumbers(ranged, unit->rangeMax, 26, 20);
        draw_sprite(jogo->buffer,ranged,720,204);
        destroy_bitmap(ranged);
    }

    //Ammo
    if(unit->maxMunicao){
        BITMAP* barra = load_bitmap("sprites/barra.bmp",NULL);

        littleNumbers(barra, unit->municao, 0, 0);
        littleNumbers(barra, unit->maxMunicao, 22, 16);
        draw_sprite(jogo->buffer,barra,672,206);

        destroy_bitmap(barra);
    }

    destroy_bitmap(move);
    destroy_bitmap(fotoAux);

    /**   PEGA O NOME:   */
    if(unit->tipo == INFANTRY) printaTexto(jogo->buffer,555,50 - espacoPreco,"Infantry");
    else if(unit->tipo == MECH) printaTexto(jogo->buffer,555,50 - espacoPreco,"Mech");
    else if(unit->tipo == RECON) printaTexto(jogo->buffer,555,50 - espacoPreco,"Recon");
    else if(unit->tipo == TANK) printaTexto(jogo->buffer,555,50 - espacoPreco,"Tank");
    else if(unit->tipo == MEDIUM_TANK) printaTexto(jogo->buffer,555,50 - espacoPreco,"Md Tank");
    else if(unit->tipo == NEO_TANK) printaTexto(jogo->buffer,555,50 - espacoPreco,"NeoTank");
    else if(unit->tipo == APC) printaTexto(jogo->buffer,555,50,"APC");
    else if(unit->tipo == ARTILLERY) printaTexto(jogo->buffer,555,50 - espacoPreco,"Artlry");
    else if(unit->tipo == ROCKETS) printaTexto(jogo->buffer,555,50 - espacoPreco,"Rockets");
    else if(unit->tipo == A_AIR) printaTexto(jogo->buffer,555,50 - espacoPreco,"A-Air");
    else if(unit->tipo == MISSILES) printaTexto(jogo->buffer,555,50 - espacoPreco,"Missiles");
    else if(unit->tipo == FIGHTER) printaTexto(jogo->buffer,555,50 - espacoPreco,"Fighter");
    else if(unit->tipo == BOMBER) printaTexto(jogo->buffer,555,50 - espacoPreco,"Bomber");
    else if(unit->tipo == B_COPTER) printaTexto(jogo->buffer,555,50 - espacoPreco,"B Copter");
    else if(unit->tipo == T_COPTER) printaTexto(jogo->buffer,555,50 - espacoPreco,"T Copter");
    else if(unit->tipo == B_SHIP) printaTexto(jogo->buffer,555,50 - espacoPreco,"B Ship");
    else if(unit->tipo == CRUISER) printaTexto(jogo->buffer,555,50 - espacoPreco,"Cruiser");
    else if(unit->tipo == LANDER) printaTexto(jogo->buffer,555,50 - espacoPreco,"Lander");
    else printaTexto(jogo->buffer,555,50 - espacoPreco,"Sub");
}

void imprimirDadosUnidades(Jogo* jogo, Unidade* unit)
{
    BITMAP* info = load_bitmap("sprites/infoUnit (24x24).bmp",NULL);
    BITMAP* blitInfo = create_bitmap(24,24);
    char texto[3];

    /** Imprime o nome na barra de baixo */
    if (unit->tipo == INFANTRY) printaTexto(jogo->buffer,30,444,"Inftry");
    else if (unit->tipo == MECH) printaTexto(jogo->buffer,30,444,"Mech");
    else if (unit->tipo == RECON) printaTexto(jogo->buffer,30,444,"Recon");
    else if (unit->tipo == TANK) printaTexto(jogo->buffer,30,444,"Tank");
    else if (unit->tipo == MEDIUM_TANK) printaTexto(jogo->buffer,30,444,"Md Tank");
    else if (unit->tipo == NEO_TANK) printaTexto(jogo->buffer,30,444,"Neo");
    else if (unit->tipo == APC) printaTexto(jogo->buffer,30,444,"APC");
    else if (unit->tipo == ARTILLERY) printaTexto(jogo->buffer,30,444,"Artly");
    else if (unit->tipo == ROCKETS) printaTexto(jogo->buffer,30,444,"Rckts");
    else if (unit->tipo == A_AIR) printaTexto(jogo->buffer,30,444,"A-Air");
    else if (unit->tipo == MISSILES) printaTexto(jogo->buffer,30,444,"Mssls");
    else if (unit->tipo == FIGHTER) printaTexto(jogo->buffer,30,444,"Fghtr");
    else if (unit->tipo == BOMBER) printaTexto(jogo->buffer,30,444,"Bmbr");
    else if (unit->tipo == B_COPTER) printaTexto(jogo->buffer,30,444,"B Cptr");
    else if (unit->tipo == T_COPTER) printaTexto(jogo->buffer,30,444,"T Cptr");
    else if (unit->tipo == B_SHIP) printaTexto(jogo->buffer,30,444,"B Shp");
    else if (unit->tipo == CRUISER) printaTexto(jogo->buffer,30,444,"Crsr");
    else if (unit->tipo == LANDER) printaTexto(jogo->buffer,30,444,"Lndr");
    else printaTexto(jogo->buffer,30,444,"Sub");

    ///HP
    sprintf(texto,"%d",(unit->HP + 9)/10); /*Imprimindo o resto dos dados */
    printaTexto(jogo->buffer,55,478,texto);

    clear_to_color(blitInfo,makecol(255,0,255));
    blit(info,blitInfo,0,0,0,0,24,24);
    draw_sprite (jogo->buffer, blitInfo, 32,478);
    ///FUEL
    sprintf(texto,"%d",unit->fuel);
    printaTexto(jogo->buffer,55,512,texto);

    clear_to_color(blitInfo,makecol(255,0,255));
    blit(info,blitInfo,0, 24 + 72*(!unit->fuel), 0,0, 24,24);
    draw_sprite (jogo->buffer, blitInfo, 32,512);

    ///AMMO
    if(unit->maxMunicao)
    {
        sprintf(texto,"%d",unit->municao);
        printaTexto(jogo->buffer,55,546,texto);

        clear_to_color(blitInfo,makecol(255,0,255));
        blit(info,blitInfo,0, 48 + 24*(!unit->municao), 0,0, 24,24);
        draw_sprite (jogo->buffer, blitInfo, 32,546);
    }

    imprimirLargeInfo_Unit (jogo, unit,0);

    destroy_bitmap(blitInfo);
    destroy_bitmap(info);
}

int isWater(Terreno* terrain)
{
    if (terrain->tipo == 'S' || terrain->tipo == 'R' || terrain->tipo == 'O' || terrain->tipo == 'F') return 1;
    else return 0;
}

void imprimirLargeInfo_Ter (Jogo* jogo, TipoTerreno ter,int player)
{
    blit(jogo->imgs->TerHUD,jogo->buffer,0,400*jogo->cor[jogo->dia%2],512,16,272,400);
    int aux,aux2 = 0, recoverTipo = 0;

    if (ter == HQ && jogo->cor[player-1]){ ///Escolhe a foto do HQ
        blit(jogo->imgs->fotoTer,jogo->buffer,0,128*(13 + jogo->cor[player-1]),571,106,96,128);
    }
    else blit(jogo->imgs->fotoTer,jogo->buffer,0,128*ter,571,106,96,128);

    if (ter < PLAIN)
    {
        BITMAP* auxTer = load_bitmap("sprites/LargeInfo aux (95x57).bmp",NULL);
        BITMAP* recover = load_bitmap("sprites/recoverTipo (36x28).bmp",NULL);

        if (ter == AIRPORT) recoverTipo = 1;
        if (ter == PORT) recoverTipo = 2;

        blit(auxTer,jogo->buffer,0,57*jogo->cor[jogo->dia%2],670,176,95,57);
        blit(recover,jogo->buffer,0,28*recoverTipo,732,204,36,28);

        destroy_bitmap(auxTer);
        destroy_bitmap(recover);
    }

    BITMAP* tipo = load_bitmap("sprites/tipo movimento_Ter (64x30).bmp",NULL);
    for (aux = 0;aux < 7;aux++){
        if (jogo->custoMovimento[aux][ter] != 99){
            masked_blit(tipo,jogo->buffer,0,aux*30,576+(aux2%2)*112,272 + (aux2/2)*32,64,30);
            bigNumbers(jogo->buffer,jogo->custoMovimento[aux][ter],646+(aux2%2)*112,272 + (aux2/2)*32);
            aux2++;
        }
    }
    destroy_bitmap(tipo);
}

void imprimirDadosTerreno(Jogo* jogo, int i, int j)
{
    BITMAP* info = load_bitmap("sprites/infoTer (33x36).bmp",NULL);
    BITMAP* blitInfo = create_bitmap(33,36);
    Terreno* terrain = jogo->matriz[i][j]->terrain;
    char texto[6];
    char texto2[15];
    TipoTerreno tipo = numTerreno(terrain->tipo);

    switch(terrain->tipo)
    {
    case 'C':
        sprintf(texto,"City");
        sprintf(texto2,"City");
        tipo = CITY;
        break;
    case 'H':
        sprintf(texto,"HQ");
        sprintf(texto2,"HQ");
        break;
    case 'B':
        sprintf(texto,"Base");
        sprintf(texto2,"Base");
        break;
    case 'A':
        sprintf(texto,"Arprt");
        sprintf(texto2,"Airport");
        break;
    case 'P':
        sprintf(texto,"Port");
        sprintf(texto2,"Port");
        break;
    case 'L':
        sprintf(texto,"Plain");
        sprintf(texto2,"Plain");
        break;
    case 'O':
        sprintf(texto,"Sea");
        sprintf(texto2,"Sea");
        break;
    case 'S':
        sprintf(texto,"Shoal");
        sprintf(texto2,"Shoal");
        break;
    case 'T':
        sprintf(texto,"Wood");
        sprintf(texto2,"Wood");
        break;
    case 'F':
        sprintf(texto,"Reef");
        sprintf(texto2,"Reef");
        break;
    case 'M':
        sprintf(texto,"Mtn");
        sprintf(texto2,"Mt");
        break;
    case 'R':
        sprintf(texto,"River");
        sprintf(texto2,"River");
        break;
    case 'E':
        if ((i > 0 && i < 24 && isWater(jogo->matriz[i-1][j]->terrain) && isWater(jogo->matriz[i+1][j]->terrain))||(j > 0 && j < 29 && isWater(jogo->matriz[i][j-1]->terrain) && isWater(jogo->matriz[i][j+1]->terrain)))
        {
            sprintf(texto,"Brdg");
            sprintf(texto2,"Bridge");
            tipo = BRIDGE;
        }
        else
        {
            sprintf(texto,"Road");
            sprintf(texto2,"Road");
        }
        break;
    default:
        break;
    }

    printaTexto(jogo->buffer,278,444,texto); /* Passando a string pra função de graci */

    ///DEFENCE
    sprintf(texto,"%d",terrain->defesa); /*Imprimindo o resto dos dados */
    printaTexto(jogo->buffer,313,478,texto);

    clear_to_color(blitInfo,makecol(255,0,255));
    blit(info,blitInfo,0,0,0,0,33,36);
    draw_sprite (jogo->buffer, blitInfo, 278,470);

    ///CAPTURE
    if (terrain->propriedade)
    {
        sprintf(texto,"%d",terrain->capture);
        printaTexto(jogo->buffer,313,520,texto);

        clear_to_color(blitInfo,makecol(255,0,255));
        blit(info,blitInfo,0,36, 0,0, 33,36);
        draw_sprite (jogo->buffer, blitInfo, 278,517);
    }
    if(!jogo->matriz[i][j]->unit)
    {
        BITMAP* defesa = load_bitmap("sprites/defesa.bmp",NULL);
        int d;
        imprimirLargeInfo_Ter (jogo, tipo,jogo->matriz[i][j]->terrain->propriedade);
        printaTexto(jogo->buffer,555,50,texto2);

        for (d = 0; d < terrain->defesa; d++)
        {
            stretch_sprite(jogo->buffer,defesa,710 + 16*(d%3) + 8*(d/3),136 + 16*(d/3),16,16);
        }

        destroy_bitmap(defesa);
    }

    destroy_bitmap(blitInfo);
    destroy_bitmap(info);
}

void MostraDadosTile(Jogo* jogo)
{
    int j = jogo->cursor->x;
    int i = jogo->cursor->y;

    imprimirDadosTerreno(jogo, i,j);

    if(jogo->matriz[i][j]->unit) /* Se houver uma unidade naquele tile */
    {
        imprimirDadosUnidades(jogo, jogo->matriz[i][j]->unit);
    }
}

void imprimeHUD(Jogo* jogo)
{
    int i;
    BITMAP* num = load_bitmap("sprites/playerNum (13x19).bmp",NULL);
    BITMAP* numAux = create_bitmap(13,19);
    Jogador* player = jogo->jogadores;
    draw_sprite(jogo->buffer,jogo->imgs->hud,0,0);

    for (i = 0; i < 2; i++)
    {
        blit(num , numAux, 0, 19*i, 0,0, 13, 19);
        draw_sprite(jogo->imgs->cash,numAux,244, 24 + 68*jogo->cor[i]);
        blit(jogo->imgs->cash , jogo->buffer, 0, 68*jogo->cor[player->numJogador-1], 512,432 + 84*i, 273, 68);
        whiteNumbers(jogo->buffer, player->money, 632, 438 + 84*i);
        player = player->prox;
    }
    MostraDadosTile(jogo);
    destroy_bitmap(num);
    destroy_bitmap(numAux);
}

void imprimeMapa(Jogo* jogo)
{
    clear_to_color(jogo->buffer, makecol(255,255,255));

    imprimeHUD(jogo);
    draw_sprite(jogo->buffer, jogo->mapa, 16, 16);
    draw_sprite(jogo->buffer, jogo->constucoes[timeLuz > 1], 16, 16);
    imprimeUnidades(jogo);
    if (!timeUnit) printaCursor(jogo);
}

Unidade* NovaUnidade(TipoUnidade unitType, int jogador){
    Unidade* newUnit = (Unidade*) malloc (sizeof (Unidade));
    newUnit->jogador = jogador;
    newUnit->HP = 100;
    newUnit->tipo = unitType;
    newUnit->jogou = 1;
    newUnit->rangeMax = 0;
    newUnit->maxMunicao = 0;

    switch (unitType)
    {
    case INFANTRY:
        newUnit->municao = -1;
        newUnit->movimento = 3;
        newUnit->tipoMov = INF;
        newUnit->fuel = 99;
        newUnit->maxFuel = 99;
        newUnit->range = 1;
        break;
    case MECH:
        newUnit->municao = 3;
        newUnit->maxMunicao = 3;
        newUnit->movimento = 2;
        newUnit->tipoMov = MEC;
        newUnit->fuel = 70;
        newUnit->maxFuel = 70;
        newUnit->range = 1;
        break;
    case RECON:
        newUnit->municao = -1;
        newUnit->movimento = 8;
        newUnit->tipoMov = TYR;
        newUnit->fuel = 80;
        newUnit->maxFuel = 80;
        newUnit->range = 1;
        break;
    case TANK:
        newUnit->municao = 9;
        newUnit->maxMunicao = 9;
        newUnit->movimento = 6;
        newUnit->tipoMov = TRD;
        newUnit->fuel = 70;
        newUnit->maxFuel = 70;
        newUnit->range = 1;
        break;
    case MEDIUM_TANK:
        newUnit->municao = 8;
        newUnit->maxMunicao = 8;
        newUnit->movimento = 5;
        newUnit->tipoMov = TRD;
        newUnit->fuel = 50;
        newUnit->maxFuel = 50;
        newUnit->range = 1;
        break;
    case NEO_TANK:
        newUnit->municao = 9;
        newUnit->maxMunicao = 9;
        newUnit->movimento = 6;
        newUnit->tipoMov = TRD;
        newUnit->fuel = 99;
        newUnit->maxFuel = 99;
        newUnit->range = 1;
        break;
    case APC:
        newUnit->movimento = 6;
        newUnit->tipoMov = TRD;
        newUnit->fuel = 70;
        newUnit->maxFuel = 70;
        newUnit->transporte[0] = NULL;
        newUnit->range = 0;
        break;
    case ARTILLERY:
        newUnit->municao = 9;
        newUnit->maxMunicao = 9;
        newUnit->movimento = 5;
        newUnit->tipoMov = TRD;
        newUnit->fuel = 50;
        newUnit->maxFuel = 50;
        newUnit->range = 2;
        newUnit->rangeMax = 3;
        break;
    case ROCKETS:
        newUnit->municao = 6;
        newUnit->maxMunicao = 6;
        newUnit->movimento = 5;
        newUnit->tipoMov = TYR;
        newUnit->fuel = 50;
        newUnit->maxFuel = 50;
        newUnit->range = 3;
        newUnit->rangeMax = 5;
        break;
    case A_AIR:
        newUnit->municao = 9;
        newUnit->maxMunicao = 9;
        newUnit->movimento = 5;
        newUnit->tipoMov = TRD;
        newUnit->fuel = 60;
        newUnit->maxFuel = 60;
        newUnit->range = 1;
        break;
    case MISSILES:
        newUnit->municao = 6;
        newUnit->maxMunicao = 6;
        newUnit->movimento = 4;
        newUnit->tipoMov = TYR;
        newUnit->fuel = 50;
        newUnit->maxFuel = 50;
        newUnit->range = 3;
        newUnit->rangeMax = 5;
        break;
    case FIGHTER:
        newUnit->municao = 9;
        newUnit->maxMunicao = 9;
        newUnit->movimento = 9;
        newUnit->tipoMov = AIR;
        newUnit->fuel = 99;
        newUnit->maxFuel = 99;
        newUnit->range = 1;
        break;
    case BOMBER:
        newUnit->municao = 9;
        newUnit->maxMunicao = 9;
        newUnit->movimento = 7;
        newUnit->tipoMov = AIR;
        newUnit->fuel = 99;
        newUnit->maxFuel = 99;
        newUnit->range = 1;
        break;
    case B_COPTER:
        newUnit->municao = 6;
        newUnit->maxMunicao = 6;
        newUnit->movimento = 6;
        newUnit->tipoMov = AIR;
        newUnit->fuel = 99;
        newUnit->maxFuel = 99;
        newUnit->range = 1;
        break;
    case T_COPTER:
        newUnit->movimento = 6;
        newUnit->tipoMov = AIR;
        newUnit->fuel = 99;
        newUnit->maxFuel = 99;
        newUnit->range = 0;
        newUnit->transporte[0] = NULL;
        break;
    case B_SHIP:
        newUnit->municao = 9;
        newUnit->maxMunicao = 9;
        newUnit->movimento = 5;
        newUnit->tipoMov = WTR;
        newUnit->fuel = 99;
        newUnit->maxFuel = 99;
        newUnit->range = 2;
        newUnit->rangeMax = 6;
        break;
    case CRUISER:
        newUnit->municao = 9;
        newUnit->maxMunicao = 9;
        newUnit->movimento = 6;
        newUnit->tipoMov = WTR;
        newUnit->fuel = 99;
        newUnit->maxFuel = 99;
        newUnit->range = 1;
        newUnit->transporte[0] = NULL;
        newUnit->transporte[1] = NULL;
        break;
    case LANDER:
        newUnit->movimento = 6;
        newUnit->tipoMov = TRA;
        newUnit->fuel = 99;
        newUnit->maxFuel = 99;
        newUnit->transporte[0] = NULL;
        newUnit->transporte[1] = NULL;
        newUnit->range = 0;
        break;
    case SUBMARINE:
        newUnit->municao = 6;
        newUnit->maxMunicao = 6;
        newUnit->tipoMov = WTR;
        newUnit->movimento = 5;
        newUnit->fuel = 60;
        newUnit->maxFuel = 60;
        newUnit->range = 1;
        newUnit->dived = 0;
        break;
    default: ///casos como MECH2
        break;
    }

    return newUnit;
}

void comprarUnidade(Jogo* jogo, int x, int y, TipoUnidade unitType)
{
    Unidade* newUnit = NovaUnidade(unitType,jogo->jogadores->numJogador);
    jogo->matriz[y][x]->unit = newUnit;
    newUnit->jogou = 1;
    newUnit->prox = jogo->jogadores->unitList;
    jogo->jogadores->unitList = newUnit;
}

int tipo2(TipoUnidade tipo){ ///Diz se a unidade tá no "tipo2" ex: Mech2
    if (tipo == MECH2 || tipo == TANK2 || tipo == MEDIUM_TANK2 || tipo == NEO_TANK2|| tipo == B_COPTER2 || tipo == CRUISER2) return 1;
    else return 0;
}

void Loja(Jogo* jogo){
    int i = 0,aux,a,b,a2,b2, frames;
    BITMAP* blitAux = create_bitmap(16,16);
    BITMAP* seta = load_bitmap("sprites/setaLoja.bmp",NULL);

    Unidade* unit = NovaUnidade((TipoUnidade)(jogo->UnidadeLoja),jogo->jogadores->numJogador);
    imprimirLargeInfo_Unit (jogo, unit,15);
    free(unit);
    char texto[6];
    sprintf(texto,"%d",jogo->unitValue[jogo->UnidadeLoja]);
    printaTexto(jogo->buffer, 660 + 22*(jogo->unitValue[jogo->UnidadeLoja] < 10000), 67, texto);

    /// define o que se pode comprar
    if (jogo->matriz[jogo->cursor->y][jogo->cursor->x]->terrain->tipo == 'B'){
        a = 0;
        b = 14;
        a2 = 0;
        b2 = 10;
        aux = 0;
    }
    else if (jogo->matriz[jogo->cursor->y][jogo->cursor->x]->terrain->tipo == 'A'){
        a = 15;
        b = 19;
        a2 = 11;
        b2 = 14;
        aux = 15;
    }
    else{
        a = 20;
        b = 24;
        a2 = 15;
        b2 = 18;
        aux = 20;
    }
    /// move na loja e faz a compra
    if (timeCursor == 0){
        if (key[KEY_UP]){
            if (jogo->UnidadeLoja > a && !key[KEY_DOWN]){
                jogo->UnidadeLoja--;
                if (tipo2((TipoUnidade)jogo->UnidadeLoja)) jogo->UnidadeLoja--;
                timeCursor = 8;
            }
        }
        else if (key[KEY_DOWN] && jogo->UnidadeLoja < b){
            jogo->UnidadeLoja++;
            if (tipo2((TipoUnidade)jogo->UnidadeLoja)) jogo->UnidadeLoja++;
            timeCursor = 8;
        }
        else if (key[KEY_Z] && jogo->jogadores->money >= jogo->unitValue[jogo->UnidadeLoja]){
            comprarUnidade(jogo, jogo->cursor->x, jogo->cursor->y, (TipoUnidade)jogo->UnidadeLoja);
            jogo->jogadores->money -= jogo->unitValue[jogo->UnidadeLoja];
            jogo->UnidadeLoja = -1;
            timeCursor = 8;
        }
        else if (key[KEY_X]) jogo->UnidadeLoja = -1;
    }
    ///imprime

    for (i = a2; i <= b2; i++){
        clear_to_color (blitAux,makecol(255,0,255));

        if (i == 0 || (i >= 2 && i <= 9) || i == 11 || i == 13 || i == 14 || i == 18)  frames = 3;
        else frames = 2;
        if (jogo->jogadores->money >= jogo->unitValue[aux]) blit(jogo->imgs->unitsON , blitAux, 48 * jogo->cor[jogo->jogadores->numJogador - 1] + 16*(timeLuz % frames), i*16, 0, 0, 16, 16);
        else blit(jogo->imgs->unitsOFF , blitAux, 48 * jogo->cor[jogo->jogadores->numJogador - 1] + 16*(timeLuz % frames), i*16, 0, 0, 16, 16);
        stretch_sprite (jogo->buffer, blitAux, 525, 40 + (i-a2) * 32,32,32);
        aux++;
        if (i != b2 && tipo2((TipoUnidade)aux)) aux++;
    }
    aux = jogo->UnidadeLoja;
    if (jogo->UnidadeLoja > 22) aux -= 6;
    else if (jogo->UnidadeLoja > 18) aux -= 5;
    else if (jogo->UnidadeLoja > 9) aux -= 4;
    else if (jogo->UnidadeLoja > 7) aux -= 3;
    else if (jogo->UnidadeLoja > 5) aux -= 2;
    else if (jogo->UnidadeLoja > 2) aux -= 1;

    draw_sprite(jogo->buffer,seta,500 + timeLuz%2,45 + (aux-a2)*32);
    destroy_bitmap(blitAux);
    destroy_bitmap(seta);
}

int podeRecuperar(Tile* atual,Jogo* jogo)
{
    int retorno = 0;

    if ((atual->unit->tipo >= INFANTRY && atual->unit->tipo <= MISSILES) && (atual->terrain->tipo == 'H' || atual->terrain->tipo == 'C' || atual->terrain->tipo == 'B'))
    {
        retorno = 1;
    }
    else if ((atual->unit->tipo >= FIGHTER && atual->unit->tipo <= T_COPTER) && (atual->terrain->tipo == 'P'))
    {
        retorno = 1;
    }
    else if ((atual->unit->tipo >= B_SHIP && atual->unit->tipo <= SUBMARINE) && (atual->terrain->tipo == 'A'))
    {
        retorno = 1;
    }

    retorno = retorno && (atual->unit->jogador == jogo->jogadores->numJogador);

    return retorno;
}

int gastaCombustivel(Unidade* atual)
{
    int gasto;

    switch (atual->tipo)
    {
    case FIGHTER:
    case BOMBER:
        gasto = 5;
        break;
    case B_COPTER:
    case T_COPTER:
        gasto = 2;
        break;
    case B_SHIP:
    case CRUISER:
    case LANDER:
        gasto = 1;
        break;
    case SUBMARINE:
        if (atual->dived == 1) gasto = 5;
        else gasto = 1;
        break;
    default:
        gasto = 0;
        break;
    }

    return gasto;
}

void atualizaTerrenoEUnidades(Jogo* jogo) /* Essas coisas todas acontecem no começo do turno do jogador */
{
    int i,j;

    int recuperou;

    jogo->jogadores->money += jogo->jogadores->numPropriedades * 1000;

    for (i=0; i<25; i++) /* Percorre o terreno procurando unidades */
    {
        for (j=0; j<30; j++)
        {
            if ( jogo->matriz[i][j]->unit != NULL  ) /*Se há unidades no tile */
            {
                if (jogo->dia % 2 == 0) /* Lembrando que dois turnos fazem um dia! */
                {
                    jogo->matriz[i][j]->unit->fuel -= gastaCombustivel(jogo->matriz[i][j]->unit); /* Calcula o gasto de combustível da unidade */
                }

                if (jogo->matriz[i][j]->unit->jogador-1 == jogo->dia%2 && jogo->matriz[i][j]->terrain->propriedade == jogo->matriz[i][j]->unit->jogador && podeRecuperar(jogo->matriz[i][j], jogo)) /*Verifica se a unidade está em um tile que pertence ao jogador */
                { //só acontece no turno do jogador
                    if(jogo->matriz[i][j]->unit->HP+20 > 100) recuperou = 100 - jogo->matriz[i][j]->unit->HP;
                    else recuperou = 20;

                    jogo->matriz[i][j]->unit->HP += 20; /* Atualiza o HP */

                    if ( jogo->matriz[i][j]->unit->HP > 100)
                    {
                        jogo->matriz[i][j]->unit->HP = 100;
                    }

                    jogo->jogadores-> money -= recuperou/100 * jogo->unitValue[jogo->matriz[i][j]->unit->tipo];
                }

                jogo->matriz[i][j]->unit->jogou = 0; /*Libera a unidade pra jogar de novo */
            }
        }
    }
}

void passarTurno( Jogo* jogo )
{
    jogo->jogadores = jogo->jogadores->prox;

    jogo->dia++;

    atualizaTerrenoEUnidades(jogo);
}
