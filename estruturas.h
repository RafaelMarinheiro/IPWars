#include<stdio.h>
#include<allegro.h>

#ifndef ESTRUTURAS_H_INCLUDED
#define ESTRUTURAS_H_INCLUDED

int timeLuz;
int timeCursor;
int timeUnit;

enum tipoTerreno
{
    CITY,
    HQ,
    BASE,
    AIRPORT,
    PORT,
    PLAIN,
    OCEAN,
    SHOAL,
    FOREST,
    REEFS,
    MOUNT,
    RIVER,
    ROAD,
    BRIDGE //usado só em algumas funções para printar diferente (deve ser salvo como ROAD)
};

enum tipoMovimento
{
    INF,        //Infantaria
    MEC,        //MECH
    TRD,        //Tread
    TYR,        //Tires
    AIR,        //Air
    WTR,        //Water (boat)
    TRA         //Water (transport) - pois o barco de transporte pode andar na praia
};

enum direcao
{
    UP = 1,// @Djeefther Garantir que todos são diferentes de 0
    DOWN =2,
    LEFT =3,
    RIGHT =4,
    HERE// @Djeefther preciso para a função caminho poder desenhar corretamente o caminho.
};

enum tipoUnidade //Alguém gosta de enums? SIM (Todos)
{
    INFANTRY,
    MECH,
    MECH2,  //Somente usar no DamageChart (tiro secundário)
    RECON,
    TANK,
    TANK2,   //Somente usar no DamageChart (tiro secundário)
    MEDIUM_TANK,
    MEDIUM_TANK2,    //Somente usar no DamageChart (tiro secundário)
    NEO_TANK,
    NEO_TANK2,      // Somente usar no DamageChart (tiro secundário)
    APC,
    ARTILLERY,
    ROCKETS,
    A_AIR,
    MISSILES,
    FIGHTER,
    BOMBER,
    B_COPTER,
    B_COPTER2,      //Somente usar no DamageChart (tiro secundário)
    T_COPTER,
    B_SHIP,
    CRUISER,
    CRUISER2,       //Somente usar no DamageChart (tiro secundário)
    LANDER,
    SUBMARINE
};

typedef enum tipoUnidade TipoUnidade;
typedef enum tipoMovimento TipoMovimento;
typedef enum tipoTerreno TipoTerreno;
typedef enum direcao Direcao;

typedef struct unidade
{
	short int jogador;
	short int HP;                         //HP max == 100
	TipoUnidade tipo;
    TipoMovimento tipoMov;
	short int jogou;                // 0 == Ainda não jogou | 1 == já jogou
	short int municao;
	short int maxMunicao;
	short int movimento;
	short int fuel;
	short int maxFuel;
	short int range;                      // Caso seja 0, a unidade não pode atacar (Transporte)
	short int rangeMax;
	short int dived;                      // Exclusivo para submarinos, 1 é mergulhado, 0 é na superfície
	struct unidade* transporte [2];       // Exclusivo para transporte
	struct unidade* prox;                 // Linked List
}Unidade;

typedef struct terreno
/// @Djeefther , esses campos não poderiam ser tudo Char?
/// @Jota , pra que se preocupar com memória? '-'
/// @Djeefther porque não?
{
    char tipo;
    short int propriedade;
    short int capture;
    short int defesa;
}Terreno;

typedef struct tile
{
    Terreno* terrain;
    Unidade* unit;
}Tile;

typedef struct imgs{ //para BITMAPS grandes que são usados muito
    BITMAP* unitsON;
    BITMAP* unitsOFF;
    BITMAP* hud;
    BITMAP* UnitHUD;
    BITMAP* TerHUD;
    BITMAP* fotoUnit;
    BITMAP* fotoTer;
    BITMAP* cash;
    BITMAP* alvo;
}Imgs;

typedef struct jogador
{
    Unidade* unitList;
    int numJogador;
    Terreno* HQ;
    int numPropriedades;
    int money;
    struct jogador* prox;
}Jogador;

typedef struct cursor
{
    BITMAP* imagem;
    int x;
    int y;
    Direcao dir;
}Cursor;

typedef struct ponto /// @Djeefther
///Criado para as minhas recurssões de caminho.
///Como copio muitas estruturas localmente é bom que esta pesa apenas 4 bytes e assim é copiada apenas em um unico ciclo.
///Futuramente organizar com a outra estrutura ( CURSOR )
///Depois pensamos, não é necessário fazer a oura ser uma sub dessa, é possivel e menos verborragico manter as duas.

///@Jota Velho o cursor não tem só x e y tem outras coisas também, deixa de frescura de memória deixa assim pronto =P

///@Djeefther por isso não apaguei cursor. Para minhas funções é melhor assim, vei. Chamo 10! recursões copiando essa estrutura localmente.
///Pra não fazer isso um gargalo otimizei MUITA coisa, entre elas isso. Mas no resto pode ser cursor sim.

///@Djeefther Repessando agora, Jota, quantos cursores você usa? 1. Eu precisei de MUITOS cursores sem ponteiro pra BMP e direcao. E o próprio Jogo tem 2, e só 1 deveria ser cursor
///Logo é muito mais fácil tudo ser Ponto( ou cursor com minha definição ) e colocar aquelas 2 variaveis no escopo de Jogo. Pessoas se pronunciem.
{
    short int x;
    short int y;
} Ponto;

typedef struct caminho
{
    Ponto ponto;
    short int distancia;
    struct caminho* prox;
}Caminho;

typedef struct jogo
{
    Tile* matriz[25][30];
    char range[25][30];
    short int inverterSprite; //usado na função de movimento, para virar a unidade
    short int UnidadeLoja; // -1 se tiver fora da loja, um TipoUnidade se tiver na loja
    Imgs* imgs;
    BITMAP* mapa;
    BITMAP* constucoes[2];
    BITMAP* buffer;
    Cursor* cursor;
    Ponto* selecionado;
    int custoMovimento[7][13];
    int damageChart[25][25];
    int unitValue[25];
    Jogador* jogadores;
    char cor[2];
    int dia;
}Jogo;

#endif // ESTRUTURAS_H_INCLUDED
