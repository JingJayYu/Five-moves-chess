#include <iostream>
#include <fstream>
#include <cstdlib>

using namespace std;

//#define __DEBUG
#define H 5
#define W 5
#define step_limit 5

struct Position
{
    int x;
    int y;
    int step;
    struct Position* father;
    struct Position* Up;
    struct Position* Down;
    struct Position* Left;
    struct Position* Right;
};

void show(int A[H][W])
{
    int i;
    int j;
    for (i = 0; i < H; i++)
    {
        for (j = 0; j < W; j++) {
            cout << A[i][j] << " ";
        }
        cout << endl;
    }
    cout << endl;
}

void showPosition(struct Position p)
{
    cout << "(" << p.x << "," << p.y << ")";
}
void determinePosition(int A[H][W], int whoami, int whoisother, struct Position self_position[2], struct Position other_position[2], int& self_n, int& other_n)
{
    self_n = 0;
    other_n = 0;
    int i, j;
    for (i = 0; i < H; i++) {
        for (j = 0; j < W; j++)
        {
            if (A[i][j] == whoami)
            {
                self_position[self_n].y = i;
                self_position[self_n].x = j;
                self_n++;
            }
            if (A[i][j] == whoisother)
            {
                other_position[other_n].y = i;
                other_position[other_n].x = j;
                other_n++;
            }
        }
    }
#ifdef __DEBUG
    /*cout << whoami << ":";
    for (i = 0; i < self_n; i++) {
        showPosition(self_position[i]);
    }
    cout << endl;
    cout << whoisother << ":";
    for (i = 0; i < other_n; i++) {
        showPosition(other_position[i]);
    }    cout << endl;*/
#endif
}

void movechess(int A[H][W], struct Position start, struct Position end)
{
    A[end.y][end.x] = A[start.y][start.x];
    A[start.y][start.x] = 0;
}

void savePath(struct Position path[6], int step)
{
    ofstream file;
    file.open("play.txt");
    int i;
    for (i = 0; i <= step; i++) {
        file << path[i].x << " " << path[i].y << " ";
    }
    file.close();
}

struct Position* NewPath(int x, int y, int step, struct Position* father) {
    struct Position* path = new struct Position;
    path->x = x;
    path->y = y;
    path->step = step;
    path->father = father;
    path->Up = path->Down = path->Left = path->Right = NULL;
    return  path;
}

struct Position* searchPath(struct Position* path, int A[H][W], int x, int y, int who, int other, int step = 0, struct Position* father = NULL) {
    int map[H][W];
    for (int i = 0; i < H; i++)
    {
        for (int j = 0; j < W; j++)
        {
            map[i][j] = A[i][j];
        }
    }
    if (step == 0) {
        path = NewPath(x, y, step, father);
        map[y][x] = step + 3;
        if (y > 0) {
            path->Up = searchPath(path->Up, map, x, y - 1, who, other, step + 1, path);
        }
        if (y < H - 1) {
            path->Down = searchPath(path->Down, map, x, y + 1, who, other, step + 1, path);
        }
        if (x > 0) {
            path->Left = searchPath(path->Left, map, x - 1, y, who, other, step + 1, path);
        }
        if (x < W - 1) {
            path->Right = searchPath(path->Right, map, x + 1, y, who, other, step + 1, path);
        }
    }
    else if (step <= step_limit) {
        if (map[y][x] == 0) {
            path = NewPath(x, y, step, father);
            map[y][x] = step + 3;
#ifdef __DEBUG
            //show(map);
#endif    
            if (y > 0) {
                path->Up = searchPath(path->Up, map, x, y - 1, who, other, step + 1, path);
            }
            if (y < H - 1) {
                path->Down = searchPath(path->Down, map, x, y + 1, who, other, step + 1, path);
            }
            if (x > 0) {
                path->Left = searchPath(path->Left, map, x - 1, y, who, other, step + 1, path);
            }
            if (x < W - 1) {
                path->Right = searchPath(path->Right, map, x + 1, y, who, other, step + 1, path);
            }
        }
        else if (map[y][x] == other && step == step_limit) {
            path = NewPath(x, y, step, father);
            map[y][x] = step + 3;
#ifdef __DEBUG
            /*show(map);*/
#endif    
        }
    }
    return path;
}

bool checkInside(struct Position* chess, struct Position Position[], int n) {
    if (chess == NULL) {
        return false;
    }
    for (int i = 0; i < n; i++) {
        if (chess->x == Position[i].x && chess->y == Position[i].y) {
            return true;
        }
    }
    return false;
}

int FindDangerPositions(struct Position* other_path, struct Position DangerPosition[])
{
    static int i = 0;
    if (other_path == NULL) {
        return i;
    }
    if (other_path->step == step_limit) {
        if (!checkInside(other_path, DangerPosition, i)) {
            DangerPosition[i].x = other_path->x;
            DangerPosition[i].y = other_path->y;
            DangerPosition[i].step = other_path->step;
            i++;
        }
    }
    i = FindDangerPositions(other_path->Up, DangerPosition);
    i = FindDangerPositions(other_path->Down, DangerPosition);
    i = FindDangerPositions(other_path->Left, DangerPosition);
    i = FindDangerPositions(other_path->Right, DangerPosition);
    return i;
}

bool checkSurround(struct Position* p) {
    if (p == NULL) {
        return false;
    }
    if (p->Up == NULL && p->Down == NULL && p->Left == NULL && p->Right == NULL) {
        return true;
    }
    return false;
}

int FindSafePosition(struct Position* chess, struct Position SafePosition[], struct Position DangerPosition[], int DangerPosition_n) {
    static int i = 0;
    if (chess == NULL) return i;
    if (chess->step <= step_limit) {
        if (!checkInside(chess, DangerPosition, DangerPosition_n)) {
            if (!checkInside(chess, SafePosition, i)) {
                SafePosition[i].x = chess->x;
                SafePosition[i].y = chess->y;
                SafePosition[i].step = chess->step;
                i++;
            }
        }
    }
    i = FindSafePosition(chess->Up, SafePosition, DangerPosition, DangerPosition_n);
    i = FindSafePosition(chess->Down, SafePosition, DangerPosition, DangerPosition_n);
    i = FindSafePosition(chess->Left, SafePosition, DangerPosition, DangerPosition_n);
    i = FindSafePosition(chess->Right, SafePosition, DangerPosition, DangerPosition_n);
    return i;
}

int GotoTargetPosition(struct Position path[6], struct Position* chess, struct Position TargetPosition) {
    static int step = 0;
    if (chess == NULL) {
        return step;
    }
    else if (chess->step <= step_limit) {
        if (chess->x == TargetPosition.x && chess->y == TargetPosition.y) {
            step = chess->step;
            while (chess != NULL) {
                path[chess->step] = *chess;
                chess = chess->father;
            }
        }
    }
    if (path[step].step == NULL) {
        step = GotoTargetPosition(path, chess->Up, TargetPosition);
        step = GotoTargetPosition(path, chess->Down, TargetPosition);
        step = GotoTargetPosition(path, chess->Left, TargetPosition);
        step = GotoTargetPosition(path, chess->Right, TargetPosition);
    }
    return step;
}

int main(int argc, char** argv) {
    //------------  declare variable -----
    char other[10] = { 0 };
    int whoami = 0;
    int whoisother = 0;
#ifndef __DEBUG
    int A[H][W] = { 0 };
#endif 
#ifdef __DEBUG    
    int A[H][W] = { 
        {1,0,0,0,2},
        {0,0,0,0,0},
        {0,0,0,0,0},
        {0,0,0,0,0},
        {2,0,0,0,1}
    };
#endif
    
    int i, j, k;
    k = 3;
    //---------- read data ----------
    sprintf_s(other, "%s", argv[1]);
#ifdef __DEBUG    
    cout << "other's id:" << other << endl;
#endif
#ifdef __DEBUG    
    whoami = 1;
#endif
#ifndef __DEBUG
    whoami = atoi(argv[2]);
    
#endif
    if (whoami == 1) {
        whoisother = 2;
    }
    else {
        whoisother = 1;
    }

    
#ifdef __DEBUG    
    cout << "self:" << whoami << endl;
#endif
    for (i = 0; i < H; i++)
    {
        for (j = 0; j < W; j++)
        {
            A[i][j] = atoi(argv[k]);
            k++;
        }
    }
#ifdef __DEBUG    
    show(A);
#endif

    //------ determine position ----
    struct Position self_position[2] = { 0 };
    struct Position other_position[2] = { 0 };
    int self_n = 0;
    int other_n = 0;
    determinePosition(A, whoami, whoisother, self_position, other_position, self_n, other_n);

    //-------- search  path ------------------- 
    struct Position* self1_path = new struct Position;
    struct Position* self2_path = new struct Position;
    struct Position* other1_path = new struct Position;
    struct Position* other2_path = new struct Position;
    self1_path = self2_path = other1_path = other2_path = NULL;

    if (self_n == 2) {
        self1_path = searchPath(self1_path, A, self_position[0].x, self_position[0].y, whoami, whoisother);
        self2_path = searchPath(self2_path, A, self_position[1].x, self_position[1].y, whoami, whoisother);
    }
    else {
        self1_path = searchPath(self1_path, A, self_position[0].x, self_position[0].y, whoami, whoisother);
    }

    if (other_n == 2) {
        other1_path = searchPath(other1_path, A, other_position[0].x, other_position[0].y, whoisother, whoami);
        other2_path = searchPath(other2_path, A, other_position[1].x, other_position[1].y, whoisother, whoami);
    }
    else {
        other1_path = searchPath(other1_path, A, other_position[0].x, other_position[0].y, whoisother, whoami);
    }

    //-------- Find ALL DangerPosition ------------------- 
    struct Position DangerPosition[25] = { 0 };
    int DangerPosition_n = 0;
    if (other_n == 2) {
        DangerPosition_n = FindDangerPositions(other1_path, DangerPosition);
        DangerPosition_n = FindDangerPositions(other2_path, DangerPosition);
    }
    else {
        DangerPosition_n = FindDangerPositions(other1_path, DangerPosition);
    }

    //-------- decide which chess should move ------------------- 
    struct Position* chess = new struct Position;
    if (checkInside(self1_path, DangerPosition, DangerPosition_n)) {
        chess = self1_path;
    }
    else if (checkInside(self2_path, DangerPosition, DangerPosition_n)) {
        chess = self2_path;
    }
    else if (checkSurround(self1_path) && self2_path != NULL) {
        chess = self2_path;
    }
    else if (checkSurround(self2_path)) {
        chess = self1_path;
    }
    else {
        chess = self1_path;
    }

    //-------- Find ALL SafePosition ----------------
    struct Position SafePosition[25] = { 0 };
    int SafePosition_n = 0;
    SafePosition_n = FindSafePosition(chess, SafePosition, DangerPosition, DangerPosition_n);

    //-------- decide path ------------------- 
    struct Position path[6] = { 0 };
    int step = 0;
    if (checkInside(other1_path, SafePosition, SafePosition_n)) {
        step = GotoTargetPosition(path, chess, *other1_path);
    }
    else if (checkInside(other2_path, SafePosition, SafePosition_n)) {
        step = GotoTargetPosition(path, chess, *other2_path);
    }
    else {
        for (int i = 0; i < SafePosition_n; i++) {
            if (SafePosition[i].x != chess->x || SafePosition[i].y != chess->y) {
                for (int j = 0; j < other_n; j++) {
                    if (SafePosition[i].x + 1 != other_position[j].x || SafePosition[i].y + 1 != other_position[j].y || SafePosition[i].x - 1 != other_position[j].x || SafePosition[i].y - 1 != other_position[j].y)
                    {
                        step = GotoTargetPosition(path, chess, SafePosition[i]);
                    }
                }
            }
        }
    }
    //----------- movechess -----------------------
    movechess(A, path[0], path[step]);
#ifdef __DEBUG    
    show(A);
#endif
#ifdef __DEBUG    
    for (int i = 0; i <= step; i++) {
        showPosition(path[i]);
    }
#endif
    //------- save path --------
    savePath(path, step);
    return 0;
}