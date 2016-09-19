// GameView.c ... GameView ADT implementation

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
	int traps; //number of traps/vamps 
	int vampires;
	

	//if hide move is used new location is same as last
	//check whole trail for any duplicate cities
	TLink next;
}tNode;

struct gameView {
	int score;
	int health[NUM_PLAYERS];
	
	TLink trailHead[NUM_PLAYERS];
	
	Round round;
	PlayerID currPlayer;
};

static void makeTrail(GameView g);
static void updateTrail(GameView g, LocationID location, PlayerID player);
static void encounters(char* events);

static int charToPlayerID(char player);

// Creates a new GameView to summarise the current state of the game
GameView newGameView(char *pastPlays, PlayerMessage messages[])
{
    //REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	int i,j;
	int len = strlen(pastPlays);
	int id; 
	char *loc = "  ";
	char *events = "    ";
	GameView gameView = malloc(sizeof(struct gameView));
	gameView->round = (len+1)/(5*8); //40 chars including spaces make a round
	gameView->score = GAME_START_SCORE;
	
	for(i=0;i<NUM_PLAYERS-1;i++) 
		gameView->health[i] = GAME_START_HUNTER_LIFE_POINTS;
	gameView->health[PLAYER_DRACULA] = GAME_START_BLOOD_POINTS;
	
	makeTrail(gameView);
	
	for (i=0;i<len;i+=8){ 
		//printf("%d\n",i);
		gameView->currPlayer = charToPlayerID(pastPlays[i]);
		
		strncpy(loc, &pastPlays[i+1],2);
		id = abbrevToID(loc);
		/*if(abbrev == "C?"){
			id = 
		}else if(abbrev == "S?"){
			
		}else if(abbrev == "HI"){
			
		}else if(abbrev[1] == 'D'){
			
		}else if(abbrev == "TP"){
			
		}*/
		updateTrail(gameView, id, gameView->currPlayer);
		
		
		j=1;
		if (pastPlays[i+j] == 'D'){
			
			switch (pastPlays[i]){
				case 'T'://trap
					break;
				case 'V'://vampire
					break;
				case 'D'://dracula
					break;
			}
			
		}else{
			switch (pastPlays[i+j]){
				case 'T'://trap
					break;
				case 'V'://vampire
					break;
				case 'D'://dracula
					break;
			}
		}
	
	}
	gameView->currPlayer++;
	return gameView;
}
     
     
// Frees all memory previously allocated for the GameView toBeDeleted
void disposeGameView(GameView toBeDeleted)
{
    //COMPLETE THIS IMPLEMENTATION
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
	
    //REPLACE THIS WITH YOUR OWN IMPLEMENTATION
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
	g->score -= curr->vampires; //if a vampire is in a move leaving the trail

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

static void encounters(char* events){

}










