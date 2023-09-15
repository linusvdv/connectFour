import json
from dataclasses import dataclass, field


@dataclass
class position:
    board: list[list[int]] = field(
            default_factory=lambda: [[0 for j in range(6)] for i in range(7)])
    color: int = 1


def get_pos(game_start_position):
    pos = position()

    for j in range(6):
        for i in range(7):
            if game_start_position[0] == "R":
                pos.board[i][j] = 1
            elif game_start_position[0] == "Y":
                pos.board[i][j] = -1
            else:
                pos.board[i][j] = 0
            game_start_position = game_start_position[1:]

    if game_start_position[1] == "R":
        pos.color = 1
    else:
        pos.color = -1

    return pos


def pos_to_data(pos: position):
    training_data = []

    # horrizontal - 192
    horrizontal = [[[0 for y in range(6)] for x in range(4)] for color in range(2)]

    for x in range(4):
        for y in range(6):
            for i in range(4):
                if pos.board[x+i][y] == 1:
                    if horrizontal[0][x][y] != -1:
                        horrizontal[0][x][y] += 1
                    horrizontal[1][x][y] = -1
                if pos.board[x+i][y] == -1:
                    if horrizontal[1][x][y] != -1:
                        horrizontal[1][x][y] += 1
                    horrizontal[0][x][y] = -1

    for color in range(2):
        for pieces in range(4):
            for y in range(6):
                for x in range(4):
                    if horrizontal[color][x][y] == pieces or (pieces == 0 and horrizontal[color][x][y] == -1):
                        training_data.append(1)
                    else:
                        training_data.append(0)

    # vertical - 168
    vertical = [[[0 for y in range(3)] for x in range(7)] for color in range(2)]

    for x in range(7):
        for y in range(3):
            for i in range(4):
                if pos.board[x][y+i] == 1:
                    if vertical[0][x][y] != -1:
                        vertical[0][x][y] += 1
                    vertical[1][x][y] = -1
                if pos.board[x][y+i] == -1:
                    if vertical[1][x][y] != -1:
                        vertical[1][x][y] += 1
                    vertical[0][x][y] = -1

    for color in range(2):
        for pieces in range(4):
            for y in range(3):
                for x in range(7):
                    if vertical[color][x][y] == pieces or (pieces == 0 and vertical[color][x][y] == -1):
                        training_data.append(1)
                    else:
                        training_data.append(0)

    # diagonal right up/left down - 94
    d1 = [[[0 for y in range(3)] for x in range(4)] for color in range(2)]

    for x in range(4):
        for y in range(3):
            for i in range(4):
                if pos.board[x+i][y+i] == 1:
                    if d1[0][x][y] != -1:
                        d1[0][x][y] += 1
                    d1[1][x][y] = -1
                if pos.board[x+i][y+i] == -1:
                    if d1[1][x][y] != -1:
                        d1[1][x][y] += 1
                    d1[0][x][y] = -1

    for color in range(2):
        for pieces in range(4):
            for y in range(3):
                for x in range(4):
                    if d1[color][x][y] == pieces or (pieces == 0 and d1[color][x][y] == -1):
                        training_data.append(1)
                    else:
                        training_data.append(0)

    # diagonal right down/left up - 94
    d2 = [[[0 for y in range(3)] for x in range(4)] for color in range(2)]

    for x in range(4):
        for y in range(3):
            for i in range(4):
                if pos.board[x+i][y+3-i] == 1:
                    if d2[0][x][y] != -1:
                        d2[0][x][y] += 1
                    d2[1][x][y] = -1
                if pos.board[x+i][y+3-i] == -1:
                    if d2[1][x][y] != -1:
                        d2[1][x][y] += 1
                    d2[0][x][y] = -1

    for color in range(2):
        for pieces in range(4):
            for y in range(3):
                for x in range(4):
                    if d2[color][x][y] == pieces or (pieces == 0 and d2[color][x][y] == -1):
                        training_data.append(1)
                    else:
                        training_data.append(0)

    return training_data


if __name__ == "__main__":
    game_data = 0
    training_data = []
    with open("games.json", "r", encoding="utf-8") as file:
        game_data = json.load(file)

    game_num = len(game_data)
    for i in range(game_num):
        game = game_data[i]
        pos = get_pos(game[0][12:56])
        training_data.append([pos_to_data(pos), game[1]])

        game = [game[0][63:], game[1]]

        for move in game[0].split():
            x, y = int(move) % 7, int(move) // 7
            pos.board[x][y] = pos.color
            pos.color *= -1

            training_data.append([pos_to_data(pos), game[1]])

        if i % 1000 == 999:
            print(i+1, game_num)

    with open("training_data.json", "w", encoding="utf-8") as file:
        json.dump(training_data, file, ensure_ascii=False, indent=4)
