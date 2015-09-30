
#include <iostream>
#include <string.h>

using namespace std;

#define REP(i,n) for (int i=0;i<(n);i++)
#define AREP(i,a) for (int i=0;i<sizeof(a)/sizeof(a[0]);i++)

const int X = 35;
const int Y = 20;
const int SQUID_NUM_MAX = 4;
const int TURN_MAX = 350;
const int ME = 0;

namespace Cell {
    const char NEURAL = '.';
    const char ME = '0';
}

struct Pos {
    int x = 0;
    int y = 0;
    Pos () {}
    Pos (int _x, int _y) {
        x = _x; y = _y;
    }
    
    bool operator==(const Pos &other) const {
        return x == other.x && y == other.y;
    }
};

class Squid {
public:
    Pos pos;
    int remainingTime = 0;
    Squid () {}
    
    void update(int _x, int _y, int _remainingTime) {
        pos = Pos(_x, _y);
        remainingTime = _remainingTime;
    }
};

///////////////////////// base //////////////////////////////

namespace Utils {
    void dumpBoard(char board[Y][X]) { //TODO:
        REP(y, Y) {
            REP(x, X) {
                cout << board[y][x];
            } cout << endl;
        }
    }
}

namespace Controller {
    void printNext(Pos &next) {
        cout << next.x << " " << next.y << endl;
    }
}

////////////////////// controller ///////////////////////////

class Battle {
public:
    int enemyNum = 0;
    char board[Y][X];
    bool visited[Y][X];
    Squid squids[SQUID_NUM_MAX];
    const Pos unitVectors[4] = {
        Pos(1, 0),
        Pos(-1, 0),
        Pos(0, 1),
        Pos(0, -1),
    };
    
    void updateSquid(int index, int x, int y, int remainingTime) {
        squids[index].update(x, y, remainingTime);
    }
    
//    void dump
    
    void turn() {
        Pos myPos = squids[ME].pos;
        visited[myPos.y][myPos.x] = true;
        
        Pos next = Pos(0, 0);
        AREP(i, unitVectors) {
            int x = myPos.x + unitVectors[i].x;
            int y = myPos.y + unitVectors[i].y;
            if (x < 0 || x >= X || y < 0 || y >= Y) continue;
            if (board[y][x] == Cell::NEURAL) {
                next = Pos(x, y);
                break;
            } else if (!visited[y][x]) {
                next = Pos(x, y);
            }
        }
        Controller::printNext(next);
    }
};

////////////////////// mainloop ///////////////////////////

int main(int argc, const char * argv[]) {
    int enemyNum = 0;
    cin >> enemyNum; cin.ignore();
    Battle battle = Battle();
    battle.enemyNum = enemyNum;
    
    while (1) {
        int gameRound;
        cin >> gameRound; cin.ignore();
        int x, y, remainingTime;
        cin >> x >> y >> remainingTime; cin.ignore();
        battle.updateSquid(ME, x, y, remainingTime);
        
        REP(i, enemyNum) {
            int x, y, remainingTime;
            cin >> x >> y >> remainingTime; cin.ignore();
            battle.updateSquid(i + 1, x, y, remainingTime);
        }
        
        REP(row, Y) {
            string line;
            cin >> line; cin.ignore();
            memcpy(&battle.board[row][0], line.c_str(), X);
        }
        battle.turn();
    }

    return 0;
}



































