------------------------------------------------------
--IDS Projekt SQL 
--Filip Gulan (xgulan00)
--Eduard Rybar (xrybar04)
------------------------------------------------------

-------------------------------------------------------
------------------vymazanie tabuliek-------------------
-------------------------------------------------------
DROP TABLE zamestnanec CASCADE CONSTRAINT;
DROP TABLE zivocich CASCADE CONSTRAINT;
DROP TABLE druhZivocicha CASCADE CONSTRAINT;
DROP TABLE testoval CASCADE CONSTRAINT;
DROP TABLE umiestnenie CASCADE CONSTRAINT;

DROP TABLE klietka CASCADE CONSTRAINT;
DROP TABLE vybeh CASCADE CONSTRAINT;

DROP TABLE staraSa CASCADE CONSTRAINT;
DROP TABLE spravuje CASCADE CONSTRAINT;
DROP TABLE obsahuje CASCADE CONSTRAINT;
DROP SEQUENCE pocetID;

-------------------------------------------------------
-----------------vytvorenie tabuliek-------------------
-------------------------------------------------------
CREATE TABLE zamestnanec(
RodneCislo NUMBER NOT NULL,
meno VARCHAR2(25) NOT NULL,
priezvisko VARCHAR2(25) NOT NULL,
titul VARCHAR2(10),
datumNarodenia DATE NOT NULL,
adresa VARCHAR2(50) NOT NULL,
funkcia VARCHAR2(25) NOT NULL,
IBAN VARCHAR2(24)
);

CREATE TABLE zivocich(
IDZivocicha NUMBER NOT NULL,
meno VARCHAR2(25) NOT NULL,
datumNarodenia DATE NOT NULL,
datumUmrtia DATE,
trieda VARCHAR2(25) NOT NULL,
rad VARCHAR2(25) NOT NULL,
celad VARCHAR2(25) NOT NULL,
rod VARCHAR2(25) NOT NULL,
IDDruhuZivocicha NUMBER NOT NULL,
IDUmiestnenia NUMBER NOT NULL
);										

CREATE TABLE druhZivocicha(
IDDruhuZivocicha NUMBER NOT NULL,
nazov VARCHAR2(25) NOT NULL
);

CREATE TABLE testoval(
IDZivocicha NUMBER NOT NULL,
RodneCislo NUMBER NOT NULL,
hmotnostZivocicha FLOAT NOT NULL,
rozmerZivocicha FLOAT NOT NULL,
datumTestu DATE NOT NULL
);

CREATE TABLE umiestnenie(
IDUmiestnenia NUMBER NOT NULL,
nazov VARCHAR2(25) NOT NULL,
sirka NUMBER NOT NULL,
dlzka NUMBER NOT NULL,
vyska NUMBER
);
-------------------------------
CREATE TABLE klietka(
IDUmiestnenia NUMBER NOT NULL,
typ VARCHAR2(25) NOT NULL,
podstielka VARCHAR2(25) NOT NULL,
lokacia VARCHAR2(25) NOT NULL
);

CREATE TABLE vybeh(
IDUmiestnenia NUMBER NOT NULL,
teren VARCHAR2(25) NOT NULL,
povrch VARCHAR2(25) NOT NULL,
ohradenie VARCHAR2(25) NOT NULL
);
-------------------------------
CREATE TABLE staraSa(
IDZivocicha NUMBER NOT NULL,
RodneCislo NUMBER NOT NULL
);

CREATE TABLE spravuje(
IDUmiestnenia NUMBER NOT NULL,
RodneCislo NUMBER NOT NULL
);

CREATE TABLE obsahuje(
IDUmiestnenia NUMBER NOT NULL,
IDDruhuZivocicha NUMBER NOT NULL
);

-------------------------------------------------------
--------------------Vytvorenie triggery ---------------
-------------------------------------------------------

-------------------Triger na skontrolovanie ci je IBAN v spravnom formate-----------------
ALTER session SET nls_date_format='dd.mm.yyyy';
SET serveroutput ON;
CREATE OR REPLACE TRIGGER triggerUcet 
	BEFORE INSERT OR UPDATE OF IBAN ON zamestnanec 
	FOR EACH ROW
DECLARE
  ucet zamestnanec.IBAN%TYPE;
  stat VARCHAR2(2);
  cisla VARCHAR2(22);
  iductu VARCHAR2(10);
  modd NUMBER;
BEGIN
  ucet := :NEW.IBAN;
  stat := SUBSTR(ucet, 1, 2);
  cisla := SUBSTR(ucet, 3, 22);
  iductu := SUBSTR(ucet, 15, 10); --poslednych 10 cislic je ucet
  modd := TO_NUMBER(SUBSTR(iductu, 1, 1), '9') * 6 + TO_NUMBER(SUBSTR(iductu, 2, 1), '9') * 3 + TO_NUMBER(SUBSTR(iductu, 3, 1), '9') * 7 + TO_NUMBER(SUBSTR(iductu, 4, 1), '9') * 9 + TO_NUMBER(SUBSTR(iductu, 5, 1), '9') * 10 + TO_NUMBER(SUBSTR(iductu, 6, 1), '9') * 5 +TO_NUMBER(SUBSTR(iductu, 7, 1), '9') * 8 + TO_NUMBER(SUBSTR(iductu, 8, 1), '9') * 4 + TO_NUMBER(SUBSTR(iductu, 9, 1), '9') * 2 + TO_NUMBER(SUBSTR(iductu, 10, 1), '9') * 1;
  IF (LENGTH(ucet) != 24) THEN --CZ IBAN je vzdy 24 znakovy overenie ci tam  je 24 znakov
  	Raise_Application_Error (-20203, 'Malo znakov pre IBAN!');
  END IF;
  IF (LENGTH(TRIM(TRANSLATE(stat, 'ABCDEFGHIJKLMNOPQRSTUVWXYZ', ' '))) != null) THEN --overenie ci tam je skratka statu
  	Raise_Application_Error (-20204, 'Nespravny format IBAN!');
  END IF;
  IF (LENGTH(TRIM(TRANSLATE(stat, '0123456789', ' '))) != null) THEN --overenie ci zbytok je iba cisla
  	Raise_Application_Error (-20204, 'Nespravny format IBAN!');
  END IF;
  IF MOD(modd, 11) != 0 THEN --overenie pomocou vah danym algoritmom ci je delitelne 11 = validne
  	Raise_Application_Error (-20204, 'Nespravny format IBAN!');
  END IF;
END triggerUcet;
/
show errors
ALTER session SET nls_date_format='dd.mm.yyyy';

-----------------Triger na autoinkrementaciu idciek---------------
CREATE SEQUENCE pocetID; --uchovava posledne id
ALTER session SET nls_date_format='dd.mm.yyyy';
CREATE OR REPLACE TRIGGER autoincrement
  BEFORE INSERT ON umiestnenie
  FOR EACH ROW
BEGIN
  :new.IDUmiestnenia := pocetID.nextval; --dame do idcka hodnotu z sequencie +1
END autoincrement;
/
show errors
ALTER session SET nls_date_format='dd.mm.yyyy';

-------------------------------------------------------------
----------------------Procedury------------------------------
-------------------------------------------------------------

-------------------------------------------------------------
--Procedura, ktora vypise meno druhu a jeho percentualne zastupenie v danom umiestneny
--Argumenty: idZ - id druhu zivocicha ktoreho percentualne zastupenie chcem, idU - id umiestnenia, v ktorom to percentalne zastupenie chcem
-------------------------------------------------------------
ALTER session SET nls_date_format='dd.mm.yyyy';
SET serveroutput ON;
CREATE OR REPLACE PROCEDURE percentZastup(idZ IN NUMBER, idU IN NUMBER)
is
  cursor obsah is select * from obsahuje NATURAL JOIN druhZivocicha;
  polozkaObsah obsah%ROWTYPE;
  vsetci NUMBER; --pocet vsetkych druhov
  hladany NUMBER; --pocet hladaneho druhy
  menoHladaneho druhZivocicha.nazov%TYPE; --menohladaneho
BEGIN
  vsetci := 0;
  hladany := 0;
  SELECT nazov INTO menoHladaneho FROM obsahuje NATURAL JOIN druhZivocicha WHERE idZ = IDDruhuZivocicha; --zistim si menodruhu ktoremu patri to ID
  open obsah;
  loop
    fetch obsah into polozkaObsah;
    exit when obsah%NOTFOUND;
    IF (polozkaObsah.IDUmiestnenia = idU) THEN
      IF (polozkaObsah.IDDruhuZivocicha = idZ) THEN
        hladany := hladany + 1;
      END IF;
      vsetci := vsetci + 1;
    END IF;
  end loop;
  dbms_output.put_line('Zivocisny druh ' || menoHladaneho || ' ma percentualne zastupenie v danom umiestneny: ' || (hladany * 100)/vsetci || '%');
EXCEPTION
  WHEN ZERO_DIVIDE THEN --ak taky hladany v danom umiestneny neni
    dbms_output.put_line('Zivocisny druh ' || menoHladaneho || ' ma percentualne zastupenie v danom umiestneny: 0%');
  WHEN OTHERS THEN --ina vynimka
    Raise_Application_Error (-20206, 'Nastala chyba!');
END;
/
ALTER session SET nls_date_format='dd.mm.yyyy';

----------------------------------------------------------
--Procedura na vypocitanie rozlohy umiestnenia v ktorom sa zivocich nachadza a vypis vsetkych zivocichov, ktory tam su s nim
--Argument: menoZ - menozivocicha pre ktoreho tieto informacie zistujeme
----------------------------------------------------------
ALTER session SET nls_date_format='dd.mm.yyyy';
SET serveroutput ON;
CREATE OR REPLACE PROCEDURE prostredZiv(menoZ IN VARCHAR2)
is
  cursor ziv is select * from zivocich NATURAL JOIN umiestnenie;
  polozkaziv ziv%ROWTYPE;
  um ziv%ROWTYPE;
  iter NUMBER; --pocet iteraci, na zistenie ak ziadneho suseda nenajde aby sa nevypisovala hlaska ze ma tychto susedov....
BEGIN
  iter := 0;
  SELECT * INTO um FROM zivocich NATURAL JOIN umiestnenie WHERE menoZ = meno;
  dbms_output.put_line('Rozloha umiestnenia kde je zviera s menom ' || menoZ || ' je ' || um.dlzka * um.sirka || ' m2');
  open ziv;
  loop
    fetch ziv into polozkaziv;
    exit when ziv%NOTFOUND;
    IF (polozkaziv.IDUmiestnenia = um.IDUmiestnenia AND polozkaziv.meno != menoZ) THEN --ak sme nasli ze ma suseda
      IF (iter = 0) THEN
        dbms_output.put_line('V rovnakom umiestneni sa nachadzaju aj tieto zvierata:');
      END IF;
      dbms_output.put_line(polozkaziv.meno);
      iter := iter + 1;
    END IF;
  end loop;
EXCEPTION
  WHEN NO_DATA_FOUND THEN --ak zadame neexistujuceho zivocicha/ak neni este umiestneny v reale sa nestane
    dbms_output.put_line('Zivocich s menom ' || menoZ || ' neexistuje!');
  WHEN OTHERS THEN
    Raise_Application_Error (-20206, 'Nastala chyba!');
END;
/
ALTER session SET nls_date_format='dd.mm.yyyy';

-------------------------------------------------------------
--------------nastavenie primarnych klucov-------------------
-------------------------------------------------------------
ALTER TABLE zamestnanec ADD CONSTRAINT PK_zamestnanec PRIMARY KEY (RodneCislo);
ALTER TABLE zivocich ADD CONSTRAINT PK_zivocich PRIMARY KEY (IDZivocicha);
ALTER TABLE druhZivocicha ADD CONSTRAINT PK_druhZivocicha PRIMARY KEY (IDDruhuZivocicha);
ALTER TABLE umiestnenie ADD CONSTRAINT PK_umiestnenie PRIMARY KEY (IDUmiestnenia);

ALTER TABLE staraSa ADD CONSTRAINT PK_staraSa PRIMARY KEY (IDZivocicha, RodneCislo);
ALTER TABLE spravuje ADD CONSTRAINT PK_spravuje PRIMARY KEY (IDUmiestnenia, RodneCislo);
ALTER TABLE obsahuje ADD CONSTRAINT PK_obsahuje PRIMARY KEY (IDUmiestnenia, IDDruhuZivocicha);
ALTER TABLE testoval ADD CONSTRAINT PK_testoval PRIMARY KEY (IDZivocicha, RodneCislo);

ALTER TABLE klietka ADD CONSTRAINT PK_klietka PRIMARY KEY (IDUmiestnenia);
ALTER TABLE vybeh ADD CONSTRAINT PK_vybeh PRIMARY KEY (IDUmiestnenia);

--------------------------------------------------------------
-------------nastavenie cudzich klucov------------------------
--------------------------------------------------------------
ALTER TABLE zivocich ADD CONSTRAINT FK_DruhZivJe FOREIGN KEY (IDDruhuZivocicha) REFERENCES druhZivocicha;
ALTER TABLE zivocich ADD CONSTRAINT FK_jeUmiestneny FOREIGN KEY (IDUmiestnenia) REFERENCES umiestnenie;

ALTER TABLE staraSa ADD CONSTRAINT FK_ZivocichJeOpatrovany FOREIGN KEY (IDZivocicha) REFERENCES zivocich ON DELETE CASCADE;
ALTER TABLE staraSa ADD CONSTRAINT FK_ZamestnanecSaStaraO FOREIGN KEY (RodneCislo) REFERENCES zamestnanec ON DELETE CASCADE;
ALTER TABLE spravuje ADD CONSTRAINT FK_UmiestnenieJeSpravované FOREIGN KEY (IDUmiestnenia) REFERENCES umiestnenie ON DELETE CASCADE;
ALTER TABLE spravuje ADD CONSTRAINT FK_ZamestnanecSpravuje FOREIGN KEY (RodneCislo) REFERENCES zamestnanec ON DELETE CASCADE;
ALTER TABLE obsahuje ADD CONSTRAINT FK_DruhZivJeObs FOREIGN KEY (IDDruhuZivocicha) REFERENCES druhZivocicha ON DELETE CASCADE;
ALTER TABLE obsahuje ADD CONSTRAINT FK_UmiestnenieObsahuje FOREIGN KEY (IDUmiestnenia) REFERENCES umiestnenie ON DELETE CASCADE;
ALTER TABLE testoval ADD CONSTRAINT FK_Jetestovany FOREIGN KEY (IDZivocicha) REFERENCES zivocich ON DELETE CASCADE;
ALTER TABLE testoval ADD CONSTRAINT FK_Testuje FOREIGN KEY (RodneCislo) REFERENCES zamestnanec ON DELETE CASCADE;

ALTER TABLE klietka ADD CONSTRAINT FK_UmiestnenieKlietky FOREIGN KEY (IDUmiestnenia) REFERENCES umiestnenie;
ALTER TABLE vybeh ADD CONSTRAINT FK_UmiestnenieVybehu FOREIGN KEY (IDUmiestnenia) REFERENCES umiestnenie;

--------------------------------------------------------------
--------------------vkladanie do tabuliek---------------------
--------------------------------------------------------------
--Zamestananci
INSERT INTO zamestnanec (RodneCislo, meno, priezvisko, titul, datumNarodenia, adresa, funkcia, IBAN) VALUES('8802012131','Michal', 'Burgh', 'MGR', TO_DATE('01-02-1988', 'dd-mm-yyyy'), 'Trnava, 745', 'zverolekár', 'CZ6508000000192000145399');
INSERT INTO zamestnanec (RodneCislo, meno, priezvisko, titul, datumNarodenia, adresa, funkcia, IBAN) VALUES('9202034563','Anna', 'Sapkowska', null, TO_DATE('03-02-1992', 'dd-mm-yyyy'), 'Nitra, 2897', 'chovateľ', 'CZ6508000000192000145399');
INSERT INTO zamestnanec (RodneCislo, meno, priezvisko, titul, datumNarodenia, adresa, funkcia, IBAN) VALUES('8904041199','Vlado', 'Chmelo', 'ING', TO_DATE('04-04-1989', 'dd-mm-yyyy'), 'Nitra 1123', 'údržbár', 'CZ6907101781240000004159');
INSERT INTO zamestnanec (RodneCislo, meno, priezvisko, titul, datumNarodenia, adresa, funkcia, IBAN) VALUES('9003063431','Denisa', 'Rovná', null, TO_DATE('09-03-1990', 'dd-mm-yyyy'), 'Pieštany 8865', 'chovateľ', 'CZ6508000000192000145399');
INSERT INTO zamestnanec (RodneCislo, meno, priezvisko, titul, datumNarodenia, adresa, funkcia, IBAN) VALUES('8609125575','Veronika', 'Weiss', 'MUDR', TO_DATE('12-09-1986', 'dd-mm-yyyy'), 'Trnava 745', 'zverolekár', 'CZ6907101781240000004159');

--umiestnenie
INSERT INTO umiestnenie (nazov, sirka, dlzka, vyska) VALUES('Výbeh Levi', '1500,5', '800', '2');
INSERT INTO umiestnenie (nazov, sirka, dlzka, vyska) VALUES('Výbeh Safari', '2500', '1600,9', '4');
INSERT INTO umiestnenie (nazov, sirka, dlzka, vyska) VALUES('Klietka Opice', '20,4', '30', '3');
INSERT INTO umiestnenie (nazov, sirka, dlzka, vyska) VALUES('Výbeh Vlci', '25', '47', '3');

--Druh zivocicha
INSERT INTO druhZivocicha (IDDruhuZivocicha, nazov) VALUES('001', 'Lev');
INSERT INTO druhZivocicha (IDDruhuZivocicha, nazov) VALUES('002', 'Zebra');
INSERT INTO druhZivocicha (IDDruhuZivocicha, nazov) VALUES('003', 'Hroch');
INSERT INTO druhZivocicha (IDDruhuZivocicha, nazov) VALUES('004', 'Žirafa');
INSERT INTO druhZivocicha (IDDruhuZivocicha, nazov) VALUES('005', 'Orangutan');
INSERT INTO druhZivocicha (IDDruhuZivocicha, nazov) VALUES('006', 'Vlk');

--Zivocich
INSERT INTO zivocich (IDZivocicha, meno, datumNarodenia, datumUmrtia, trieda, rad, celad, rod, IDDruhuZivocicha, IDUmiestnenia) VALUES('001', 'Alex', TO_DATE('10-04-2012', 'dd-mm-yyyy'), null, 'cicavci', 'šelmy', 'kočkovité', 'Panthera', '001', '001');
INSERT INTO zivocich (IDZivocicha, meno, datumNarodenia, datumUmrtia, trieda, rad, celad, rod, IDDruhuZivocicha, IDUmiestnenia) VALUES('002', 'Marty', TO_DATE('01-08-2010', 'dd-mm-yyyy'), null, 'cicavci', 'nepárno-kopýtníci', 'koňovitý', 'Equus', '002', '002');
INSERT INTO zivocich (IDZivocicha, meno, datumNarodenia, datumUmrtia, trieda, rad, celad, rod, IDDruhuZivocicha, IDUmiestnenia) VALUES('003', 'Gloria', TO_DATE('12-01-2014', 'dd-mm-yyyy'), null, 'cicavci', 'nepárno-kopýtníci', 'hrochovitý', 'Hippopotamus', '003', '002');
INSERT INTO zivocich (IDZivocicha, meno, datumNarodenia, datumUmrtia, trieda, rad, celad, rod, IDDruhuZivocicha, IDUmiestnenia) VALUES('004', 'Melwan', TO_DATE('05-09-2008', 'dd-mm-yyyy'), null, 'cicavci', 'nepárno-kopýtníci', 'žirafovitý', 'Giraffa', '004', '002');
INSERT INTO zivocich (IDZivocicha, meno, datumNarodenia, datumUmrtia, trieda, rad, celad, rod, IDDruhuZivocicha, IDUmiestnenia) VALUES('005', 'Ipi', TO_DATE('10-04-2012', 'dd-mm-yyyy'), TO_DATE('10-03-2015', 'dd-mm-yyyy'), 'cicavci', 'primáti', 'hominidi', 'Pongo', '005', '003');
INSERT INTO zivocich (IDZivocicha, meno, datumNarodenia, datumUmrtia, trieda, rad, celad, rod, IDDruhuZivocicha, IDUmiestnenia) VALUES('006', 'WOlfi', TO_DATE('10-04-2012', 'dd-mm-yyyy'), null, 'cicavci', 'šelmy', 'psovité', 'Canis', '006', '004');

--testoval
INSERT INTO testoval (IDZivocicha, RodneCislo, hmotnostZivocicha, rozmerZivocicha, datumTestu ) VALUES('001', '8802012131', '200,56', '150,1', TO_DATE('11-06-2013', 'dd-mm-yyyy'));
INSERT INTO testoval (IDZivocicha, RodneCislo, hmotnostZivocicha, rozmerZivocicha, datumTestu ) VALUES('002', '8802012131', '220,84', '198,5', TO_DATE('18-03-2013', 'dd-mm-yyyy'));
INSERT INTO testoval (IDZivocicha, RodneCislo, hmotnostZivocicha, rozmerZivocicha, datumTestu ) VALUES('003', '8609125575', '640,20', '320,5', TO_DATE('02-10-2014', 'dd-mm-yyyy'));
INSERT INTO testoval (IDZivocicha, RodneCislo, hmotnostZivocicha, rozmerZivocicha, datumTestu ) VALUES('004', '8609125575', '340,20', '560', TO_DATE('22-07-2014', 'dd-mm-yyyy'));
INSERT INTO testoval (IDZivocicha, RodneCislo, hmotnostZivocicha, rozmerZivocicha, datumTestu ) VALUES('005', '8802012131', '113,00', '45', TO_DATE('05-12-2013', 'dd-mm-yyyy'));
INSERT INTO testoval (IDZivocicha, RodneCislo, hmotnostZivocicha, rozmerZivocicha, datumTestu ) VALUES('006', '8609125575', '120,44', '66,8', TO_DATE('23-07-2014', 'dd-mm-yyyy'));

--klietka
INSERT INTO klietka (typ, podstielka, lokacia, IDUmiestnenia) VALUES('železná', 'seno', 'Naturov pavilón', '003');

--vybeh
INSERT INTO vybeh (teren, povrch, ohradenie, IDUmiestnenia) VALUES('rovný', 'step', 'zákopa', '001');
INSERT INTO vybeh (teren, povrch, ohradenie, IDUmiestnenia) VALUES('hrbolatý', 'tajga', 'drevený plot','004');
INSERT INTO vybeh (teren, povrch, ohradenie, IDUmiestnenia) VALUES('rovný', 'step', 'drevený plot', '002');

--staraSa
INSERT INTO staraSa (IDZivocicha, RodneCislo) VALUES('001', '9003063431');
INSERT INTO staraSa (IDZivocicha, RodneCislo) VALUES('002', '9003063431');
INSERT INTO staraSa (IDZivocicha, RodneCislo) VALUES('003', '9202034563');
INSERT INTO staraSa (IDZivocicha, RodneCislo) VALUES('004', '9202034563');
INSERT INTO staraSa (IDZivocicha, RodneCislo) VALUES('005', '9202034563');
INSERT INTO staraSa (IDZivocicha, RodneCislo) VALUES('006', '9003063431');

--spravuje
INSERT INTO spravuje (IDUmiestnenia, RodneCislo) VALUES('001', '8904041199');
INSERT INTO spravuje (IDUmiestnenia, RodneCislo) VALUES('002', '8904041199');
INSERT INTO spravuje (IDUmiestnenia, RodneCislo) VALUES('003', '8904041199');
INSERT INTO spravuje (IDUmiestnenia, RodneCislo) VALUES('004', '8904041199');

--obsahuje
INSERT INTO obsahuje (IDUmiestnenia, IDDruhuZivocicha) VALUES('001', '001');
INSERT INTO obsahuje (IDUmiestnenia, IDDruhuZivocicha) VALUES('002', '002');
INSERT INTO obsahuje (IDUmiestnenia, IDDruhuZivocicha) VALUES('002', '003');
INSERT INTO obsahuje (IDUmiestnenia, IDDruhuZivocicha) VALUES('002', '004');
INSERT INTO obsahuje (IDUmiestnenia, IDDruhuZivocicha) VALUES('003', '005');
INSERT INTO obsahuje (IDUmiestnenia, IDDruhuZivocicha) VALUES('004', '006');

---------------------------------------------------
------------------Select dotazy--------------------
---------------------------------------------------

--spojenie 2 tabuliek 2X
--Vypis nazvu, sirky, dlzky, vysky umiestnenia kde sa nachadza zviera s menom Alex
SELECT U.nazov, U.sirka, U.dlzka, U.vyska
FROM zivocich Z, umiestnenie U
WHERE Z.IDUmiestnenia = U.IDUmiestnenia and Z.meno='Alex';
--Vypis mien umiestneny, ktore su typy klietka
SELECT U.nazov
FROM klietka K, umiestnenie U
WHERE K.IDUmiestnenia = U.IDUmiestnenia;

--spojenie spojenie 3 tabuliek 1X
--Vypis meno zamestnanca ktory tesotval zivocicha, meno zivocicha ktoreho tento zamestannec testoval, a datum testy kedy zamestnanec testoval tohoto zivocicha
SELECT Za.meno, Zi.meno, T.datumTestu
FROM zamestnanec Za, zivocich Zi, testoval T
WHERE T.IDZivocicha = Zi.IDZivocicha and T.RodneCislo = Za.RodneCislo;

--dotazy s klauzulou grup by a agregacnou funkciou 2X
--Pre kazdy rad vypise posledny datum narodenia, alebo kedy bolo narodene najmladsie zviera z daneho radu
SELECT Z.rad, max(Z.datumNarodenia)
FROM zivocich Z
GROUP BY Z.rad;
--Vypis poctu zamestnancov, ktory zastupuju danu funkciu
SELECT Z.funkcia, count(Z.funkcia)
FROM zamestnanec Z
GROUP BY Z.funkcia;

--predikat exists  1x
--Vypise meno takeho zamestnanca, ktory sa nestara o ziadneho zivocicha
SELECT Z.meno
FROM zamestnanec Z
WHERE NOT EXISTS
(
  SELECT S.RodneCislo
  FROM staraSa S
  WHERE Z.RodneCislo = S.RodneCislo
);

--predikat in s vnorenym selectom 1x
--Vypise informacie o zivocichovi, ktory bol testovany v juny 2013
SELECT *
FROM Zivocich Z
WHERE Z.IDZivocicha
IN 
(
  SELECT T.IDZivocicha
  FROM testoval T
  WHERE T.datumTestu BETWEEN '1-06-2013' and '30-06-2013'
);

---------------------------------------------------
--------Zavolanie procedur na ukazku---------------
---------------------------------------------------
exec percentZastup(2,2);
exec prostredZiv('Gloria');

---------------------------------------------------
-------------Explain plan a index------------------
---------------------------------------------------
--DROP INDEX indexExplain;

EXPLAIN PLAN FOR
SELECT rad, AVG(sirka)
FROM zivocich NATURAL JOIN umiestnenie
GROUP BY sirka, rad;
SELECT * FROM TABLE(DBMS_XPLAN.display);

CREATE INDEX indexExplain ON zivocich (rad);
  
EXPLAIN PLAN FOR
SELECT /*+ INDEX(zivocich indexExplain)*/ rad, AVG(sirka)
FROM zivocich NATURAL JOIN umiestnenie
GROUP BY sirka, rad;
SELECT * FROM TABLE(DBMS_XPLAN.display);

---------------------------------------------------
-------------Udelenie prav xrybar04----------------
---------------------------------------------------
GRANT ALL ON zivocich TO xrybar04;
GRANT ALL ON druhZivocicha TO xrybar04;
GRANT ALL ON testoval TO xrybar04;
GRANT ALL ON umiestnenie TO xrybar04;
GRANT ALL ON klietka TO xrybar04;
GRANT ALL ON vybeh TO xrybar04;
GRANT ALL ON obsahuje TO xrybar04;

GRANT EXECUTE ON prostredZiv TO xrybar04;
GRANT EXECUTE ON percentZastup TO xrybar04;

---------------------------------------------------
-------------Materializovany pohlad----------------
---------------------------------------------------
DROP MATERIALIZED VIEW zamestanecFun;

CREATE MATERIALIZED VIEW LOG ON zamestnanec WITH PRIMARY KEY,ROWID(funkcia)INCLUDING NEW VALUES;

CREATE MATERIALIZED VIEW zamestanecFun 
CACHE --postupne optimalizuje citanie z pohladu
BUILD IMMEDIATE --naplni pohlad hned po jeho vytvoreni
REFRESH FAST ON COMMIT --postupne optimalizuje citanie z pohladu
ENABLE QUERY REWRITE --bude pouzivany optimalizatorom
AS SELECT Z.funkcia, count(Z.funkcia) as ZastupenieFunkcie
FROM zamestnanec Z
GROUP BY Z.funkcia;

GRANT ALL ON zamestanecFun TO xrybar04;

SELECT * from zamestanecFun;
INSERT INTO zamestnanec (RodneCislo, meno, priezvisko, titul, datumNarodenia, adresa, funkcia, IBAN) VALUES('8802012151','Michal', 'Nikel', 'MGR', TO_DATE('01-02-1968', 'dd-mm-yyyy'), 'Trnava, 7995', 'zverolekár', 'CZ6508000000192000145399');
COMMIT;
SELECT * from zamestanecFun;