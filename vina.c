#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>

#include "lz.h"
#include "archive.h"

struct info
{
    char name[1025];         // nome do membro
    unsigned long uid;       // id unica do membro
    unsigned long og_Size;   // tamanho do membro original
    unsigned long comp_Size; // tamanho do membro apos compressao
    unsigned long mod_Date;  // data de modificacao do membro
    unsigned long order;     // ordem do membro dentro do archive
    unsigned long offset;    // offset do inicio do archive ate o inicio do membro
};

char *search(char *dir, char *target, unsigned long base, unsigned long s)
{
    char *temp = strtok(dir, " ");

    char *aux[base - s];

    if (s > base)
    {
        while ((temp != target) && (temp != NULL))
            temp = strtok(NULL, " ");
    }
    else
    {
        aux[0] = temp;
        unsigned long i = 1;
        while ((temp != target) && (temp != NULL))
        {
            for (i; i < base; i++)
                aux[i] = strtok(NULL, " ");
            i = 0;
            temp = strtok(NULL, " ");
        }
        return aux[s - base];
    }

    if (temp == NULL)
    {
        for (unsigned long i = base; i < s; i++)
            temp = strtok(NULL, " ");
    }

    return temp;
}

unsigned long max_Size(char *directory)
{
    unsigned long j, max = 0;
    strtok(directory, " ");

    for (int i = 0; i < 2; i++)
        strtok(NULL, " ");
    j = atoi(strtok(NULL, " "));
    if (j > max)
        max = j;
    for (int i = 0; i < 3; i++)
        strtok(NULL, " ");
    while (strtok(NULL, " ") != NULL)
    {
        for (int i = 0; i < 2; i++)
            strtok(NULL, " ");
        j = atoi(strtok(NULL, " "));
        if (j > max)
            max = j;
        for (int i = 0; i < 3; i++)
            strtok(NULL, " ");
    }
    return max;
}

int main(int argc, char **argv)
{
    FILE *archive, *membro;
    char *buffer;
    unsigned long i, max;

    if (argv[1] == "m")
        archive = fopen(argv[3], "+a");
    else
        archive = fopen(argv[2], "+a");

    max = max_Size(archive);

    if (!(buffer = malloc(max * sizeof(char))))
        return -1;

    if (argv[1] == "m")
    {
        i = 4;
        do
        {
            buffer = argv[i];
            unsigned long espaco, compresso, alvo;
            espaco = atoi(search(archive, buffer, NAME, OFFSET));
            compresso = atoi(search(archive, buffer, NAME, COMP));
            if (argv[2] != "NULL")
            {
                alvo = atoi(search(archive, argv[2], NAME, OFFSET));
                alvo += atoi(search(archive, argv[2], NAME, COMP));
            }
            else
                alvo = 0;
            move(archive, espaco, compresso, alvo, max);
            // consertar no diretorio
            i++;
        } while (argv[i] != NULL);
    }
    else
    {
        buffer = argv[3];
        i = 3;
        if (argv[1] == "x")
        {
            do
            {
                buffer = argv[i];
                unsigned long espaco, compresso, alvo;
                espaco = atoi(search(archive, buffer, NAME, OFFSET));
                compresso = atoi(search(archive, buffer, NAME, COMP));
                fseek(archive, -compresso, SEEK_END);
                alvo = ftell(archive);
                move(archive, espaco, compresso, alvo, max);
                fseek(archive, -compresso, SEEK_END);
                buffer = fgets(buffer, compresso, archive);
                // achar um jeito de truncar
                ftruncate();
                // escrever em um novo arquivo
                // consertar no diretorio
                i++;
            } while (argv[i] != NULL);
        }
        else if (argv[1] == "r")
        {
            do
            {

            } while (argv[i] != NULL);
        }
        else if (argv[1] == "c")
        {
        }
        else if (argv[1] == "i")
        {
            do
            {

            } while (argv[i] != NULL);
        }
        else if (argv[1] == "p")
        {
            do
            {

            } while (argv[i] != NULL);
        }
    }

    fclose(archive);
}