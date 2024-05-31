/*
    Authors: Carmen Hidalgo Paz, Valeria Duran Muñoz & Mariana Navarro Jiménez
    date: 04/06/2024
    description: Juego Galaga en la terminal 
*/

#include <iostream>
#include <vector>
#include <ncurses.h>
#include "clases.h"



void initialize() {
    initscr();          // Se empieza el curses
    cbreak();           // Para no tener que presionar enter para cada movimiento
    noecho();           // No escribir los inputs realizados por el jugador en pantalla
    keypad(stdscr, TRUE); // Se habilitan las teclas del teclado
    curs_set(0);        // Esconder el cursor
}

void finalize() {
    endwin();           // Se termina el curses
}

int main() {
    initialize();

    Nave ship(COLS / 2 - 2, LINES - 4);
    Enemies enemies;
    enemies.spawnEnemies(5, 2, 1);

    int ch;
    while ((ch = getch()) != 'q') {
        clear();                // Limpiar pantalla

        handleInput(ch, ship);  //Ingresar las teclas de movimiento

        enemies.updateEnemies();
        ship.draw();    // La nave se mueve
        enemies.drawEnemies();

        refresh();
        napms(20);     // Se genera un delay

        
    }

    finalize();
    return 0;
}
