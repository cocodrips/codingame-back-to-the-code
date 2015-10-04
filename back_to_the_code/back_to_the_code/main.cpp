
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
    Pos () {
        Pos(-1, -1);
    }
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
        return (x * game::Y + y) < (other.x * game::Y + other.y);
    }
    
public:
    int dist(const Pos &other) const {
        return abs(x - other.x) + abs(y - other.y);
    }
    
    bool isValid() {
        return 0 <= x && x < game::X && 0 <= y && y < game::Y;
    }
    
    void dump() {
        cerr << "Pos: (" << x << ", " << y << ")";
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
const Pos roundVectors[] = {
    Pos(1, 1),
    Pos(1, 0),
    Pos(1, -1),
    Pos(-1, 1),
    Pos(-1, 0),
    Pos(-1, -1),
    Pos(0, 1),
    Pos(0, -1),
};
const Pos unitVectors[] = {
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
                cerr << _board[y * game::X + x];  // cerr << (*this)[Pos(x, y)];
            }
            cerr << endl;
        }
    }
    
    void init(T n) {
        REP(y, game::Y) {
            REP(x, game::X) {
                _board[y * game::X + x] = n;
            }
            cerr << endl;
        }
    }
    
    int isPainted(const Pos &startPos) {
        unordered_set<Pos> visited;
        stack<Pos> cellStack;
        int colored = 1;
        visited.insert(startPos);
        cellStack.push(startPos);
        
        while (!cellStack.empty()) {
            Pos p = cellStack.top();
            cellStack.pop();
            AREP(i, util::roundVectors) {
                Pos next = p + util::roundVectors[i];
                if (!next.isValid()) return 0;
                if (visited.find(next) != visited.end()) continue;
                
                switch ((*this)[next]) {
                case Cell::NEURAL:
                    colored++;
                    cellStack.push(next);
                    visited.insert(next);
                case Cell::ME:
                    continue;
                default:
                    return 0;
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
    int depth = 0;
    int paintedCell = 0;
    vector<Pos> path;
    bool operator<(const PaintCandidate &other) const {
        return ((float)(paintedCell + depth) / depth) < ((float)(other.paintedCell + other.depth) / other.depth);
//        if (paintedCell == other.paintedCell) {
//            return depth < other.depth;
//        }
//        return paintedCell < other.paintedCell;
    }
    
    bool operator==(const PaintCandidate &other) const {
        return paintedCell == other.paintedCell && depth == other.depth;
    }
    
};


class Battle {
public:
    int enemyNum = 0;
    Board<char> board;
    Board<bool> visited;
    Squid squids[game::SQUID_NUM_MAX];
    Strategy strategy = MAX_RECT;
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

    
    void largestSpace(Pos pos, Board<char> *board, int depth, int maxDepth, vector<PaintCandidate> *candidates) {
        if (depth == maxDepth) return;
        AREP(i, util::unitVectors) {
            Pos next = pos + util::unitVectors[i];
            if ((*board)[next] == Cell::NEURAL) {
                (*board)[next] = Cell::ME;
                
                int paintedSum = 0;
                AREP(i, util::unitVectors) {
                    Pos tmp = next + util::unitVectors[i];
                    if (visited[tmp]) continue;
                    int result = (*board).isPainted(tmp);
                    paintedSum += result;
                }
                if (paintedSum == 0) {
                    largestSpace(next, board, depth + 1, maxDepth, candidates);
                } else {
                    PaintCandidate candidate;
                    candidate.depth = depth + 1;
                    candidate.paintedCell = paintedSum;
                    candidates -> push_back(candidate);
                    cerr << endl;
                    cerr << (float)(paintedSum + depth + 1) / (depth + 1)   << endl;
//                    board->dump();
                }

                (*board)[next] = Cell::NEURAL;
            }
        }
        return;
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
    
//    Pos nearestUnknownCell(const Pos pos) {
//        
//    }
    
    Pos unknownCell(const Pos myPos) {
        Pos p;
        AREP(i, util::unitVectors) {
            Pos tmp = myPos + util::unitVectors[i];
            if (!tmp.isValid()) continue;
            if (p.isValid()) {
                p = tmp;
            }
            
            if (board[tmp] == Cell::NEURAL) {
                return tmp;
            }
            
            if (!visited[tmp]) {
                return tmp;
            }
        }
        return p;
    }
    
    Pos minRect(const Pos myPos) {
        Pos closest = minDistToEnemies(myPos);
        int dist = myPos.dist(closest) / 2;
        Pos maxPos = unknownCell(myPos);
        cerr << "Default";
        maxPos.dump();
        PaintCandidate maxCandidate;
        maxCandidate.paintedCell = 0; maxCandidate.depth = 20;
        AREP(i, util::unitVectors) {
            Pos next = myPos + util::unitVectors[i];
            if (visited[next]) continue;
            vector<PaintCandidate> candidates;
            char tmp = board[next];
            if (tmp == Cell::NEURAL) {
                board[next] = Cell::ME;
            }
            vector<Pos> path;
            largestSpace(myPos + util::unitVectors[i], &board, 0, min(dist, 6), &candidates);
            cerr << "candidates: " << candidates.size() << endl;
            if (candidates.size() > 0) {
                sort(candidates.begin(), candidates.end());
                (myPos + util::unitVectors[i]).dump();
                cerr << "cell:" << candidates[candidates.size() - 1].paintedCell;
                cerr << ", depth:" << candidates[candidates.size() - 1].depth << endl;
                if (maxCandidate < candidates[candidates.size() - 1]) {
                    maxCandidate = candidates[candidates.size() - 1];
                    maxPos = myPos + util::unitVectors[i];
                }
            }
            cerr << "--------------------------------------" << endl;
            board[next] = tmp;

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
            next = minRect(myPos);
        }
        cerr << "Strategy:" << strategy << endl;
    
        controller::printNext(next);
    }
};

//////////////////////  test  /////////////////////////////
namespace test {
void isPainted() {
    Board<char> board;
    board.init(Cell::NEURAL);
    Pos nineBlock[] = {
        Pos(0, 0),
        Pos(0, 1),
        Pos(0, 2),
        Pos(0, 3),
        Pos(1, 3),
        Pos(2, 3),
        Pos(3, 3),
        Pos(3, 2),
        Pos(3, 1),
        Pos(3, 0),
        Pos(2, 0),
        Pos(1, 0),
    };
    AREP(i, nineBlock) {
        board[nineBlock[i]] = Cell::ME;
    }
    cout << "isPainted - 4: " << board.isPainted(Pos(1, 1)) << endl;
    board[Pos(3,2)] = Cell::NEURAL;
    cout << "isPainted - 0: " << board.isPainted(Pos(1, 1)) << endl;


}
    
void largestSpace() {
    Board<char> board;
    board.init(Cell::NEURAL);
    Pos nineBlock[] = {
        Pos(0, 0),
        Pos(0, 1),
        Pos(0, 2),
        Pos(0, 3),
        Pos(1, 3),
        Pos(3, 3),
        Pos(3, 2),
        Pos(3, 1),
        Pos(3, 0),
        Pos(2, 0),
        Pos(1, 0),
    };
    AREP(i, nineBlock) {
        board[nineBlock[i]] = Cell::ME;
    }
    
    Battle *battle = new Battle();
    vector<PaintCandidate> candidates;
    battle -> largestSpace(Pos(1, 3), &board, 0, 7, &candidates);
    cout << "largestSpace - 0: " << candidates.size() << endl;
    
    board[Pos(2, 3)] = Cell::ME;
    vector<PaintCandidate> candidates2;
    battle -> largestSpace(Pos(3, 3), &board, 0, 6, &candidates2);
    cout << "largestSpace - 2: " << candidates.size() << endl;

}
    
void largestSpace2() {
    Board<char> board;
    board.init(Cell::NEURAL);
    Pos nineBlock[] = {
        Pos(10, 1),
        Pos(11, 1),
        Pos(12, 1),
        Pos(12, 2),
        Pos(12, 3),
        Pos(12, 4),
        Pos(11, 4),
        Pos(10, 4),
        Pos(9, 4),
        Pos(8, 4),
        Pos(7, 4),
        Pos(7, 3),
        Pos(7, 2),
        Pos(7, 1),
    };
    AREP(i, nineBlock) {
        board[nineBlock[i]] = Cell::ME;
    }
    
    Battle *battle = new Battle();
    vector<PaintCandidate> candidates;
    battle -> largestSpace(Pos(7, 1), &board, 0, 7, &candidates);
    cout << "largestSpace2 - 0: " << candidates.size() << endl;
//    sort(candidates.begin(), candidates.end());
    REP(i, candidates.size()) {
        cerr << candidates[i].paintedCell << "," << candidates[i].depth << endl;
    }
    
    
}
    
void runner() {
    test::isPainted();
    test::largestSpace();
    test::largestSpace2();
}
}

////////////////////// mainloop ///////////////////////////

int main(int argc, const char * argv[]) {
//    test::runner(); return 0;
    
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



































