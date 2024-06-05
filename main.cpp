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


enum BossSpawnState {           // Estados del boss Galaga para aparezca varias veces
    InitialWait,                // El boss se espera unos segundos antes de aparecer por primera vez
    WaitingForBossDeath,        // Se espera a que se muera el boss
    DelayAfterDeath,            // Delay después de que se muere
    ReadyToSpawn                // Listo para volver a aparecer
};

int main() {
    bool restartGame = true;                    // Condición de reinicio del juego

    do {
        std::string highScoreFile = "highscores.txt";
        HighScores highScores(highScoreFile);
        int finalScore = 0;
        bool won = false;
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
        std::vector<std::string> menuOptions = {"INICIAR JUEGO", "INSTRUCCIONES", "    SALIR    "};
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

        std::string playerName = "Player";  // Get player name from the input

        highScores.updateHighScores(playerName, finalScore);
        highScores.displayHighScores();
        napms(6000);                                // Se genera un delay de 6 segundos

        finalize();                                     // Terminar programa

    } while (restartGame);                              // Se vuelve a hacer el loop

    return 0;
}