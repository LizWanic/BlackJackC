// ----------------------------------------------------------------------
// file: card.h
//
// Description: This is the header file for the CARD module. This module
//    manages a deck of cards by shuffling and dealing a deck as needed.
//
// Created: 2016-05-03 (P. Clark)
//
// ----------------------------------------------------------------------
#ifndef CARD_H
#define CARD_H

#define CLUBS 1
#define HEARTS 2
#define SPADES 3
#define DIAMONDS 4

#define ACE 1
#define JACK 11
#define QUEEN 12
#define KING 13




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
extern void card_get(unsigned char *suit, unsigned char *pattern);



#endif
// end of card.h
