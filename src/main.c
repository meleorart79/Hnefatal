/* Hnefatal - Jeu de stratégie inspiré du Hnefatafl
		réalisé par David Dakeu, le 11/11/2026
		pour l'utiliser (faux pour le moment):
			gcc -Wall hnefatal.c
			./a.out
*/
#include <stdio.h>
#include <string.h>
#include "rules.h"
#include "board.h"
#include "ui.h"

const char* game_title(void) { return "HNEFATAL"; }
const char* game_version(void) { return "0.3.0"; }

void print_help(void)
{
	printf("\n========== %s %s - AIDE ==========\n\n", game_title(void), game_version(void));

	printf("OBJECTIF :\n");
	printf("  Les attaquants tentent de capturer le roi défenseur.\n");
	printf("  Les défenseurs tentent de faire échapper leur roi. (Le bord du plateau)\n");
	
	printf("COMMANDES :\n");
	printf("  F2 to F5   Deplacer une piece de F2 vers F5.\n");
	printf("  --help     Afficher cette aide.\n");
	printf("  FF         Abandonner la partie.\n");
	printf("  Q          Quitter le jeu.\n\n");

	printf("COMMENT JOUER :\n");
	printf("  - Les joueurs jouent chacun leur tour.\n");
	printf("  - Entrez la position de depart et la destination.\n");
	printf("  - Les deplacements se font horizontalement ou verticalement.\n");
	printf("  - Une piece ne peut pas traverser une autre piece.\n");
	printf("  - Seul le roi peut occuper ou traverser le chateau central.\n\n");
	
	printf("REGLES DE CAPTURE :\n");
	printf("  - Une piece est capturée si elle est en sandwich par deux pieces ennemies.\n");
	printf("  - Un groupe de pieces peut être capturé si elles sont encerclées par des pieces ennemies.\n");
	printf("  - Contrairement au bord du plateau, le chateau central non-occupé compte comme une piece ennemie.\n");

	printf("=====================================\n\n");
}

int main(void) {
	char to_col = 'A', from_col = 'A';
	int  to_row = 1, from_row = 1, game_over = 0;
	Party turn_player = PARTY_A;
	Board board;

	Position last_move;
	int has_last_move = 0;

	if (!board_init(&board)) {
		fprintf(stderr, "Erreur lors de l'initialisation du plateau.\n");
		return 1;
	}

	print_help();

	do { /* __ boucle sur les coups joués au sein d'une partie */
		turn_player = (turn_player == PARTY_A) ? PARTY_D : PARTY_A;

		render_board(&board, has_last_move ? &last_move : NULL);
		printf("Tour du joueur %c : ", turn_player == PARTY_A ? 'A' : 'D');

		Position from_pos, to_pos;
		MoveResult result;
		do {
			char input[100];
			char extra;
			char command[7], command1, command2;

			if (fgets(input, sizeof(input), stdin) == NULL)
				return 0;

			/* --help */
			if (sscanf(input, " %6s %c", command, &extra) == 1 &&
				strcmp(command, "--help") == 0) {
				print_help();
				result = MOVE_ERR_BAD_ARGUMENT;
				continue;
			}

			/* Quitter sans déclarer de vainqueur */
			if (sscanf(input, " %c %c", &command1, &extra) == 1 &&
				(command1 == 'Q' || command1 == 'q')) {
				printf("Fermeture du jeu.\n");
				return 0;
			}

			/* Abandonner la partie */
			if (sscanf(input, " %c%c %c", &command1, &command2, &extra) == 2 &&
				(command1 == 'F' || command1 == 'f') &&
				(command2 == 'F' || command2 == 'f')) {

				printf("Le joueur %c abandonne. Victoire du joueur %c !\n",
					turn_player == PARTY_A ? 'A' : 'D',
					turn_player == PARTY_A ? 'D' : 'A');

				game_over = 1;
				break;
			}

			/* Lire un déplacement */
			if (sscanf(input, " %c%d to %c%d %c",
				&from_col, &from_row, &to_col, &to_row, &extra) != 4) {

				printf("Format: F2 to F5. Try again: ");
				result = MOVE_ERR_BAD_ARGUMENT;
				continue;
			}

			from_pos.row = from_row;
			from_pos.column = from_col;
			to_pos.row = to_row;
			to_pos.column = to_col;

			result = check_move(&board, &from_pos, &to_pos, turn_player);

			if (result != MOVE_OK)
				printf("%s Try again: ", move_error_message(result));

		} while (result != MOVE_OK);

		if (game_over) {
			break;
		}

		apply_move(&board, &from_pos, &to_pos);

		last_move = to_pos;
		has_last_move = 1;

		check_capture(&board, &to_pos);

		/* test état partie */
		switch (is_game_over(&board)) {
		case 0:
			game_over = 0;
			break;

		case 1:
			printf("Les attaquants ont réussi à capturer le roi!\n");
			game_over = 1;
			break;

		case 2:
			printf("Les défenseurs ont réussi à exfiltrer leur roi!\n");
			game_over = 1;
			break;

		default:
			printf("Il n'y a plus d'interêt à capturer le roi qui a perdu tout ses défenseurs! Match nul.\n");
			game_over = 1;
			break;
		}
	} while (!game_over);
	render_board(&board, has_last_move ? &last_move : NULL);

	/* __ Fin du programme */
	printf("GG\n");
	return 0;
}