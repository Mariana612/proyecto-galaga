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
    nodelay(stdscr, TRUE); // Set getch to non-blocking
}

void finalize() {
    endwin();           // Se termina el curses
}

// Imprimir menú
void drawMenu(const std::vector<std::string>& options, int highlight) {
    clear();    // Limpiar pantalla
    int y = LINES / 2 - options.size() / 2; // Imprimir menú en el centro
    int x = COLS / 2 - options[0].size() / 2;
    for (size_t i = 0; i < options.size(); ++i) { // Marcar la opción elegida 
        if (highlight == i) {
            attron(A_REVERSE);
            mvaddstr(y + i, x, options[i].c_str());
            attroff(A_REVERSE);
        } else {
            mvaddstr(y + i, x, options[i].c_str()); // Imprimir el texto
        }
    }
    refresh();  // Refrescar la pantalla
}

// Imprimir título
void drawTitulo(const std::vector<std::string>& titulo) {
    clear();    // Limpiar pantalla
    int y = LINES / 2 - titulo.size() / 2;  // Imprimir el título en el centro
    int x = COLS / 2 - titulo[0].size() / 2;
    for (size_t i = 0; i < titulo.size(); ++i) {
        mvaddstr(y + i, x, titulo[i].c_str()); // Imprimir el texto
    }
    refresh();  // Refrescar la pantalla
}

int main() {
    initialize();
    std::vector<std::string> titulo = { // El arte del título
" .          \\      .    _____ _____ __    _____ _____ _____", 
"      .      \\   ,     |   __|  _  |  |  |  _  |   __|  _  |",
"   .          o     .  |  |  |     |  |__|     |  |  |     |", 
"     .         \\       |_____|__|__|_____|__|__|_____|__|__|", 
"               #\\##\\#      .                              .   ", 
"             #  #O##\\###                .                      ",
"   .        #*#  #\\##\\###                       .             ",
"        .   ##*#  #\\##\\##               .                     ",
"      .      ##*#  #o##\\#         .                            ",
"          .     *#  #\\#     .                    .             ",
"                      \\          .                         .",
"____^/\\___^--____/\\____O______________/\\/\\---/\\___________-",
"   /\\^   ^  ^    ^                  ^^ ^  '\\ ^          ^     ",
"         --           -            --  -      -         ---  __ ",
"   --  __                      ___--  ^  ^                      "
        };

    drawTitulo(titulo); // Llamar a la función que muestra el título en pantalla
    napms(5000); // Delay de 5 segundos
    std::vector<std::string> menuOptions = {"Empezar Juego", "Salir"};  // Opciones del menú
    int highlight = 0;

    bool running = true;
    while (running) {
        drawMenu(menuOptions, highlight); // Llamar a la función que muestra el menú en pantalla

        int choice = getch();   // Input del usuario
        switch (choice) {
            case KEY_UP:    // Si se presiona la flecha de arriba
                highlight = (highlight == 0) ? menuOptions.size() - 1 : highlight - 1;
                break;
            case KEY_DOWN: // Si se presiona la flecha de abajo
                highlight = (highlight == menuOptions.size() - 1) ? 0 : highlight + 1;
                break;
            case 10: // Si se presiona el enter
                if (highlight == 0) {
                    // Iniciar el juego
                    clear();    // Limpiar pantalla
                    // Imprimir mensaje
                    mvprintw(LINES / 2, COLS / 2 - 5, "Empezando Juego...");
                    refresh();  // Refrescar la pantalla
                    napms(2000); // Delay de 2 segundos
                    running = false; // Salirse del loop a donde está el juego
                } else if (highlight == 1) {
                    //Terminar el programa
                    finalize();
                    return 0;
                }
                break;
        }
    }
    
    Nave ship(COLS / 2 - 7, LINES - 6); // Colocar nave
    ship.initializeLifeArt();           // Asignar vidas

    Enemies enemies;
    enemies.spawnEnemies(5, 2, 1);

    int ch; // Input del usuario
    int vidas = ship.lives; // Contador de vidas
    clear();    // Limpiar pantalla
    ship.drawNave();    // Mostrar la nave
    ship.drawLife(COLS / 5, LINES); // Mostrar las vidas
    while ((ch = getch()) != 'q') { // Mientras no se presione la letra q, seguir con el juego
        clear();    // Limpiar pantalla

        handleInput(ch, ship);  //Ingresar las teclas de movimiento

        if (ship.lives == -1) break;    // Si ya no se tienen vidas, terminar el juego

        if (ship.lives != vidas){ // Si se pierde una vida, dar momento de respiro al jugador
        clear();    // Limpiar pantalla
        enemies.drawEnemies();  // Mostrar enemigos
        ship.drawLife(COLS / 5, LINES); // Mostrar vidas 
        mvprintw(LINES / 2, COLS / 2 - 5, "READY"); // Mensaje de alerta
        refresh();  // Refrescar la pantalla
        napms(2000);    // Se genera un delay de 2 segundos
        };

        vidas = ship.lives; // Actualizar contador de vidas
        enemies.updateEnemies();
        ship.drawNave();    // La nave se mueve
        ship.drawLife(COLS / 5, LINES); // Mostrar vidas
        enemies.drawEnemies();

        refresh();  // Refescar la pantalla
        napms(20); // Delay para desacelerar el loop
    }
    clear();    // Limpiar pantalla
    enemies.drawEnemies();  // Mostrar enemigos
    refresh();  // Refrescar pantalla

    napms(1000);    // Se genera un delay de 1 segundo
    clear();    // Limpiar pantalla
    mvprintw(LINES / 2, COLS / 2 - 5, "FIN DEL JUEGO"); // Imprimir mensaje
    refresh();  // Refrescar la pantalla
    napms(2000);    // Se genera un delay de 2 segundos
    
    finalize(); // Terminar programa
    return 0;
}

