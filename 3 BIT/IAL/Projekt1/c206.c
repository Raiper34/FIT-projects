	
/* c206.c **********************************************************}
{* Téma: Dvousmìrnì vázaný lineární seznam
**
**                   Návrh a referenèní implementace: Bohuslav Køena, øíjen 2001
**                            Pøepracované do jazyka C: Martin Tuèek, øíjen 2004
**                                            Úpravy: Bohuslav Køena, øíjen 2014
**
** Implementujte abstraktní datový typ dvousmìrnì vázaný lineární seznam.
** U¾iteèným obsahem prvku seznamu je hodnota typu int.
** Seznam bude jako datová abstrakce reprezentován promìnnou
** typu tDLList (DL znamená Double-Linked a slou¾í pro odli¹ení
** jmen konstant, typù a funkcí od jmen u jednosmìrnì vázaného lineárního
** seznamu). Definici konstant a typù naleznete v hlavièkovém souboru c206.h.
**
** Va¹ím úkolem je implementovat následující operace, které spolu
** s vý¹e uvedenou datovou èástí abstrakce tvoøí abstraktní datový typ
** obousmìrnì vázaný lineární seznam:
**
**      xxxDLInitList ...... inicializace seznamu pøed prvním pou¾itím,
**      DLDisposeList ... zru¹ení v¹ech prvkù seznamu,
**      xxxDLInsertFirst ... vlo¾ení prvku na zaèátek seznamu,
**      xxxDLInsertLast .... vlo¾ení prvku na konec seznamu, 
**      xxxDLFirst ......... nastavení aktivity na první prvek,
**      xxxDLLast .......... nastavení aktivity na poslední prvek, 
**      xxxDLCopyFirst ..... vrací hodnotu prvního prvku,
**      xxxDLCopyLast ...... vrací hodnotu posledního prvku, 
**      xxxDLDeleteFirst ... zru¹í první prvek seznamu,
**      xxxDLDeleteLast .... zru¹í poslední prvek seznamu, 
**      DLPostDelete .... ru¹í prvek za aktivním prvkem,
**      DLPreDelete ..... ru¹í prvek pøed aktivním prvkem, 
**      xxxDLPostInsert .... vlo¾í nový prvek za aktivní prvek seznamu,
**      xxxDLPreInsert ..... vlo¾í nový prvek pøed aktivní prvek seznamu,
**      xxxDLCopy .......... vrací hodnotu aktivního prvku,
**      xxxDLActualize ..... pøepí¹e obsah aktivního prvku novou hodnotou,
**      xxxDLSucc .......... posune aktivitu na dal¹í prvek seznamu,
**      xxxDLPred .......... posune aktivitu na pøedchozí prvek seznamu, 
**      xxxDLActive ........ zji¹»uje aktivitu seznamu.
**
** Pøi implementaci jednotlivých funkcí nevolejte ¾ádnou z funkcí
** implementovaných v rámci tohoto pøíkladu, není-li u funkce
** explicitnì uvedeno nìco jiného.
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

#include "c206.h"

int solved;
int errflg;

void DLError() {
/*
** Vytiskne upozornìní na to, ¾e do¹lo k chybì.
** Tato funkce bude volána z nìkterých dále implementovaných operací.
**/	
    printf ("*ERROR* The program has performed an illegal operation.\n");
    errflg = TRUE;             /* globální promìnná -- pøíznak o¹etøení chyby */
    return;
}

void DLInitList (tDLList *L) {
/*
** Provede inicializaci seznamu L pøed jeho prvním pou¾itím (tzn. ¾ádná
** z následujících funkcí nebude volána nad neinicializovaným seznamem).
** Tato inicializace se nikdy nebude provádìt nad ji¾ inicializovaným
** seznamem, a proto tuto mo¾nost neo¹etøujte. V¾dy pøedpokládejte,
** ¾e neinicializované promìnné mají nedefinovanou hodnotu.
**/
	L->First = NULL;
	L->Act = NULL;
	L->Last = NULL;
}

void DLDisposeList (tDLList *L) {
/*
** Zru¹í v¹echny prvky seznamu L a uvede seznam do stavu, v jakém
** se nacházel po inicializaci. Ru¹ené prvky seznamu budou korektnì
** uvolnìny voláním operace free. 
**/
	while(L->First != NULL) //prejde vsetky prvky zoznamu
	{
		if(L->First == L->Act) //ak je ruseny prvok aktivny, tak zrusi aktivitu 
		{
			L->Act = NULL;
		}
		if(L->First == L->Last) //ak je prvy zaroven poslednym tak zrusi ukazovanie na posledny
		{
			L->Last = NULL;
		}
		tDLElemPtr pomocny = L->First; //dame prvy do pomocnej aby sme ho mohli uvolnit
		L->First = L->First->rptr; //prvok napravo od prveho sa stava prvym
		free(pomocny);
	}
}

void DLInsertFirst (tDLList *L, int val) {
/*
** Vlo¾í nový prvek na zaèátek seznamu L.
** V pøípadì, ¾e není dostatek pamìti pro nový prvek pøi operaci malloc,
** volá funkci DLError().
**/
	tDLElemPtr pomocny = malloc(sizeof(struct tDLElem));
	if(pomocny == NULL) //chyba alokacie
	{
		DLError();
	}
	else //alokacia prebehla spravne
	{
		pomocny->data = val; //nahrame data
		pomocny->lptr = NULL; //prvok nalavo od pomocneho bude ukazovat na null
		pomocny->rptr = L->First; //prvok napravo od pomocneho bude ukazovat na prvy prvok
		if(L->Last == NULL) //ak to je prve vlozenie
		{
			L->Last = pomocny; //posledny je nas vlozeny prvok
		}
		else //nieje prve vlozenie
		{
			L->First->lptr = pomocny; //prvok nalavo prveho bude ukazovat na pomocny
		}
		L->First = pomocny; //prvy je nas vlozeny prvok
	}	
}

void DLInsertLast(tDLList *L, int val) {
/*
** Vlo¾í nový prvek na konec seznamu L (symetrická operace k DLInsertFirst).
** V pøípadì, ¾e není dostatek pamìti pro nový prvek pøi operaci malloc,
** volá funkci DLError().
**/ 	
	tDLElemPtr pomocny = malloc(sizeof(struct tDLElem));
	if(pomocny == NULL) //chyba alokacie
	{
		DLError();
	}
	else //bezchybna alokacia
	{
		pomocny->data = val; //ulozime data
		pomocny->rptr = NULL; //napravo od pomocneho nebude ukazovat nikam lebo pomocny bude posledny
		pomocny->lptr = L->Last; //nalavo pomocneho bude ukazovat na posledny prvok
		if(L->Last == NULL) //ak vkladame prvy prvok
		{
			L->First = pomocny; //posledny je zaroven prvy teda nas pomocny
		}
		else //nevkladame prvy prvok
		{
			L->Last->rptr = pomocny; //napravo od posledneho bude ukazovat na nas pomocny, pomocny sa stava posledny
		}
		L->Last = pomocny; //pomocny je posledny
	}
}

void DLFirst (tDLList *L) {
/*
** Nastaví aktivitu na první prvek seznamu L.
** Funkci implementujte jako jediný pøíkaz (nepoèítáme-li return),
** ani¾ byste testovali, zda je seznam L prázdný.
**/
	L->Act = L->First;
}

void DLLast (tDLList *L) {
/*
** Nastaví aktivitu na poslední prvek seznamu L.
** Funkci implementujte jako jediný pøíkaz (nepoèítáme-li return),
** ani¾ byste testovali, zda je seznam L prázdný.
**/
	L->Act = L->Last;	
}

void DLCopyFirst (tDLList *L, int *val) {
/*
** Prostøednictvím parametru val vrátí hodnotu prvního prvku seznamu L.
** Pokud je seznam L prázdný, volá funkci DLError().
**/
	if(L->First == NULL) //zoznam je prazdny
	{
		DLError();
	}
	else //zoznam neni prazdny
	{
		*val = L->First->data;
	}	
}

void DLCopyLast (tDLList *L, int *val) {
/*
** Prostøednictvím parametru val vrátí hodnotu posledního prvku seznamu L.
** Pokud je seznam L prázdný, volá funkci DLError().
**/
	if(L->First == NULL) //zoznam je prazdny
	{
		DLError();
	}
	else //zoznam neni prazdny
	{
		*val = L->Last->data;
	}	
}

void DLDeleteFirst (tDLList *L) {
/*
** Zru¹í první prvek seznamu L. Pokud byl první prvek aktivní, aktivita 
** se ztrácí. Pokud byl seznam L prázdný, nic se nedìje.
**/
	if(L->First != NULL) //zoznam nebol prazdny
	{
		tDLElemPtr pomocny = L->First; //dame si prvy do pomocnej nech ho mozme uvolnit neskor
		if(L->First == L->Last) //ak bol prvy zaroven posledny tak nastavime aktivitu a posledny tiez na nul
		{
			L->First = NULL;
			L->Last = NULL;
			L->Act = NULL;
		}
		else
		{
			if(L->First == L->Act) //ak bol prvy aktivny, zrusime aktivitu tiez
			{
				L->Act = NULL;
			}
			L->First = pomocny->rptr; //prvym sa stava prvok napravo od prveho
			L->First->lptr = NULL; //novy prvy prvok bude ukzovat nalavo na null
		}
	free(pomocny);
	}
}	

void DLDeleteLast (tDLList *L) {
/*
** Zru¹í poslední prvek seznamu L. Pokud byl poslední prvek aktivní,
** aktivita seznamu se ztrácí. Pokud byl seznam L prázdný, nic se nedìje.
**/ 
	if(L->First != NULL)
	{
		tDLElemPtr pomocny = L->Last;
		if(L->First == L->Last) //ak bol prvy zaroven posledny tak nastavime aktivitu a posledny tiez na nul
		{
			L->First = NULL;
			L->Last = NULL;
			L->Act = NULL;
		}
		else
		{
			if(L->Last == L->Act) //ak bol posledny aktivny, zrusime aktivitu tiez
			{
				L->Act = NULL;
			}
			L->Last = pomocny->lptr; //posledny sa stava prvok nalavo od posleneho 
			L->Last->rptr = NULL; //novy posledny bude napravo ukazovat na null
		}
	free(pomocny);
	}	
}

void DLPostDelete (tDLList *L) {
/*
** Zru¹í prvek seznamu L za aktivním prvkem.
** Pokud je seznam L neaktivní nebo pokud je aktivní prvek
** posledním prvkem seznamu, nic se nedìje.
**/
	if(L->Act != NULL && L->Act != L->Last) //zoznam je aktivny a aktivny neni posledny
	{
		tDLElemPtr pomocny = L->Act->rptr;
		L->Act->rptr = pomocny->rptr; //preskocime ruseny prvok
		if(L->Last == pomocny) //ak je ruseny prvok posledny
		{
			L->Last = L->Act;
		}
		else //ak neni posledny
		{
			pomocny->rptr->lptr = L->Act; //preskocime ruseny prvok
		}
		free(pomocny);
	}	
}

void DLPreDelete (tDLList *L) {
/*
** Zru¹í prvek pøed aktivním prvkem seznamu L .
** Pokud je seznam L neaktivní nebo pokud je aktivní prvek
** prvním prvkem seznamu, nic se nedìje.
**/
	if(L->Act != NULL && L->Act != L->First) ///zoznam je aktivny a aktivny neni posledny
	{
		tDLElemPtr pomocny = L->Act->lptr;
		L->Act->lptr = pomocny->lptr; //preskocime ruseny prvok
		if(L->First == pomocny) //ak je ruseny prvok prvy
		{
			L->First = L->Act;
		}
		else //ak neni prvy
		{
			pomocny->lptr->rptr = L->Act; //preskocime ruseny prvok
		}
		free(pomocny);
	}			
}

void DLPostInsert (tDLList *L, int val) {
/*
** Vlo¾í prvek za aktivní prvek seznamu L.
** Pokud nebyl seznam L aktivní, nic se nedìje.
** V pøípadì, ¾e není dostatek pamìti pro nový prvek pøi operaci malloc,
** volá funkci DLError().
**/
	if(L->Act != NULL) //zoznam je aktivny
	{	
		tDLElemPtr pomocny = malloc(sizeof(struct tDLElem));
		if(pomocny == NULL) //malloc sa nepodaril
		{
			DLError();
		}
		else //malloc sa podaril tak napojime novy prvok
		{
			pomocny->data = val;
			pomocny->lptr = L->Act;
			pomocny->rptr = L->Act->rptr;
			L->Act->rptr = pomocny;
			if(L->Act != L->Last) //ak aktivny prvok nebol posledny
			{
				pomocny->rptr->lptr = pomocny;
			}
			else //ak bol posledny
			{
				L->Last = pomocny;
			}
		}
	}	
}

void DLPreInsert (tDLList *L, int val) {
/*
** Vlo¾í prvek pøed aktivní prvek seznamu L.
** Pokud nebyl seznam L aktivní, nic se nedìje.
** V pøípadì, ¾e není dostatek pamìti pro nový prvek pøi operaci malloc,
** volá funkci DLError().
**/
	if(L->Act != NULL) //zoznam je aktivny
	{	
		tDLElemPtr pomocny = malloc(sizeof(struct tDLElem));
		if(pomocny == NULL) //malloc sa nepodaril
		{
			DLError();
		}
		else //malloc sa podaril tak napojime novy prvok
		{
			pomocny->data = val;
			pomocny->rptr = L->Act;
			pomocny->lptr = L->Act->lptr;
			L->Act->lptr = pomocny;
			if(L->Act != L->First) //ak aktivny nebol prvy
			{
				pomocny->lptr->rptr = pomocny;
			}
			else //ak bol prvy
			{
				L->First = pomocny;
			}
		}
	}	
}

void DLCopy (tDLList *L, int *val) {
/*
** Prostøednictvím parametru val vrátí hodnotu aktivního prvku seznamu L.
** Pokud seznam L není aktivní, volá funkci DLError ().
**/
	if(L->Act == NULL) //neni aktivny
	{
		DLError();
	}
	else //je aktivny
	{
		*val = L->Act->data;
	}	
}

void DLActualize (tDLList *L, int val) {
/*
** Pøepí¹e obsah aktivního prvku seznamu L.
** Pokud seznam L není aktivní, nedìlá nic.
**/
	if(L->Act != NULL)
	{
		L->Act->data = val;
	}	
}

void DLSucc (tDLList *L) {
/*
** Posune aktivitu na následující prvek seznamu L.
** Není-li seznam aktivní, nedìlá nic.
** V¹imnìte si, ¾e pøi aktivitì na posledním prvku se seznam stane neaktivním.
**/
	if(L->Act != NULL) //zoznam je aktivny
	{
		if(L->Act == L->Last) //ak je aktivny posledny, aktivita sa straca
		{
			L->Act = NULL;
		}
		else //inak aktivitu posuneme doprava
		{
			L->Act = L->Act->rptr;
		}
	}	
}


void DLPred (tDLList *L) {
/*
** Posune aktivitu na pøedchozí prvek seznamu L.
** Není-li seznam aktivní, nedìlá nic.
** V¹imnìte si, ¾e pøi aktivitì na prvním prvku se seznam stane neaktivním.
**/
	if(L->Act != NULL) //zoznam je aktivny
	{
		if(L->Act == L->First) //ak je aktivny prvy, aktivita sa straca
		{
			L->Act = NULL;
		}
		else //inak aktivitu posuneme dolava
		{
			L->Act = L->Act->lptr;
		}
	}	
}

int DLActive (tDLList *L) {
/*
** Je-li seznam aktivní, vrací true. V opaèném pøípadì vrací false.
** Funkci implementujte jako jediný pøíkaz.
**/
	return(L->Act != NULL);
}

/* Konec c206.c*/
