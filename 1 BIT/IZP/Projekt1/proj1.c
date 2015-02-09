/*
 * Soubor:  main.c
 * Datum:   25.10.2013
 * Autor:   1BIA - Filip Gulan, xgulan00@fit.vutbr.cz
 * Projekt: 1BIT cislo 1: Pocitanie slov
 * Popis viz:
        https://wis.fit.vutbr.cz/FIT/st/cwk.php?title=IZP:Projekt1&id=9416
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#define MNOZINA (((znakvstup < '0') || (znakvstup > '9' && znakvstup < 'A') || (znakvstup > 'Z' && znakvstup < 'a') || (znakvstup > 'z') ) && (znakvstup != '-') && (znakvstup != '_')) //mnozina obsahujuca oddelovace slov
#define MNOZINAARG (((argv[1][0] < '0') || (argv[1][0] > '9' && argv[1][0] < 'A') || (argv[1][0] > 'Z' && argv[1][0] < 'a') || (argv[1][0] > 'z') ) && (argv[1][0] != '-') && (argv[1][0] != '_') && (argv[1][0] != '^') && (argv[1][0] != '.') && (argv[1][0] != ':'))
#define DEBUGZAPNUTY (1)
#define DEBUGVYPNUTY (0)
#define OBMEDZENIECHAR (80)     //obmedzenie na pocet znakov pri debugu
#define NEHLADAPOZICIU (0)
#define CIFRA (10)              //napr prechod z jendotiek na desiakty...
#define CHYBNAPOZ (0)           //chybna pozicia

/*
 * Funkcia na vypisovanie napovedy.
 */
int napoveda()
{
    printf("Pocitanie slov 2013 \nVytvoril Filip Gulan (xgulan00) \n \n \nVitajte v napovede k programu Pocitanie slov 2013 vytvoreny v ramci 1 projektu pre premet IZP. \n \n");
    printf("Program sluzi na pocitanie slov, ktore obsahuju blizsie specifikovany znak. Znak, popripade umiestnenie znaku specifikujeme pomocou argumentov. \n \n");
    printf("Program spustajte v nasleduucej podobe: ./proj1 --help alebo ./proj1 X [N] [-d]\n \n");
    printf("Popis argumentov: \n \n--help  vypise na obrazovku napovedu k programu\n \n");
    printf("X       reprezentuje hladanyznak, argument je povinny, moze obsahovat znak 0-9, a-z, A-Z, znak - alebo _ .");
    printf("Alebo moze obsahovat specialny znak : ktory symbolizuje lubovolne cislo od 0-9, znak ^ symbolizuje lubovolne velke pismeno A-Z a znak . symbolizuje lubovolny znak.\n \n");
    printf("[N]     predstavuje cislo pozicie hladaneho znaku v slove, je to volitelny argument, cislo je kladne prve pismeno v slove ma poziciu 1 \n \n");
    printf("[-d]    zapina tzv. ladiaci rezim, kedy sa vypisuju na obrazovku vsetky slova na novy riadok, argument je nepovinny. \n");
    return 0;
}

/*
 * Funkcia na ziskanie z pola znakov cislo.
 */
int chartoint(char *argument)
{
    long int poziciah = 0;           //poziciahladaneho znaku
    int i = 0;                  //na zistenie prebehnuteho poctu iteracii
    while (argument[i] != 0)
    {
        if (argument[i] > '9' || argument[i] < '0')
        {
           poziciah = CHYBNAPOZ;
           break;
        }
        poziciah = (argument[i] - '0') + poziciah * CIFRA;
        i++;
    }
    if (poziciah > INT_MAX)
    {
        poziciah = CHYBNAPOZ;
    }
    return poziciah;
}

/*
 * Funkcia na pocitanie slov s danym blizsie specifikovanym znakom.
 */
int moznosti(int hladanyznak, int poziciah, int debug)
{
    int znakvstup;                  //znakzovstupu
    int prepinacmedzera = 0;        //prepinac medzera 0 nieje medzera 1 je medzera
    int obmedzenieznakov = 0;       //premena na to aby sa nevypisovali znaky v slove na 80+ mieste
    int prepinacslovo = 0;          //prepinac medzera 0 nieje slovo 1 je slovo
    int pocet = 0;                  //pocet slov zso specifikovanym znakom
    int poziciaznak = 0;            //pozicia hladaneho znaku
    int prvyznak = 0;               //premena na zistenie prveho znaku

    while (( znakvstup = getchar()) != EOF)         //cyklus na ziskanie znakov zo vstupu
    {
        poziciaznak++;
        if (hladanyznak == ':')                     //hlada cislice v slove
        {
            if ((znakvstup >= '0' && znakvstup <= '9') && prepinacslovo == 0 && poziciah == 0)
            {
                pocet++;
                prepinacslovo = 1;
            }
            if ((znakvstup >= '0' && znakvstup <= '9') && prepinacslovo == 0 && poziciah == poziciaznak)
            {
                pocet++;
                prepinacslovo = 1;
            }
            else if (MNOZINA)
            {
                prepinacslovo = 0;
                poziciaznak = 0;
            }
        }
        else if (hladanyznak == '^')                   //hlada velke pismena v slove
        {
            if ((znakvstup >= 'A' && znakvstup <= 'Z') && prepinacslovo == 0 && poziciah == 0)
            {
                pocet++;
                prepinacslovo = 1;
            }
            if ((znakvstup >= 'A' && znakvstup <= 'Z') && prepinacslovo == 0 && poziciah == poziciaznak)
            {
                pocet++;
                prepinacslovo = 1;
            }
            else if (MNOZINA)
            {
                prepinacslovo = 0;
                poziciaznak = 0;
            }
        }
        else if (hladanyznak == '.')                        //hlada vsetky pismena v slove
        {
            if (MNOZINA)
            {
                prepinacslovo = 0;
                poziciaznak = 0;
            }
            else if (prepinacslovo == 0 && poziciah == 0)
            {
                pocet++;
                prepinacslovo = 1;
            }
            else if (prepinacslovo == 0 && poziciah == poziciaznak)
            {
                pocet++;
                prepinacslovo = 1;
            }
        }
        else                                                    //inak hlada blizsie specifikovany znak
        {
            if ((znakvstup == hladanyznak) && prepinacslovo == 0 && poziciah == 0)
            {
                pocet++;
                prepinacslovo = 1;
            }
            if ((znakvstup == hladanyznak) && prepinacslovo == 0 && poziciah == poziciaznak)
            {
                pocet++;
                prepinacslovo = 1;
            }
            else if (MNOZINA)
            {
                prepinacslovo = 0;
                poziciaznak = 0;
            }
        }

        if (debug == DEBUGZAPNUTY)                                 //debug mod, vypisuje vsetky slova
        {
            if((MNOZINA) && prepinacmedzera == 0 && prvyznak == 1)
            {
                putchar('\n');
                prepinacmedzera = 1;
                obmedzenieznakov = 0;
            }
            else if (!(MNOZINA) && obmedzenieznakov <= OBMEDZENIECHAR)
            {
                putchar(znakvstup);
                prepinacmedzera = 0;
                obmedzenieznakov++;
                prvyznak = 1;
            }
        }
    }
    printf("%d\n", pocet);
    return 0;
}

/*
 * Funkcia main, ktora spracovava argumenty a nasledne vola funkcie
 */
int main(int argc, char *argv[])
{
    if (argc == 2 && !(MNOZINAARG))
    {
        if (strcmp(argv[1], "--help") == 0)                     //vypisanie napovedy
        {
            napoveda();
        }
        else if (strlen(argv[1]) == 1)
        {
            moznosti(argv[1][0], NEHLADAPOZICIU ,DEBUGVYPNUTY);
        }
        else
        {
            printf("Argumenty neboli zadane spravne!\n");
            return 1;
        }
    }
    else if (argc == 3 && !(MNOZINAARG))
    {
        if (strlen(argv[1]) == 1 && strcmp(argv[2], "-d") == 0)
        {
            moznosti(argv[1][0], NEHLADAPOZICIU, DEBUGZAPNUTY);
        }
        else if (strlen(argv[1]) == 1 && strcmp(argv[2], "-d") != 0)
        {
            int poziciah = chartoint(argv[2]);
            if (poziciah <= CHYBNAPOZ)
            {
                printf("Argumenty neboli zadane spravne!\n");
                return 1;
            }
            moznosti(argv[1][0], poziciah, DEBUGVYPNUTY);
        }
        else
        {
            printf("Argumenty neboli zadane spravne!\n");
            return 1;
        }
    }
    else if (argc == 4 && !(MNOZINAARG))
    {
        if (strlen(argv[1]) == 1 && strcmp(argv[2], "-d") == 0)
        {
            int poziciah = chartoint(argv[3]);
            if (poziciah <= CHYBNAPOZ)
            {
                printf("Argumenty neboli zadane spravne!\n");
                return 1;
            }
            moznosti(argv[1][0], poziciah, DEBUGZAPNUTY);
        }
        else if (strlen(argv[1]) == 1 && strcmp(argv[3], "-d") == 0)
        {
            int poziciah = chartoint(argv[2]);
            if (poziciah <= CHYBNAPOZ)
            {
                printf("Argumenty neboli zadane spravne!\n");
                return 1;
            }
            moznosti(argv[1][0], poziciah, DEBUGZAPNUTY);
        }
        else
        {
            printf("Argumenty neboli zadane spravne!\n");
            return 1;
        }
    }
    else
    {
        printf("Argumenty neboli zadane spravne!\n");
        return 1;
    }
    return 0;
}
