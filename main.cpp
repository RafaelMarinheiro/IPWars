#include <allegro.h>
#include "estruturas.h"
#include "mecanica.c"
#include "start.c"
#include <stdio.h>

inline void init()
{
	allegro_init();
	set_color_depth(32);
	set_gfx_mode(GFX_AUTODETECT_WINDOWED, 800, 600, 0, 0);

	install_timer();
	install_keyboard();
	install_mouse();
}

inline void deinit()
{
	clear_keybuf();
	allegro_exit();
}

///@Eduardo fiz essa parte para facilitar o acompanhamento do que j� foi feito
/// e do que precissa ser feito, sem precissar rodar o jogo,

///Info das ultimas mudan�as: (Podem deletar isso depois)
///@Eduardo, seta vermelha adicionada;
///@Eduardo, unidades se movendo com restri��o ao jogador (clica na ponta da seta)
///@Eduardo, aba da direita completa, com informa��es do terreno OU UNIDADE;
///@Eduardo, bot�o 'X' retrocede a sele��o;
///@Eduardo, Range de Ataque (usa o 'X');
///@Graciliano, sele��o de Personagem (Cor);
///@Eduardo, LOJA FEITA!

///TODO:
///Batalha -
///mini-menu com op��es -
///Menu Inicial -

int main()
{
    freopen("entrada.in","r", stdin);
	init();

    install_int(timerLuz,280); //timer para piscar e para anima��es em geral
    install_int(timerCursor,20);
    install_int(timerUnit,20);

    Jogo* jogo = inicializaJogo();
    show_mouse(screen);

    menuEscolhaPlayers(jogo);

    montaConstrucoes(jogo);

    comprarUnidade(jogo, 5, 2, SUBMARINE);
    passarTurno(jogo);
    comprarUnidade(jogo, 8, 2, FIGHTER);
    passarTurno(jogo);
    passarTurno(jogo);

    while (!key[KEY_ESC])
	{
	    imprimeMapa(jogo);

        if (jogo->UnidadeLoja != -1){
            Loja(jogo);
        }
	    else if(!timeUnit && checkMovCursor())
	    {
	       moveCursor(jogo);
	    }


        draw_sprite(screen,jogo->buffer,0,0);
	}
    destroy_bitmap(jogo->buffer);

	deinit();
	return 0;
}
END_OF_MAIN()
