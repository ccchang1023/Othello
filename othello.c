#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <stdbool.h>
#include <string.h>
#define BLACK 1
#define WHITE 2

int i,j,chessBoard[10][10]={0};
int weighting_table[8][8]={90,-60,10,10,10,10,-60,90,
                             -60,-80,5,5,5,5,-80,-60,
                             10,5,1,1,1,1,5,10,
                             10,5,1,1,1,1,5,10,
                             10,5,1,1,1,1,5,10,
                             10,5,1,1,1,1,5,10,
                             -60,-80,5,5,5,5,-80,-60,
                             90,-60,10,10,10,10,-60,90};

typedef struct{
    int x,y;
}POSITION;

void showChessBoard();
bool playChess(POSITION p,int cur_round);
int getLength(POSITION *p);
int alphabeta(POSITION node,int depth,int alpha,int beta,bool maximizingPlayer);
int settlement(int color);
void refresh(POSITION p,int cur_round);
POSITION* getSuccessor(int cur_round);
POSITION getNextStep(POSITION *p_set,int depth,int cur_round);

int main()
{
    int input_x,input_y,p_length,value,depth,cur_round=BLACK;
    bool gameOver,black_skip,white_skip;
    char auto_switch='n',garbage;
    gameOver=black_skip=white_skip=false;
    chessBoard[3][4]=chessBoard[4][3]=1;    //black initial
    chessBoard[3][3]=chessBoard[4][4]=2;    //white initial
    //chessBoard[3][2]=chessBoard[2][3]=chessBoard[5][4]=2;    //white initial
    POSITION *p_set,nextStep;
    printf("Please enter the depth: ");
    scanf("%d%c",&depth,&garbage);
    do{
       system("clear");
       showChessBoard();
        if(auto_switch!='y')
        {
            printf("press any key to move on (enter y will auto complete): ");
	       	scanf("%c%c",&auto_switch,&garbage);
		}
       //printf("got %d\n",evaluation(chessBoardTemp));
       if(cur_round==BLACK)
       {
        printf("last move : WHITE played on %d  %c\n",nextStep.x+1,nextStep.y+65);
        p_set = getSuccessor(BLACK);
        if(getLength(p_set)==0)
        {
            if(white_skip)
                gameOver=true;
            else
                black_skip=true;
            cur_round=WHITE;
            continue;
        }
        black_skip=false;
        nextStep = getNextStep(p_set,depth,BLACK);
        playChess(nextStep,BLACK);
        /*POSITION input;
        scanf("%d %d",&input.x,&input.y);
        input.x--;
        input.y--;
        if(!playChess(input,BLACK))
            continue;*/
       }
       else
       {
        printf("last move : BLACK played on %d  %c\n",nextStep.x+1,nextStep.y+65);
        p_set = getSuccessor(WHITE);
        if(getLength(p_set)==0)
        {
            if(black_skip)
                gameOver=true;
            else
                white_skip=true;
            cur_round=BLACK;
            continue;
        }
        white_skip=false;
        nextStep = getNextStep(p_set,depth,WHITE);
        playChess(nextStep,WHITE);
       }
       cur_round=(cur_round==BLACK)?WHITE:BLACK;
    }while(!gameOver);
    int black_num=settlement(BLACK),white_num=settlement(WHITE);
    printf("Depth : %d\n",depth);
    printf("BLACK:%d  WHITE:%d\n",black_num,white_num);
    if(black_num>white_num)
        printf("BLACK WINS!\n");
    else if(black_num==white_num)
        printf("TIE!\n");
    else
        printf("WHITE WINS!\n");
    return 0;
}



int alphabeta(POSITION node,int depth,int alpha,int beta,bool maximizingPlayer){
    //make_move
    int chessBoardTemp[10][10];
    memcpy(chessBoardTemp,chessBoard,sizeof(chessBoard)); // save the game before every DFS
    int cur_round = (maximizingPlayer)?1:2;
    playChess(node,cur_round);

    POSITION *successor;
    successor = (maximizingPlayer)?getSuccessor(WHITE):getSuccessor(BLACK);
    int i,t,m,s_len = getLength(successor);
    int weight = weighting_table[node.x][node.y];
    if(depth==0||s_len==0)
    {
        free(successor);
        return (maximizingPlayer)?-10*s_len-weight:10*s_len+weight;//the heuristic value of node
        //return (maximizingPlayer)?settlement(WHITE):settlement(BLACK);//the heuristic value of node
    }
    if(maximizingPlayer)
    {
        m = alpha;
        for(i=0;i<s_len;i++)
        {
            t = alphabeta(successor[i],depth-1,m,beta,false);
            m = (alpha>t)?alpha:t;
            //m=max(alpha,alphabeta(successor[i],depth-1,alpha,beta,false));
            if(m>=beta)
                break;
        }
    }
    else
    {
        m = beta;
        for(i=0;i<s_len;i++)
        {
            t = alphabeta(successor[i],depth-1,alpha,m,true);
            m = (beta<t)?beta:t;
            //m=min(beta,alphabeta(successor[i],depth-1,alpha,beta,true));
            if(m<=alpha)
                break;
        }
    }
    //clear move
    memcpy(chessBoard,chessBoardTemp,sizeof(chessBoardTemp));
    free(successor);
    return m;
}

void showChessBoard(){
    for(i=0;i<8;i++)
        printf("    %c",i+65);
    for(i=0;i<8;i++)
    {
        printf("\n\n");
        printf("%d   ",i+1);
        for(j=0;j<8;j++)
        {
            if(chessBoard[i][j]==0)
                printf("-    ");
            else if(chessBoard[i][j]==1)
                printf("x    ");
            else if(chessBoard[i][j]==2)
                printf("O    ");
            else
             printf("!    ");
       }
    }
    putchar('\n');
}

bool playChess(POSITION p,int cur_round){
    if(chessBoard[p.x][p.y]!=0)
    {
        printf("Cant't play on %d %d,try again\n",p.x+1,p.y+1);
        return false;
    }
    chessBoard[p.x][p.y]=cur_round;
    refresh(p,cur_round);
    return true;
}

int settlement(int color){
    int i,j,num=0;
    for(i=0;i<8;i++)
        for(j=0;j<8;j++)
        if(chessBoard[i][j]==color)
            num++;
    return num;
}

void refresh(POSITION p,int cur_round){

    POSITION *flip_set;
    int i,x,y,walker,num;
    int target = (cur_round==1)?2:1;
    chessBoard[p.x][p.y] = cur_round;

    flip_set = (POSITION *)malloc(sizeof(POSITION)*10);
    num = 0;
    for(y=p.y+1;y<8;y++) //east searching
    {
        walker = chessBoard[p.x][y];
        if(walker==0)
            break;
        else if(walker==target)
        {
            flip_set[num].x = p.x;
            flip_set[num++].y = y;
        }
        else if(walker==cur_round)
        {
            for(i=0;i<num;i++)
                chessBoard[flip_set[i].x][flip_set[i].y] = cur_round;
            break;
        }
    }
    num = 0;
    for(x=p.x+1;x<8;x++) //south searching
    {
        walker = chessBoard[x][p.y];
        if(walker==0)
            break;
        else if(walker==target)
        {
            flip_set[num].x = x;
            flip_set[num++].y = p.y;
        }
        else if(walker==cur_round)
        {
            for(i=0;i<num;i++)
                chessBoard[flip_set[i].x][flip_set[i].y] = cur_round;
            break;
        }
    }

    num = 0;
    for(y=p.y-1;y>=0;y--) //west searching
    {
        walker = chessBoard[p.x][y];
        if(walker==0)
            break;
        else if(walker==target)
        {
            flip_set[num].x = p.x;
            flip_set[num++].y = y;
        }
        else if(walker==cur_round)
        {
            for(i=0;i<num;i++)
                chessBoard[flip_set[i].x][flip_set[i].y] = cur_round;
            break;
        }
    }
    num = 0;
    for(x=p.x-1;x>=0;x--) //north searching
    {
        walker = chessBoard[x][p.y];
        if(walker==0)
            break;
        else if(walker==target)
        {
            flip_set[num].x = x;
            flip_set[num++].y = p.y;
        }
        else if(walker==cur_round)
        {
            for(i=0;i<num;i++)
                chessBoard[flip_set[i].x][flip_set[i].y] = cur_round;
            break;
        }
    }

    num = 0;
    for(x=p.x+1,y=p.y+1;x<8&&y<8;x++,y++) //south east searching
    {
        walker = chessBoard[x][y];
        if(walker==0)
            break;
        else if(walker==target)
        {
            flip_set[num].x = x;
            flip_set[num++].y = y;
        }
        else if(walker==cur_round)
        {
            for(i=0;i<num;i++)
                chessBoard[flip_set[i].x][flip_set[i].y] = cur_round;
            break;
        }
    }

    num = 0;
    for(x=p.x+1,y=p.y-1;x<8&&y>=0;x++,y--) //south west searching
    {
        walker = chessBoard[x][y];
        if(walker==0)
            break;
        else if(walker==target)
        {
            flip_set[num].x = x;
            flip_set[num++].y = y;
        }
        else if(walker==cur_round)
        {
            for(i=0;i<num;i++)
                chessBoard[flip_set[i].x][flip_set[i].y] = cur_round;
            break;
        }
    }

    num = 0;
    for(x=p.x-1,y=p.y-1;x>=0&&y>=0;x--,y--) //north west searching
    {
        walker = chessBoard[x][y];
        if(walker==0)
            break;
        else if(walker==target)
        {
            flip_set[num].x = x;
            flip_set[num++].y = y;
        }
        else if(walker==cur_round)
        {
            for(i=0;i<num;i++)
                chessBoard[flip_set[i].x][flip_set[i].y] = cur_round;
            break;
        }
    }

    num = 0;
    for(x=p.x-1,y=p.y+1;x>=0&&y<8;x--,y++) //north east searching
    {
        walker = chessBoard[x][y];
        if(walker==0)
            break;
        else if(walker==target)
        {
            flip_set[num].x = x;
            flip_set[num++].y = y;
        }
        else if(walker==cur_round)
        {
            for(i=0;i<num;i++)
                chessBoard[flip_set[i].x][flip_set[i].y] = cur_round;
            break;
        }
    }
    free(flip_set);
    return ;
}

POSITION* getSuccessor(int cur_round){
    POSITION *p_set = (POSITION *)malloc(sizeof(POSITION)*60);
    int i,j,x,y,walker,num_of_elements = 0;
    int target = (cur_round==1)?2:1;
    bool keep_search,target_detect;

    for(i=0;i<8;i++)
    for(j=0;j<8;j++)
    {
        keep_search = (chessBoard[i][j]==0)?true:false;
        target_detect = false;
        if(!keep_search)
            continue;
        for(y=j+1;y<8;y++) //east searching
        {
            walker = chessBoard[i][y];
            if(walker==0)
                break;
            else if(walker==target)
                target_detect = true;
            else if(walker==cur_round&&target_detect)
            {
                p_set[num_of_elements].x = i;
                p_set[num_of_elements++].y = j;
                keep_search = false;
                break;
            }
            else
                break;
        }

        if(!keep_search)
            continue;
        target_detect = false;
        for(x=i+1;x<8;x++) //south searching
        {
            walker = chessBoard[x][j];
            if(walker==0)
                break;
            else if(walker==target)
                target_detect = true;
            else if(walker==cur_round&&target_detect)
            {
                p_set[num_of_elements].x = i;
                p_set[num_of_elements++].y = j;
                keep_search = false;
                break;
            }
            else
                break;
        }

        if(!keep_search)
            continue;
        target_detect = false;
        for(y=j-1;y>=0;y--) //west searching
        {
            walker = chessBoard[i][y];
            if(walker==0)
                break;
            else if(walker==target)
                target_detect = true;
            else if(walker==cur_round&&target_detect)
            {
                p_set[num_of_elements].x = i;
                p_set[num_of_elements++].y = j;
                keep_search = false;
                break;
            }
            else
                break;
        }

        if(!keep_search)
            continue;
        target_detect = false;
        for(x=i-1;x>=0;x--) //north searching
        {
            walker = chessBoard[x][j];
            if(walker==0)
                break;
            else if(walker==target)
                target_detect = true;
            else if(walker==cur_round&&target_detect)
            {
                p_set[num_of_elements].x = i;
                p_set[num_of_elements++].y = j;
                keep_search = false;
                break;
            }
            else
                break;
        }

        if(!keep_search)
            continue;
        target_detect = false;
        for(x=i+1,y=j+1;x<8&&y<8;x++,y++) //south east searching
        {
            walker = chessBoard[x][y];
            if(walker==0)
                break;
            else if(walker==target)
                target_detect = true;
            else if(walker==cur_round&&target_detect)
            {
                p_set[num_of_elements].x = i;
                p_set[num_of_elements++].y = j;
                keep_search = false;
                break;
            }
            else
                break;
        }

        if(!keep_search)
            continue;
        target_detect = false;
        for(x=i+1,y=j-1;x<8&&y>=0;x++,y--) //south west searching
        {
            walker = chessBoard[x][y];
            if(walker==0)
                break;
            else if(walker==target)
                target_detect = true;
            else if(walker==cur_round&&target_detect)
            {
                p_set[num_of_elements].x = i;
                p_set[num_of_elements++].y = j;
                keep_search = false;
                break;
            }
            else
                break;
        }

        if(!keep_search)
            continue;
        target_detect = false;
        for(x=i-1,y=j-1;x>=0&&y>=0;x--,y--) //north west searching
        {
            walker = chessBoard[x][y];
            if(walker==0)
                break;
            else if(walker==target)
                target_detect = true;
            else if(walker==cur_round&&target_detect)
            {
                p_set[num_of_elements].x = i;
                p_set[num_of_elements++].y = j;
                keep_search = false;
                break;
            }
            else
                break;
        }

        if(!keep_search)
            continue;
        target_detect = false;
        for(x=i-1,y=j+1;x>=0&&y<8;x--,y++) //north east searching
        {
            walker = chessBoard[x][y];
            if(walker==0)
                break;
            else if(walker==target)
                target_detect = true;
            else if(walker==cur_round&&target_detect)
            {
                p_set[num_of_elements].x = i;
                p_set[num_of_elements++].y = j;
                keep_search = false;
                break;
            }
            else
                break;
        }
    }

    p_set[num_of_elements].x = -1;  // x=-1 & y=-1 means the end of the array , (for length computing)
    p_set[num_of_elements].y = -1;
    return p_set;
}

int getLength(POSITION *p)
{
    int i,length = 0;
    for(i=0;p[i].x!=-1||p[i].y!=-1;i++)
        length++;
    return length;
}

POSITION getNextStep(POSITION *p_set,int depth,int cur_round){
    int i,judge,value,p_length = getLength(p_set);
    POSITION best_p;
    if(cur_round==BLACK)
    {
        judge=INT_MIN;
        for(i=0;i<p_length;i++)
        {
            value = alphabeta(p_set[i],depth,INT_MIN,INT_MAX,true);
            if(value>judge)
            {
             judge = value;
             best_p = p_set[i];
            }
            //printf("value of %d %d is %d\n",p_set[i].x+1,p_set[i].y+1,value);
        }
    }
    else
    {
        judge=INT_MAX;
        for(i=0;i<p_length;i++)
        {
            value = alphabeta(p_set[i],depth,INT_MIN,INT_MAX,false);
            if(value<judge)
            {
             judge = value;
             best_p = p_set[i];
            }
            //printf("value of %d %d is %d\n",p_set[i].x+1,p_set[i].y+1,value);
        }
    }
    printf("play on %d %d\n",best_p.x+1,best_p.y+1);
    return best_p;
}
