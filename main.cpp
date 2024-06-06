/*
    Authors: Carmen Hidalgo Paz, Valeria Durán Muñoz & Mariana Navarro Jiménez
    date: 04/06/2024
    description: Juego Galaga en la terminal 
*/

#include <chrono>
#include <cstring>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

#include "highScore.h"

void initialize() {
    initscr();                  // Se empieza el ncurses
    cbreak();                   // Para no tener que presionar enter para cada movimiento
    noecho();                   // No escribir los inputs realizados por el jugador en pantalla
    keypad(stdscr, TRUE);       // Se habilitan las teclas del teclado
    curs_set(0);                // Esconder el cursor
    nodelay(stdscr, FALSE);     // Set getch a non-blocking

//-----------COLORES----------------
    start_color();              // Para poder utilizar los colores
    use_default_colors();       // Usar el color de la terminal como fondo
    
    // Parejas de colores para el ASCII art del juego
    init_pair(1, COLOR_YELLOW, -1);
    init_pair(2, COLOR_BLUE, -1);
    init_pair(3, COLOR_RED, -1);
    init_pair(4, COLOR_CYAN, -1);
    init_pair(5, COLOR_MAGENTA, -1);
    init_pair(6, COLOR_GREEN, -1);

//-----------MÚSICA----------------    
    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        fprintf(stderr, "No se pudo inicializar SDL: %s\n", SDL_GetError());
        exit(1);
    }
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        fprintf(stderr, "No se pudo inicializar SDL_mixer: %s\n", Mix_GetError());
        exit(1);
    }
    Mix_Music *backgroundMusic = Mix_LoadMUS("MEGALOVANIA.mp3");                    // Canción que empieza cuando se inicia el programa
    if (!backgroundMusic) {
        fprintf(stderr, "No se pudo cargar la música: %s\n", Mix_GetError());
        exit(1);
    }
    Mix_PlayMusic(backgroundMusic, -1); 
    Mix_VolumeMusic(30); 
}

void finalize() {
    Mix_CloseAudio();
    SDL_Quit();
    endwin();                   // Se termina el curses
}

// --------------------MENÚ--------------------
void drawMenu(const std::vector<std::string>& options, int highlight) {
    clear();                                        // Limpiar pantalla
    int y = LINES / 2 - options.size() / 2;         // Imprimir menú en el centro
    int x = COLS / 2 - options[0].size() / 2;
    for (size_t i = 0; i < options.size(); ++i) {   // Marcar la opción elegida 
        if (highlight == i) {
            attron(A_REVERSE);
            mvaddstr(y + i, x, options[i].c_str());
            attroff(A_REVERSE);
        } else {
            mvaddstr(y + i, x, options[i].c_str()); // Imprimir el texto
        }
    }
    refresh();                                     // Refrescar la pantalla
}

// --------------------TÍTULO--------------------
void drawTitulo(const std::vector<std::string>& titulo) {
    clear();                                        // Limpiar pantalla
    int y = LINES / 2 - titulo.size() / 2;          // Imprimir el título en el centro
    int x = COLS / 2 - titulo[0].size() / 2;
    for (size_t i = 0; i < titulo.size(); ++i) {
        mvaddstr(y + i, x, titulo[i].c_str());      // Imprimir el texto
    }
    refresh();                                      // Refrescar la pantalla
}

// --------------------INSTRUCCIONES--------------------
void drawInstructions() {
    std::vector<std::string> instructions;          // ASCII art de las instrucciones
    instructions = {
            "    ___ _  _  ___ _____ ___ _   _  ___ ___ ___ ___  _  _ ___ ___ ",
            "   |_ _| \\| /  __|_   _| _ \\ | | |/ __/ __|_ _/ _ \\| \\| | __/ __|",
            "    | || .  \\__  \\ | | |   / |_| | (_ | (_ | | (_) | .` | _|\\__ \\",
            "   |___|_|\\_| ___/ |_| |_|_\\ ___/ \\___\\___|___\\___/|_|\\_|___|___/",
            "                                                                ",
            "CONTROLES: [<-] para moverse a la izquierda, [->] para moverse a la derecha,",
            " ",
            "           [_____] para disparar",
            " ",
            "Para ganar el juego se debe llegar a más de 9999 puntos",
            " ",
            "TRES TIPOS DE ENEMIGOS:",
            " ",
            "  /---\\     .'='.      /---\\",
            "  \\-o-/     /|=|\\     --WWW--",
            "                     {#######}",
            " ",
            "El último es el boss Galaga. Si el jugador tiene suficientes vidas se lleva la",
            "nave del jugador. Si se destruye al enemigo, se recupera la nave y el jugador",
            "puede utilizar ambas.",
            " ",
            "PRESIONE CUALQUIER TECLA PARA DEVOLVERSE AL MENU...",
        };
    
    clear();                                            // Limpiar pantalla
    int y = LINES / 2 - instructions.size() / 2;        // Imprimir las instrucciones en el centro
    int x = COLS / 2 - instructions[0].size() / 2;
    for (size_t i = 0; i < instructions.size(); ++i) {
        mvaddstr(y + i, x, instructions[i].c_str());    // Imprimir el texto
    }
    refresh();                                          // Refrescar la pantalla
}

//--------------------ESTRELLAS--------------------
struct Star {
    int x, y;
};

std::vector<Star> initializeStars(int numStars) {       // Vector con la cantidad de estrellas que se quieren
    std::vector<Star> stars;
    stars.reserve(numStars);

    for (int i = 0; i < numStars; ++i) {                // Las estrellas se colocan en posiciones aleatorias
        int x = rand() % COLS;
        int y = rand() % LINES;
        stars.push_back({x, y});
    }

    return stars;
}

void drawStars(const std::vector<Star>& stars) {        // Las estrellas son representadas por un punto
    for (const auto& star : stars) {
        mvprintw(star.y, star.x, ".");
    }
}


// --------------------FUNCIONES DE LA PUNTUACIÓN--------------------
/*void drawFinalScore()
{
    int row, col;
    getmaxyx(stdscr, row, col); // Obtener el tamaño de la pantalla
    char header[][37] =
        {
            "     ___  ___ ___  _ __ ___        ",
            "    / __|/ __/ _ \\| '__/ _ \\      ",
            "    \\__ \\ (_| (_) | | |  __/       ",
            "    |___/\\___\\___/|_|  \\___|       ",
            "-----------------------------------",
    };
    clear();  
    // Mostrar el encabezado en pantalla usando ncurse
    for (int i = 0; i < 5; ++i)
    {
        mvprintw(row / 2 - 10 + i, (col - 37) / 2, header[i]);
    }
    char format[MAX_PLAYERS][60]; // Ensure enough space for the format

    // Create the format strings
    for (int i = 0; i < MAX_PLAYERS; ++i)
    {
        snprintf(format[i], sizeof(format[i]), " NAME: %-10s - SCORE: %d", names[i].c_str(), scores[i]);
    }

    // Mostrar en pantalla usando ncurses
    for (int i = 0; i < MAX_PLAYERS; ++i)
    {
        mvprintw(row / 2 - 4 + i, (col - 37) / 2, format[i]);
    }
    refresh(); // Actualizar la pantalla

}*/


void updateScore(char* score_line, int score)
{
    sprintf(score_line,"SCORE:%-6d",score);

}

void drawScore(int num) {
    char score_lines[][12]=
    {
        "__________",
        "SCORE:    ",

    };
    int num_lines = sizeof(score_lines)/sizeof(score_lines[0]);
    updateScore(score_lines[1],num);
    int starty = LINES - num_lines;
    int startx = COLS -strlen(score_lines[0]);

    for(int i = 0; i < num_lines; ++i){
        mvprintw(starty +i, startx, "%s",score_lines[i]);
    }
}

// --------------------REGISTRO DE NUEVO HIGH SCORE--------------------
void drawNameTitle() {
    std::vector<std::string> nameTitle;                     // ASCII art de las instrucciones
    nameTitle = {
            " ___  ___  __  _   __  _____  ___   __   ___    _   _   __   __ __  __  ___  ___ ",
            "| _\\ | __|/ _]| |/' _/|_   _|| _ \\ /  \\ | _ \\  | \\ | | /__\\ |  V  ||  \\| _ \\| __|",
            "| v /| _|| [/\\| |`._`.  | |  | v /| /\\ || v /  |   ' || \\/ || \\_/ || -<| v /| _|",
            "|_|_\\|___|\\__/|_||___/  |_|  |_|_\\|_||_||_|_\\  |_|\\__| \\__/ |_| |_||__/|_|_\\|___|",
            "                                                                ",
            "Utilize las teclas [^] [v] [<] [>] y el [ENTER] para ingresar su nombre",
            "Solo 3 letras permitidas",
        };
    
    int y = LINES / 2 - 7 - nameTitle.size() / 2;           // Imprimir las instrucciones en el centro
    int x = COLS / 2 - nameTitle[0].size() / 2;
    for (size_t i = 0; i < nameTitle.size(); ++i) {
        attron(COLOR_PAIR(NORMAL_PAIR));
        mvaddstr(y + i, x, nameTitle[i].c_str());           // Imprimir el texto
        attroff(COLOR_PAIR(NORMAL_PAIR));
    }
}

void drawNameLetters(int selectedLetterIndex) {
    std::vector<std::string> letters = {
        "[A] [B] [C] [D] [E] [F] [G] [H] [I]",
        "[J] [K] [L] [M] [N] [O] [P] [Q] [R]",
        "[S] [T] [U] [V] [W] [X] [Y] [Z] [-]",
    };

    int y = LINES / 2 - letters.size() / 2;                 // Centrar el texto
    int x = COLS / 2 - letters[0].size() / 2;

    // Imprimir las letras
    for (size_t i = 0; i < letters.size(); ++i) {
        mvaddstr(y + i, x, letters[i].c_str());
    }

    // Subrayar el char
    attron(A_REVERSE);
    int row = selectedLetterIndex / 9;
    int col = selectedLetterIndex % 9;
    mvaddch(y + row, x + col * 4 + 1, letters[row][col * 4 + 1]);
    attroff(A_REVERSE);
}

// Obtener el nombre del jugador
std::string getPlayerName() {
    std::string playerName;                                  // Variable que guarda el nombre del jugador
    int selectedLetterIndex = 0;                             // Letra que se debe subrayar
    int lettersChosen = 0;                                   // Letra escogida

    while (lettersChosen < 3) {
        erase();                                            // Limpiar la pantalla
        drawNameTitle();                                    // Mostrar el título
        drawNameLetters(selectedLetterIndex);               // Mostrar las letras
        // Se muestra el nombre del jugador
        mvprintw(LINES / 2 + 4, COLS / 2 - 5, "NOMBRE JUGADOR: %s", playerName.c_str());
        int letter = getch();                               // Obtener input del usuario
        switch (letter) {
            case KEY_UP:                                    // Moverse para arriba
                if (selectedLetterIndex >= 9) {             // Si lo seleccionado está mínimo en la segunda fila
                    selectedLetterIndex -= 9;               // Se resta 9 para ir a la fila anterior
                }
                break;
            case KEY_DOWN:                                  // Moverse para abajo
                if (selectedLetterIndex + 9 < 27) {         // Si la selección + 9 es menos de 27 (la cantidad de chars que se pueden escoger) 
                    selectedLetterIndex += 9;               // Se suma 9 para ir a la siguiente fila
                }
                break;
            case KEY_LEFT:                                  // Moverse para la izquierda
                if (selectedLetterIndex % 9 != 0) {         // Si lo seleccionado no está al borde izquierdo (no es un múltiplo de 9)
                    selectedLetterIndex--;                  // Se resta 1 para moverse un espacio a la izquierda
                    if (selectedLetterIndex == 26) {        // Si se llega al char especial
                        selectedLetterIndex--;              // Se decrementa en 1
                    }
                }
                break;
            case KEY_RIGHT:                                 // Moverse para la derecha
                // Si lo seleccionado no está al borde derecho y si al sumarle uno este no se pasa de la última posición
                if ((selectedLetterIndex + 1) % 9 != 0 && selectedLetterIndex < 26) {
                    selectedLetterIndex++;                  // Se suma 1 para moverse un espacio a la derecha
                }
                break;
            case 10:                                        // Presionar enter
                if (selectedLetterIndex < 26) {             // Si lo escogido es una letra
                    playerName += 'A' + selectedLetterIndex;// Se guarda la letra escogida
                } else {
                    playerName += '-';                      // Si se escogió el último char, se guarda el símbolo "-"
                }
                lettersChosen++;
                // Se muestra el nombre del jugador
                mvprintw(LINES / 2 + 4, COLS / 2 - 5, "NOMBRE JUGADOR: %s", playerName.c_str());
                refresh();                                  // Refrescar la pantalla
                break;
        }
    }

    return playerName;
}

enum BossSpawnState {           // Estados del boss Galaga para aparezca varias veces
    InitialWait,                // El boss se espera unos segundos antes de aparecer por primera vez
    WaitingForBossDeath,        // Se espera a que se muera el boss
    DelayAfterDeath,            // Delay después de que se muere
    ReadyToSpawn                // Listo para volver a aparecer
};

int main() {
    bool restartGame = true;                    // Condición de reinicio del juego

    do {
        // Archivo que contiene los nombres y las puntuaciones
        std::string highScoreFile = "highscores.txt";
        // Trabajar con el archivo de las puntuaciones
        HighScores highScores(highScoreFile);

        int finalScore = 0;
        bool won = false;
        int choice = getch();               // Input del usuario

        srand(time(0));
        initialize();
        std::vector<std::string> titulo = {     // El arte del título
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

        drawTitulo(titulo);                     // Llamar a la función que muestra el título en pantalla
        napms(5000);                            // Delay de 5 segundos
        // Opciones del menú
        std::vector<std::string> menuOptions = {"INICIAR JUEGO", "INSTRUCCIONES", " PUNTUACION  ","    SALIR    "};
        int highlight = 0;                      // Contador de opción señalada
        bool running = true;                    // Se está corriendo el menú
        while (running) {
            drawMenu(menuOptions, highlight);   // Llamar a la función que muestra el menú en pantalla

            int choice = getch();               // Input del usuario
            switch (choice) {
                case KEY_UP:                    // Si se presiona la flecha de arriba
                    highlight = (highlight == 0) ? menuOptions.size() - 1 : highlight - 1;
                    break;
                case KEY_DOWN:                  // Si se presiona la flecha de abajo
                    highlight = (highlight == menuOptions.size() - 1) ? 0 : highlight + 1;
                    break;
                case 10:                        // Si se presiona el enter
                    if (highlight == 0) {
                        // Iniciar el juego
                        clear();                // Limpiar pantalla
                                                // Imprimir mensaje de inicio del juego
                        mvprintw(LINES / 2, COLS / 2 - 5, "EMPEZANDO JUEGO...");
                        refresh();              // Refrescar la pantalla
                        napms(2000);            // Delay de 2 segundos
                        running = false;        // Salirse del loop a donde está el juego
                    } else if (highlight == 1) {
                        //Desplegar instrucciones
                        drawInstructions();
                        getch();                // Esperar a que se ingrese una tecla
                    }else if (highlight == 2) {
                        //Terminar el programa
                        highScores.drawFinalScore();
                        getch();   // Esperar una entrada del usuario

                    }
                    else if (highlight == 3) {
                        //Terminar el programa
                        finalize();
                        return 0;
                    }
                    break;
            }
        }
    
        Nave ship(COLS / 2 - 7, LINES - 6);     // Colocar nave
        ship.initializeLifeArt();               // Asignar vidas

        Enemies enemies;                        // Instancia de enemigos
        // Estado inicial del boss
        BossSpawnState bossSpawnState = InitialWait;

        int ch;                                 // Input del usuario
        int vidas = ship.lives;                 // Contador de vidas
        erase();                                // Limpiar pantalla
        ship.drawNave();                        // Mostrar la nave
        ship.drawLife(COLS / 5, LINES);         // Mostrar las vidas
        drawScore(0);

        // Tiempo de los estados
        std::chrono::time_point<std::chrono::steady_clock> stateStartTime = std::chrono::steady_clock::now();

        // Tiempo de inicio del juego
        auto startTime = std::chrono::steady_clock::now();
        // Periodo de delay antes de que aparezcan los enemigos
        const double delayPeriod = 4.0;

        const int numStars = 95;                                                // Cantidad de estrellas
        auto stars = initializeStars(numStars);

        while ((ch = getch()) != 'q') {                                         // Mientras no se presione la letra q, seguir con el juego
            auto currentTime = std::chrono::steady_clock::now();                // Tiempo actual
            std::chrono::duration<float> elapsed = currentTime - startTime;     // Tiempo que ha pasado desde el incio hasta ahora

            nodelay(stdscr, TRUE);                                              // Para que los enemigos se muevan sin importar los movimientos del jugador
            erase();                                                            // Limpiar pantalla
    
            drawStars(stars);                                                   // Dibujar las estrellas
    
            for (auto& enemy : enemies.enemyList) {
                if (!enemy->isAlive) continue;
                if (enemies.checkCollision(enemy, ship)) {                      // Si hay colision entre el jugador y los enemigos normales entonces break
                    break;
                }
            }

            if (ship.lives == -1) break;                                        // Si ya no se tienen vidas, terminar el juego

            // Estados del boss
            std::chrono::duration<double> elapsedStateTime = currentTime - stateStartTime;
            switch (bossSpawnState) {
                case InitialWait:                                               // Espera inicial
                    if (elapsed.count() >= 30.0) {                              // Cuando hayan pasado 30 segundos, aparece el boss
                        enemies.spawnBoss(ship.x);                              // Crear boss
                        stateStartTime = currentTime;                           // Actualizar contador de tiempo
                        bossSpawnState = WaitingForBossDeath;                   // El estado cambia
                    }
                    break;

                case WaitingForBossDeath:                                       // Esperando a que se muera
                    if (enemies.isBossDead()) { 
                        stateStartTime = currentTime;                           // Actualizar contador de tiempo
                        bossSpawnState = DelayAfterDeath;                       // El estado cambia
                    }
                    break;

                case DelayAfterDeath:                                           // Espera después de la muerte
                    if (elapsedStateTime.count() >= 12.0) {                     // Que pasen 12 segundos
                        bossSpawnState = ReadyToSpawn;                          // El estado cambia
                    }
                    break;

                case ReadyToSpawn:                                              // Puede volver a aparecer
                    if (enemies.isBossDead()) {
                        enemies.spawnBoss(ship.x);                              // Crear boss
                        stateStartTime = currentTime;                           // Actualizar contador de tiempo
                        bossSpawnState = WaitingForBossDeath;                   // El estado cambia
                    }
                    break;
            }

            // Que los enemigos aparezcan por primera vez después de un tiempo de espera
            if (elapsed.count() >= delayPeriod) {
                if (enemies.areAllNonBossEnemiesDefeated()) {                   // Si todos las naves enemigas estan derrotadas entonces se actualiza la ola de enemigos
                    enemies.updateWave();
                }
            }

            handleInput(ch, ship);                                              // Ingresar las teclas de movimiento
    
            enemies.updateEnemies(ship.x, ship);                                // Actualiza las naves enemigas
            enemies.drawEnemies();                                              // Dibuja los enemigos
            ship.drawNave();                                                    // Dibuja la nave
            ship.drawLife(COLS / 5, LINES);                                     // Mostrar vidas
            ship.updateBalasPos();
            enemies.checkCollisionBala(ship, finalScore);                       // Si hubo colision con las naves enemigas
            drawScore(finalScore);
            enemies.checkBulletCollision(ship);                                 // Revisar si hubo colisión de las balas enemigas con el jugador
            ship.drawBalas();

            refresh();                                                          // Refrescar la pantalla
            napms(20);                                                          // Delay para desacelerar el loop
    
            if (finalScore > 9999) {
                won = true;
                break;
            }
        }
        erase();                                        // Limpiar pantalla
        drawStars(stars);                               // Dibujar las estrellas
        enemies.drawEnemies();                          // Mostrar enemigos
        refresh();                                      // Refrescar pantalla

        if(!won)
        {
            napms(1000);                                // Se genera un delay de 1 segundo
            clear();                                    // Limpiar pantalla
            // Imprimir mensaje
            mvprintw(LINES / 2, COLS / 2 - 5, "FIN DEL JUEGO");
            // Imprimir puntuación
            mvprintw(LINES / 2 +1, COLS / 2 - 5, "SCORE:%d",finalScore);
            refresh();                                  // Refrescar la pantalla
            napms(6000);                                // Se genera un delay de 6 segundos
        }
        else{
            napms(1000);                                // Se genera un delay de 1 segundo
            clear();                                    // Limpiar pantalla
            // Imprimir mensaje
            mvprintw(LINES / 2, COLS / 2 - 5, "GANASTE");
            // Cantidad de vidas restantes
            mvprintw(LINES / 2 + 1, COLS / 2 - 5, "VIDAS RESTANTES: %d",ship.lives + 1);
            if (ship.lives + 1 == 3){
                finalScore = finalScore + 500;

            } else if (ship.lives + 1 == 2){
                finalScore = finalScore + 200;

            } else if (ship.lives + 1 == 1){
                finalScore = finalScore + 50;
            }
            // Imprimir puntuación
            mvprintw(LINES / 2 + 2, COLS / 2 - 5, "SCORE: %d",finalScore);
            refresh();                                  // Refrescar la pantalla
            napms(6000);                                // Se genera un delay de 6 segundos
        }
        
        int lowestScore = highScores.getLowestScore();  // Obtener la menor puntuación

        if (finalScore > lowestScore) {                 // Si la nueva puntuación es mayor a la menor puntuación
            clear();
            attron(COLOR_PAIR(TURRET_PAIR));
            mvprintw(LINES / 2, COLS / 2 - 5, "NUEVO HIGH SCORE");
            attroff(COLOR_PAIR(TURRET_PAIR));
            refresh();                                  // Refrescar la pantalla
            napms(2000);                                // Delay de 2 segundos
            std::string playerName = getPlayerName();   // Se obtiene el nombre del jugador
            napms(1000);                                // Delay de 1 segundo
            attron(COLOR_PAIR(BOSS_PAIR));
            mvprintw(LINES / 2 + 7, COLS / 2 - 5, "NOMBRE REGISTRADO");
            attroff(COLOR_PAIR(BOSS_PAIR));
            refresh();                                  // Refrescar la pantalla
            napms(2000);                                // Delay de 2 segundos
            // Se guarda el nombre y el puntaje
            highScores.updateHighScores(playerName, finalScore);
        }

        finalize();                                     // Terminar programa

    } while (restartGame);                              // Se vuelve a hacer el loop

    return 0;
}