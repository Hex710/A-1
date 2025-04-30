#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>

#include "archive.h"

struct archive *create_Archive(FILE *arc)
{
    struct archive *a;
    long i, size;

    a->members = NULL;
    a->size = 0;

    a->members = malloc(sizeof(struct member));
    fread(a->members[0], 8, 7, arc);
    size = (a->members[0]->offset - 1);
    i = 1;

    while (ftell(arc) < size)
    {
        realloc(a->members, sizeof(struct member) * (i + 1));
        fread(a->members[i], sizeof(struct member), 1, arc);
        i++;
    }

    a->size = i;

    return a;
}

struct member *create_Member(char *name, unsigned long uid, unsigned long off)
{
    struct member *m;

    if (!(m = malloc(sizeof(struct member))))
        return NULL;

    strncpy(m->name, name, 1025);
    FILE *f = fopen(name, "r");
    time_t t;
    time(&t);
    struct tm *mod = localtime(&t);
    m->mod_Date = (long)((1000000 * mod->tm_mday) + (10000 * mod->tm_mon) + mod->tm_year);
    fseek(f, 0, SEEK_END);
    m->comp_Size = ftell(f);
    m->uid = m->order = uid;
    m->offset = off;

    return m;
}

int move(FILE *archive, unsigned long start, unsigned long tam, unsigned long target, unsigned long max)
{
    if (((start + tam) == target) || (start == target))
        return 0;

    FILE *aux;
    char buffer[max];
    unsigned long i, j = target;

    aux = fopen("target", "w+");

    fseek(archive, start, SEEK_SET);
    fgets(buffer, tam, archive);
    fputs(buffer, aux);

    if (target > start)
    {
        i = (start + tam + 1);
        while ((i + max) < target)
        {
            fseek(archive, j, SEEK_SET);
            fgets(buffer, max, archive);
            fseek(archive, start, SEEK_SET);
            fputs(buffer, archive);
            start += max;
            j += max;
            i += max;
        }
        if (i < target)
        {
            fseek(archive, j, SEEK_SET);
            fgets(buffer, (target - i), archive);
            fseek(archive, start, SEEK_SET);
            fputs(buffer, archive);
        }
        fgets(buffer, tam, aux);
        fputs(buffer, archive);
    }
    else
    {
        i = (start - 1);
        while ((i - max) > target)
        {
            fseek(archive, j, SEEK_SET);
            fgets(buffer, max, archive);
            fseek(archive, start, SEEK_SET);
            fputs(buffer, archive);
            start += max;
            j += max;
            i -= max;
        }
        if (i > target)
        {
            fseek(archive, j, SEEK_SET);
            fgets(buffer, (i - target), archive);
            fseek(archive, start, SEEK_SET);
            fputs(buffer, archive);
        }

        fgets(buffer, tam, aux);
        fseek(archive, target, SEEK_SET);
        fputs(buffer, archive);
    }

    fclose(aux);

    return 1;
}

int insert_member(struct archive *a, struct member *m)
{
    if ((!a) || (!m))
        return 0;

    unsigned long i, aux;

    if (!(a->size))
        a->members = malloc(sizeof(struct member));
    else
        realloc(a->members, ((a->size + 1) * sizeof(struct member)));
    a->members[(a->size)++] = m;

    return 1;
}

struct member *remove_Member(struct archive *a, char *name)
{
    if ((!a) || !(a->size))
        return NULL;

    struct member *m, *h;
    long aux, start, end;
    FILE *dir;

    while (a->members[aux]->name != name)
        aux++;
    m = a->members[aux];
    (a->size)--;
    if (aux != a->size)
    {
        h = a->members[a->size];
        a->members[aux] = h;
    }

    realloc(a->members, a->size);

    return m;
}

int print_Member(struct member *m)
{
    if (!m)
        return 0;

    unsigned long day, month, year;

    year = (m->mod_Date % 10000);
    month = ((m->mod_Date / 10000) % 100);
    day = ((m->mod_Date / 1000000) % 100);

    printf("nome: %s\n", m->name);
    printf("UID: %lu\n", m->uid);
    printf("tamanho original: %lu\n", m->og_Size);
    printf("tamnho em disco: %lu\n", m->comp_Size);
    printf("data de modificação: %lu/%lu/%lu\n", day, month, year);
    printf("%lu\n", m->order);

    return 1;
}

struct member *search_Archive(struct archive *a, char *name)
{
    if (!a)
        return NULL;

    struct member *m;

    for (unsigned long i = 0; i < a->size; i++)
    {
        m = a->members[i];
        if (m->name == name)
            return m;
    }
    return NULL;
}

unsigned long find_Size(struct archive *a)
{
    if (!a)
        return 0;

    unsigned long i = 0;

    while (a->members[i]->order != 1)
        i++;

    return a->members[i]->offset;
}

int overwrite(FILE *archive, struct archive *a, unsigned long max)
{
    if ((!archive) || (!a))
        return 0;

    long add;
    unsigned long tam;

    tam = find_Size(a);

    if (tam != (sizeof(struct member) * a->size))
    {
        move(archive, tam, fseek(archive, 0, SEEK_END), (a->size * sizeof(struct member)), max);
        add = (a->size * sizeof(struct member) - tam);
        for (unsigned long i = 0; i < a->size; i++)
            a->members[i]->offset += add;
    }
    rewind(archive);
    for (unsigned long i = 0; i < a->size; i++)
    {
        fwrite(a->members[i], sizeof(struct member), 1, archive);
    }
    return;
}

int destroy_Archive(struct archive *a)
{
    if (!a)
        return 0;

    struct member *m;

    while (a->size > 0)
    {
        m = remove_Member(a, a->members[--(a->size)]->name);
        if (!m)
            return 0;
        free(m);
    }

    free(a->members);
    free(a);

    return 1;
}

int extract_Member(FILE *arc, struct archive *a, char *name, unsigned long max)
{
    if (!a)
        return 0;

    FILE *aux;
    struct member *m;
    char *buffer[max];
    unsigned long end;

    m = remove_Member(a, name);
    aux = fopen(m->name, "w+");
    fseek(arc, m->offset, SEEK_SET);
    fgets(buffer, m->comp_Size, arc);
    fputs(buffer, aux);
    fseek(arc, 0, SEEK_END);
    end = ftell(arc);
    move(arc, m->offset, m->comp_Size, end, max);
    truncate(arc, (end - m->comp_Size));

    return 1;
}