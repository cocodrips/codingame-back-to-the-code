Y = 20
X = 35
ME = 0
ALL = [
    (1, 0),
    (0, 1),
    (-1, 0),
    (0, -1),
]


class Position():
    def __init__(self, y, x):
        self.y = y
        self.x = x


class Squid():
    def __init__(self):
        self.pos = None
        self.remaining_time = 0

    def update(self, x, y, remaining_time):
        self.pos = Position(y, x)
        self.remaining_time = remaining_time


class Fields():
    def __init__(self, squid_num):
        """
        board: [y][x]
        """
        self.board = dimarray(Y, X)
        self.squids = [Squid() for _ in xrange(squid_num)]


class Battle():
    def __init__(self, enemy_num):
        self.enemy_num = enemy_num
        self.fields = Fields(enemy_num + 1)

    def update_squid(self, index, x, y, remaining_time):
        self.fields.squids[index].update(x, y, remaining_time)

    def turn(self):
        self.fields.squids[ME]
        print "0 0"


# Utils
def dimarray(y, x, default=0):
    return [[default for xx in xrange(x)] for yy in xrange(y)]


def mainloop(battle):
    game_round = int(raw_input())
    x, y, remaining_time = map(int, raw_input().split())
    battle.fields.squids[ME].update(x, y, remaining_time)
    for e in xrange(battle.enemy_num):
        x, y, remaining_time = map(int, raw_input().split())
        battle.fields.squids[e + 1].update(x, y, remaining_time)

    for y in xrange(Y):
        battle.fields.board[y] = list(raw_input())

    battle.turn()

if __name__ == '__main__':
    opponent_count = int(raw_input())
    battle = Battle(opponent_count)
    while True:
        mainloop(battle)