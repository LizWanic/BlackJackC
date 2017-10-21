// ----------------------------------------------------------------------
// file: card.c
//
// Description: This file implements the CARD module. Its job is to 
//     create an interface for providing cards from a shuffled deck of
//     52 standing playing cards. Each call to card_get() will return
//     the top card in that shuffled deck. If all the cards get used, 
//     then the deck is invisibly (and unknowingly) reshuffled.
//
// Created: 2016-05-03 (P. Clark)
// ----------------------------------------------------------------------
#include "card.h"
#include "common.h"
#include <time.h>
#include <stdlib.h>
#include <unistd.h>

#define CARDS_PER_DECK 52
#define MIN_HEARTS 13
#define MIN_SPADES 26
#define MIN_DIAMONDS 39
#define CARD_OPTIONS 13
#define AVAILABLE 0
#define NOT_AVAILABLE 1

static unsigned int Deck_shuffled = FALSE;
static unsigned char dealt[CARDS_PER_DECK];
static unsigned char number_dealt = 0;


// Get a card from the current deck.
// suit: This is interpreted as follows:
//     1 = Clubs
//     2 = Hearts
//     3 = Spades
//     4 = Diamonds
// pattern: This is interpreted as the 
//     1 = Ace
//     2..10 as expected
//     11 = Jack
//     12 = Queen
//     13 = King


extern void card_get(unsigned char *suit, unsigned char *pattern)
{

        // initialize random number generator
        srandom(time(NULL));

        // once 52 cards have been drawn, shuffle the deck
        if (number_dealt == CARDS_PER_DECK){
                Deck_shuffled = FALSE;
                number_dealt = 0;
        }

        // set all slots in the dealt array to 0 (new deck)
        if (!Deck_shuffled) {
                
                for (int i=0; i<CARDS_PER_DECK; ++i){
                       dealt[i] = AVAILABLE;
                }; 

                Deck_shuffled = TRUE;
        }
        
        // loop to select the card
        while (1){
                // get a random number between 0 and 51
                int j = random() % CARDS_PER_DECK;

                // check if that position in the dealt deck is available
                if (dealt[j] == AVAILABLE){
                        
                        // determine the suit
                        if (j<MIN_HEARTS){
                                *suit = CLUBS;
                        }else if ((j>=MIN_HEARTS) && (j<MIN_SPADES)){
                                *suit = HEARTS;
                        }else if ((j>=MIN_SPADES) && (j<MIN_DIAMONDS)){
                                *suit = SPADES;
                        }else{
                                *suit = DIAMONDS;
                        }

                        // determine the pattern
                        switch ((j%CARD_OPTIONS)+1){
                                case 1: *pattern = ACE;
                                        break;
                                case 2: *pattern = 2;
                                        break;
                                case 3: *pattern = 3;
                                        break;
                                case 4: *pattern = 4;
                                        break;
                                case 5: *pattern = 5;
                                        break;
                                case 6: *pattern = 6;
                                        break;
                                case 7: *pattern = 7;
                                        break;
                                case 8: *pattern = 8;
                                        break;
                                case 9: *pattern = 9;
                                        break;
                                case 10: *pattern = 10;
                                         break;
                                case 11: *pattern = JACK;
                                         break;
                                case 12: *pattern = QUEEN;
                                         break;
                                case 13: *pattern = KING;
                                         break;
                        }

                        // mark the card as no longer available 
                        // exit if statement
                        dealt[j] = NOT_AVAILABLE;
                        break;
                }

        }
        
        // increment the number of cards dealt
        number_dealt++;

} // card_get()


// end of card.c
