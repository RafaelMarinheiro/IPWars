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
    BRIDGE //usado s� em algumas fun��es para printar diferente (deve ser salvo como ROAD)
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
    UP = 1,// @Djeefther Garantir que todos s�o diferentes de 0
    DOWN =2,
    LEFT =3,
    RIGHT =4,
    HERE// @Djeefther preciso para a fun��o caminho poder desenhar corretamente o caminho.
};

enum tipoUnidade //Algu�m gosta de enums? SIM (Todos)
{
    INFANTRY,
    MECH,
    MECH2,  //Somente usar no DamageChart (tiro secund�rio)
    RECON,
    TANK,
    TANK2,   //Somente usar no DamageChart (tiro secund�rio)
    MEDIUM_TANK,
    MEDIUM_TANK2,    //Somente usar no DamageChart (tiro secund�rio)
    NEO_TANK,
    NEO_TANK2,      // Somente usar no DamageChart (tiro secund�rio)
    APC,
    ARTILLERY,
    ROCKETS,
    A_AIR,
    MISSILES,
    FIGHTER,
    BOMBER,
    B_COPTER,
    B_COPTER2,      //Somente usar no DamageChart (tiro secund�rio)
    T_COPTER,
    B_SHIP,
    CRUISER,
    CRUISER2,       //Somente usar no DamageChart (tiro secund�rio)
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
	short int jogou;                // 0 == Ainda n�o jogou | 1 == j� jogou
	short int municao;
	short int maxMunicao;
	short int movimento;
	short int fuel;
	short int maxFuel;
	short int range;                      // Caso seja 0, a unidade n�o pode atacar (Transporte)
	short int rangeMax;
	short int dived;                      // Exclusivo para submarinos, 1 � mergulhado, 0 � na superf�cie
	struct unidade* transporte [2];       // Exclusivo para transporte
	struct unidade* prox;                 // Linked List
}Unidade;

typedef struct terreno
/// @Djeefther , esses campos n�o poderiam ser tudo Char?
/// @Jota , pra que se preocupar com mem�ria? '-'
/// @Djeefther porque n�o?
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

typedef struct imgs{ //para BITMAPS grandes que s�o usados muito
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
///Criado para as minhas recurss�es de caminho.
///Como copio muitas estruturas localmente � bom que esta pesa apenas 4 bytes e assim � copiada apenas em um unico ciclo.
///Futuramente organizar com a outra estrutura ( CURSOR )
///Depois pensamos, n�o � necess�rio fazer a oura ser uma sub dessa, � possivel e menos verborragico manter as duas.

///@Jota Velho o cursor n�o tem s� x e y tem outras coisas tamb�m, deixa de frescura de mem�ria deixa assim pronto =P

///@Djeefther por isso n�o apaguei cursor. Para minhas fun��es � melhor assim, vei. Chamo 10! recurs�es copiando essa estrutura localmente.
///Pra n�o fazer isso um gargalo otimizei MUITA coisa, entre elas isso. Mas no resto pode ser cursor sim.

///@Djeefther Repessando agora, Jota, quantos cursores voc� usa? 1. Eu precisei de MUITOS cursores sem ponteiro pra BMP e direcao. E o pr�prio Jogo tem 2, e s� 1 deveria ser cursor
///Logo � muito mais f�cil tudo ser Ponto( ou cursor com minha defini��o ) e colocar aquelas 2 variaveis no escopo de Jogo. Pessoas se pronunciem.
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
    short int inverterSprite; //usado na fun��o de movimento, para virar a unidade
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
