# TODO
- sehnat všechny textury (Pixmaps)
- vysvětlit Patrikovi ideu za OO designem
- zobrazování/načítání mapy
- hýbání s pacmanem

# Jak vyvíjet?
1. Vygenerovat Makefile z project file-u pomocí `qmake pacman.pro`
2. Spustit překlad pomocí `make`

### Tipy
- Nový makefile se musí vygenerovat při:
  - úpravě project file-u
  - vytvoření nových zdrojáků v src/

Pro funkci LSP je nutné vygenerovat compile_commands.json, lze pomocí pomocí nástroje Bear `make clean && bear -- make` Již je přidán v .gitignore
