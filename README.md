# Hnefatal

**Version :** 0.3.0 · **C11 · Jeu en console**

Hnefatal est un jeu de stratégie inspiré du Hnefatafl, qui se joue sur un plateau de 11×11 cases.

- **Les attaquants** doivent capturer le roi.
- **Les défenseurs** doivent permettre au roi d'atteindre le bord du plateau.

## Règles

- Les pièces se déplacent horizontalement ou verticalement, comme les tours aux échecs.
- Elles ne peuvent pas sauter par-dessus d'autres pièces ni traverser le château central.
- Seul le roi peut occuper le château.
- **Prise par sandwich :** une pièce ennemie est prise lorsqu'elle est encadrée par deux pièces alliées. Le château vide peut également servir à réaliser une prise.
- **Prise par encerclement :** un groupe d'au moins deux pièces connectées est capturé lorsqu'aucune de ses pièces ne dispose d'un déplacement légal. Les groupes touchant le bord du plateau sont immunisés.
- Le roi ne bénéficie d'aucune protection particulière contre ces prises.

La partie se termine lorsque le roi est capturé, atteint le bord du plateau, que tous les pions défenseurs sont éliminés ou que tous les attaquants sont éliminés.

## Commandes

| Entrée | Action |
|---|---|
| `F2 to F5` | Déplacer une pièce |
| `--help` | Afficher l'aide |
| `FF` | Abandonner |
| `Q` | Quitter |

## Développement

Le projet est organisé autour de quatre fichiers principaux :

- `board.c` : création du plateau, gestion et conversion des positions.
- `rules.c` : validation des déplacements, prises et conditions de victoire.
- `ui.c` : affichage du plateau dans le terminal.
- `main.c` : boucle de jeu et gestion des entrées.

Les tests se trouvent dans le dossier `tests/` et couvrent le plateau, les déplacements et les prises.

## Compilation

Nécessite GCC et GNU Make.

```bash
make        # Compiler
make run    # Lancer le jeu
make test   # Exécuter les tests
make clean  # Nettoyer les fichiers de compilation
```

## Format des positions

Les positions utilisent une notation inspirée du FEN :

- `a` : attaquant
- `d` : défenseur
- `k` : roi
- Nombre : cases vides consécutives
- `/` : séparation entre les rangées

Exemple de position initiale :

```text
3aaaaa3/5a5/11/a4d4a/a3ddd3a/aa1ddkdd1aa/a3ddd3a/a4d4a/11/5a5/3aaaaa3
```

---

*Développé par David Dakeu.*