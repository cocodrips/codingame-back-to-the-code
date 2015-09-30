
#include <iostream>
#include <string.h>

using namespace std;

#define REP(i,n) for (int i=0;i<(n);i++)
#define AREP(i,a) for (int i=0;i<sizeof(a)/sizeof(a[0]);i++)

///////////////////////// base //////////////////////////////

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

template <typename T>
struct Board {
private:
    T _board[Y * X] = {};
public:
    T operator[](Pos pos) const {
        return _board[pos.y * X + pos.x];
    }
    
    T &operator[](Pos pos) {
        return _board[pos.y * X + pos.x];
    }
    
    void dump() {
        REP(y, Y) {
            REP(x, X) {
                cerr << _board[y * X + x];
            } cerr << endl;
        }
    }

};

////////////////////// controller ///////////////////////////

namespace Controller {
void printNext(Pos &next) {
    cout << next.x << " " << next.y << endl;
}
}

////////////////////// simulator  ///////////////////////////
namespace simulator {

    
}



////////////////////// ai ///////////////////////////////////

class Battle {
public:
    int enemyNum = 0;
    Board<char> board;
    Board<bool> visited;
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
    
    void turn() {
        Pos myPos = squids[ME].pos;
        visited[myPos] = true;
        
        Pos next = Pos(0, 0);
        AREP(i, unitVectors) {
            int x = myPos.x + unitVectors[i].x;
            int y = myPos.y + unitVectors[i].y;
            Pos p = Pos(x, y);
            if (x < 0 || x >= X || y < 0 || y >= Y) continue;
            if (board[p] == Cell::NEURAL) {
                next = p;
                break;
            } else if (!visited[myPos]) {
                next = p;
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
        
        battle.board.dump();
        REP(i, enemyNum) {
            int x, y, remainingTime;
            cin >> x >> y >> remainingTime; cin.ignore();
            battle.updateSquid(i + 1, x, y, remainingTime);
        }
        
        REP(y, Y) {
            string line;
            cin >> line; cin.ignore();
            REP(x, X) {
                battle.board[Pos(x, y)] = line[x];
            }
        }
        battle.turn();
    }

    return 0;
}



































