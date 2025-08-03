#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>

#define MAX_FICHEROS 1000
#define MAX_LONGITUD 1000

char **listarFicheros(const char *ruta)
{
    DIR *dir = opendir(ruta);
    if (!dir)
        return NULL;

    char **res = malloc(MAX_FICHEROS * sizeof(char *));
    struct dirent *entrada;
    int i = 0;
    while ((entrada = readdir(dir)) != NULL && i < MAX_FICHEROS)
    {
        if (entrada->d_type == DT_REG) // Solo archivos regulares
        {
            res[i] = malloc(MAX_LONGITUD);
            strncpy(res[i], entrada->d_name, MAX_LONGITUD - 1);
            res[i][MAX_LONGITUD - 1] = '\0';
            i++;
        }
    }
    // Rellenar el resto con cadenas vacías
    while (i < MAX_FICHEROS)
    {
        res[i] = malloc(1);
        res[i][0] = '\0';
        i++;
    }
    closedir(dir);
    return res;
}

void inicializar()
{
    if (SDL_Init(SDL_INIT_AUDIO) < 0)
    {
        perror(strcat("Error: ", SDL_GetError()));
        exit(1);
    }
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
    {
        perror(strcat("Error: ", SDL_GetError()));
        exit(1);
    }
}

void tocarCancion(char **cancion)
{
    Mix_Music *musica = NULL;
    int i = 0;
    char rutaCompleta[MAX_LONGITUD];
    while (1)
    {
        if (cancion[i][0] != '\0')
        {
            strcpy(rutaCompleta, "./canciones/");
            strcat(rutaCompleta, cancion[i]);
            musica = Mix_LoadMUS(rutaCompleta);
            if (musica)
            {
                Mix_PlayMusic(musica, 1);
                while (Mix_PlayingMusic())
                {
                    SDL_Delay(100);
                }
                Mix_FreeMusic(musica);
            }
            else
                fprintf(stderr, "Error al cargar la canción %s: %s\n", cancion[i], Mix_GetError());
        }
        i++;
        if (i >= MAX_FICHEROS)
            i = 0;
    }
}

int main(int argc, char *argv[])
{
    inicializar();
    char **canciones = listarFicheros("./canciones");
    tocarCancion(canciones);
    for (int i = 0; i < MAX_FICHEROS; i++)
    {
        free(canciones[i]);
    }
    free(canciones);
    Mix_CloseAudio();
    SDL_Quit();
    return 0;
}