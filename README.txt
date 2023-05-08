# Implementovaná funkčnost:
  - hlavní menu - tlačítka pro výběr režimu hry a ukončení
  - normální hra (panáček se snaží dostat do cíle, než ho sní duchové)
    - pohyb panáčka pomocí kláves:
        W
      A   S
        D
    - samostatný pohyb duchů, s výběrem cesty pomocí algoritmu A*, kteří
      jsou naváděni na panáčka
    - systém klíčů, panáček musí posbírat všechny klíče, aby mohl vyhrát dosáhnutím cíle
    - ukazatel hraného času a počítadla klíčů nad herní mapou
    - automatické logování hry do souboru
  - zpětné přehrávání logované hry

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
