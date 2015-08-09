#!/usr/bin/env python3.3
# -*- coding: utf-8 -*-
#SYN:xgulan00

import argparse
import sys
import re
import io

import parser
import styler

"""
Funkcia ktora vytlaci napovedu na stdin
"""
def napoveda():
    print("• --help vitiskne napovedu\n"
    "• --format=filename urceni formatovaciho souboru. Soubor bude obsahovat libovolne\n"
    "mnozstvi formatovacich zaznamu. Formatovaci zaznam se bude skladat z regularniho vyrazu\n"
    "vymezujiciho formatovany text a prikazu pro formatovani tohoto textu. Detailni popis viz nize.\n"
    "• --input=filename urceni vstupniho souboru v kodovani UTF-8.\n"
    "• --output=filename urceni vystupniho souboru opet v kodovani UTF-8 s naformatovanym\n"
    "vstupnim textem. Formatovani bude na vystupu realizovano nekterymi HTML elementy na\n"
    "popis formatovani. Regularni vyrazy ve formatovacim souboru urci, ktery text se naformatuje,\n"
    "a formatovaci informace prirazene k danemu vyrazu urci, jakym zpusobem se tento text naformatuje.\n"
    "• --br prida element <br /> na konec kazdeho radku puvodniho vstupniho textu (az po aplikaci formatovacich prikazu).")
    exit(0)

#nastavenie vsetkych moznych argumentov
argumentParser = argparse.ArgumentParser(add_help=False)
argumentParser.add_argument('--help', action="count", dest="help")
argumentParser.add_argument('--input', action="append", default=None, dest="input")
argumentParser.add_argument('--output', action="append", default=None, dest="output")
argumentParser.add_argument('--format', action="append", default=None, dest="format")
argumentParser.add_argument('--br', action="count", dest="br")

#parsovanie argumentov, ak narazi na chybu napr nepodporovany argument tak vypise chybu
try:
    argumenty = argumentParser.parse_args()
except:
    print("Argumenty neboli zadane spravne", file=sys.stderr)
    sys.exit(1)

#ak je zadany help
if argumenty.help is not None:
    if len(sys.argv) > 2: #ak je zadane viacere argumenty = chyba lebo sa nemozu miesat s help
        print("Argumenty neboli zadane spravne", file = sys.stderr)
        sys.exit(1)
    else:
        napoveda()

#osetrenia aby nemohli sa zadavat viac krat tie iste argumenty
if argumenty.input is not None and (len(argumenty.input) > 1):
    print("Argumenty neboli zadane spravne", file = sys.stderr)
    sys.exit(1)
if argumenty.output is not None and (len(argumenty.output) > 1):
    print("Argumenty neboli zadane spravne", file = sys.stderr)
    sys.exit(1)
if argumenty.format is not None and (len(argumenty.format) > 1):
    print("Argumenty neboli zadane spravne", file = sys.stderr)
    sys.exit(1)
if argumenty.help is not None and argumenty.help > 1:
    print("Argumenty neboli zadane spravne", file = sys.stderr)
    sys.exit(1)
if argumenty.br is not None and argumenty.br > 1:
    print("Argumenty neboli zadane spravne", file = sys.stderr)
    sys.exit(1)

if argumenty.input is not None: #je zadany prepinac input tak otvorim subor
    try:
        with open(argumenty.input[0], 'r') as subor:
            vstup = subor.read()
    except:
        print("Subor sa nepodarilo otvorit alebo neexisuje!", file = sys.stderr)
        sys.exit(2)
    subor.close()
else: #prepinac input neni zadany berem zo stdin vstup
    vstup = sys.stdin.read()

vystup = sys.stdout

#format ak je zadany tak formatujem podla neho, inak iba kopirujem stdin na stdout alebo zo suboru do suboru....
if argumenty.format is not None: #format je zadany
    try:
        with open(argumenty.format[0], 'r') as subor:
            suborFormat = subor.read()
    except: #ak sa vyskytla chyba s formatomvacim suborom, tak sa chovam ako keby tam nebol a kopirujem in na out
        spracovanyVstup = vstup
        if argumenty.br is not None: #odriadkovavam, ak je zadnay br
            spracovanyVstup = re.sub('\n', '<br />\n', spracovanyVstup)

        if argumenty.output is not None: #ak je zadany output tak davam do suboru vystup
            try:
                vystup = open(argumenty.output[0], 'w+')
            except:
                print("Subor sa nepodarilo otvorit alebo vytorit!", file = sys.stderr)
                sys.exit(3)
        print(spracovanyVstup, end="", file = vystup)
        sys.exit(0)

    suborFormat = io.StringIO(suborFormat) #aby sa choval ako subor
    subor.close()
    format = parser.spracujFormat(suborFormat)
    #var_dump(format)
    spracovanyVstup = styler.aplikujStyl(format, vstup)
    #var_dump(pozicie) #ak chcem testovat treba vratit pozicie zo styler.py

    if argumenty.br is not None: #ak je br stylujem konce riadkov
        spracovanyVstup = re.sub('\n', '<br />\n', spracovanyVstup)

    if argumenty.output is not None: #ak je zadany output tak davam vystup doneho
        try:
            vystup = open(argumenty.output[0], 'w+')
        except:
            print("Subor sa nepodarilo otvorit alebo vytorit!", file = sys.stderr)
            sys.exit(3)
    print(spracovanyVstup, end="", file = vystup)

else: #format neni zadany tak iba vstup kopirujem na vystup
    spracovanyVstup = vstup
    if argumenty.output is not None: #zadany je vystup tak vystup davam donho
        try:
            vystup = open(argumenty.output[0], 'w+')
        except:
            print("Subor sa nepodarilo otvorit alebo vytorit!", file = sys.stderr)
            sys.exit(3)

    if argumenty.br is not None: #ak je prepinac br tak davam na konce riadkov br
        spracovanyVstup = re.sub('\n', '<br />\n', spracovanyVstup)

    try:
        print(spracovanyVstup, end="", file = vystup)
    except:
        print("Nepodarilo sa zapisat informacie!", file = sys.stderr)
        sys.exit(4)