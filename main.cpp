/*
    Authors: Carmen Hidalgo Paz, Valeria Duran Muñoz & Mariana Navarro Jiménez
    date: 04/06/2024
    description: Juego Galaga en la terminal 
*/

#include <iostream>
#include <vector>
#include <ncurses.h>
#include "clases.h"
#include <chrono>


void initialize() {
    initscr();          // Se empieza el curses
    cbreak();           // Para no tener que presionar enter para cada movimiento
    noecho();           // No escribir los inputs realizados por el jugador en pantalla
    keypad(stdscr, TRUE); // Se habilitan las teclas del teclado
    curs_set(0);        // Esconder el cursor
    nodelay(stdscr, FALSE); // Set getch to non-blocking
}

void finalize() {
    endwin();           // Se termina el curses
}

void drawMenu(const std::vector<std::string>& options, int highlight) {
    clear();
    int y = LINES / 2 - options.size() / 2;
    int x = COLS / 2 - options[0].size() / 2;
    for (size_t i = 0; i < options.size(); ++i) {
        if (highlight == i) {
            attron(A_REVERSE);
            mvaddstr(y + i, x, options[i].c_str());
            attroff(A_REVERSE);
        } else {
            mvaddstr(y + i, x, options[i].c_str());
        }
    }
    refresh();
}

void drawTitulo(const std::vector<std::string>& titulo) {
    clear();
    int y = LINES / 2 - titulo.size() / 2;
    int x = COLS / 2 - titulo[0].size() / 2;
    for (size_t i = 0; i < titulo.size(); ++i) {
        mvaddstr(y + i, x, titulo[i].c_str());
    }
    refresh();
}

int main() {
    initialize();
    std::vector<std::string> titulo = { // El arte de la nave es un vector
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

    drawTitulo(titulo);
    refresh();  // Ensure the message is shown
    napms(5000); // Delay for 2000 milliseconds (2 seconds)
    std::vector<std::string> menuOptions = {"Empezar Juego", "Salir"};
    int highlight = 0;

    bool running = true;
    while (running) {
        drawMenu(menuOptions, highlight);

        int choice = getch();
        switch (choice) {
            case KEY_UP:
                highlight = (highlight == 0) ? menuOptions.size() - 1 : highlight - 1;
                break;
            case KEY_DOWN:
                highlight = (highlight == menuOptions.size() - 1) ? 0 : highlight + 1;
                break;
            case 10: // Enter key
                if (highlight == 0) {
                    // Start Game
                    clear();
                    mvprintw(LINES / 2, COLS / 2 - 5, "Empezando Juego...");
                    refresh();
                    napms(2000); // Delay for 2000 milliseconds (2 seconds)
                    running = false; // Exit loop and start the game
                } else if (highlight == 1) {
                    // Quit
                    finalize();
                    return 0;
                }
                break;
        }
    }
    
    Nave ship(COLS / 2 - 7, LINES - 6); // Initialize ship at the center bottom
    ship.initializeLifeArt();           // Initialize the life art

    Enemies enemies;
    enemies.spawnEnemies(5, 2, 1);

    int ch;
    clear();         // Limpiar pantalla
    ship.drawNave();    // La nave se mueve
    ship.drawLife(COLS / 5, LINES);

    auto startTime = std::chrono::steady_clock::now();
    bool bossSpawned = false;

    while ((ch = getch()) != 'q') {
        auto currentTime = std::chrono::steady_clock::now();
        std::chrono::duration<float> elapsed = currentTime - startTime;
        nodelay(stdscr, TRUE);
        clear();                // Limpiar pantalla

        handleInput(ch, ship);  //Ingresar las teclas de movimiento

        ship.drawNave();    // La nave se mueve
        ship.drawLife(COLS / 5, LINES);

        if (elapsed.count() >= 10.0 && !bossSpawned){
            enemies.spawnBoss(ship.x);
            bossSpawned = true;
        }

        enemies.updateEnemies(ship.x, ship);
        enemies.drawEnemies();
        
        if (ship.lives == -1) break;

        refresh();
        napms(20); // Delay to slow down the game loop
    }
    clear();
    enemies.drawEnemies();
    ship.drawLife(COLS / 5, LINES);
    refresh();

    napms(1000);     // Se genera un delay
    clear();
    mvprintw(LINES / 2, COLS / 2 - 5, "FIN DEL JUEGO");
    refresh();
    napms(2000);     // Se genera un delay
    
    finalize();
    return 0;
}

