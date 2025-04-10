#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lz.h"

struct info
{
    char name[1025];
    int uid;
    int og_Size;
    int comp_Size;
    int mod_Date;
    int order;
    int offset;
};

int search_Offset(FILE *archive, char *name)
{
    FILE *temp = strtok(archive, " ");

    while (temp != name)
        temp = strtok(NULL, " ");

    for (int i = 0; i < 6; i++)
        temp = strtok(NULL, " ");

    return (int)temp;
    /* find a way to save the pointer, but then send the read into the directory to find the offset for the wanted file */
}

int main()
{
    FILE *archive;
    char *buffer, *op, *token, *temp;
    fscanf(stdin, buffer);

    op = strtok(buffer, " ");

    temp = strtok(NULL, " ");

    if (op == "m")
    {
        token = temp;
        temp = strtok(NULL, " ");
    }

    archive = fopen(temp, "+a");

    if (op == "m")
    {
        temp = strtok(NULL, " ");
        search_Offset(archive, temp);
    }
    else if (op == "x")
    {
    }
    else if (op == "r")
    {
    }
    else if (op == "c")
    {
    }
    else
    {

        if (op == "ip")
        {
        }
        else if (op == "ic")
        {
        }
    }

    fclose(archive);
}