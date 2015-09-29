import sys

Y = 20
X = 35
ME = 0
ALL = [
    (1, 0),
    (0, 1),
    (-1, 0),
    (0, -1),
]

class Cell():
    NEURAL = '.'
    ME = '0'

class Position():
    def __init__(self, y, x):
        self.y = y
        self.x = x
    def __repr__(self):
        return str(self.y) + ", " + str(self.x)

    def __eq__(self, other):
        return self and other and self.x == other.x and self.y == other.y

class Squid():
    def __init__(self):
        self.pos = None
        self.remaining_time = 0

    def update(self, x, y, remaining_time):
        self.pos = Position(y, x)
        self.remaining_time = remaining_time

class Battle():
    def __init__(self, enemy_num):
        self.enemy_num = enemy_num
        self.board = dimarray(Y, X, '.')
        self.visit_board = dimarray(Y, X, False)
        self.squids = [Squid() for _ in xrange(enemy_num + 1)]

    def update_squid(self, index, x, y, remaining_time):
        self.squids[index].update(x, y, remaining_time)

    def dump_table(self):
        for y in xrange(Y):
            for x in xrange(X):
                sys.stderr.write(self.board[y][x])
            sys.stderr.write('\n')

    def turn(self):
        p = self.squids[ME].pos
        self.visit_board[p.y][p.x] = True
        self.dump_table()

        next = Position(0, 0)
        sys.stderr.write("current: {}, {}\n".format(p.y, p.x))
        for uy, ux in ALL:
            y = p.y + uy
            x = p.x + ux
            if 0 <= y < Y and 0 <= x < X:
                sys.stderr.write("{}, {}: {}\n".format(y, x, self.board[y][x]))
                if self.board[y][x] == Cell.NEURAL:
                    next = Position(y, x)
                    break
                elif not self.visit_board[y][x]:
                    next = Position(y, x)
                # elif not next:
                #     next = Position(y, x)
        print next.x, next.y


# Utils
def dimarray(y, x, default):
    return [[default for xx in xrange(x)] for yy in xrange(y)]


def mainloop(battle):
    game_round = int(raw_input())
    x, y, remaining_time = map(int, raw_input().split())
    battle.squids[ME].update(x, y, remaining_time)
    for e in xrange(battle.enemy_num):
        x, y, remaining_time = map(int, raw_input().split())
        battle.squids[e + 1].update(x, y, remaining_time)

    for y in xrange(Y):
        battle.board[y] = list(raw_input())

    battle.turn()

if __name__ == '__main__':
    opponent_count = int(raw_input())
    battle = Battle(opponent_count)
    while True:
        mainloop(battle)