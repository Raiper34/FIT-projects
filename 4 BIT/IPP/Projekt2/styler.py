#!/usr/bin/env python3
# -*- coding: utf-8 -*-
#SYN:xgulan00

import re

pozicie = []

"""
Funkcia ktora vypocita poziciu vzhladom na uz aplikovane atagy ktore su v od tohoto tagu nalavo
Argumenty: styl pre ktory to pocitam, poziciaZac ktora je zaciatocna pozicia bez vsetkoho
Vracia: danu vypocitanu poziciu
"""
def vypocitajpoziciu(styl, poziciaZac):
    global pozicie
    poziciaVypocitana = poziciaZac
    if "/" in styl: #ak je tam / tak je to ukoncovaci tag a pocitam inak ako pre otvaraci
        for pozicia in pozicie:
            if poziciaZac > pozicia["def"]: #ak je tam uz dany tag, ktory je skor, tak pridam do pozicie tagu dlzky vsetkych jeho predchodcov
                poziciaVypocitana += len(pozicia["styl"])
    else: #pocitam pre otvaraci tag
        for pozicia in pozicie:
            if poziciaZac >= pozicia["def"]:
                poziciaVypocitana += len(pozicia["styl"])
    return poziciaVypocitana

"""
Funkcia ktora mi podla stylu vracia presny otvaraci tag ako ma vyzerat
Argumenty: styl ktory prevadzam na presny tag
Vracia: presny tag
"""
def otvaraciTag(styl):
    if "size" in styl:
        pomocna = re.sub("^", "<font ", styl)
        pomocna = re.sub("$", ">", pomocna)
        pomocna = re.sub(":", "=", pomocna)
        return pomocna
    if "color" in styl:
        pomocna = re.sub("^", "<font ", styl)
        pomocna = re.sub("$", ">", pomocna)
        pomocna = re.sub(":", "=#", pomocna)
        return pomocna
    elif "bold" in styl:
        return "<b>"
    elif "italic" in styl:
        return "<i>"
    elif "teletype" in styl:
        return "<tt>"
    else:
        return "<u>"

"""
Funkcia ktora mi podla stylu vracia presny ukoncovaci tag ako ma vyzerat
Argumenty: styl ktory prevadzam na presny tag
Vracia: presny tag
"""
def ukoncovaciTag(styl):
    if "size" in styl:
        return "</font>"
    elif "color" in styl:
        return "</font>"
    elif "bold" in styl:
        return "</b>"
    elif "italic" in styl:
        return "</i>"
    elif "teletype" in styl:
        return "</tt>"
    else:
        return "</u>"

"""
Funkcia, ktora aplikuje z pomocneho slovnika dane stily podla regularneho vyrazu
Argumenty: formatovaci slovnik, vtup ktory stylujem
Vracia: ostylovany vstup
"""
def aplikujStyl(format, vstup):
    global pozicie
    for zaznam in format: #prechadzam slovnik a kazdy styl + regularny vyraz
        for vyskyt in re.finditer(zaznam["regularny"], vstup, re.DOTALL): #najde mi defaulne pozicie kde sa ma regularny vyraz aplikovat
            if vyskyt.group(0) == "":
                continue
            pozicie.append({'styl': otvaraciTag(zaznam["styl"]), 'def': vyskyt.start(), 'pozicia': vypocitajpoziciu(otvaraciTag(zaznam["styl"]), vyskyt.start())})
            pozicie.append({'styl': ukoncovaciTag(zaznam["styl"]), 'def': vyskyt.end(), 'pozicia': vypocitajpoziciu(ukoncovaciTag(zaznam["styl"]), vyskyt.end())})
    for pozicia in pozicie: #podla vypocitanych umiestneny aplikujem dany styl/tag na dane miesto
        vstup = vstup[:pozicia["pozicia"]] + pozicia["styl"] + vstup[pozicia["pozicia"]:]
    return vstup