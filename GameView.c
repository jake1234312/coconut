// GameView.c ... GameView ADT implementation

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "Globals.h"
#include "Game.h"
#include "GameView.h"
#include "Map.h" //... if you decide to use the Map ADT

typedef struct tNode *TLink;

typedef struct tNode{
	LocationID location;
	int traps; //number of traps/vamps (only appear in drac's trail)
	int vampires;
	
	TLink next; 
}tNode;

struct gameView {
	int score;
	int health[NUM_PLAYERS];
	
	TLink trailHead[NUM_PLAYERS];
	// the head of the linked list contains the current location
	
	Round round;
	PlayerID currPlayer;
};

static void makeTrail(GameView g);
static void updateTrail(GameView g, LocationID location, PlayerID player);
static void encounters(GameView g, char* events, PlayerID player);

static int myAbbrevToID(char *abbrev);
static int myIDToType(GameView g, PlayerID player);
static int charToPlayerID(char player);

// Creates a new GameView to summarise the current state of the game
GameView newGameView(char *pastPlays, PlayerMessage messages[])
{
	int i;
	int len = strlen(pastPlays);
	int id;
	int player = PLAYER_DRACULA;
	char loc[] = "  ";
	char events[] = "    ";
	GameView gameView = malloc(sizeof(struct gameView));
	gameView->round = (len+1)/(5*8); //40 chars including spaces make a round
	gameView->score = GAME_START_SCORE;
	
	for(i=0;i<NUM_PLAYERS-1;i++) //initialise health
		gameView->health[i] = GAME_START_HUNTER_LIFE_POINTS;
		
	gameView->health[PLAYER_DRACULA] = GAME_START_BLOOD_POINTS;
	gameView->currPlayer = 0;
	makeTrail(gameView); //make array of lists (6x6)
	for (i=0;i<len;i+=8){ //for each set of 8chars(one move)
		player = charToPlayerID(pastPlays[i]);
		
		strncpy(loc, &pastPlays[i+1],2);
		id = myAbbrevToID(loc);
		updateTrail(gameView, id, player);
		
		strncpy(events, &pastPlays[i+3],4);
		encounters(gameView, events, player);
		
		
		if(player == PLAYER_DRACULA){
			gameView->score -= SCORE_LOSS_DRACULA_TURN;
			if (gameView->trailHead[player]->location == CASTLE_DRACULA || 
				 gameView->trailHead[player]->location == TELEPORT)
				gameView->health[player] += LIFE_GAIN_CASTLE_DRACULA;
				
			if (myIDToType(gameView,player) == SEA)
					gameView->health[player] -= LIFE_LOSS_SEA;
		} else {
			if (gameView->trailHead[player]->location == ST_JOSEPH_AND_ST_MARYS || gameView->health[player] <= 0){
				gameView->score -= SCORE_LOSS_HUNTER_HOSPITAL;
				gameView->health[player] = GAME_START_HUNTER_LIFE_POINTS;
			}else if ((gameView->trailHead[player]->location == gameView->trailHead[player]->next->location) &&
				       (gameView->health[player] < GAME_START_HUNTER_LIFE_POINTS)){
				gameView->health[player] += LIFE_GAIN_REST;
				printf("rest\n");
			}
			
		}
				
	}	
	gameView->currPlayer = (player + 1)%NUM_PLAYERS; //current player is actually the next player(when given to the AI)
	return gameView;
}
     
     
// Frees all memory previously allocated for the GameView toBeDeleted
void disposeGameView(GameView toBeDeleted)
{
    int i,j;
    TLink curr,prev;
    
    for (i=0;i<NUM_PLAYERS;i++){
    	curr = toBeDeleted->trailHead[i];
    	for (j=0;j<TRAIL_SIZE;j++){
    		prev = curr;
    		curr = curr->next;
    		free(prev);
    	}   	
    }
    free( toBeDeleted );
}


//// Functions to return simple information about the current state of the game

// Get the current round
Round getRound(GameView currentView)
{
    return currentView->round;
}

// Get the id of current player - ie whose turn is it?
PlayerID getCurrentPlayer(GameView currentView)
{
    return currentView->currPlayer;
}

// Get the current score
int getScore(GameView currentView)
{
    return currentView->score;
}

// Get the current health points for a given player
int getHealth(GameView currentView, PlayerID player)
{
    return currentView->health[player];
}

// Get the current location id of a given player
LocationID getLocation(GameView currentView, PlayerID player)
{
    return currentView->trailHead[player]->location;
}

//// Functions that return information about the history of the game

// Fills the trail array with the location ids of the last 6 turns
void getHistory(GameView currentView, PlayerID player,
                            LocationID trail[TRAIL_SIZE])
{
	TLink curr;
	int i = 0;
	for (curr = currentView->trailHead[player];curr != NULL;curr = curr->next){
		printf("update location %d at %d\n",curr->location,i);
		trail[i++] = curr->location;
	}
}

//// Functions that query the map to find information about connectivity

// Returns an array of LocationIDs for all directly connected locations

LocationID *connectedLocations(GameView currentView, int *numLocations,
                               LocationID from, PlayerID player, Round round,
                               int road, int rail, int sea)
{
    //REPLACE THIS WITH YOUR OWN IMPLEMENTATION
    return NULL;
}

static void makeTrail(GameView g){
	int i,j;
	TLink curr;
	for (i=0;i<NUM_PLAYERS;i++){
		for (j=0;j<TRAIL_SIZE;j++){
			if (j == 0){
				g->trailHead[i] = malloc(sizeof(tNode));
				curr = g->trailHead[i];
			} else {
				curr->next = malloc(sizeof(tNode));
				curr = curr->next;
			}
			curr->location = NOWHERE;
			curr->traps = 0;
			curr->vampires = 0;
		}
	}
}

static void updateTrail(GameView g, LocationID location, PlayerID player){
	TLink first = g->trailHead[player];
	TLink curr = first->next->next->next->next->next;//last node
	first->next->next->next->next->next = NULL;

	g->trailHead[player] = curr;
	//if a vampire is in a move leaving the trail
	printf("player %d is at %d\n",player,location);
	curr->location = location;
	curr->vampires = 0;
	curr->traps = 0;
	curr->next = first;
}

static int charToPlayerID(char player){
	int id=0;
	switch (player){
		case 'G':
			id = PLAYER_LORD_GODALMING;break;
		case 'S':
			id = PLAYER_DR_SEWARD;break;
		case 'H':
			id = PLAYER_VAN_HELSING;break;
		case 'M':
			id = PLAYER_MINA_HARKER;break;
		case 'D':
			id = PLAYER_DRACULA;break;
	}
	return id;
}

static int myAbbrevToID(char *abbrev){
	int id = abbrevToID(abbrev);
	//TODO special locations update events
	// if drac double backs into sea
		if(strcmp(abbrev, "C?") == 0){
			id = CITY_UNKNOWN;
		}else if(strcmp(abbrev, "S?") == 0){
			id = SEA_UNKNOWN;
		}else if(strcmp(abbrev, "HI") == 0){
			id = HIDE;
		}else if(abbrev[0] == 'D'){ //DOUBLE_BACK
			id = HIDE + abbrev[1]-'0'; //convert ascii char to int
		}else if(strcmp(abbrev, "TP") == 0){
			id = TELEPORT;
		}
	return id;
}

static int myIDToType(GameView g, PlayerID player){
	int type = UNKNOWN;
	int i = 0;
	TLink curr = g->trailHead[player];
	LocationID location = curr->location;
	
	//hide case is irrellevant for now
	if (location >= DOUBLE_BACK_1 && location <= DOUBLE_BACK_5){
		for (i=0;i<location-HIDE;i++) curr = curr->next; //how far back
		location = curr->location;
	}
	switch (location){
		case CITY_UNKNOWN:
			type = LAND;
			break;
		case SEA_UNKNOWN:
			type = SEA;
			break;
		case HIDE:
			type = UNKNOWN;
			break; //not correct but doesnt matter
		case TELEPORT:
			location = CASTLE_DRACULA;
		default:
			type = idToType(location);
	}
	return type;		
}

static void encounters(GameView g, char* events, PlayerID player){
	int i;
	//TODO what happens when health/score/blood drops bellow 0
	for (i=0;i<4;i++){
		if (player == PLAYER_DRACULA){
			switch (events[i]){
				case 'T': //trap placed
					g->trailHead[player]->traps++;
					break;
				case 'M': //malfunction
					//TODO this might be already taken care of
					break;
				case 'V':
					if (i < 2){ //vamp placed
						g->trailHead[player]->vampires++;
					} else { //vamp matured
						g->score -= SCORE_LOSS_VAMPIRE_MATURES;
					}
					break;
			}
		}else{
			switch (events[i]){
				case 'T'://trap sprung
					g->health[player] -= LIFE_LOSS_TRAP_ENCOUNTER;
					//TODO remove from drac trail
					break;
				case 'V'://vampire slain
					//remove from drac trail
					break;
				case 'D'://dracula encounter
					g->health[player] -= LIFE_LOSS_DRACULA_ENCOUNTER;
					g->health[PLAYER_DRACULA] -= LIFE_LOSS_HUNTER_ENCOUNTER;
					break;
			}
		}
	}
}
