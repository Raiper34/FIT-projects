/*
 * Soubor:  main.c
 * Datum:   1.12.2013
 * Autor:   1BIA - Filip Gulan, xgulan00@stud.fit.vutbr.cz
 * Projekt: 1BIT cislo 3: Hladanie obrazcov
 * Popis viz:
        https://wis.fit.vutbr.cz/FIT/st/cwk.php?title=IZP:Projekt3&csid=538331&id=9416
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define POCIATOK (0)    //pociatocna hodnota premennej
#define VYNUL (0)       //vynulovanie
#define POSUN (1)       //posunutie v matici napr matica ma 5 prvkov ale najvecsi index je 5-POSUN
#define NEUSPES (1)     //neuspesna kontrola, v utvare je 0
#define FARBA (1)       //bunka je farebna
#define NEFARBA (0)     //bunka nie je farebna
#define SUCCES (0)      //uspesne ukoncenie programu
#define FAULT (1)       //neuspesne ukoncenie programu s chybou...
#define TESTONLY (0)    //parameter funkcie na testovanie
#define HLINE (1)       //parameter funkcie na hline hladnie
#define VLINE (2)       //parameter funkcie na vline hladanie
#define SQUARE (3)      //parameter funkcie na hladanie squarez

/*
 *Vlastna struktura Bitmam na definovanie matice
 */
typedef struct {
  int rows;
  int cols;
  char *cells;
} Bitmap;

/*
 *Funkcia na ziskanie farby bunky
 */
char getcolor(Bitmap *bitmap, int x, int y)
{
    int farba = bitmap->cells[x * bitmap->cols + y];
    return farba;
}
/*
 *Funkcia na vypocet najvecsej horizontalnej ciary
*/
int find_hline(Bitmap *bitmap, int *x1, int *y1, int *x2, int *y2)
{
    int dlzkanovy = POCIATOK;
    int dlzkastary = POCIATOK;
    *x1 = POCIATOK;
    *y1 = POCIATOK;
    *x2 = POCIATOK;
    *y2 = POCIATOK;
    int zacX = POCIATOK;
    int zacY = POCIATOK;
    int i = POCIATOK;
    int j = POCIATOK;
    for(i = POCIATOK; i < bitmap->rows; i++)
    {
        for(j = POCIATOK; j < bitmap->cols; j++)
        {
            if (getcolor(bitmap, i, j) == NEFARBA)          //ak nema farbu tak sa vynuluje dlzka
            {
                dlzkanovy = POCIATOK;
            }
            else if ((getcolor(bitmap, i, j) == FARBA) && (dlzkanovy == POCIATOK))       //ak ma farbu tak pocita
            {
                zacX = i;
                zacY = j;
                dlzkanovy++;
            }
            else
            {
                dlzkanovy++;
            }
            if (dlzkanovy == bitmap->cols)      //optimalizacia, ak sa dlzka rovna stlpcom tak skonci hned pretoze vecsi nenajde
            {
                *x1 = zacX;
                *y1 = zacY;
                *x2 = i;
                *y2 = j;
                return SUCCES;
            }
            if (dlzkanovy >dlzkastary)      //vlozenie novych premenych ak je najdeny vecsi
            {
                *x1 = zacX;
                *y1 = zacY;
                *x2 = i;
                *y2 = j;
                dlzkastary = dlzkanovy;
            }

        }
        dlzkanovy = POCIATOK;
    }
    return SUCCES;
}

/*
 *Funkcia na vypocet najvecsej vertikalnej ciary
*/
int find_vline(Bitmap *bitmap, int *x1, int *y1, int *x2, int *y2)
{
    int dlzkanovy = POCIATOK;
    int dlzkastary = POCIATOK;
    *x1 = POCIATOK;
    *y1 = POCIATOK;
    *x2 = POCIATOK;
    *y2 = POCIATOK;
    int zacX = POCIATOK;
    int zacY = POCIATOK;
    int i = POCIATOK;
    int j = POCIATOK;
    for(j = POCIATOK; j < bitmap->cols; j++)
    {
        for(i = POCIATOK; i < bitmap->rows; i++)
        {
            if (getcolor(bitmap, i, j) == NEFARBA)      //nema farbu vynuluje sa dlzka
            {
                dlzkanovy = POCIATOK;
            }
            else if ((getcolor(bitmap, i, j) == FARBA) && (dlzkanovy == POCIATOK))     //ma farbu tak pocita dlzku
            {
                zacX = i;
                zacY = j;
                dlzkanovy++;
            }
            else
            {
                dlzkanovy++;
            }
            if (dlzkanovy == bitmap->rows)  //optimalizacia, ak sa dlzka rovna riadkom tak skonci hned pretoze vecsi nenajde
            {
                *x1 = zacX;
                *y1 = zacY;
                *x2 = i;
                *y2 = j;
                return SUCCES;
            }
            if ((dlzkanovy >= dlzkastary) && (*x1 > zacX))      //opatrenie na najdenie najvecieho s najmensimi suradnicami
            {
                *x1 = zacX;
                *y1 = zacY;
                *x2 = i;
                *y2 = j;
                dlzkastary = dlzkanovy;
            }
            if (dlzkanovy > dlzkastary)                     //vlozenie novych premenych ak je najdeny vecsi
            {
                *x1 = zacX;
                *y1 = zacY;
                *x2 = i;
                *y2 = j;
                dlzkastary = dlzkanovy;
            }
        }
        dlzkanovy = POCIATOK;
    }
    return SUCCES;
}

/*
 *Funkcia na otvorenie a uzavretie suboru
 */
int find_square(Bitmap *bitmap, int *x1, int *y1, int *x2, int *y2)
{
    int zacX = POCIATOK;
    int zacY = POCIATOK;
    int konX = POCIATOK;
    int konY = POCIATOK;
    *x1 = POCIATOK;
    *y1 = POCIATOK;
    *x2 = POCIATOK;
    *y2 = POCIATOK;
    int i = POCIATOK;
    int j = POCIATOK;
    int velkostnovy = POCIATOK;      //velkost noveho najdeneho stvorca
    int velkoststary = POCIATOK;     //velkost stareho najdeneho stvorca
    int a = POCIATOK;                //zmensenie hladaneho stvorca
    int d = POCIATOK;                //pomocna premena na jednotlive testovania po obvode
    int c;                           //pocetriadkov a stlpcov v aktualnom testovanom stvorci
    int neuspesne = POCIATOK;        //premnena na testovanie ci sa po obvode nachadzaju iba jedenotky
    for(i = POCIATOK; i < bitmap->rows; i++)
    {
        for(j = POCIATOK; j < bitmap->cols; j++)
        {
            if (bitmap->rows - i > bitmap->cols -j)     //ak je viac riadkov ako stlpcov
            {
                c = bitmap->cols - j;
            }
            else
            {
                c = bitmap->rows - i;
            }

            while (a < c)               //vytvoreneny stvorec ktory sa vzdy zmensuje
            {
                neuspesne = POCIATOK;
                if (bitmap->rows - i > bitmap->cols -j)     //nastavenie aktualnej dlzky strany
                {
                    velkostnovy = bitmap->cols -j - a - POSUN;
                }
                else
                {
                    velkostnovy = bitmap->rows - i - a - 1;
                }
                zacX = i;
                zacY = j;
                konX = i + velkostnovy;
                konY = j + velkostnovy;
                d = zacY;
                while(d <= konY)        //test usecky1
                {
                    if(getcolor(bitmap, zacX, d) == NEFARBA)
                    {
                        neuspesne = NEUSPES;
                        break;
                    }
                    d++;
                }
                d = zacX;
                while(d <= konX)        //test usecky2
                {
                    if(getcolor(bitmap, d, zacY) == NEFARBA)
                    {
                        neuspesne = NEUSPES;
                        break;
                    }
                    d++;
                }
                d = konX;
                while(d >= zacX)        //test useky3
                {
                    if(getcolor(bitmap, d, konY) == NEFARBA)
                    {
                        neuspesne = NEUSPES;
                        break;
                    }
                    d--;
                }
                d = konY;
                while(d >= zacY)        //test usecky 4
                {
                    if(getcolor(bitmap, konX, d) == NEFARBA)
                    {
                        neuspesne = NEUSPES;
                        break;
                    }
                    d--;
                }
                if (neuspesne == POCIATOK)      //ak su vsade jednotky po obvode
                {
                    if (velkostnovy > velkoststary)
                    {
                        *x1 = zacX;
                        *y1 = zacY;
                        *x2 = konX;
                        *y2 = konY;
                        velkoststary = velkostnovy;
                        break;
                    }

                }
                a++;
            }
            a = POCIATOK;

        }
    }
    return SUCCES;
}

/*
 *Funkcia na otvorenie a uzavretie suboru
 */
int test(char *menos, int mod)
{
    FILE *subor;
    if ((subor = fopen(menos, "r"))== NULL)       //nespravne otvoreny subor
    {
        fprintf(stderr,"Subor sa nepodarilo otvorit!\n");
        return FAULT;
    }
    else                                            //spravne otvoreny subor
    {
        Bitmap bitmap =
        {
            .rows = POCIATOK,
            .cols = POCIATOK,
            .cells = POCIATOK,
        };
        int i;      //na indexovanie
        int prvok;  //na ukladanie obsahu
        int spravne = POCIATOK;     //na zistenie ci je nacitany spravny pocet
        int pocets = POCIATOK;      //ci je bitmap taka ako zadana
        char a = '0';               //na zistenie EOFu
        int x1;
        int x2;
        int y1;
        int y2;
        if ((spravne = fscanf(subor,"%d %d", &bitmap.rows, &bitmap.cols)) != 2) //test ci su tam iba cisla
        {
            printf("Invalid\n");
            free(bitmap.cells);
            fclose(subor);
            return FAULT;
        }
        bitmap.cells = (char *)malloc(sizeof(char) * bitmap.rows * bitmap.cols);    //alokovanie pamete
        for (i = POCIATOK; i < bitmap.rows * bitmap.cols; i++)      //naplnanie matice a testovanie po prvkoch
        {
            spravne = POCIATOK;

            if ((spravne = fscanf(subor,"%d",&prvok)) == 1)         //test ci tam je ciselna hodnota
            {
                pocets++;
            }
            else
            {
                printf("Invalid\n");
                free(bitmap.cells);
                fclose(subor);
                return FAULT;
            }
            bitmap.cells[i] = prvok;
            if (bitmap.cells[i] != FARBA && bitmap.cells[i] != NEFARBA)     //ina honota ako 0 a 1
            {
                printf("Invalid\n");
                free(bitmap.cells);
                fclose(subor);
                return FAULT;
            }
        }
        while ((a = getc(subor)) != EOF)    //testovanie znakov po matici
        {
            if (a > ' ')
            {
                printf("Invalid\n");
                free(bitmap.cells);
                fclose(subor);
                return FAULT;
            }
        }
        if (pocets != (bitmap.rows * bitmap.cols))  //riadkov alebo stlpcov je menej
        {
            printf("Invalid\n");
            free(bitmap.cells);
            fclose(subor);
            return FAULT;
        }
        if (mod == TESTONLY)    //spustenie testu
        {
            printf("Valid\n");
        }
        else if (mod == HLINE)  //spustenie hline
        {
            find_hline(&bitmap, &x1, &y1, &x2, &y2);
            if ((x1 == x2) && (y1 == y2))
            {
                printf("V matici sa nenachadza hladany obrazec!\n");
                free(bitmap.cells);
                fclose(subor);
                return SUCCES;
            }
            printf("%d %d %d %d\n", x1, y1, x2, y2);
        }
        else if (mod == VLINE) //spustenie vline
        {
            find_vline(&bitmap, &x1, &y1, &x2, &y2);
            if ((x1 == x2) && (y1 == y2))
            {
                printf("V matici sa nenachadza hladany obrazec!\n");
                free(bitmap.cells);
                fclose(subor);
                return SUCCES;
            }
            printf("%d %d %d %d\n", x1, y1, x2, y2);
        }
        else if (mod == SQUARE)     //spustenie square
        {
            find_square(&bitmap, &x1, &y1, &x2, &y2);
            if ((x1 == x2) && (y1 == y2))
            {
                printf("V matici sa nenachadza hladany obrazec!\n");
                free(bitmap.cells);
                fclose(subor);
                return SUCCES;
            }
            printf("%d %d %d %d\n", x1, y1, x2, y2);
        }
        if (fclose(subor) == EOF)           //nepodarene zavretie
        {
        fprintf(stderr,"Subor sa nepodarilo zavriet!\n");
        }
        free(bitmap.cells);                 //freee
    }
    return SUCCES;
}

int help()
{
    printf("Hladanie obrazcov 2013 \nVytvoril Filip Gulan (xgulan00) \n \n \nVitajte v napovede k programu Hladanie obrazcov 2013 vytvoreny v ramci 3 projektu pre premet IZP. \n \n");
    printf("Program sluzi na hladnie najvecsej vodorovnej a zvyslej ciary.\n \n");
    printf("Program spustajte v nasleduucej podobe: ./proj2 --help , ./proj3 --test soubor.txt , ./proj3 --hline obrazek.txt , ./proj3 --vline obrazek.txt alebo ./proj3 --square obrazek.txt\n \n");
    printf("Popis argumentov: \n \n--help  vypise na obrazovku napovedu k programu\n \n");
    printf("--hline X    Najde najdlhsiu vodorovnu ciaru z suboru X, ktory je dany uzivaatelom a vypise jej zaciatocne a koncove suradnice.\n \n");
    printf("--vline X    Najde najdlhsiu zvyslu ciaru z suboru X, ktory je dany uzivaatelom a vypise jej zaciatocne a koncove suradnice. \n \n");
    printf("--square    Najde a vypise zaciatocne a koncove suradnice najvecsieho stvorca\n");
    return SUCCES;
}

int main(int argc, char *argv[])
{
    if (argc == 2)
    {
        if (strcmp(argv[1], "--help") == 0)
        {
            help();
            return SUCCES;
        }
        else
        {
            fprintf(stderr,"Argumenty neboli zadane spravne!\n");
            return FAULT;
        }

    }
    if (argc == 3)
    {
        if (strcmp(argv[1], "--test") == 0)
        {
            test(argv[2], TESTONLY);
        }
        else if (strcmp(argv[1], "--hline") == 0)
        {
            test(argv[2], HLINE);
        }
        else if (strcmp(argv[1], "--vline") == 0)
        {
            test(argv[2], VLINE);
        }
        else if (strcmp(argv[1], "--square") == 0)
        {
            test(argv[2], SQUARE);
        }
        else
        {
            fprintf(stderr,"Argumenty neboli zadane spravne!\n");
            return FAULT;
        }
    }
    else
    {
        fprintf(stderr,"Argumenty neboli zadane spravne!\n");
        return FAULT;
    }
    return SUCCES;
}
