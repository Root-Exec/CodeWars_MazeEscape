#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#define XLOWBOUNDS (currentX - 1 >= 0)
#define XHIGHBOUNDS (currentX + 1 < maze->height)
#define YLOWBOUNDS (currentY - 1 >= 0)
#define YHIGHBOUNDS (currentY + 1 < maze->width)

typedef struct s_Maze
{
    unsigned    height, width;
    char**      grid;
}   Maze;

/*
** @param maze a valid Maze* (see description) with consistent height, width and grid.
** Note: maze->grid[0][0] is the top left-hand corner and maze->grid[maze->height - 1][maze->width - 1] is the bottom right-hand corner.
** @return a heap-allocated string composed of the instructions 'F', 'R', 'B' and 'L'. NULL if no exit is reachable.
** Note: It is not strictly necessary to null terminate that array of char, but  I recommend doing so,
** both for good practice and to avoid a segfault in my validator if your solution doesn't reach the exit.
*/

struct node {
    char type;
    int x;
    int y;
    int distTravelled;
    double h_factor;
    char explored;
    struct node* forward;
    struct node* next;   
};

typedef struct node Node;

struct exit {
    long x;
    long y;
};
typedef struct exit Exit;


Node* createNode(int x, int y, char t) {
    Node* newNode = calloc(1, sizeof(Node));
    newNode->type = t;
    newNode->x = x;
    newNode->y = y;
    newNode->h_factor = 0;
    newNode->distTravelled = 0;
    newNode->explored = 0;
    newNode->next = NULL;
    newNode->forward = NULL;
    return newNode;
}

int sortFunc(const void* a, const void* b) {
    Node* aa = (Node*)a;
    Node* bb = (Node*)b;
  
    return (bb->h_factor - aa->h_factor);
}

char* escape(const Maze *maze) {
 
    char* path = calloc(maze->height * maze->width, sizeof(char));
    char* p_ptr = path;
    long numNodes = maze->height * maze->width;
    long nodesStack = 0;
    Node** nodesToReview = calloc(numNodes, sizeof(Node*));
    Node** map = malloc(maze->height * sizeof(Node*));
    Node* newNode;
    Node* traceRoute;
    long startX = 0;
    long startY = 0;
    long currentX, currentY;
    char validPath = 0;
    long closestDist = numNodes;
    Node* neighbor = NULL;
   
    Exit* exits = calloc(100, sizeof(Exit));
    Exit* exit;
    Exit nearestExit;
    int exits_p = 0;
    
  
    //set up map with nodes and prep for meta data. Finds an exit
    for (unsigned i = 0; i < maze->height; i++) {
        map[i] = malloc(maze->width * sizeof(Node));
        for (unsigned j = 0; j < maze->width; j++) {
            newNode = createNode(i, j, maze->grid[i][j]);
            memcpy(&map[i][j], newNode, sizeof(Node));
            if (map[i][j].type != ' ' && map[i][j].type != '#') {
                startX = i;
                startY = j;
            }
            
            //check for potential exit. If it is, add it to list of exits. Will later find the closest
            if ( (i == 0 && map[i][j].type == ' ') || (i == maze->height - 1 && map[i][j].type == ' ') || (j == 0 && map[i][j].type == ' ') || (j == maze->width - 1 && map[i][j].type == ' ') ) {
                  exit = calloc(1, sizeof(Exit));
                  exit->x = i;
                  exit->y = j;
                  memcpy(&exits[exits_p++], exit, sizeof(Exit));
            }      
        }
    }
    
    long compareDist;
    //find closest exit
    for (int i = 0; i < exits_p; i++) {
        compareDist = labs(exits[i].x - startX) + labs(exits[i].y - startY);
        if (compareDist < closestDist) {
            closestDist = compareDist;
            memcpy(&nearestExit, &exits[i], sizeof(Exit));
        }
    }  
    
    map[startX][startY].explored = 1;
    map[startX][startY].distTravelled = 0;
    nodesToReview[nodesStack] = &map[startX][startY];
    
    //main pathfinding loop
    while( nodesStack >= 0) {
       
        //current Node is the one with the shortest distance travelled (breadth first search)
        currentX = nodesToReview[nodesStack]->x;
        currentY = nodesToReview[nodesStack]->y;
        
        //search each neighbor node. If valid space and not visited then add it, if valid space and visited with a longer distance than current + 1, change pointer to current Node and update distance
        //if neighbor node is an map edge, update next pointer and break out of loop... exit found
        
        //north neighbor
        neighbor = &map[currentX - 1][currentY]; 
        
        if (XLOWBOUNDS && neighbor->type == ' ' && neighbor->explored == 0) {
            
             if (neighbor->x == 0 || neighbor->y == 0 || neighbor->x == maze->height - 1 || neighbor->y == maze->width - 1) {
                neighbor->next = &map[currentX][currentY];
                neighbor->distTravelled = map[currentX][currentY].distTravelled + 1;
                validPath = 1;
                break;
             }
         
             neighbor->explored = 1;
             neighbor->distTravelled = map[currentX][currentY].distTravelled + 1;
             neighbor->next = &map[currentX][currentY];
             neighbor->h_factor = neighbor->distTravelled + (labs(neighbor->x - nearestExit.x) + labs(neighbor->y - nearestExit.y));
             nodesToReview[nodesStack++] = neighbor;
        
        } else if (XLOWBOUNDS && neighbor->type == ' ' && neighbor->explored == 1) {
             if (map[currentX][currentY].distTravelled + 1 < neighbor->distTravelled) {
                neighbor->distTravelled = map[currentX][currentY].distTravelled + 1;
                neighbor->h_factor = neighbor->distTravelled + (labs(neighbor->x - nearestExit.x) + labs(neighbor->y - nearestExit.y));
                neighbor->next = &map[currentX][currentY];
             }
        }
        
        
        //south neighbor
        neighbor = &map[currentX + 1][currentY];
        if (XHIGHBOUNDS && neighbor->type == ' ' && neighbor->explored == 0) {
             if (neighbor->x == 0 || neighbor->y == 0 || neighbor->x == maze->height - 1 || neighbor->y == maze->width - 1) {
                neighbor->next = &map[currentX][currentY];
                neighbor->distTravelled = map[currentX][currentY].distTravelled + 1;
                validPath = 1;
                break;
             }
            
             neighbor->explored = 1;
             neighbor->distTravelled = map[currentX][currentY].distTravelled + 1;
             neighbor->next = &map[currentX][currentY];
             neighbor->h_factor = neighbor->distTravelled + (labs(neighbor->x - nearestExit.x) + labs(neighbor->y - nearestExit.y));
             nodesToReview[nodesStack++] = neighbor;
          
        } else if (XHIGHBOUNDS && neighbor->type == ' ' && neighbor->explored == 1) {
             if (map[currentX][currentY].distTravelled + 1 < neighbor->distTravelled) {
                neighbor->distTravelled = map[currentX][currentY].distTravelled + 1;
                neighbor->h_factor = neighbor->distTravelled + (labs(neighbor->x - nearestExit.x) + labs(neighbor->y - nearestExit.y));
                neighbor->next = &map[currentX][currentY];
             }
        }
      
        //east neighbor
        neighbor = &map[currentX][currentY + 1];
        if (YHIGHBOUNDS && neighbor->type == ' ' && neighbor->explored == 0) {
             if (neighbor->x == 0 || neighbor->y == 0 || neighbor->x == maze->height - 1 || neighbor->y == maze->width - 1) {
                neighbor->next = &map[currentX][currentY];
                neighbor->distTravelled = map[currentX][currentY].distTravelled + 1;
                validPath = 1;
                break;
             }
             
             neighbor->explored = 1;
             neighbor->distTravelled = map[currentX][currentY].distTravelled + 1;
             neighbor->next = &map[currentX][currentY];
             neighbor->h_factor = neighbor->distTravelled + (labs(neighbor->x - nearestExit.x) + labs(neighbor->y - nearestExit.y));
             nodesToReview[nodesStack++] = neighbor;
          
        } else if (YHIGHBOUNDS && neighbor->type == ' ' && neighbor->explored == 1) {
             if (map[currentX][currentY].distTravelled + 1 < neighbor->distTravelled) {
                neighbor->distTravelled = map[currentX][currentY].distTravelled + 1;
                neighbor->h_factor = neighbor->distTravelled + (labs(neighbor->x - nearestExit.x) + labs(neighbor->y - nearestExit.y));
                neighbor->next = &map[currentX][currentY];
             }
        }      
 
        //west neighbor
        neighbor = &map[currentX][currentY - 1];
        if (YLOWBOUNDS && neighbor->type == ' ' && neighbor->explored == 0) {
             if (neighbor->x == 0 || neighbor->y == 0 || neighbor->x == maze->height - 1 || neighbor->y == maze->width - 1) {
                neighbor->next = &map[currentX][currentY];
                neighbor->distTravelled = map[currentX][currentY].distTravelled + 1;
                validPath = 1;
                break;
             }    
          
             neighbor->explored = 1;
             neighbor->distTravelled = map[currentX][currentY].distTravelled + 1;
             neighbor->next = &map[currentX][currentY];
             neighbor->h_factor = neighbor->distTravelled + (labs(neighbor->x - nearestExit.x) + labs(neighbor->y - nearestExit.y));
             nodesToReview[nodesStack++] = neighbor;
          
        } else if (YLOWBOUNDS && neighbor->type == ' ' && neighbor->explored == 1) {
             if (map[currentX][currentY].distTravelled + 1 < neighbor->distTravelled) {
                neighbor->distTravelled = map[currentX][currentY].distTravelled + 1;
                neighbor->h_factor = neighbor->distTravelled + (labs(neighbor->x - nearestExit.x) + labs(neighbor->y - nearestExit.y));
                neighbor->next = &map[currentX][currentY];
             }
        } 
        
        //decrement stack pointer
        nodesStack--;
      
        //order list based of longest to shortest distance travelled (breadth first search, taking node at top of stack)
        if (nodesStack > 0) qsort(nodesToReview, nodesStack, sizeof(Node*), sortFunc);     
    }
    
    //path trace back to starting location. During backtrace, build path using opposite direction. If I'm moving right, write left to path
    traceRoute = neighbor;
    
    if (validPath == 0) return NULL;
    char dir;
  
    while (traceRoute->next) {
        //figure out direction coming from
        
        if (traceRoute->next->x > traceRoute->x) {
            dir = '^';
        } else if (traceRoute->next->x < traceRoute->x) {
            dir = 'V';
        } else if (traceRoute->next->y > traceRoute->y) {
            dir = '<';
        } else if (traceRoute->next->y < traceRoute->y) {
            dir = '>';
        }
        
        //has to be a move forward command to get to the point
        *p_ptr++ = 'F'; 
        //check if we are starting location
        if (traceRoute->next->next == NULL) {
          
            traceRoute = traceRoute->next;
            switch (dir) {
                  
                  case '^':
                      if (traceRoute->type == '>')        *p_ptr++ = 'L';
                      else if (traceRoute->type == 'v')   *p_ptr++ = 'B';
                      else if (traceRoute->type == '<')   *p_ptr++ = 'R';
                      break;
                
                  case '>':
                      if (traceRoute->type == 'v')        *p_ptr++ = 'L';
                      else if (traceRoute->type == '^')   *p_ptr++ = 'R';
                      else if (traceRoute->type == '<')   *p_ptr++ = 'B';
                      break;                

                  case 'V':
                      if (traceRoute->type == '>')        *p_ptr++ = 'R';
                      else if (traceRoute->type == '<')   *p_ptr++ = 'L';
                      else if (traceRoute->type == '^')   *p_ptr++ = 'B';
                      break;                
                
                  case '<':
                      if (traceRoute->type == '>')        *p_ptr++ = 'B';
                      else if (traceRoute->type == 'v')   *p_ptr++ = 'R';
                      else if (traceRoute->type == '^')   *p_ptr++ = 'L';
                      break;                
            }
            break;
            
        } else {
      
            //go to the next node and look ahead for a turn and add the turn. 

            traceRoute = traceRoute->next;
       
            switch (dir) {
                  case '^':
                      if (traceRoute->next->y > traceRoute->y)      *p_ptr++ = 'R';
                      else if (traceRoute->next->y < traceRoute->y) *p_ptr++ = 'L';
                      break;
                  case '>':
                      if (traceRoute->next->x > traceRoute->x)      *p_ptr++ = 'R';
                      else if (traceRoute->next->x < traceRoute->x) *p_ptr++ = 'L';
                      break;
                  case '<':
                      if (traceRoute->next->x > traceRoute->x)      *p_ptr++ = 'L';
                      else if (traceRoute->next->x < traceRoute->x) *p_ptr++ = 'R';
                      break;
            
                  case 'V':
                      if (traceRoute->next->y > traceRoute->y)      *p_ptr++ = 'L';
                      else if (traceRoute->next->y < traceRoute->y) *p_ptr++ = 'R';
             }
            traceRoute->type = dir;
        }
    }

    char temp;
    for (unsigned i = 0, j = strlen(path) - 1; i < strlen(path) / 2; i++, j--) {
          temp = path[i];
          path[i] = path[j];
          path[j] = temp;
    }
    
    if (validPath) {
        return path;
    } else {
        return NULL;
    }
}