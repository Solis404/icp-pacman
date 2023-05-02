# TODO
### Nutné pro zápočet
- implementovat pathfinding (další třída, log. reprezentace mapy?)
- implementovat N duchů (vyřešit textury, ovládání pomocí pathfinding)
- implementace logování hry a zpětného přehrávání
- implementace "interaktivních prvků (životy pacmana, počet sesbíraných klíčů, ...)"

### Když bude čas/bude bavit
- zpracovat obrázky do qrescources, zapéct do executable
- implementace menu a načítání hry pomocí GUI
- zrobustnění metody pro načítání mapy (lze rozdělit do více)
- zlepšení user experience pacmana (zatočení v pásmu, ne jen na středu)

# Jak vyvíjet?
1. Vygenerovat Makefile z project file-u pomocí `qmake pacman.pro`
2. Spustit překlad pomocí `make`

### Tipy
- Nový makefile se musí vygenerovat při:
  - úpravě project file-u
  - vytvoření nových zdrojáků v src/

Pro funkci LSP je nutné vygenerovat compile_commands.json, lze pomocí pomocí nástroje Bear `make clean && bear -- make` Již je přidán v .gitignore
