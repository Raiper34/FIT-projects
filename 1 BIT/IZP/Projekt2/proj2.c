/*
 * Soubor:  main.c
 * Datum:   25.11.2013
 * Autor:   1BIA - Filip Gulan, xgulan00@stud.fit.vutbr.cz
 * Projekt: 1BIT cislo 2: Iteracne vypocty
 * Popis viz:
        https://wis.fit.vutbr.cz/FIT/st/cwk.php?title=IZP:Projekt2&csid=538331&id=9416
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PIPOL (1.57079632679489661923)   //PI/2
#define PI (3.14159265358979323846264)   //PI/2
#define EPS (0.000000000001)     //presnost vypoctu pri asine
#define EPSTROJ (0.00000000000000000001)     //presnost vypoctu pri asine vo vypocte uhlov trojuholniku
#define OBRATENEZNAM ((-1))     //na nasobenie aby som dostal abs hodnotu
#define DLZKAPOLA (6)           //dlzka pola pre ukladanie argumentov
#define KLADNEC (0)             // na porovnavanie ci je kladne (aj 0) alebo zaporne
#define POCIATOK (1)            //pociatok rady a iteracie
#define HORNAHRANICA (1)        // horna hranica sinusu
#define DOLNAHRANICA (-1)       //dolna hranica sinusu
#define POSUNUTIE (2)           //posunutie aby bolo kvoli prvym 2 argumentom, ktore nechceme
#define INKREMENT (2)           //inkrementuje hodnotu o hodotu 2 pri asine
#define NIEJESTR (0)           //ak nieje strana
#define HORNAHPRES (0.9)       //horna hranica pre rozhodovanie vyberu presnosti
#define DOLNAHPRES (-0.9)       //dolna hranica pre rozhodovanie presnosti
#define INKREMENTPOC (-0.9)     //pociatocn stav inkrementu

/*
 * Funkcia na vypisanie napovedy programu
 */
int help()
{
    printf("Iteracne vypocty 2013 \nVytvoril Filip Gulan (xgulan00) \n \n \nVitajte v napovede k programu Iteracne vypocty 2013 vytvoreny v ramci 2 projektu pre premet IZP. \n \n");
    printf("Program sluzi na vypocitanie uhlov vseobecneho trojuholnika.\n \n");
    printf("Program spustajte v nasleduucej podobe: ./proj2 --help , ./proj2 --sqrt X , ./proj2 --asin X alebo ./proj2 --triangle AX AY BX BY CX CY\n \n");
    printf("Popis argumentov: \n \n--help  vypise na obrazovku napovedu k programu\n \n");
    printf("--sqrt X    Vypocita a vypise druhu odmocninu z vami zadaneho cisla X. X musi byt kladne cislo! \n \n");
    printf("--asin X    Vypocita a vypise inverznu funkciu k funkcii sinus, teda arcus sinus. X je v intervale <1,-1>. Vysledok operacie je v radianoch. \n \n");
    printf("--triangle AX AY BX BY CX CY    Vypocita a vypise 3 uhly trojuholnika, kotry je dany vrcholmi A=AX,AY, B=BX,BY a C=CX,CY. Vysledok operacie je v radianoch.\n");
    return 0;
}

/*
 * Funkcia na absolutnu hodnotu, v pripade zaporneho cisla vynasobi -1
 */
double my_abs(double absolutnay)
{
    if (absolutnay < KLADNEC)
    {
        absolutnay = absolutnay * OBRATENEZNAM;
    }
    return absolutnay;
}

/*
 * Funkcia na pocitanie druhej odmocniny zo zadaneho cisla pomocou newtonovej metody
 */
double my_sqrt(double x)
{
    double starey = POCIATOK;
    double novey = (x / starey + starey) / 2;
    while((my_abs(starey - novey)) >= my_abs(EPS * novey))
    {
        starey = novey;
        novey = (x / starey + starey) / 2;
    }
    return novey;
}

/*
 * Funkcia na vypocitanie inverznej funkcie sinusu.
 * Pocita pomocou taylorovho polynomu
 * Vysledok vyjde v radianoch.
 */
double my_asin(double x, double presnost)
{
    double nove = x;
    double stare = POCIATOK;
    double a = POCIATOK;
    long i = INKREMENTPOC;
    double b = x;
    double xx = x * x;
    if (x == HORNAHRANICA)
    {
        nove = PIPOL;
    }
    else if (x == DOLNAHRANICA)
    {
        nove = OBRATENEZNAM * PIPOL;
    }
    else if (x == 0)
    {
        nove = 0;
    }
    else
    {
        if (x < HORNAHPRES && x > DOLNAHPRES)   //normalne pocitanie pomocou taylorovej rady pre cisla z intervalu <0.9,-0.9>
        {
            while(my_abs(nove - stare) >= (presnost * my_abs(nove)))
            {
                stare = nove;
                a = a * (1.0 + i) / (2.0 + i);
                b = b * xx;
                nove = nove + (a * (b / (3.0 + i)));
                i = i + INKREMENT;
            }
        }
        else        //presnejsie pocitanie na zmensenie poctu iteraci pre cisla z intervalu vecsie/mensie ako 0.9/-0.9
        {
            nove = PIPOL - my_asin(my_sqrt(1 - (x*x)), EPS);
            if (x < KLADNEC)
            {
                nove = nove * OBRATENEZNAM;
            }
        }

    }
	return nove;
}

/*
 * Funkcia na vypocet velkosti uhlov trojuholnika.
 * Pomocou pytagorovej vety ziskame strany a.b,c.
 * Pomocou vztahu Pi/2 - arcus sinus ziskame arcus cosinus (alfa, beta, gama)
 */
double triangle(double ax, double ay, double bx, double by, double cx, double cy)
{
    double c = my_sqrt(my_abs(((bx - ax) * (bx - ax))  + ((by - ay) * (by - ay))));
    double b = my_sqrt(my_abs(((cx - ax) * (cx - ax))  + ((cy - ay) * (cy - ay))));
    double a = my_sqrt(my_abs(((cx - bx) * (cx - bx))  + ((cy - by) * (cy - by))));
    if (a + b <= c || a + c <= b || b + c <= a || a == NIEJESTR || b == NIEJESTR || c == NIEJESTR || c != c || b != b || a != a)    //test trojholnikovej nerovnocsti/nulovej strany a nan-u
    {
    	printf("nan\nnan\nnan\n");
    	return 0;
    }
    else
    {
    	double alfa = (PIPOL) - (my_asin(((b * b) + (c * c) - (a * a)) / (2.0 * b * c), EPSTROJ));
        double beta = (PIPOL) - (my_asin(((a * a) + (c * c) - (b * b)) / (2.0 * a * c), EPSTROJ));
        double gama = PI - alfa - beta;
        printf("%.10e\n%.10e\n%.10e\n", alfa, beta, gama);
    }

    return 0;
}

/*
 * Funkcia na spracovanie argumentov
 */
int main(int argc,char *argv[])
{
    if (argc == 2)	//ak su argumenty 2
    {
        if (strcmp(argv[1],"--help") == 0)
        {
            help();
        }
        else
        {
            printf("Argumenty neboli zadane spravne!\n");
            return 1;
        }
    }
    else if (argc == 3)		//ak su argumenty 3
    {
        char *chyba;
        char *argument = argv[2];
        double cislo;
        cislo = strtod(argument, &chyba);
        if (chyba == argument)
        {
        printf("Argumenty neboli zadane spravne!\n");
        return 1;
        }
        else if (*chyba != '\n' && *chyba != 0)
        {
        printf("Argumenty neboli zadane spravne!\n");
        return 1;
        }
        else
        {
            if (strcmp(argv[1],"--sqrt") == 0)		//sqrt, odmocnina
            {
                if (cislo < KLADNEC)
                {
                    printf("nan\n");
                }
                else
                {
                    double sqrtvar = my_sqrt(cislo);
                    printf("%.10e\n", sqrtvar);
                }
            }
            else if (strcmp(argv[1],"--asin") == 0)		//asinus pocitnie arcus sinus
            {
                if (cislo < DOLNAHRANICA || cislo > HORNAHRANICA)
                {
                    printf("nan\n");
                }
                else
                {
                    double asinvar = my_asin(cislo, EPS);
                    printf("%.10e\n", asinvar);
                }
            }
            else
            {
                printf("Argumenty neboli zadane spravne!\n");
                return 1;
            }
        }
    }
    else if (argc == 8)		//ak je argumentov 8
    {
        if (strcmp(argv[1],"--triangle") == 0)		//triangle pocitanie uhlov
        {
            int i = 0;
            double pole[6];
            char *chyba2;
            char *argument2;
            while (i < DLZKAPOLA)		// naplnenie pola jednotlivymi argumentami pre lahsiu pracu
            {
                argument2 = argv[i + POSUNUTIE];
                pole[i] = strtod(argument2, &chyba2);
                if (chyba2 == argument2)
                {
                    printf("Argumenty neboli zadane spravne!\n");
                    return 1;
                }
                if (*chyba2 != '\n' && *chyba2 != 0)
                {
                    printf("Argumenty neboli zadane spravne!\n");
                    return 1;
                }
                i++;
            }
            triangle(pole[0], pole[1], pole[2], pole[3], pole[4], pole[5]);
        }
    }
    else
    {
        printf("Argumenty neboli zadane spravne!\n");
        return 1;
    }
    return 0;
}
