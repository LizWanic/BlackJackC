// ---------------------------------------------------------------------
// file: table.c
//
// Description: This file implements the TABLE module. It manages the
//     board table for a blackjack game.
//
// Created: 2016-05-03 (P. Clark)
//
// 2016-06-24 (P. Clark)
//     Display updated stats *before* saying you won/lost.
//     Hide the dealer's second card until it is supposed to be shown.
//     Allow player to press 'q' to quit instead of 'c' to continue.
// 2016-10-26  (P. Clark)
//     Added 'static' to internal functions.
// ---------------------------------------------------------------------
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include "common.h"
#include "card.h"
#include "table.h"


#define TABLE_MIN_COLS 80
#define TABLE_MIN_ROWS 24
#define TABLE_INITIALIZED 4567
#define CLEAR_SCREEN "\033[2J"

#define RESET  "\033[0m"
#define BLACK  "\033[42;30m"
#define NORMAL "\033[42;30m"
#define RED    "\033[42;31m"
#define HIDDEN "\033[42;32m"
#define BLUE   "\033[42;34m"


#define MOVE_TOP_LEFT  "\033[1;1H"
#define MOVE_FOR_INPUT "\033[23;1H"
#define DEALER_ROW 3
#define DEALER_COL 20
#define PLAYER_ROW 3
#define PLAYER_COL 60
#define WINS_ROW 20
#define LOSS_ROW 21
#define STARTING_CARD_ROW (DEALER_ROW + 2)
#define MESSAGE_START_ROW 15
#define MESSAGE_START_COL 30

// Shapes for displaying cards (in unicode)
#define SPADE       "\u2660" /* black spade */
#define CLUB        "\u2663" /* black club */
#define HEART       "\u2665" /* red heart */
#define DIAMOND     "\u2666" /* red diamond */

// Module database (i.e., file-level globals)
static unsigned int Table_state = 0;
static unsigned int Table_wins;
static unsigned int Table_losses;
static unsigned int Table_rows;
static unsigned int Table_cols;
static unsigned char Num_cards_dealer;
static unsigned char Next_card_player;
static unsigned char Second_suit;
static unsigned char Second_pattern;
static unsigned char Hidden_shown;


// ************************************************************************
// ***************** I N T E R N A L   F U N C T I O N S ******************
// ************************************************************************

static void draw_menu(void)
{
        printf(MOVE_TOP_LEFT);
        printf("\n\n");
        printf(BLUE);
        printf(" Menu\n");
        printf("------\n");
        printf(RED);    printf("H");
        printf(NORMAL); printf("=Hit\n");
        printf(RED);    printf("S");
        printf(NORMAL); printf("=Stand\n");
        printf(RED);    printf("Q");
        printf(NORMAL); printf("=Quit\n");
} // draw_menu()



static void draw_stats(void)
{
        // Score
        printf(BLUE);
        printf("\033[%d;1H", WINS_ROW);
        printf("Wins:   %4d", Table_wins);
        printf("\033[%d;1H", LOSS_ROW);
        printf("Losses: %4d", Table_losses);
        printf("\n\n");
        printf(HIDDEN);
        printf(MOVE_TOP_LEFT);
        fflush(stdout);
} // draw_stats()



static void show_card(const unsigned char suit,
               const unsigned char pattern,
               const unsigned char row,
               const unsigned char col)
{
        // Move to the correct location and set the color
        // that is appropriate to the suit.
        printf("\033[%d;%dH", row, col);
        switch (suit) {
                case HEARTS:
                case DIAMONDS:
                        printf(RED);
                        break;
                case SPADES:
                case CLUBS:
                        printf(BLACK);
                        break;
                default:
                        printf(BLUE);
                        break;
        }


        // Display the card 
        if (pattern > ACE && pattern < JACK) {
                printf("%d ", pattern);
        } else {
                switch (pattern) {
                        case JACK:
                                printf("J");
                                break;
                        case QUEEN:
                                printf("Q");
                                break;
                        case KING:
                                printf("K");
                                break;
                        case ACE:
                                printf("A");
                                break;
                        default:
                                printf("X");
                                break;
                }
                printf(" ");
        }
        switch (suit) {
                case CLUBS: 
                        printf(CLUB);
                        break;
                case HEARTS:
                        printf(HEART);
                        break;
                case SPADES:
                        printf(SPADE);
                        break;
                case DIAMONDS: 
                        printf(DIAMOND);
                        break;
                default:
                        printf("X");
                        break;
        }
        printf(HIDDEN);
        printf(MOVE_TOP_LEFT);
        fflush(stdout);
} // show_card()



static void draw_table(void) 
{
        // give us a green table
        printf(NORMAL);
        printf(CLEAR_SCREEN);

        // Title
        printf(MOVE_TOP_LEFT);
        printf(RED);
        printf("                                B L A C K J A C K\n");

        // menu
        draw_menu();

        // Headings
        printf(BLUE);
        printf("\033[%d;%dH", DEALER_ROW, DEALER_COL);
        printf("Dealer");
        printf("\033[%d;%dH", DEALER_ROW+1, DEALER_COL);
        printf("------");
        printf("\033[%d;%dH", PLAYER_ROW, PLAYER_COL);
        printf("You");
        printf("\033[%d;%dH", PLAYER_ROW+1, PLAYER_COL);
        printf("---");

        // wins, losses
        draw_stats();

        printf(MOVE_TOP_LEFT);
        fflush(stdout);
} // draw_table()


// ************************************************************************
// ***************** E X T E R N A L   F U N C T I O N S ******************
// ************************************************************************

extern int table_init(void)
{
        struct winsize term;
        int result = SUCCESS;

        // Determine the coordinates of the terminal
        ioctl(0, TIOCGWINSZ, &term);
        Table_rows = term.ws_row;
        Table_cols = term.ws_col;
        if (Table_rows < TABLE_MIN_ROWS || Table_cols < TABLE_MIN_COLS) {
                printf("The terminal must have a minimimum size of "
                       "%d x %d\n", TABLE_MIN_COLS, TABLE_MIN_ROWS);
                result = -1;
        } 
        
        if (result == SUCCESS) {
                Table_state = TABLE_INITIALIZED;
                Table_wins = 0;
                Table_losses = 0;
                Num_cards_dealer = 0;
                Next_card_player = STARTING_CARD_ROW;
                Hidden_shown = 0;
        }

        return result; 
} // table_init()



extern int table_reset(void)
{
        int result = SUCCESS;

        if (Table_state != TABLE_INITIALIZED) {
                result = -1;
        } else {
                Num_cards_dealer = 0;
                Next_card_player = STARTING_CARD_ROW;
                Hidden_shown = 0;

                draw_table();
        }

        return result;
} // table_reset()



extern void table_exit(void)
{
        printf(RESET);
        printf(CLEAR_SCREEN);
        printf(MOVE_TOP_LEFT);
        fflush(stdout);
} // table_exit()



extern int table_get_input(void)
{
        char input = 0;

        printf(HIDDEN);
        printf(MOVE_TOP_LEFT);
        fflush(stdout);
        do {
                read(STDIN_FILENO, &input, 1);
        } while (input == 10);
        printf(MOVE_TOP_LEFT);
        fflush(stdout);

        return input;
} // table_get_input()




extern void table_player_card(const unsigned char suit,
                              const unsigned char pattern)
{
        show_card(suit, pattern, Next_card_player++, PLAYER_COL);
} // table_player_card()



extern void table_dealer_card(const unsigned char suit,
                              const unsigned char pattern)
{
        ++Num_cards_dealer;
        if (Num_cards_dealer == 2) {
                // If this is dealer's second card, then save it for later
                // display (because it will be hidden from player).
                Second_suit = suit;
                Second_pattern = pattern;
                printf("\033[%d;%dH", DEALER_ROW+3, DEALER_COL);
                printf(BLACK);
                printf("? ?");
        } else if (Num_cards_dealer == 3) {
                // We are now dealing third card -- display 2nd card now
                // before showing the third card.
                show_card(Second_suit, Second_pattern, 
                          DEALER_ROW+3, DEALER_COL);
                show_card(suit, pattern, 
                          DEALER_ROW+4, DEALER_COL);
                Hidden_shown = TRUE;
        } else {
                show_card(suit, pattern, 
                          DEALER_ROW+1+Num_cards_dealer, DEALER_COL);
        }
} // table_dealer_card()



extern void table_player_won(void)
{
        int row = MESSAGE_START_ROW;
        int input;

        if (!Hidden_shown) {
                show_card(Second_suit, Second_pattern, 
                          DEALER_ROW+3, DEALER_COL);
        }

        // update stats
        ++Table_wins;
        draw_stats();

        printf(RED);
        printf("\033[%d;%dH", row++, MESSAGE_START_COL);
        printf("+------------------------+");
        printf("\033[%d;%dH", row++, MESSAGE_START_COL);
        printf("|         YOU WON!       |");
        printf("\033[%d;%dH", row++, MESSAGE_START_COL);
        printf("|                        |");
        printf("\033[%d;%dH", row++, MESSAGE_START_COL);
        printf("|   enter C to continue  |");
        printf("\033[%d;%dH", row++, MESSAGE_START_COL);
        printf("+------------------------+");
        fflush(stdout);

        // wait 
        do {
                input = table_get_input();
                if (input == 'q' || input == 'Q') {
                        exit(0);
                }
        } while (input != 'C' && input != 'c');
} // table_player_won()


extern void table_player_draw(void)
{
        int row = MESSAGE_START_ROW;
        int input;

        if (!Hidden_shown) {
                show_card(Second_suit, Second_pattern, 
                          DEALER_ROW+3, DEALER_COL);
        }

        printf(RED);
        printf("\033[%d;%dH", row++, MESSAGE_START_COL);
        printf("+------------------------+");
        printf("\033[%d;%dH", row++, MESSAGE_START_COL);
        printf("|          DRAW          |");
        printf("\033[%d;%dH", row++, MESSAGE_START_COL);
        printf("|                        |");
        printf("\033[%d;%dH", row++, MESSAGE_START_COL);
        printf("|   enter C to continue  |");
        printf("\033[%d;%dH", row++, MESSAGE_START_COL);
        printf("+------------------------+");
        fflush(stdout);

        // wait 
        do {
                input = table_get_input();
                if (input == 'q' || input == 'Q') {
                        exit(0);
                }
        } while (input != 'C' && input != 'c');
} // table_player_draw()



extern void table_player_lost(void)
{
        int row = MESSAGE_START_ROW;
        int input;

        if (!Hidden_shown) {
                show_card(Second_suit, Second_pattern, 
                          DEALER_ROW+3, DEALER_COL);
        }

        // update stats
        ++Table_losses;
        draw_stats();

        printf(RED);
        printf("\033[%d;%dH", row++, MESSAGE_START_COL);
        printf("+------------------------+");
        printf("\033[%d;%dH", row++, MESSAGE_START_COL);
        printf("|         YOU LOST       |");
        printf("\033[%d;%dH", row++, MESSAGE_START_COL);
        printf("|                        |");
        printf("\033[%d;%dH", row++, MESSAGE_START_COL);
        printf("|   enter C to continue  |");
        printf("\033[%d;%dH", row++, MESSAGE_START_COL);
        printf("+------------------------+");
        fflush(stdout);

        // wait 
        do {
                input = table_get_input();
                if (input == 'q' || input == 'Q') {
                        exit(0);
                }
        } while (input != 'C' && input != 'c');
} // table_player_lost()



// end of table.c
