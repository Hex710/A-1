#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "lz.h"

#define NAME 0
#define UID 1
#define OG 2
#define COMP 3
#define DATE 4
#define ORDER 5
#define OFFSET 6

struct info
{
    char name[1025]; // nome do membro
    int uid;         // id unica do membro
    int og_Size;     // tamanho do membro original
    int comp_Size;   // tamanho do membro apos compressao
    int mod_Date;    // data de modificacao do membro
    int order;       // ordem do membro dentro do archive
    int offset;      // offset do inicio do archive ate o inicio do membro
};

char *inttoa(int i)
{
    int c, j = i;
    c = 0;

    while (j)
    {
        j /= 10;
        c++;
    }

    j = c;

    char *str[c + 1];
    if (i < 0)
    {
        str[0] = "-";
        i = -i;
    }

    while (i > 0)
    {
        str[j--] = ((i % 10) + "0");
        i /= 10;
    }

    str[j] = "\0";

    return str;
}

char *search(FILE *archive, char *target, int base, int s)
{
    FILE *temp = strtok(archive, " ");

    char *aux[base - s];

    if (s > base)
    {
        while ((temp != target) && (temp != NULL))
            temp = strtok(NULL, " ");
    }
    else
    {
        aux[0] = temp;
        int i = 1;
        while ((temp != target) && (temp != NULL))
        {
            for (i; i < base; i++)
                aux[i] = strtok(NULL, " ");
            i = 0;
            temp = strtok(NULL, " ");
        }
        return aux[s];
    }

    if (temp == NULL)
    {
        for (int i = base; i < s; i++)
            temp = strtok(NULL, " ");
    }

    return temp;
}

int move(FILE *archive, char *antecessor, int espaco, int tam, int atual)
{
    // create two files to store the info from the members
    FILE *list, *target;
    char *buffer;
    int pulo, ant, num = atual;

    list = fopen("list", "w+");
    target = fopen("target", "w+");

    // one by one copy the members, that appear after the one you want to move, into a file then concaten them out
    do
    {
        ant = num;
        num = search(archive, inttoa((num + 1)), ORDER, ORDER);
    } while (num);

    do
    {
        pulo = atoi(search(archive, inttoa(ant), ORDER, OFFSET));
        fseek(archive, pulo, SEEK_SET);
        fgets(buffer, atoi(search(archive, inttoa(ant), ORDER, COMP)), archive);
        fputs(buffer, list);
        // concatenar
        ant--;
    } while (ant > atual);

    // copy the member you wish to move into the secondfile then cut it
    fseek(archive, espaco, SEEK_SET);
    fgets(buffer, tam, archive);
    fputs(buffer, target);
    // concatenar

    // one by one copy the members, that appear before the one you want to move, but after the one you want to move to, into a file then concaten them out
    num = search(archive, antecessor, NAME, ORDER);
    ant = (atual - 1);

    do
    {
        pulo = atoi(search(archive, inttoa(ant), ORDER, OFFSET));
        fseek(archive, pulo, SEEK_SET);
        fgets(buffer, atoi(search(archive, inttoa(ant), ORDER, COMP)), archive);
        fputs(buffer, list);
        // concatenar
        ant = atoi(search(archive, inttoa(ant - 1), ORDER, ORDER));
    } while (ant > (num + 1));

    // write the member you wanted to move into the archive, then write the others in the opposite order you took them
    fgets(buffer, tam, target);
    fputs(buffer, archive);

    do
    {
        pulo = (-1 * atoi(search(archive, inttoa(ant), ORDER, COMP)));
        fseek(archive, pulo, SEEK_END);
        pulo = -pulo;
        fgets(buffer, pulo, list);
        fputs(buffer, archive);
        // concatenar
        ant = atoi(search(archive, inttoa(ant + 1), ORDER, ORDER));
    } while (ant);

    // fix the positions in the directory
    fclose(list);
    fclose(target);
}

int main(int argc, char **argv)
{
    FILE *archive, *membro;
    char *buffer;
    int op, i;

    buffer = argv[1];
    if (buffer == "m")
        op = 0;
    else if (buffer == "x")
        op = 1;
    else if (buffer == "r")
        op = 2;
    else if (buffer == "c")
        op = 3;
    else if (buffer == "ip")
        op = 4;
    else
        op = 5;

    buffer = argv[2];

    if (op == 0)
    {
        membro = fopen("temp", "a+");
        fputs(buffer, membro);
        buffer = argv[3];
    }

    archive = fopen(buffer, "+a");

    if (op == 0)
    {
        i = 4;
        do
        {
            buffer = argv[i];
            int espaco, compresso;
            espaco = atoi(search(archive, buffer, NAME, OFFSET));
            compresso = atoi(search(archive, buffer, NAME, COMP));
            fgets(buffer, 1025, membro);
            move(archive, buffer, espaco, compresso); // veja o que por no fgets como o primeiro parametro
            i++;
        } while (argv[i] != NULL);
    }
    else
    {
        buffer = argv[3];
        i = 3;
        if (op == 1)
        {
            do
            {

            } while (argv[i] != NULL);
        }
        else if (op == 2)
        {
            do
            {

            } while (argv[i] != NULL);
        }
        else if (op == 3)
        {
        }
        else
        {
            if (op == 4)
            {
                do
                {

                } while (argv[i] != NULL);
            }
            else if (op == 5)
            {
                do
                {

                } while (argv[i] != NULL);
            }
        }
    }

    fclose(archive);
}