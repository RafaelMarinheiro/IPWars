#define NEUTRO 3

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
}

char auxContorno (char c)
{
    char retorno;
    if (c == 'S' || c == 'R' || c == 'O' || c == 'F') retorno = 'A';
    else if (c == 'E') retorno = 'E';
    else retorno = 'O';

    return retorno;
}

void mudaConstrucao(Jogo* jogo,int i, int j, int player){
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

void montaConstrucoes(Jogo* jogo){
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

    jogo->cor[0] = 0;
    jogo->cor[1] = 3;

    preencheDC(jogo);

    jogo->jogadores[0] = (Jogador*) malloc(sizeof(Jogador));
    jogo->jogadores[1] = (Jogador*) malloc(sizeof(Jogador));

    jogo->jogadores[0]->unitList = NULL;
    jogo->jogadores[1]->unitList = NULL;

    geraTerreno(jogo);
    montaConstrucoes(jogo);

    jogo->cursor = (Cursor*) malloc (sizeof(Cursor));
    jogo->cursor->x = 0;
    jogo->cursor->y = 0;

    return jogo;
}
