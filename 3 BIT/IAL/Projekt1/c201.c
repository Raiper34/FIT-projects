
/* c201.c *********************************************************************}
{* Téma: Jednosmìrný lineární seznam
**
**                     Návrh a referenèní implementace: Petr Pøikryl, øíjen 1994
**                                          Úpravy: Andrea Nìmcová listopad 1996
**                                                   Petr Pøikryl, listopad 1997
**                                Pøepracované zadání: Petr Pøikryl, bøezen 1998
**                                  Pøepis do jazyka C: Martin Tuèek, øíjen 2004
**	                                      Úpravy: Bohuslav Køena, øíjen 2014
**
** Implementujte abstraktní datový typ jednosmìrný lineární seznam.
** U¾iteèným obsahem prvku seznamu je celé èíslo typu int.
** Seznam bude jako datová abstrakce reprezentován promìnnou typu tList.
** Definici konstant a typù naleznete v hlavièkovém souboru c201.h.
** 
** Va¹ím úkolem je implementovat následující operace, které spolu s vý¹e
** uvedenou datovou èástí abstrakce tvoøí abstraktní datový typ tList:
**
**      xxxInitList ...... inicializace seznamu pøed prvním pou¾itím,
**      xxDisposeList ... zru¹ení v¹ech prvkù seznamu,
**      xxxInsertFirst ... vlo¾ení prvku na zaèátek seznamu,
**      xxxFirst ......... nastavení aktivity na první prvek,
**      xxxCopyFirst ..... vrací hodnotu prvního prvku,
**      xxDeleteFirst ... zru¹í první prvek seznamu,
**      xxPostDelete .... ru¹í prvek za aktivním prvkem,
**      xxPostInsert .... vlo¾í nový prvek za aktivní prvek seznamu,
**      xxxCopy .......... vrací hodnotu aktivního prvku,
**      xxxActualize ..... pøepí¹e obsah aktivního prvku novou hodnotou,
**      xxxSucc .......... posune aktivitu na dal¹í prvek seznamu,
**      xxxActive ........ zji¹»uje aktivitu seznamu.
**
** Pøi implementaci funkcí nevolejte ¾ádnou z funkcí implementovaných v rámci
** tohoto pøíkladu, není-li u dané funkce explicitnì uvedeno nìco jiného.
**
** Nemusíte o¹etøovat situaci, kdy místo legálního ukazatele na seznam 
** pøedá nìkdo jako parametr hodnotu NULL.
**
** Svou implementaci vhodnì komentujte!
**
** Terminologická poznámka: Jazyk C nepou¾ívá pojem procedura.
** Proto zde pou¾íváme pojem funkce i pro operace, které by byly
** v algoritmickém jazyce Pascalovského typu implemenovány jako
** procedury (v jazyce C procedurám odpovídají funkce vracející typ void).
**/

#include "c201.h"

int solved;
int errflg;

void Error() {
/*
** Vytiskne upozornìní na to, ¾e do¹lo k chybì.
** Tato funkce bude volána z nìkterých dále implementovaných operací.
**/	
    printf ("*ERROR* The program has performed an illegal operation.\n");
    errflg = TRUE;                      /* globální promìnná -- pøíznak chyby */
}

void InitList (tList *L) {
/*
** Provede inicializaci seznamu L pøed jeho prvním pou¾itím (tzn. ¾ádná
** z následujících funkcí nebude volána nad neinicializovaným seznamem).
** Tato inicializace se nikdy nebude provádìt nad ji¾ inicializovaným
** seznamem, a proto tuto mo¾nost neo¹etøujte. V¾dy pøedpokládejte,
** ¾e neinicializované promìnné mají nedefinovanou hodnotu.
**/	
	L->Act = NULL;
	L->First = NULL;
}

void DisposeList (tList *L) {
/*
** Zru¹í v¹echny prvky seznamu L a uvede seznam L do stavu, v jakém se nacházel
** po inicializaci. Ve¹kerá pamì» pou¾ívaná prvky seznamu L bude korektnì
** uvolnìna voláním operace free.
***/
	while(L->First != NULL) //postupne rusenie vsetkych prvkov porade
	{
		if(L->First == L->Act) //ak narazim na aktualny zrusim aj ten
		{
			L->Act = NULL;
		}
		tElemPtr pomocny = L->First; //dame prvy prvok do pomocnej aby sme ho mohli uvolnit
		L->First = L->First->ptr; //druhy prvek sa stane prvym
		free(pomocny);
	}
}

void InsertFirst (tList *L, int val) {
/*
** Vlo¾í prvek s hodnotou val na zaèátek seznamu L.
** V pøípadì, ¾e není dostatek pamìti pro nový prvek pøi operaci malloc,
** volá funkci Error().
**/
	tElemPtr pomocny = malloc(sizeof(struct tElem));
	if(pomocny == NULL) // ak je chyba alokacie
	{
		Error();
	}
	else //inak vlozí prvek
	{
		pomocny->data = val; //nahrame data
		pomocny->ptr = L->First; //prvy prvok sa stava druhym
		L->First = pomocny; //a pomocny sa stane prvy
	}
}

void First (tList *L) {
/*
** Nastaví aktivitu seznamu L na jeho první prvek.
** Funkci implementujte jako jediný pøíkaz, ani¾ byste testovali,
** zda je seznam L prázdný.
**/	
 	L->Act = L->First;
}

void CopyFirst (tList *L, int *val) {
/*
** Prostøednictvím parametru val vrátí hodnotu prvního prvku seznamu L.
** Pokud je seznam L prázdný, volá funkci Error().
**/
	if(L->First != NULL) //ak nieco obsahuje
	{
		*val = L->First->data; //ulozim do val hodnotu
	}
	else //ak je prazdny nastane chyba
	{
		Error();	
	}
	
}

void DeleteFirst (tList *L) {
/*
** Zru¹í první prvek seznamu L a uvolní jím pou¾ívanou pamì».
** Pokud byl ru¹ený prvek aktivní, aktivita seznamu se ztrácí.
** Pokud byl seznam L prázdný, nic se nedìje.
**/
	if(L->First != NULL) //zoznam nieje prazdny
	{
		if(L->First == L->Act) //ak je aktivny tak aktivitu zrusi
		{
			L->Act = NULL;
		}
		tElemPtr pomocny = L->First; //dame prvy prvok do pomocnej aby sme ho mohli uvolnit
		L->First = L->First->ptr; //druhy prvek sa stane prvym
		free(pomocny);	
	}
}	

void PostDelete (tList *L) {
/* 
** Zru¹í prvek seznamu L za aktivním prvkem a uvolní jím pou¾ívanou pamì».
** Pokud není seznam L aktivní nebo pokud je aktivní poslední prvek seznamu L,
** nic se nedìje.
**/
	if(L->Act != NULL && L->Act->ptr != NULL) //zoznam je aktivny a aktivny neni posledny
	{
		tElemPtr pomocny = L->Act->ptr; //do pomocneho dam prvok za aktivnym aby som ho mohol uvolnit
		L->Act->ptr = L->Act->ptr->ptr; //2prvok za aktivnym bude 1 prvok za aktivnym 
		free(pomocny);	
	}
}

void PostInsert (tList *L, int val) {
/*
** Vlo¾í prvek s hodnotou val za aktivní prvek seznamu L.
** Pokud nebyl seznam L aktivní, nic se nedìje!
** V pøípadì, ¾e není dostatek pamìti pro nový prvek pøi operaci malloc,
** zavolá funkci Error().
**/
	if(L->Act != NULL) //zoznam je aktivny
	{
		tElemPtr pomocny = malloc(sizeof(struct tElem));
		if(pomocny == NULL) //nastala chyba pri alokacii
		{
			Error();
		}
		else //nenastala chyba pri alokacii
		{
			pomocny->data = val; //ulozim data
			pomocny->ptr = L->Act->ptr; //nasledujuci prvok aktivneho sa stane nasledujuci prvok pomocneho
			L->Act->ptr = pomocny; //pomocny sa stane nasledujuci aktivneho
		}
	}	
}

void Copy (tList *L, int *val) {
/*
** Prostøednictvím parametru val vrátí hodnotu aktivního prvku seznamu L.
** Pokud seznam není aktivní, zavolá funkci Error().
**/
	if(L->Act != NULL) //ak je aktivny
	{
		*val = L->Act->data; //do val ulozime hodnotu aktualneho prvku
	}
	else //ak neni aktivny vola chybovu hlasku
	{
		Error();
	}	
}

void Actualize (tList *L, int val) {
/*
** Pøepí¹e data aktivního prvku seznamu L hodnotou val.
** Pokud seznam L není aktivní, nedìlá nic!
**/
	if(L->Act != NULL) //zoznam je aktivny
	{
		L->Act->data = val; //prepise data
	}	
}

void Succ (tList *L) {
/*
** Posune aktivitu na následující prvek seznamu L.
** V¹imnìte si, ¾e touto operací se mù¾e aktivní seznam stát neaktivním.
** Pokud seznam L není aktivní, nedìlá nic.
**/
	if(L->Act != NULL) //zoznam je aktivny
	{
		if(L->Act->ptr != NULL)
		{
			L->Act = L->Act->ptr; //posunitie aktivity
		}
		else
		{
			L->Act = NULL; //ak bol aktivny posledny, aktivita sa straca
		}
	}	
}

int Active (tList *L) {		
/*
** Je-li seznam L aktivní, vrací TRUE. V opaèném pøípadì vrací FALSE.
** Tuto funkci implementujte jako jediný pøíkaz return. 
**/
 	return(L->Act != NULL);	
}

/* Konec c201.c */
