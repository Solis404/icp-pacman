# TODO
### Nutné pro zápočet
- implementovat pathfinding (další třída, log. reprezentace mapy?)
- implementovat N duchů (vyřešit textury, ovládání pomocí pathfinding)

### Když bude čas/bude bavit
- zpracovat obrázky do qrescources, zapéct do executable
- implementace menu a načítání hry pomocí GUI
- zlepšení user experience pacmana (zatočení v pásmu, ne jen na středu)

# Jak vyvíjet?
1. Vygenerovat Makefile z project file-u pomocí `qmake pacman.pro`
2. Spustit překlad pomocí `make`

### Tipy
- Nový makefile se musí vygenerovat při:
  - úpravě project file-u
  - vytvoření nových zdrojáků v src/

Pro funkci LSP je nutné vygenerovat compile_commands.json, lze pomocí pomocí nástroje Bear `make clean && bear -- make` Již je přidán v .gitignore

# Implementační detaily

### Pohyb pacmana
- pacman se pohybuje na mapě načtené ze souboru
- může se pohybovat pouze po volných(černých) políčcích, pokud narazí do zdi
  (modrého políčka) zastaví se
- pacmanův pohyb je řízen časovačem, každých N ms se pohne o jeden px v daném směru
- směr pohybu pacmana lze řídit klávesamy WASD
- pacman se může pohybovat pouze po mřížce (ke změně směru smí nastat pouze ve středu políčka)

### Interakce pacmana s okolím
- pacman interreaguje se svým okolím kolizí
- prvky s kterými pacman interreaguje:
  1. klíče, může je sebrat
  2. cíl, pokud dojde do cíle a všechny klíče jsou sebrané, hra končí úspěchem
  3. TODO - duchové

### Logování
- hra lze spustit v logovacím režimu, potom se průběh hry ukládá do souboru a jde zpětně přehrát
- při přehrávání lze přepínat mezi normálním a zpětným přehráváním pomocí klávesy R
