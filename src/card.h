#ifndef CARD_H
#define CARD_H

#include "usrtypes.h"
#include <allegro5/allegro.h>

typedef enum card_type
{
    ATTACK, DEFENSE, STATUS
} card_type_t;

typedef enum status_type
{
    PREDICTION, ACCURACY, CALIBER, VELOCITY, MUZZLE
} status_type_t;

typedef struct card
{
    card_type_t type;
    char* name;
    char* description;
    ALLEGRO_BITMAP* bitmap;
} card_t;

#endif