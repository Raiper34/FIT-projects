/******************************************************************************
 * Projekt - Zaklady pocitacove grafiky - IZG
 * spanel@fit.vutbr.cz
 *
 * $Id:$
 */

#ifndef Student_H
#define Student_H

/******************************************************************************
 * Includes
 */

#include "render.h"
#include "fragment.h"
#include "vector_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
 * Studentsky renderer, do ktereho muzete dopisovat svuj kod
 */

/* Typ/ID rendereru */
extern const int STUDENT_RENDERER;

/* Jadro vaseho rendereru */
typedef struct S_StudentRenderer
{
    /* Kopie default rendereru
     * Typicky trik jak implementovat "dedicnost" znamou z C++
     * Ukazatel na strukturu lze totiz pretypovat na ukazatel
     * na prvni prvek struktury a se strukturou S_StudentRenderer
     * tak pracovat jako s S_Renderer... */
    S_Renderer  base;
    
    /* Zde uz muzete doplnovat svuj kod dle libosti */
    /* ??? */
    struct S_FragVec *node_buffer;
    int *head_pointer_buffer; //budem malokovat miesto, je to jendorozmerne pole integerov, aby bolo 2 rozmerne tak mapujem  
} S_StudentRenderer;


/******************************************************************************
 * Nasledujici fce budete nejspis muset re-implementovat podle sveho
 */

/* Funkce vytvori vas renderer a nainicializuje jej */
S_Renderer * studrenCreate();

/* Funkce korektne zrusi renderer a uvolni pamet */
void studrenRelease(S_Renderer **ppRenderer);

/* Funkce inicializuje buffery ve vasem rendereru */
void studrenCreateBuffers(S_Renderer *pRenderer, int width, int height);

/* Funkce vycisti buffery ve vasem rendereru pred kreslenim noveho framu */
void studrenClearBuffers(S_Renderer *pRenderer);

/* Nova fce pro rasterizaci trojuhelniku s podporou vykreslovani s pruhlednosti
 * v1, v2, v3 - ukazatele na vrcholy trojuhelniku ve 3D pred projekci
 * n1, n2, n3 - ukazatele na normaly ve vrcholech ve 3D pred projekci
 * x1, y1, ... - vrcholy trojuhelniku po projekci do roviny obrazovky */
void studrenDrawTriangle(S_Renderer *pRenderer,
                         S_Coords *v1, S_Coords *v2, S_Coords *v3,
                         S_Coords *n1, S_Coords *n2, S_Coords *n3,
                         int x1, int y1,
                         int x2, int y2,
                         int x3, int y3
                         );

/* Vykresli i-ty trojuhelnik modelu pomoci nove fce studrenDrawTriangle()
 * Pred vykreslenim aplikuje na vrcholy a normaly trojuhelniku
 * aktualne nastavene transformacni matice!
 * i - index trojuhelniku */
void studrenProjectTriangle(S_Renderer *pRenderer, S_Model *pModel, int i);


/******************************************************************************
 * Deklarace fci, ktere jsou umistene ve student.c a pouzivaji se v main.c
 */

/* Funkce pro vyrenderovani modelu slozeneho z trojuhelniku (viz main.c) */
void renderModel(S_Renderer *pRenderer, S_Model *pModel);

/* Upravena funkce pro vyrenderovani cele sceny, tj. vykresleni modelu */
void renderStudentScene(S_Renderer *pRenderer, S_Model *pModel);


#ifdef __cplusplus
}
#endif

#endif /* Student_H */

/*****************************************************************************/
/*****************************************************************************/
