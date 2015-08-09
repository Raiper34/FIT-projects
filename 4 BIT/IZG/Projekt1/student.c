/******************************************************************************
 * Projekt - Zaklady pocitacove grafiky - IZG
 * spanel@fit.vutbr.cz
 *
 * $Id: student.c 416 2015-03-10 08:07:46Z spanel $
 */

#include "student.h"
#include "transform.h"
#include "fragment.h"

#include <memory.h>
#include <math.h>


/*****************************************************************************
 * Globalni promenne a konstanty
 */

/* Typ/ID rendereru (nemenit) */
const int           STUDENT_RENDERER = 1;

/* Castecne pruhledny material */
const S_Material    MAT_STUD_AMBIENT  = { 0.2, 0.8, 0.2, 0.4 };
const S_Material    MAT_STUD_DIFFUSE  = { 0.2, 0.8, 0.2, 0.4 };
const S_Material    MAT_STUD_SPECULAR = { 0.8, 0.8, 0.8, 1.0 };


/*****************************************************************************
 * Funkce vytvori vas renderer a nainicializuje jej
 */

S_Renderer * studrenCreate()
{
    S_StudentRenderer * renderer = (S_StudentRenderer *)malloc(sizeof(S_StudentRenderer));
    IZG_CHECK(renderer, "Cannot allocate enough memory");

    /* inicializace default rendereru */
    renderer->base.type = STUDENT_RENDERER;
    renInit(&renderer->base);

    /* nastaveni ukazatelu na upravene funkce */
    /* napr. renderer->base.releaseFunc = studrenRelease; */
    /* ??? */
    /////////////////////////////////////////////////////////
    renderer->base.projectTriangleFunc = studrenProjectTriangle; //prepisanie funkci na moje funkcie
    renderer->base.createBuffersFunc = studrenCreateBuffers;
    renderer->base.clearBuffersFunc = studrenClearBuffers;
    renderer->base.releaseFunc = studrenRelease;

    /* inicializace nove pridanych casti */
    /* ??? */
    renderer->node_buffer = vecCreateEmpty(sizeof(S_Frag));
    renderer->head_pointer_buffer = NULL;
    //printf("a");
    /////////////////////////////////////////////////////////

    return (S_Renderer *)renderer;
}

/*****************************************************************************
 * Funkce korektne zrusi renderer a uvolni pamet
 */

void studrenRelease(S_Renderer **ppRenderer)
{
    S_StudentRenderer * renderer;

    if( ppRenderer && *ppRenderer )
    {
        /* ukazatel na studentsky renderer */
        renderer = (S_StudentRenderer *)(*ppRenderer);

        /* pripadne uvolneni pameti */
        /* ??? */
        /////////////////////////////////////////////////////////
        vecRelease(&(renderer->node_buffer));
        if(renderer->head_pointer_buffer)
        {
            free(renderer->head_pointer_buffer);
            renderer->head_pointer_buffer = NULL;
        }
        //printf("b");
        /////////////////////////////////////////////////////////

        /* fce default rendereru */
        renRelease(ppRenderer);
    }
}

/*****************************************************************************
 * Funkce inicializuje buffery ve vasem rendereru
 */

void studrenCreateBuffers(S_Renderer *pRenderer, int width, int height)
{
    S_StudentRenderer   * renderer;

    IZG_ASSERT(pRenderer && width > 0 && height > 0);

    /* ukazatel na studentsky renderer */
    renderer = (S_StudentRenderer *)pRenderer;

    /* alokace pameti pro buffery a vymazani obsahu bufferu */
    /* ??? */
    /////////////////////////////////////////////////////////
    //printf("c");
    renderer->head_pointer_buffer = malloc(sizeof(int) * width * height);
    int i = 0;
    for(i = 0; i < width*height; i++) //inicializujem si head pointer buffer samymi -1
    {
        renderer->head_pointer_buffer[i] = -1;
    }
    vecInit(renderer->node_buffer, sizeof(S_Frag));
    /////////////////////////////////////////////////////////

    /* zavolame take puvodni funkci */
    renCreateBuffers(pRenderer, width, height);
}

/*****************************************************************************
 * Funkce vycisti buffery ve vasem rendereru pred kreslenim noveho framu
 */

void studrenClearBuffers(S_Renderer *pRenderer)
{
    S_StudentRenderer   * renderer;

    IZG_ASSERT(pRenderer);

    /* ukazatel na studentsky renderer */
    renderer = (S_StudentRenderer *)pRenderer;

    /* vymazeme a inicializujeme buffery */
    /* ??? */
    /////////////////////////////////////////////////////////
    int i = 0;
    for(i = 0; i < renderer->base.frame_w * renderer->base.frame_h; i++) //to iste ako hore
    {
        renderer->head_pointer_buffer[i] = -1;
    }
    vecClear(renderer->node_buffer);
    vecInit(renderer->node_buffer, sizeof(S_Frag));
    //printf("d");
    /////////////////////////////////////////////////////////

    /* zavolame take puvodni funkci */
    renClearBuffers(pRenderer);
}

/******************************************************************************
 * Nova fce pro rasterizaci trojuhelniku s podporou vykreslovani s pruhlednosti
 * v1, v2, v3 - ukazatele na vrcholy trojuhelniku ve 3D pred projekci
 * n1, n2, n3 - ukazatele na normaly ve vrcholech ve 3D pred projekci
 * x1, y1, ... - vrcholy trojuhelniku po projekci do roviny obrazovky
 */

void studrenDrawTriangle(S_Renderer *pRenderer,
                         S_Coords *v1, S_Coords *v2, S_Coords *v3,
                         S_Coords *n1, S_Coords *n2, S_Coords *n3,
                         int x1, int y1,
                         int x2, int y2,
                         int x3, int y3
                         )
{
    /* zaklad fce zkopirujte z render.c */
    /* ??? */
    int         minx, miny, maxx, maxy;
    int         a1, a2, a3, b1, b2, b3, c1, c2, c3;
    int         /*s1,*/ s2, s3;
    int         x, y, e1, e2, e3;
    double      alpha, beta, w1, w2, w3, z;
    S_RGBA      col1, col2, col3, color;

    IZG_ASSERT(pRenderer && v1 && v2 && v3 && n1 && n2 && n3);

    /* vypocet barev ve vrcholech */
    col1 = pRenderer->calcReflectanceFunc(pRenderer, v1, n1);
    col2 = pRenderer->calcReflectanceFunc(pRenderer, v2, n2);
    col3 = pRenderer->calcReflectanceFunc(pRenderer, v3, n3);

    /* obalka trojuhleniku */
    minx = MIN(x1, MIN(x2, x3));
    maxx = MAX(x1, MAX(x2, x3));
    miny = MIN(y1, MIN(y2, y3));
    maxy = MAX(y1, MAX(y2, y3));

    /* oriznuti podle rozmeru okna */
    miny = MAX(miny, 0);
    maxy = MIN(maxy, pRenderer->frame_h - 1);
    minx = MAX(minx, 0);
    maxx = MIN(maxx, pRenderer->frame_w - 1);

    /* vektory urcene vrcholy 1-2 a 1-3 */
    a1 = x2 - x1;
    a3 = x3 - x1;
    b1 = y2 - y1;
    b3 = y3 - y1;

    /* overeni counterclockwise orientace troj. pomoci vektoroveho soucinu */
    if( (a1 * b3 - b1 * a3) < 0 )
    {
        /* spatna orientace -> prohodime vrcholy 2 a 3 */
        SWAP(x2, x3);
        SWAP(y2, y3);

        /* a take barvy vrcholu */
        SWAP(col2.red, col3.red);
        SWAP(col2.green, col3.green);
        SWAP(col2.blue, col3.blue);
        SWAP(col2.alpha, col3.alpha);
    }

    /* Pineduv alg. rasterizace troj.
       hranova fce je obecna rovnice primky Ax + By + C = 0
       primku prochazejici body (x1, y1) a (x2, y2) urcime jako
       (y1 - y2)x + (x2 - x1)y + x1y2 - x2y1 = 0 */

    /* normala primek - vektor kolmy k vektoru mezi dvema vrcholy, tedy (-dy, dx) */
    a1 = y1 - y2;
    a2 = y2 - y3;
    a3 = y3 - y1;
    b1 = x2 - x1;
    b2 = x3 - x2;
    b3 = x1 - x3;

    /* koeficient C */
    c1 = x1 * y2 - x2 * y1;
    c2 = x2 * y3 - x3 * y2;
    c3 = x3 * y1 - x1 * y3;

    /* vypocet hranove fce (vzdalenost od primky) pro protejsi body */
    /*s1 = a1 * x3 + b1 * y3 + c1;*/
    s2 = a2 * x1 + b2 * y1 + c2;
    s3 = a3 * x2 + b3 * y2 + c3;

    /* uprava koeficientu C pro korektni vyplnovani, viz "OpenGL top-left rule" */
    /* https://books.google.cz/books?id=3ljRBQAAQBAJ&pg=PA73 */
    if( (y1 == y2 && x2 > x1) || y2 < y1 )
    {
        c1 -= 1;
    }
    if( (y2 == y3 && x3 > x2) || y3 < y2 )
    {
        c2 -= 1;
    }
    if( (y3 == y1 && x1 > x3) || y1 < y3 )
    {
        c3 -= 1;
    }

    /* koeficienty pro barycentricke souradnice */
    alpha = 1.0 / (ABS(s2) + 1);
    beta = 1.0 / (ABS(s3) + 1);

    //////////////////////////////////////////////////
    S_StudentRenderer   * renderer; ///////////////////////////////////////////////////////////////////////////////
    renderer = (S_StudentRenderer *)pRenderer;
    /////////////////////////////////////////////////

    /* vyplnovani... */
    for( y = miny; y <= maxy; ++y )
    {
        /* inicilizace hranove fce v bode (minx, y) */
        e1 = a1 * minx + b1 * y + c1;
        e2 = a2 * minx + b2 * y + c2;
        e3 = a3 * minx + b3 * y + c3;

        for( x = minx; x <= maxx; ++x )
        {
            if( e1 >= 0 && e2 >= 0 && e3 >= 0 )
            {
                /* interpolace pomoci barycentrickych souradnic
                   e1, e2, e3 je aktualni vzdalenost bodu (x, y) od primek */
                w1 = alpha * e2;
                w2 = beta * e3;
                w3 = 1.0 - w1 - w2;

                /* interpolace z-souradnice */
                z = w1 * v1->z + w2 * v2->z + w3 * v3->z;
                
                /* interpolace barvy */
                color.red = ROUND2BYTE(w1 * col1.red + w2 * col2.red + w3 * col3.red);
                color.green = ROUND2BYTE(w1 * col1.green + w2 * col2.green + w3 * col3.green);
                color.blue = ROUND2BYTE(w1 * col1.blue + w2 * col2.blue + w3 * col3.blue);
                color.alpha = ROUND2BYTE(w1 * col1.alpha + w2 * col2.alpha + w3 * col3.alpha);

                /* vykresleni bodu */
                //if( z < DEPTH(pRenderer, x, y) )
                //{
                //    PIXEL(pRenderer, x, y) = color;
                //    DEPTH(pRenderer, x, y) = z;
                //}
                /////////////////////////////////////////////
                if(z < DEPTH(pRenderer, x, y))
                {
                    if(color.alpha == 255.0) //ak je objekt nepriehlandy, tak ho hned vykreslim
                    {
                        PIXEL(pRenderer, x, y) = color;
                        DEPTH(pRenderer, x, y) = z;
                    }
                    else //inak naplnam buffre
                    {
                        int index = (y * renderer->base.frame_w) + x; //namapujem si suradnicu
                        int hodnotaIndexu = renderer->head_pointer_buffer[index]; //ziskam hodnotu na danej suradnici
                        S_Frag fragmentik = makeFrag(color, z, hodnotaIndexu); //urobim fragment
                        int novyindex = vecPushBack(renderer->node_buffer, &fragmentik); //ulozim fragment
                        renderer->head_pointer_buffer[index] = novyindex; //aktualizujem index
                    }
                }
                /////////////////////////////////////////////
            }

            /* hranova fce o pixel vedle */
            e1 += a1;
            e2 += a2;
            e3 += a3;
        }
    }
}

/******************************************************************************
 * Vykresli i-ty trojuhelnik modelu pomoci nove fce studrenDrawTriangle()
 * Pred vykreslenim aplikuje na vrcholy a normaly trojuhelniku
 * aktualne nastavene transformacni matice!
 * i - index trojuhelniku
 */

void studrenProjectTriangle(S_Renderer *pRenderer, S_Model *pModel, int i)
{
    /* zaklad fce zkopirujte z render.c */
    /* ??? */
    S_Coords    aa, bb, cc;             /* souradnice vrcholu po transformaci */
    S_Coords    naa, nbb, ncc;          /* normaly ve vrcholech po transformaci */
    S_Coords    nn;                     /* normala trojuhelniku po transformaci */
    int         u1, v1, u2, v2, u3, v3; /* souradnice vrcholu po projekci do roviny obrazovky */
    S_Triangle  * triangle;

    IZG_ASSERT(pRenderer && pModel && i >= 0 && i < trivecSize(pModel->triangles));

    /* z modelu si vytahneme trojuhelnik */
    triangle = trivecGetPtr(pModel->triangles, i);

    /* transformace vrcholu matici model */
    trTransformVertex(&aa, cvecGetPtr(pModel->vertices, triangle->v[0]));
    trTransformVertex(&bb, cvecGetPtr(pModel->vertices, triangle->v[1]));
    trTransformVertex(&cc, cvecGetPtr(pModel->vertices, triangle->v[2]));

    /* promitneme vrcholy trojuhelniku na obrazovku */
    trProjectVertex(&u1, &v1, &aa);
    trProjectVertex(&u2, &v2, &bb);
    trProjectVertex(&u3, &v3, &cc);

    /* pro osvetlovaci model transformujeme take normaly ve vrcholech */
    trTransformVector(&naa, cvecGetPtr(pModel->normals, triangle->v[0]));
    trTransformVector(&nbb, cvecGetPtr(pModel->normals, triangle->v[1]));
    trTransformVector(&ncc, cvecGetPtr(pModel->normals, triangle->v[2]));

    /* normalizace normal */
    coordsNormalize(&naa);
    coordsNormalize(&nbb);
    coordsNormalize(&ncc);

    /* transformace normaly trojuhelniku matici model */
    trTransformVector(&nn, cvecGetPtr(pModel->trinormals, triangle->n));
    
    /* normalizace normaly */
    coordsNormalize(&nn);

    /* je troj. privraceny ke kamere, tudiz viditelny? */
    if( !renCalcVisibility(pRenderer, &aa, &nn) )
    {
        /* odvracene troj. vubec nekreslime */
        return;
    }

    /* rasterizace trojuhelniku */
    //////////////////////////////////////
    studrenDrawTriangle(pRenderer,
                    &aa, &bb, &cc,
                    &naa, &nbb, &ncc,
                    u1, v1, u2, v2, u3, v3
                    );
    //////////////////////////////////////
}

/******************************************************************************
 ******************************************************************************
 * Funkce pro vyrenderovani sceny, tj. vykresleni modelu
 * Upravte tak, aby se model vykreslil s pruhlednosti a materialem
 */

void renderStudentScene(S_Renderer *pRenderer, S_Model *pModel)
{
    /* zaklad fce zkopirujte z main.c */
    /* ??? */
    /* test existence frame bufferu a modelu */
    IZG_ASSERT(pModel && pRenderer);

    /* nastavit projekcni matici */
    trProjectionPerspective(pRenderer->camera_dist, pRenderer->frame_w, pRenderer->frame_h);

    /* vycistit model matici */
    trLoadIdentity();

    /* nejprve nastavime posuv cele sceny od/ke kamere */
    trTranslate(0.0, 0.0, pRenderer->scene_move_z);

    /* nejprve nastavime posuv cele sceny v rovine XY */
    trTranslate(pRenderer->scene_move_x, pRenderer->scene_move_y, 0.0);

    /* natoceni cele sceny - jen ve dvou smerech - mys je jen 2D... :( */
    trRotateX(pRenderer->scene_rot_x);
    trRotateY(pRenderer->scene_rot_y);

    ///////////////////////////////////////////
    S_StudentRenderer   * renderer; ///////////////////////////////////////////////////////////////////////////////
    renderer = (S_StudentRenderer *)pRenderer;

    S_Matrix matica; //pomocna premena na ulozenie matice aby sa mi nezmenila
    trGetMatrix(&matica); //ulozime si maticu
    trTranslate(-1.1, 0.0, 0.0); //posunieme model dolava
    ///////////////////////////////////////////

    /* nastavime material */
    renMatAmbient(pRenderer, &MAT_RED_AMBIENT);
    renMatDiffuse(pRenderer, &MAT_RED_DIFFUSE);
    renMatSpecular(pRenderer, &MAT_RED_SPECULAR);

    /* a vykreslime nas model */
    renderModel(pRenderer, pModel);

    /////////////////////////////////////////////
    trSetMatrix(&matica); //nacitame si ulozenu maticu
    trTranslate(1.1, 0.0, 0.0); //posunieme model doprava
    renMatAmbient(pRenderer, &MAT_STUD_AMBIENT); //nastavime materialy
    renMatDiffuse(pRenderer, &MAT_STUD_DIFFUSE);
    renMatSpecular(pRenderer, &MAT_STUD_SPECULAR);
    renderModel(pRenderer, pModel); //vykreslime druhy objekt
    ////////////////////////////////////////////
    typedef struct R_RGBA //pomocna stuktura aby som mohol pracovat s double
    {
        double red;
        double green;
        double blue;
        double alpha;
    } R_RGBA;

    int x,y; //aby som mohol rpechadza po znadlivo 2d matici
    for( y = 0; y < renderer->base.frame_h; ++y ) //stlpce
    {
        for( x = 0; x < renderer->base.frame_w; ++x ) //riadky
        {
            int index = (y * renderer->base.frame_w) + x;
            int hodnotaIndexu = renderer->head_pointer_buffer[index];
            
            if(hodnotaIndexu != -1) //ak je hodnota -1, tak tam nc neni nic nerobim
            {
                S_Frag fragmentik1 = fragvecGet(renderer->node_buffer, hodnotaIndexu); //ulzoim si fragment 
                while(fragmentik1.next != -1) //prejdem vsekty fragmenty
                {
                    S_Frag fragmentik2 = fragvecGet(renderer->node_buffer, hodnotaIndexu); //ulozim si znovu fragment
                    while(fragmentik2.next != -1) //prejdem vsetky fragmenty
                    {
                        if(fragmentik1.depth > fragmentik2.depth) //podmienka na zoradenie, pouzivam uplne najnaivnejsi algoritmus
                        {
                            //iba prehadzujem v pripade ze to neni zoradene
                            R_RGBA pomocnacolor;
                            pomocnacolor.red = fragmentik1.color.red;
                            pomocnacolor.green = fragmentik1.color.green;
                            pomocnacolor.blue = fragmentik1.color.blue;
                            pomocnacolor.alpha = fragmentik1.color.alpha;
                            fragmentik1.color = fragmentik2.color;

                            fragmentik2.color.red = pomocnacolor.red;
                            fragmentik2.color.blue = pomocnacolor.blue;
                            fragmentik2.color.green = pomocnacolor.green;
                            fragmentik2.color.alpha = pomocnacolor.alpha;

                            double pomocnadepth = fragmentik1.depth;
                            fragmentik1.depth = fragmentik2.depth;
                            fragmentik2.depth = pomocnadepth;
                        }
                        fragmentik2 = fragvecGet(renderer->node_buffer, fragmentik2.next); //presuniem sa na dalsi fragment
                    }
                    fragmentik1 = fragvecGet(renderer->node_buffer, fragmentik1.next); //presuniem sa na dalsi fragment
                }
                fragmentik1 = fragvecGet(renderer->node_buffer, hodnotaIndexu);
                R_RGBA dest; dest.red = 0.0; dest.blue = 0.0; dest.green = 0.0; dest.alpha = 1.0; //pocitocne inicializacie
                R_RGBA src;
                while(fragmentik1.next != -1) //idem rata podla vzorcov zozadania
                {
                    src.red = fragmentik1.color.red;
                    src.green = fragmentik1.color.green;
                    src.blue = fragmentik1.color.blue;
                    src.alpha = fragmentik1.color.alpha;

                    src.alpha = src.alpha/255.0; //alfu vydelim 255 aby som bol v rozmedzi 0-1
                    dest.red = dest.alpha*(src.red)+dest.red;
                    dest.blue = dest.alpha*(src.blue)+dest.blue;
                    dest.green = dest.alpha*(src.green)+dest.green;
                    dest.alpha = (1.0 - src.alpha)*dest.alpha;
                    fragmentik1 = fragvecGet(renderer->node_buffer, fragmentik1.next); //posuniem sa na dalsi a idem ratat jeho
                }
                //este posledne fragmenty pridam
                src.red = fragmentik1.color.red;
                src.green = fragmentik1.color.green;
                src.blue = fragmentik1.color.blue;
                src.alpha = fragmentik1.color.alpha;

                src.alpha = src.alpha/255.0;
                dest.red = dest.alpha*(src.red)+dest.red;
                dest.blue = dest.alpha*(src.blue)+dest.blue;
                dest.green = dest.alpha*(src.green)+dest.green;
                dest.alpha = (1.0 - src.alpha)*dest.alpha;

                //a nakoniec pridam background
                dest.red = dest.alpha*(pRenderer->frame_buffer[index].red)+dest.red;
                dest.blue = dest.alpha*(pRenderer->frame_buffer[index].blue)+dest.blue;
                dest.green = dest.alpha*(pRenderer->frame_buffer[index].green)+dest.green;
                dest.alpha = 1.0;

                //a teraz z mojej pomocnej aby som mohol pocitat s double pridam do original SRGB
                S_RGBA destFinal;
                destFinal.red = dest.red;
                destFinal.green = dest.green;
                destFinal.blue = dest.blue;
                destFinal.alpha = dest.alpha;
                PIXEL(pRenderer, x, y) = destFinal; //zapisem do frame bufferu boalaaaaa
            }
        }
    }
}


/*****************************************************************************
 *****************************************************************************/
