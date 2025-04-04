#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define MAX_STR 50
#define UP 1
#define RIGHT 2
#define DOWN 3
#define LEFT 4

typedef struct {
  int h; //Coluna atual
  int w; //Linha atual
} Position;

typedef struct{
  Position pos;
  int d;
} NextObject;

typedef struct{
  Position pos;
  char *id;
}Bot;

//Tainha(10-19), Cioba(20-29), Robalo(30-39)
enum peixes{TAINHA, CIOBA, ROBALO};

//COMANDOS
void MOVE(int N){
    if(N == 1) printf("UP\n");
    if(N == 2) printf("RIGHT\n");
    if(N == 3) printf("DOWN\n");
    if(N == 4) printf("LEFT\n");
}

void readData(Position pos, int fs[pos.w][pos.h], int *nBots) {
  int n;
  char str[MAX_STR];

  // lê os dados da área de pesca
  for (int i = 0; i < pos.w; i++) {   
    for (int j = 0; j < pos.h; j++) {
      scanf("%i", &fs[i][j]);
    }
  }
  // lê os dados dos bots
  scanf(" BOTS %i", &n); n = n >= 1? n : 1;
  *nBots = n;
} 

int distance(Position myBot, Position bias){
  int x = 0, y = 0;
  x = myBot.w > bias.w ? myBot.w - bias.w : bias.w - myBot.w;
  y = myBot.h > bias.h ? myBot.h - bias.h : bias.h - myBot.h;

  return x + y;
}

void defineOtherBots(Bot *bots, int nBots){
    for(int i = 0; i < nBots; i++){
        while(bots[i].id == NULL) bots[i].id = (char *) malloc(sizeof(char) * MAX_STR);
        scanf(" %s %d %d", bots[i].id, &bots[i].pos.w, &bots[i].pos.h);
    }
}

void defMyBot(Bot *myBot, Bot *bots, int nBots){
  for(int i = 0; i < nBots; i++){
     if(strcmp(myBot->id, bots[i].id) == 0){
       myBot->pos = bots[i].pos;
       break;
     }
  }
}

NextObject* searchNextFishes(Position size, int fs[size.w][size.h], Bot *mb){
    NextObject *nextFishes = NULL;
    while(nextFishes == NULL) nextFishes = (NextObject *) malloc(sizeof(NextObject)*3);
    nextFishes[TAINHA].d = -1;  nextFishes[CIOBA].d = -1;   nextFishes[ROBALO].d = -1;
    Position aux;
    int i, j, d;
    for (i = 0; i < size.w; i++)
    {
      for (j = 0; j < size.h; j++)
      {
        if(fs[i][j] != 0 || fs[i][j] != 1){
          aux.w = i; aux.h = j;
          d = distance(mb->pos, aux);
          //tainha
          if(11 < fs[i][j] && fs[i][j] <= 19){
            if(nextFishes[TAINHA].d > -1){
              if(d < nextFishes[TAINHA].d){
                nextFishes[TAINHA].d = d;
                nextFishes[TAINHA].pos = aux; }   
            } else {
                nextFishes[TAINHA].d = d;
                nextFishes[TAINHA].pos = aux; 
            }
          } //cioba
          else if(21 < fs[i][j] && fs[i][j] <= 29){
            if(nextFishes[CIOBA].d > -1){
              if(d < nextFishes[CIOBA].d){
                nextFishes[CIOBA].d = d;
                nextFishes[CIOBA].pos = aux;  }   
            } else {
                nextFishes[CIOBA].d = d;
                nextFishes[CIOBA].pos = aux; }
          } //robalo
          else if(31 < fs[i][j] && fs[i][j] <= 39){
            if(nextFishes[ROBALO].d > -1){
              if(d < nextFishes[ROBALO].d){
                nextFishes[ROBALO].d = d;
                nextFishes[ROBALO].pos = aux; }   
            } else {
                nextFishes[ROBALO].d = d;
                nextFishes[ROBALO].pos = aux; }
          }  
        }
      }//for j
    } //for /i
    return nextFishes;
}

Bot searchNextBot(int nBots, Bot *bots, Position pos, char myID[MAX_STR]){
  int menor = -1, d;
  Bot nBot;
  nBot.id = NULL; 
  while(nBot.id == NULL) nBot.id = (char *) malloc(sizeof(char) * MAX_STR); 
  for(int i = 0; i < nBots; i++){
    d = distance(bots[i].pos, pos);
    if(menor == -1){
      menor = d;
      nBot.pos = bots[i].pos;
      strcpy(nBot.id, bots[i].id);
      if(strcmp(bots[i].id, myID) == 0) break;
    } else if(d < menor){
      menor = d;
      nBot.pos = bots[i].pos;
      strcpy(nBot.id, bots[i].id);
      if(strcmp(bots[i].id, myID) == 0) break;
    }
  }
  return nBot;
}

Bot* searchNextBotsToFishes(int nBots, Bot *bots, NextObject *fishes, char myID[MAX_STR]){
  Bot *nextBotsToFishes = NULL;
  while(nextBotsToFishes == NULL) nextBotsToFishes = (Bot *) malloc(sizeof(Bot) * 3);
  
  for(int i = 0; i < 3; i++){
    nextBotsToFishes[i] = searchNextBot(nBots, bots, fishes[i].pos, myID);
  }

  return nextBotsToFishes;
}

Position mapFocus(Position size, int fs[size.w][size.h], Bot *myBot, Bot *bots, int nBots){
  int fromFishToBot[3], indices[3] = {-1, -1, -1}, count = 0, i, j;
  Position focus;
  focus.h = -1;
  focus.w = -1;
  NextObject *nextFishes = searchNextFishes(size, fs, myBot); 
  Bot *nextBots = searchNextBotsToFishes(nBots, bots, nextFishes, myBot->id);

  for(i = 0; i < 3; i++){
    fromFishToBot[i] = distance(nextBots[i].pos, nextFishes[i].pos);
    if(strcmp(myBot->id, nextBots[i].id) == 0){ indices[i] = i; count++;}
  }

  if (count == 1){
    for(i = 0; i < 3; i++) if(indices[i] == i) focus = nextBots[i].pos;
  }
  else if (count == 2){
    for(i = 0; i < 3; i++){
        for (j = 0; j < 3; j++){
          if(i != j)
            if(strcmp(nextBots[i].id, nextBots[j].id) == 0){
              if(fromFishToBot[j] <= fromFishToBot[i]) focus = nextBots[j].pos;
              else focus = nextBots[i].pos;
            }
          }
        if(strcmp(nextBots[i].id, nextBots[j].id) == 0) break;
      }
    } 
  else if(count == 3){
    for(i = 0; i < 2; i++){
      if(fromFishToBot[i] < fromFishToBot[i+1])
        focus = nextFishes[i].pos;
      else
        focus =  nextFishes[i+i].pos;
    }
  }
  return focus;
}

int hasBot(Bot *bots, int h, int w, int nBots){
  if(h == -1 || w == -1){return 1;}

  for(int i = 0; i < nBots; i++)
    if(bots[i].pos.h == h && bots[i].pos.w == w) return 1;
  return 0;
}

int goUP(Bot *bots, Bot *myBot, int nBots){
  if(!hasBot(bots, myBot->pos.h, myBot->pos.w - 1, nBots)) return 1;
  else return 0;
}
int goRIGHT(Bot *bots, Bot *myBot, int nBots){
  if(!hasBot(bots, myBot->pos.h + 1, myBot->pos.w, nBots)) return 1;
  else return 0;
}
int goDOWN(Bot *bots, Bot *myBot, int nBots){
  if(!hasBot(bots, myBot->pos.h, myBot->pos.w + 1, nBots)) return 1;
  else return 0;
}
int goLEFT(Bot *bots, Bot *myBot, int nBots){
   if(!hasBot(bots, myBot->pos.h - 1, myBot->pos.w, nBots)) return 1;
  else return 0;
}

int maxExtLimit(Position size, Position max, Bot *bots, Bot *myBot, int nBots){
  int focus = -1;
  if(myBot->pos.w < max.w){
        if(goDOWN(bots, myBot, nBots)){ 
          MOVE(DOWN);
        } else if(myBot->pos.h <= max.h && myBot->pos.h < size.h-1){
            MOVE(RIGHT);

        } else if(myBot->pos.h > max.h){
          MOVE(LEFT);
        }
      } 
      else if(myBot->pos.w > max.w){
        if(goUP(bots, myBot, nBots)){
          MOVE(UP);
          } else if(myBot->pos.h <= max.h){ MOVE(RIGHT);} 
          else if(myBot->pos.h > max.h){ MOVE(LEFT); }
      } else{
        if(myBot->pos.h < max.h){
          // if(goRIGHT(bots, myBot, nBots)) 
          MOVE(RIGHT);
        } else if(myBot->pos.h <= size.h-1){
          //(if(goLEFT(bots, myBot, nBots))
          MOVE(LEFT);
        } else{
          focus = -2;
          if(goUP(bots, myBot, nBots)) MOVE(UP);
          else if(myBot->pos.h < size.h-1){
            MOVE(RIGHT);
          }  else MOVE(LEFT);
        }
      }
      return focus;
}
int minExtLimit(Position size, Position min, Bot *bots, Bot *myBot, int nBots){
  int focus = -2;
  if(myBot->pos.w > min.w){
        if(goUP(bots, myBot, nBots)){ 
          MOVE(UP);
        } else if(myBot->pos.h >= min.h && myBot->pos.h > 0){
           MOVE(LEFT);
        } else if(myBot->pos.h < min.h){
            MOVE(RIGHT);
        }
      } 
      else if(myBot->pos.w < min.w){
        if(goDOWN(bots, myBot, nBots)){
          MOVE(DOWN);
          } else if(myBot->pos.h >= min.h  && myBot->pos.h > 0){
             MOVE(LEFT);

        } else if(myBot->pos.h < min.h){ 
          MOVE(RIGHT);
        }
      } else{
        if(myBot->pos.h > min.h){MOVE(LEFT);} 
        else if(myBot->pos.h >= 0){MOVE(RIGHT);} 
        else{
          focus = -1;
          if(goDOWN(bots, myBot, nBots)) MOVE(DOWN);
          else if(myBot->pos.h > 0){MOVE(LEFT);}
            else MOVE(RIGHT);
        }
      }
      return focus;
}
 
int onFocus(Position focus, Position myBot){
  if(focus.h == myBot.h && focus.w == myBot.w) return 1;
  else return 0;
}
int onFocusFishable(Position size, int fs[size.w][size.h], Position myBot){
  int isFishable = 0;
  if(fs[myBot.w][myBot.h] > 30) isFishable = (fs[myBot.w][myBot.h] % 30)-1;
  else if(fs[myBot.w][myBot.h] > 20)  isFishable = (fs[myBot.w][myBot.h] % 20)-1;
  else  isFishable = (fs[myBot.w][myBot.h] % 10)-1;

  return isFishable;
}

void moveToFocus(Position size, Position focus, Bot  *myBot, Bot *bots, int nBots, int fs[size.w][size.h]){
  //Pode estar à direita(ou cima ou  embaixo), à esquerda(ou cima ou embaixo) ou à na mesma linha(ou cima ou embaixo)
  //UP_RIGHT

  if(myBot->pos.w > focus.w && myBot->pos.h < focus.h){
    if(goUP(bots, myBot, nBots))MOVE(UP);
    else if(goRIGHT(bots, myBot, nBots))MOVE(RIGHT);
    else if(myBot->pos.w > 0) MOVE(UP);
    else if(myBot->pos.h < (size.h-1)) MOVE(RIGHT);
    else if(fs[myBot->pos.w][myBot->pos.h] == 0 || fs[myBot->pos.w][myBot->pos.h] == 1)printf("FISH\n");
  }
  //UP_LEFT
  if(myBot->pos.w > focus.w && myBot->pos.h > focus.h){
    if(goUP(bots, myBot, nBots))MOVE(UP);
    else if(goLEFT(bots, myBot, nBots))MOVE(LEFT);
    else if(myBot->pos.w > 0) MOVE(UP);
    else if(myBot->pos.h > 0) MOVE(LEFT);
    else if(fs[myBot->pos.w][myBot->pos.h] == 0 || fs[myBot->pos.w][myBot->pos.h] == 1)printf("FISH\n");
  }
  //DOWN_RIGHT
  if(myBot->pos.w < focus.w && myBot->pos.h < focus.h){
    if(goDOWN(bots, myBot, nBots))MOVE(DOWN);
    else if(goRIGHT(bots, myBot, nBots))MOVE(RIGHT);
    else if(myBot->pos.w < size.w-1) MOVE(DOWN);
    else if(myBot->pos.h < size.h-1) MOVE(RIGHT);
    else if(fs[myBot->pos.w][myBot->pos.h] == 0 || fs[myBot->pos.w][myBot->pos.h] == 1)printf("FISH\n");
  }
  
  //DOWN_LEFT
  if(myBot->pos.w < focus.w && myBot->pos.h > focus.h){
    if(goDOWN(bots, myBot, nBots))MOVE(DOWN);
    else if(goLEFT(bots, myBot, nBots))MOVE(LEFT);
    else if(myBot->pos.w < size.w-1) MOVE(DOWN);
    else if(myBot->pos.h > 0) MOVE(LEFT);
    else if(fs[myBot->pos.w][myBot->pos.h] == 0 || fs[myBot->pos.w][myBot->pos.h] == 1)printf("FISH\n");
  }

  //UP
  if(myBot->pos.w > focus.w && myBot->pos.h == focus.h){
    if(myBot->pos.w > 0) MOVE(UP);
    else printf("INVALID\n");
  }
  //RIGHT
   if(myBot->pos.w == focus.w && myBot->pos.h < focus.h){
    if(myBot->pos.h < (size.h-1)) MOVE(RIGHT);
    else printf("INVALID\n");
  }
  //DOWN
  if(myBot->pos.w < focus.w && myBot->pos.h == focus.h){
    if(myBot->pos.h < size.w-1) MOVE(DOWN);
    else printf("INVALID\n");
  }
  //LEFT
  if(myBot->pos.w == focus.w && myBot->pos.h > focus.h){
    if(myBot->pos.h > 0) MOVE(LEFT);
    else printf("INVALID\n");
  }

}

Position nextHarbor(Position size, int fs[size.w][size.h], Position myBot){
  Position nextHarbor; nextHarbor.w = -1; nextHarbor.h = -1;
  Position array;
  for(int i = 0; i < size.w; i++){
    for(int j = 0; j < size.h; j++){
      array.w = i; array.h = j;
      if(fs[i][j] == 1){
        if(nextHarbor.h == -1){
          nextHarbor.w = array.w;
          nextHarbor.h = array.h;
        }else{
          if(distance(myBot, array) < distance(myBot, nextHarbor)){
            nextHarbor.w = array.w;
            nextHarbor.h = array.h;
          }
        }
      }
    }
  }
  return nextHarbor;
}

int main() {
  char line[MAX_STR]; 
  char fishes_str[][MAX_STR] =  {{"MULLET\0"}, {"SNAPPER\0"}, {"SEABASS\0"}};
  int *totalBots = NULL, fishes = 10, count = 0; 
  Bot *bots = NULL;
  Bot *myBot = NULL;

  setbuf(stdin, NULL);   // stdin, stdout e stderr não terão buffers
  setbuf(stdout, NULL);  // assim, nada é "guardado temporariamente"
  setbuf(stderr, NULL);

  // === INÍCIO DA PARTIDA ===
  Position size, focus, min, max, harbor;
  focus.w = -1; focus.h = -1;
  harbor.w = -1; harbor.h = -1;
  scanf("AREA %i %i", &size.w, &size.h);  // lê a dimensão da área de pesca: altura (h) x largura (w)
  int fishingArea[size.w][size.h]; 

  min.h = (size.h-1)/4;     min.w = (size.w-1)/4;
  max.h = (size.h-1)*(3/4); max.w = (size.w-1)*(3/4);
  //Tainha, Cioba, Robalo

  //Referência dos Ponteiros
  while(totalBots == NULL) totalBots = (int*) malloc(sizeof(int));
  while(myBot == NULL) myBot = (Bot *) malloc(sizeof(Bot));
  while(myBot->id == NULL) myBot->id = (char *) malloc(sizeof(char) * MAX_STR);

  scanf(" ID %s", myBot->id);
  // === PARTIDA === 
  while (1) {
    // LÊ OS DADOS DO JOGO E ATUALIZA OS DADOS DO BOT
    readData(size, fishingArea, totalBots); //Leitura dos dados
    bots = (Bot*) malloc(sizeof(Bot)* (*totalBots));
    defineOtherBots(bots, *totalBots);
    defMyBot(myBot, bots, *totalBots);
    
    if (fishes < 10){
      if((focus.h == -1 || focus.h == -2) && (count == 0 || !(onFocus(focus, myBot->pos))))
        focus = mapFocus(size, fishingArea, myBot, bots, *totalBots);

    if(focus.h == -1){
      focus.h = maxExtLimit(size, max, bots, myBot, *totalBots);
    } else if(focus.h == -2){
      focus.h = minExtLimit(size, max, bots, myBot, *totalBots);
    } else {
      if(onFocus(focus, myBot->pos)){
        if(onFocusFishable(size, fishingArea, myBot->pos)){
          printf("FISH\n");
        } else{
          focus = mapFocus(size, fishingArea, myBot, bots, *totalBots);
          if(focus.h == -1)
            focus.h = maxExtLimit(size, max, bots, myBot, *totalBots);
          else{
            moveToFocus(size, focus, myBot, bots, *totalBots, fishingArea);
          }
        }
      } else{
        moveToFocus(size, focus, myBot, bots, *totalBots, fishingArea);
      }
    }
    scanf("%s", line);
    if(strcmp(line, fishes_str[TAINHA]) == 0 || strcmp(line, fishes_str[CIOBA]) == 0 || strcmp(line, fishes_str[ROBALO]) == 0){
      fishes++;
    } 
  } else {
    if(harbor.h == -1) harbor = nextHarbor(size, fishingArea, myBot->pos);
    if(!(onFocus(harbor, myBot->pos))){
      moveToFocus(size, harbor, myBot, bots, *totalBots, fishingArea);
      scanf("%s", line);
    } else{
      printf("SELL\n");
      scanf("%s", line);
      harbor.h = -1; harbor.w = -1;
      fishes = 0;
    }
  }
    count++;
  }

  //Liberando Memória Utilizada
  for(int i = 0; i < *totalBots; i++) 
    free((bots+i));
  free(bots); bots = NULL;
  free(totalBots); totalBots = NULL;

  return 0;
}