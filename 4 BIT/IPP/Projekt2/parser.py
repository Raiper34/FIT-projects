#!/usr/bin/env python3.3
# -*- coding: utf-8 -*-
#SYN:xgulan00

import re
import sys

"""
Funkcia kde je spracovane rozsirenie NQS
Argumenty: regularny vyraz na upravenie
Vracia: regularny vyraz upraveny aby viacnasobnych kvalifikatorov
"""
def rozsirenie(regularny):
    rozsireniReg = ""
    poslednyznak = ""
    for pismenko in regularny: #ak je percento iba si ho uchovavam ci je posledny aby som vedel ze ktore +* je uz noralne alebo escapovane
        if pismenko == "%":
            if poslednyznak == "%":
                poslednyznak = ""
                rozsireniReg += pismenko
            else:
                poslednyznak = "%"
                rozsireniReg += pismenko

        elif pismenko == "*":
            if poslednyznak == "%":
                poslednyznak = ""
                rozsireniReg += pismenko
            elif poslednyznak == "*":
                poslednyznak = "*"
            elif poslednyznak == "+":
                rozsireniReg = rozsireniReg[:-1]
                rozsireniReg += pismenko
                poslednyznak = "*"
            else:
                poslednyznak = "*"
                rozsireniReg += pismenko

        elif pismenko == "+":
            if poslednyznak == "%":
                poslednyznak = ""
                rozsireniReg += pismenko
            elif poslednyznak == "*":
                poslednyznak = "*"
            elif poslednyznak == "+":
                poslednyznak = "+"
            else:
                poslednyznak = "+"
                rozsireniReg += pismenko

        else:
            rozsireniReg += pismenko
            poslednyznak = ""
    return rozsireniReg

"""
Funkcia na zistenie ci je tag validny
Argument: tag na validaciu
"""
def validnyTag(styl):
    if styl == "bold" or styl == "italic" or styl == "underline" or styl == "teletype":
        pass

    elif "size:" in styl:
        try: #zistim ci to je cislo a ak neni tak vyhodim vynimku
            cislo = float(re.sub('size:', '', styl))
        except:
            print("Neni to validne  " + styl, file = sys.stderr)
            exit(4)
            return
        if cislo > 7.0 or cislo < 1.0: #cislo musi byt z toho intervalu ainak chyba
            print("Neni to validne  " + styl, file = sys.stderr)
            exit(4)

    elif "color:" in styl:
        try: #zistim ci to je hexa cislo a ak neni tak vyhodim vynimku
            cislo = int(re.sub('color:', '', styl), 16)
        except:
            print("Neni to validne hexa " + styl, file = sys.stderr)
            exit(4)
        if cislo > int('FFFFFF', 16) or cislo < int('000000', 16): #skontrolujeme povoleny rozsah hexa cisla
             print("Neni to validne rozsah  " + styl, file = sys.stderr)
             exit(4)

    else: #vsetko ostatne je nevalidne
        print("Neni to validne  " + styl, file = sys.stderr)
        exit(4)

"""
Funkcia na prevedenie SYN regexu na Python regex a zistenie ci je validny
Argument: syn regularny vyraz
Vracia: pythonovy regularny vyraz
"""
def validnyRegex(regularny):
    pythonRegularny = ""
    poslednyznak = ""
    negacia = ""
    regularny = rozsirenie(regularny)

    #Osetrenie chyb s bodkou napr  .. bodka na konci na zaciatku...
    if re.match(".*(([^%]|^)([%][%])*)\.\..*", regularny) or re.match("^\..*", regularny) or re.match(".*(([^%]|^)([%][%])*)\.$", regularny):
        print("Regex nevalidny  . " + regularny, file = sys.stderr)
        exit(4)
    #Osetenie chyb s zatvorkami, prazdne zatvorky...
    if re.match(".*(([^%]|^)([%][%])*)\(\).*", regularny):
        print("Regex nevalidny  () " + regularny, file = sys.stderr)
        exit(4)
    #Osetrenie chyb s zvislitkami, napr na konic zaciatku, duplicitna...
    if re.match(".*(([^%]|^)([%][%])*)\|$", regularny) or re.match("^\|.*", regularny) or re.match(".*(([^%]|^)([%][%])*)\|\|.*", regularny):
        print("Regex nevalidny  | " + regularny, file = sys.stderr)
        exit(4)
    #Osetrenie chyb s negaciou, napr na konci, negacia a bodka...
    if re.match(".*(([^%]|^)([%][%])*)!\..*", regularny) or re.match(".*(([^%]|^)([%][%])*)!\|.*", regularny) or re.match(".*(([^%]|^)([%][%])*)!\*.*", regularny) or re.match(".*(([^%]|^)([%][%])*)!!.*", regularny) or re.match(".*(([^%]|^)([%][%])*)!\+.*", regularny):
        print("Regex nevalidny  ! " + regularny, file = sys.stderr)
        exit(4)
    #Osetrenie ostatnych chyb
    if re.match(".*(([^%]|^)([%][%])*)\|\..*", regularny) or re.match(".*(([^%]|^)([%][%])*)\.\|.*", regularny) or re.match("^\+.*", regularny) or re.match("^\*.*", regularny) or re.match(".*(([^%]|^)([%][%])*)\.\*.*", regularny) or re.match(".*(([^%]|^)([%][%])*)\.\+.*", regularny):
        print("Regex nevalidny  | " + regularny, file = sys.stderr)
        exit(4)

    for pismenko in regularny: #prechadzam po pismenku

        if pismenko == "%": #znak na ktory som narazil
            if poslednyznak == "%": #ak bol poslendy znak %, tak to bude mat nejaky specialny vyznam
                pythonRegularny += "[" + "%" + "]" #ak je nastavena negacia tak znegujem
                poslednyznak = ""
                negacia = ""
            else:
                poslednyznak = pismenko #ak nema specialny vyznam iba to tam pridam do posledneho znaku lebo moze za nim nasledovat nieco co % ovplyvni

        elif pismenko == "a":
            if poslednyznak == "%":
                if negacia == "":
                    pythonRegularny += "."
                    poslednyznak = ""
                    negacia = ""
                else:
                    pythonRegularny += "(3\[\]\!\@\#\$RAIPER\%\^\^\&\*\(\)4)"
                    poslednyznak = ""
                    negacia = ""
            else:
                pythonRegularny += "[" + negacia + pismenko + "]"
                poslednyznak = pismenko
                negacia = ""

        elif pismenko == "!":
            if poslednyznak == "%":
                pythonRegularny += negacia + "!"
                poslednyznak = ""
                negacia = ""
            else:
                negacia = "^"
                poslednyznak = pismenko

        elif pismenko == ".":
            if poslednyznak == "%":
                pythonRegularny += negacia + "\."
                poslednyznak = ""
                negacia = ""
            else:
                poslednyznak = pismenko

        #Znaky ktore predstavuju regexy nejaky vecich mnozi napr monzina obsahujuca male a velke pismena....
        elif pismenko == "d":
            if poslednyznak == "%":
                pythonRegularny += "[" + negacia + "0-9" + "]"
                poslednyznak = ""
                negacia = ""
            else:
                pythonRegularny += "[" + negacia + pismenko + "]"
                poslednyznak = pismenko
                negacia = ""

        elif pismenko == "l":
            if poslednyznak == "%":
                pythonRegularny += "[" + negacia + "a-z" + "]"
                poslednyznak = ""
                negacia = ""
            else:
                pythonRegularny += "[" + negacia + pismenko + "]"
                poslednyznak = pismenko
                negacia = ""

        elif pismenko == "L":
            if poslednyznak == "%":
                pythonRegularny += "[" + negacia + "A-Z" + "]"
                poslednyznak = ""
                negacia = ""
            else:
                pythonRegularny += "[" + negacia + pismenko + "]"
                poslednyznak = pismenko
                negacia = ""

        elif pismenko == "w":
            if poslednyznak == "%":
                pythonRegularny += "[" + negacia + "a-zA-Z" + "]"
                poslednyznak = ""
                negacia = ""
            else:
                pythonRegularny += "[" + negacia + pismenko + "]"
                poslednyznak = pismenko
                negacia = ""

        elif pismenko == "W":
            if poslednyznak == "%":
                pythonRegularny += "[" + negacia + "a-zA-Z0-9_" + "]"
                poslednyznak = ""
                negacia = ""
            else:
                pythonRegularny += "[" + negacia + pismenko + "]"
                poslednyznak = pismenko
                negacia = ""


        #Specialne znaky, pri ktorych sa rozhodujem ci ich exsapujem, alebo proste to budu specialne python znaky
        elif pismenko == "|" or pismenko == "+" or pismenko == "(" or pismenko == ")" or pismenko == "*":
            if poslednyznak == "%":
                pythonRegularny += "[" + negacia + "\\" + pismenko + "]"
                poslednyznak = ""
                negacia = ""
            else:
                pythonRegularny += negacia + pismenko
                poslednyznak = pismenko
                negacia = ""


        #Biele znaky
        elif pismenko == "t" or pismenko == "n":
            if poslednyznak == "%":
                pythonRegularny += "[" + negacia + "\\" +pismenko + "]"
                poslednyznak = ""
                negacia = ""
            else:
                pythonRegularny += "[" + negacia + pismenko + "]"
                poslednyznak = pismenko
                negacia = ""

        elif pismenko == "s":
            if poslednyznak == "%":
                pythonRegularny += "[" + negacia + " \t\n\r\f\v" + "]"
                poslednyznak = ""
                negacia = ""
            else:
                pythonRegularny += "[" + negacia + pismenko + "]"
                poslednyznak = pismenko
                negacia = ""

        #Specialne znaky, ktore pythona, ktore musim escapovat aby sa nevykonali
        elif pismenko == "[" or pismenko == "]" or pismenko == "{" or pismenko == "}" or pismenko == "$" or pismenko == "?" or pismenko == "\\" or pismenko == "^":
            pythonRegularny += "[" + negacia + "\\" + pismenko + "]"
            poslednyznak = pismenko
            negacia = ""

        #Vsetko ostatne iba skopirujem, je to ok
        else:
            if poslednyznak == "%":
                print("Regex nevalidny  " + regularny, file = sys.stderr)
                exit(4)
            else:
                pythonRegularny += "[" + negacia + pismenko + "]"
                poslednyznak = pismenko
                negacia = ""

    #Ak su tieto posledne, tak ich este pridam, lebo inak sa nenageneruju ale aj tak bude dalej chyba pri kontrole regexu
    if poslednyznak == "%" or poslednyznak == "!":
        pythonRegularny += "\\"

    #Otestujem ci to je validny regex
    try:
        re.compile(pythonRegularny)
    except re.error:
        print("Regex nevalidny  " + regularny, file = sys.stderr)
        exit(4)
    return pythonRegularny

"""
Funkcia na spracovanie formatovacieho suboru
Arguent: formatovaci subor
Vracia: rozparsovany formatovaci subor ako slovnik
"""
def spracujFormat(suborFormat):
    format = []
    for riadok in suborFormat:
        if not riadok.isspace():
            regularny = re.match('^.*?\t+', riadok) #ziskam si v subore regularny vyraz podla ktoreho pojdem stylovat
            if regularny is not None: #ak tam nejaky regularny je tak ma cenu pokracovat, inak nie
                styly = re.sub(re.escape(regularny.group()), '', riadok)
                regularny = re.sub('\t', '', regularny.group()) #odstranim prebytocne biele tabulatori
                regularny = validnyRegex(regularny)

                for styl in styly.split(','): #zistim si vsetky styly ktore sa v danom riakdu rpe dany regex nachadzaju
                    styl = re.sub('^\s*', '', styl) #odstranim prebytocne biele znaky na zaciatkua  na konci
                    styl = re.sub('\s*$', '', styl)
                    if styl and (not styl.isspace()): #ak styl neni prazdny string
                        validnyTag(styl)
                        format.append({'regularny':regularny, 'styl': styl})
    return format