
#include <algorithm>
#include <iostream>
#include <stack>
#include <unordered_set>
#include <vector>


using namespace std;

#define REP(i,n) for (int i=0;i<(n);i++)
#define AREP(i,a) for (int i=0;i<sizeof(a)/sizeof(a[0]);i++)

///////////////////////// base //////////////////////////////
namespace game {
const int X = 35;
const int Y = 20;
const int SQUID_NUM_MAX = 4;
const int TURN_MAX = 350;
const int ME = 0;
}

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
    
    Pos operator+(const Pos &other) const {
        return Pos(x + other.x, y + other.y);
    }
    
    bool operator<(const Pos &other) const {
        return (x + y) < (other.x + other.y);
    }
    
public:
    int dist(const Pos &other) const {
        return abs(x - other.x) + abs(y - other.y);
    }
    
    bool isValid() {
        return 0 <= x && x < game::X && 0 <= y && y < game::Y;
    }

};

namespace std
{
    template <>
    struct hash<Pos>
    {
        std::size_t operator () ( Pos const & key ) const
        {
            return key.x * game::Y + key.y;
        }
    };
}

////////////////////// controller ///////////////////////////

namespace controller {
    void printNext(const Pos &next) {
        cout << next.x << " " << next.y << endl;
    }
}


namespace util {
const Pos roundVectors[9] = {
    Pos(1, 1),
    Pos(1, 0),
    Pos(1, -1),
    Pos(-1, 1),
    Pos(-1, 0),
    Pos(-1, -1),
    Pos(0, 1),
    Pos(0, -1),
};
const Pos unitVectors[4] = {
    Pos(1, 0),
    Pos(-1, 0),
    Pos(0, 1),
    Pos(0, -1),
};
}

//////////////////////           ///////////////////////////

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
    T _board[game::Y * game::X] = {};
public:
    T operator[](Pos pos) const {
        return _board[pos.y * game::X + pos.x];
    }
    
    T &operator[](Pos pos) {
        return _board[pos.y * game::X + pos.x];
    }
    
    void dump() {
        REP(y, game::Y) {
            REP(x, game::X) {
                cerr << _board[y * game::X + x];
            } cerr << endl;
        }
    }
    
    int isPainted(const Pos &startPos) {
        unordered_set<Pos> visited;
        stack<Pos> cellStack;
        int colored = 0;
        visited.insert(startPos);
        cellStack.push(startPos);
        return 0;
        while (!cellStack.empty()) {
            Pos p = cellStack.top();
            AREP(i, util::roundVectors) {
                Pos next = p + util::roundVectors[i];
                if (visited.find(next) != visited.end()) continue;
                if (!next.isValid()) return 0;
                
                switch ((*this)[next]) {
                case Cell::NEURAL:
                    colored++;
                    cellStack.push(next);
                    visited.insert(next);
                case Cell::ME:
                    break;
                default:
                    return 0;
                    break;
                }
            }
        }
        return colored;
    }
};



////////////////////// simulator  ///////////////////////////
namespace simulator {
    void nextTurn(Board<char> *board, Pos *nextPositions) {
    // 重なってた場合は塗らない
    }

}


void paint(Board<char> *board, char color, Pos pos) {
    int paintCount = 0;
    REP(i, game::SQUID_NUM_MAX) {
        if ((*board)[util::unitVectors[i]]) {
            paintCount++;
        }
    }
    
    if (paintCount < 2) {
        return;
    }
    
}



////////////////////// ai ///////////////////////////////////
enum Strategy {
    CORNER,
    MAX_RECT,
};

struct PaintCandidate {
    int depth;
    int paintedCell;
    bool operator<(const PaintCandidate &other) const {
        if (paintedCell == other.paintedCell) {
            return depth < other.depth;
        }
        return paintedCell < other.paintedCell;
    }
    
    bool operator==(const PaintCandidate &other) const {
        return  paintedCell == other.paintedCell && depth == other.depth;
    }
    
};


class Battle {
public:
    int enemyNum = 0;
    Board<char> board;
    Board<bool> visited;
    Squid squids[game::SQUID_NUM_MAX];
    Strategy strategy = CORNER;
    Pos *nextDest = nullptr;

    
    void updateSquid(int index, int x, int y, int remainingTime) {
        squids[index].update(x, y, remainingTime);
    }
    
    Pos getCorner(Pos currentPos) {
        int x = currentPos.x < (game::X / 2) ? 0 : game::X - 1;
        int y = currentPos.y < (game::Y / 2) ? game::Y - 1 : 0;
        return Pos(x, y);
    }

    Pos minDistToEnemies(const Pos &myPos) {
        Pos p = squids[1].pos;
        int dist = myPos.dist(p);
        REP (i, enemyNum - 1) {
            Pos enemy = squids[i + 1].pos;
            if (p.dist(enemy) < dist) {
                p = enemy;
                dist = p.dist(enemy);
            }
        }
        return p;
    }

    
    void largestSpace(Pos prev, Pos pos, Board<char> *board, int depth, int maxDepth, vector<PaintCandidate> *candidates) {
        if (depth == maxDepth) return;
        AREP(i, util::unitVectors) {
            Pos next = pos + util::unitVectors[i];
            if (next == prev) continue;
            if ((*board)[next] == Cell::NEURAL) {
                (*board)[next] = Cell::ME;
                int result = (*board).isPainted(pos);
                if (result == 0) {
                    largestSpace(pos, next, board, depth + 1, maxDepth, candidates);
                } else {
                    PaintCandidate candidate;
                    candidate.depth = depth + 1;
                    candidate.paintedCell = result;
                }
                (*board)[next] = Cell::NEURAL;
                return;
            }
        }
    }
    
    Pos corner(Pos myPos) {
        if (nextDest == nullptr) {
            Pos p = getCorner(myPos); //TODO
            nextDest = new Pos(p.x, p.y);
        }
        
        //
        if (myPos.y != nextDest -> y) return Pos(myPos.x, nextDest -> y);
       return *nextDest;
    }
    
    Pos minRect(const Pos myPos) {
        Pos closest = minDistToEnemies(myPos);
        int dist = myPos.dist(closest);
        Pos maxPos = util::unitVectors[0];
        PaintCandidate maxCandidate;
        maxCandidate.paintedCell = 0; maxCandidate.depth = 20;
        AREP(i, util::unitVectors) {
            vector<PaintCandidate> candidates;
            largestSpace(myPos, myPos + util::unitVectors[i], &board, 0, max(dist, 6), &candidates);
            sort(candidates.begin(), candidates.end());
            if (maxCandidate < candidates[candidates.size() - 1]) {
                maxCandidate = candidates[candidates.size() - 1];
                maxPos = util::unitVectors[i];
            }
        }
        return maxPos;
    }
    
    void turn() {
        Pos myPos = squids[game::ME].pos;
        Pos next = Pos(0, 0);
        
        visited[myPos] = true;
        
        if (strategy == CORNER) {
            next = corner(myPos);
            if (next == myPos) {
                // next step;
                next = Pos(0, 0);
                strategy = MAX_RECT;
            }

        }
        
        if (strategy == MAX_RECT) {
            next = Pos(0, 0);//minRect(myPos);
        }
        cerr << strategy << endl;
    
        controller::printNext(next);
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
        battle.updateSquid(game::ME, x, y, remainingTime);
        
        REP(i, enemyNum) {
            int x, y, remainingTime;
            cin >> x >> y >> remainingTime; cin.ignore();
            battle.updateSquid(i + 1, x, y, remainingTime);
        }
        
        REP(y, game::Y) {
            string line;
            cin >> line; cin.ignore();
            REP(x, game::X) {
                battle.board[Pos(x, y)] = line[x];
            }
        }
//        battle.board.dump();
        battle.turn();
    }
    
    return 0;
}



































