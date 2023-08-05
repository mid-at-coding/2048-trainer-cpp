import random
import math
import msvcrt  # another method I attempted for detecting keystrokes
import keyboard

board = [2, 4, 8, 2, 8, 32, 32, 256, 4, 512, 2048, 4096, 2, 8192, 16384, 32768]

is_moved = 0
is_alive = 1
zeroes = []

old_board = []
table_board = []
tile_sum = 0
move_played = ""
table_state_display = ""
table_positions = [0, 1, 2, 3, 4, 5, 6, 7, 8, 12]
best_move = ""
accuracy = 1

saved_table_board = []

tile_values = [0, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384, 32768, 65536]
map_normal_notation = ["_   ", "2   ", "4   ", "8   ", "16  ", "32  ", "64  ", "128 ", "256 ", "512 ", "1k  ",
                       "2k  ", "4k  ", "8k  ", "16k ", "32k ", "65k "]
map_tg_notation = ["_  ", "2  ", "4  ", "8  ", "16 ", "32 ", "64 ", "12 ", "25 ", "51 ", "1k ", "2k ", "4k ", "8k ",
                   "6k ", "3k ", "5k "]
map_table_notation = ["0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "a", "b", "c", "d", "e", "f", "g"]
map_friks_notation = ["_    ", "I    ", "II   ", "III  ", "IV   ", "V    ", "VI   ", "VII  ", "VIII ", "IX   ",
                      "X    ", "XI   ", "XII  ", "XIII ", "XIV  ", "XV   ", "XVI  "]
combo = 0

PGN_map = ' !"#$%&\'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~ÇüéâäàåçêëèïîìÄÅÉæÆôöòûùÿÖÜø£Ø×ƒá'



inaccuracies = open("inaccuracies.txt", "w")

def move(pos1, pos2, pos3, pos4):
    global is_moved

    # see orientation
    if board[pos1] == 0:
        orientation = '0'
    else:
        orientation = '1'

    if board[pos2] == 0:
        orientation = orientation + '0'
    else:
        orientation = orientation + '1'

    if board[pos3] == 0:
        orientation = orientation + '0'
    else:
        orientation = orientation + '1'

    if board[pos4] == 0:
        orientation = orientation + '0'
    else:
        orientation = orientation + '1'

    if '10' in orientation:
        is_moved = 1

    # move all pieces to one side
    if board[pos4] == 0:
        if board[pos3] == 0:
            if board[pos2] == 0:
                if board[pos1] == 0:
                    return 0
                else:
                    board[pos4] = board[pos1]
                    board[pos1] = 0
            else:
                board[pos4] = board[pos2]
                board[pos3] = board[pos1]
                board[pos2] = 0
                board[pos1] = 0
        else:
            board[pos4] = board[pos3]
            board[pos3] = board[pos2]
            board[pos2] = board[pos1]
            board[pos1] = 0
    if board[pos3] == 0:
        if board[pos2] == 0:
            board[pos3] = board[pos1]
            board[pos1] = 0
        else:
            board[pos3] = board[pos2]
            board[pos2] = board[pos1]
            board[pos1] = 0
    else:
        if board[pos2] == 0:
            board[pos2] = board[pos1]
            board[pos1] = 0
    # merge tiles

    if board[pos4] == board[pos3] and board[pos4] > 0:
        board[pos4] = board[pos3] * 2
        board[pos3] = board[pos2]
        board[pos2] = board[pos1]
        board[pos1] = 0
        is_moved = 1

    if board[pos3] == board[pos2] and board[pos3] > 0:
        board[pos3] = board[pos2] * 2
        board[pos2] = board[pos1]
        board[pos1] = 0
        is_moved = 1

    if board[pos2] == board[pos1] and board[pos2] > 0:
        board[pos2] = board[pos1] * 2
        board[pos1] = 0
        is_moved = 1


def make_tile():
    if is_moved == 1:
        zeroes.clear()
        for x in range(len(board)):
            if board[x] == 0:
                zeroes.append(x)
        position_random = zeroes[math.floor(random.random() * len(zeroes))]
        tile_random = 2 ** (math.floor(random.random() + 1.1))
        board[position_random] = tile_random


def show_board():


    # convert to other notation for display

    display_board = []
    table_board = []

    for i in range(len(board)):
        display_board.append(map_normal_notation[tile_values.index(board[i])])

    print(display_board[0] + display_board[1] + display_board[2] + display_board[3])
    print(display_board[4] + display_board[5] + display_board[6] + display_board[7])
    print(display_board[8] + display_board[9] + display_board[10] + display_board[11])
    print(display_board[12] + display_board[13] + display_board[14] + display_board[15])
    print("--------------")


def make_old_board():
    global old_board
    global table_state_display

    # generate buffer board used for sum and table reference
    old_board.clear()
    table_board.clear()
    for i in range(len(board)):
        old_board.append(board[i])

    # convert old board to table notation

    for i in range(len(old_board)):
        table_board.append(map_table_notation[tile_values.index(old_board[i])])

    # display old table state
    table_state_display = ""
    for i in range(len(table_positions)):
        table_state_display = table_state_display + str(table_board[table_positions[i]])


def search_table():
    global accuracy
    global tile_sum
    global move_played
    global best_move
    global combo
    global table_id
    global inaccuracies
    tile_sum = 0
    for i in range(len(table_positions)):
        tile_sum = tile_sum + old_board[table_positions[i]]
    tile_sum = str(tile_sum)

    file = open("F:/" + table_id + "/" + tile_sum + ".txt", "r")

    for x in file:
        if x[:10] == table_state_display:

            print(x)
            moves = x.split()
            if moves[1] == moves[moves.index(move_played) + 1]:
                combo += 1
                if combo < 2:
                    print('Perfect! Your move, ' + move_played + ', was the best move!')
                else:
                    print('Perfect! Your move, ' + move_played + ', was the best move! ' + str(combo) + ' in a row!')
            else:
                for i in range(int(len(moves) / 2 - 1)):
                    if moves[2 * i + 3] == moves[1]:
                        current_move_accuracy = float(moves[moves.index(move_played) + 1]) / float(moves[1])
                        best_move = moves[2 * i + 2]

                        if current_move_accuracy > 0.999999999999999:
                            combo += 1
                            if combo < 2:
                                print('Perfect! Your move, ' + move_played + ', was the best move!')
                            else:
                                print('Perfect! Your move, ' + move_played + ', was the best move! ' + str(
                                    combo) + ' in a row!')
                        else:
                            combo = 0
                            if current_move_accuracy >= 0.99999:
                                print("Basically perfect! You played " + str(move_played) + ", which is " + str(current_move_accuracy * 100) + "% accurate, when the optimal move was " + str(best_move))
                            elif current_move_accuracy >= 0.9999:
                                print("Great move! You played " + str(move_played) + ", which is " + str(current_move_accuracy * 100) + "% accurate, when the optimal move was " + str(best_move))
                            elif current_move_accuracy >= 0.999:
                                print("Good move. You played " + str(move_played) + ", which is " + str(current_move_accuracy * 100) + "% accurate, when the optimal move was " + str(best_move))
                            elif current_move_accuracy >= 0.99:
                                print("Inaccuracy. You played " + str(move_played) + ", which is " + str(current_move_accuracy * 100) + "% accurate, when the optimal move was " + str(best_move))
                            elif current_move_accuracy >= 0.95:
                                print("Mistake. You played " + str(move_played) + ", which is " + str(current_move_accuracy * 100) + "% accurate, when the optimal move was " + str(best_move))
                            elif current_move_accuracy >= 0.8:
                                print("BLUNDER. You played " + str(move_played) + ", which is " + str(current_move_accuracy * 100) + "% accurate, when the optimal move was " + str(best_move))
                            elif current_move_accuracy >= 0.5:
                                print("DONKEY. You played " + str(move_played) + ", which is " + str(current_move_accuracy * 100) + "% accurate, when the optimal move was " + str(best_move))
                            elif current_move_accuracy >= 0.1:
                                print("FROG. You played " + str(move_played) + ", which is " + str(current_move_accuracy * 100) + "% accurate, when the optimal move was " + str(best_move))
                            elif current_move_accuracy >= 0.01:
                                print("BUFFOON. You played " + str(move_played) + ", which is " + str(current_move_accuracy * 100) + "% accurate, when the optimal move was " + str(best_move))
                            elif current_move_accuracy > 0:
                                print("TROLL. You played " + str(move_played) + ", which is " + str(current_move_accuracy * 100) + "% accurate, when the optimal move was " + str(best_move))
                            else:
                                print("CLOWN. You played " + str(move_played) + ", which is " + str(current_move_accuracy * 100) + "% accurate, when the optimal move was " + str(best_move))
                            accuracy = accuracy * float(moves[moves.index(move_played) + 1]) / float(moves[1])
                            print("your current combined accuracy is " + str(accuracy*100) + "%")
                            if current_move_accuracy < 0.998:
                                inaccuracies.write(str(accuracy) + " " + table_state_display + " you played " + move_played + " and the best move was " + best_move + ", accuracy of " + str(current_move_accuracy * 100) + " (" + str(moves[moves.index(move_played) + 1]) + " vs " + str(moves[1]) + ")\n")
                        break

            break

    file.close()


def move_left():
    global is_moved
    global move_played
    is_moved = 0
    move_played = "L"
    make_old_board()
    move(3, 2, 1, 0)
    move(7, 6, 5, 4)
    move(11, 10, 9, 8)
    move(15, 14, 13, 12)
    make_tile()
    if is_moved == 1:
        search_table()
        show_board()


def move_right():
    global is_moved
    global move_played
    is_moved = 0
    move_played = "R"
    make_old_board()
    move(0, 1, 2, 3)
    move(4, 5, 6, 7)
    move(8, 9, 10, 11)
    move(12, 13, 14, 15)
    make_tile()
    if is_moved == 1:
        search_table()
        show_board()


def move_up():
    global is_moved
    global move_played
    is_moved = 0
    move_played = "U"
    make_old_board()
    move(12, 8, 4, 0)
    move(13, 9, 5, 1)
    move(14, 10, 6, 2)
    move(15, 11, 7, 3)
    make_tile()
    if is_moved == 1:
        search_table()
        show_board()


def move_down():
    global is_moved
    global move_played
    is_moved = 0
    move_played = "D"
    make_old_board()
    move(0, 4, 8, 12)
    move(1, 5, 9, 13)
    move(2, 6, 10, 14)
    move(3, 7, 11, 15)
    make_tile()
    if is_moved == 1:
        search_table()
        show_board()


def detect_death():
    global is_alive
    if 0 in board:
        is_alive = 1
    elif board[0] == board[1] or board[4] == board[5] or board[8] == board[9] or board[12] == board[13] or board[1] == \
            board[2] or board[5] == board[6] or board[9] == board[10] or board[13] == board[14] or board[2] == board[
        3] or board[7] == board[6] or board[11] == board[10] or board[15] == board[14] or board[0] == board[4] or board[
        1] == board[5] or board[2] == board[6] or board[3] == board[7] or board[4] == board[8] or board[5] == board[
        9] or board[6] == board[10] or board[7] == board[11] or board[8] == board[12] or board[9] == board[13] or board[
        10] == board[14] or board[11] == board[15]:
        is_alive = 1
    else:
        is_alive = 0

    if board[10] == 1024:
        is_alive = 2


def make_all_tiles():
    if is_moved == 1:
        zeroes.clear()
        for i in range(len(board)):
            if board[i] == 0:
                zeroes.append(i)
        for i in range(len(zeroes)):
            board[zeroes[i]] = tile_spawn
            make_old_board()
            table_string = ""
            for x in range(len(table_board)):
                table_string = table_string + table_board[x]
            position_0.write(str(table_string))
            position_0.write("\n")
            board[zeroes[i]] = 0



def left_all_spawns():
    global is_moved
    is_moved = 0
    move(3, 2, 1, 0)
    move(7, 6, 5, 4)
    move(11, 10, 9, 8)
    move(15, 14, 13, 12)
    if is_moved == 1:
        make_all_tiles()


def reset_board():
    pass

def generate_positions():
    global tile_spawn
    global position_0
    global position_2_below
    global position_4_below

    sum = 0
    for i in range(len(board)):
        sum = sum + board[i]
    print(sum)
    starting_sum = sum

    position_0 = open("positions/" + str(sum) + ".txt", "w")
    position_2_below = 0
    position_4_below = 0
    make_old_board()
    for i in range(len(table_board)):
        position_0.write(str(table_board[i]))
    position_0.write("\n")
    position_0.close()  # write the first position that this starts with

    sum += 2
    position_4_below = position_2_below
    position_2_below = open("positions/" + str(sum-2) + ".txt", "r")
    position_0 = open("positions/" + str(sum) + ".txt", "w")  # open the files

    saved_table_board.clear()
    for line in position_2_below:
        for char in range(len(line)):
            saved_table_board.append(line[char])
        print(saved_table_board)

    tile_spawn = 2
    left_all_spawns()
    reset_board()
    position_0.close()


opcode = 0
'''
this sets what the program will do. opcodes are as follows:
0 = play a game from the given starting position, referencing an already existing table (working - 2023-07-02)
1 = make a table, then stop
2 = analyze a game from a PGN replay
'''


# generate_positions()

if opcode == 2:
    board = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]

    position_map_BRH = [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15]
    position_map_BLH = [3, 2, 1, 0, 7, 6, 5, 4, 11, 10, 9, 8, 15, 14, 13, 12]
    position_map_TRH = [12, 13, 14, 15, 8, 9, 10, 11, 4, 5, 6, 7, 0, 1, 2, 3]
    position_map_TLH = [15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0]

    move_map_BRH = [0, 1, 2, 3]
    move_map_BLH = [0, 3, 2, 1]
    move_map_TRH = [2, 1, 0, 3]
    move_map_TLH = [2, 3, 0, 1]

    replay = open("replay.txt", encoding="utf-8")
    replay_text = str(replay.read())
    print(str(replay_text))
    replay.close()
    moves_made = 0
    for i in replay_text:
        replay_move = move_map_BRH[int(math.floor(PGN_map.index(i) / 32))]
        replay_tile = 2**(math.floor((PGN_map.index(str(i)) / 16) % 2)+1)
        replay_position = position_map_BRH[int((PGN_map.index(str(i)) % 16) % 4) * 4 + (math.floor((PGN_map.index(str(i)) % 16) / 4))]

        if moves_made >= 2:
            if replay_move == 3:
                move_played = "L"
                make_old_board()
                move(3, 2, 1, 0)
                move(7, 6, 5, 4)
                move(11, 10, 9, 8)
                move(15, 14, 13, 12)

            elif replay_move == 1:
                move_played = "R"
                make_old_board()
                move(0, 1, 2, 3)
                move(4, 5, 6, 7)
                move(8, 9, 10, 11)
                move(12, 13, 14, 15)

            elif replay_move == 0:
                move_played = "U"
                make_old_board()
                move(12, 8, 4, 0)
                move(13, 9, 5, 1)
                move(14, 10, 6, 2)
                move(15, 11, 7, 3)

            elif replay_move == 2:
                move_played = "D"
                make_old_board()
                move(0, 4, 8, 12)
                move(1, 5, 9, 13)
                move(2, 6, 10, 14)
                move(3, 7, 11, 15)

        board[replay_position] = replay_tile
        moves_made += 1

        # show_board()

        _32k_endgame_sums = [32256, 48640, 56832, 60928, 62976, 64000]
        _65k_endgame_sums = [64512]

        if board[9]+board[10]+board[11]+board[13]+board[14]+board[15] in _32k_endgame_sums:
            sum_combo += 1
            if sum_combo >= 2:
                if board[9] == 512:
                    table_positions = [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 12]
                    table_id = "32k_dpdf_from_setup"
                    search_table()
                    show_board()
                elif board[10] == 512:
                    table_positions = [0, 1, 2, 3, 4, 5, 6, 7, 8, 10, 12]
                    table_id = "32k-dpdf-2k-512-1k"
                    search_table()
                    show_board()
                elif board[11] == 512:
                    table_positions = [0, 1, 2, 3, 4, 5, 6, 7, 8, 11, 12]
                    table_id = "32k-dpdf-reverse-L"
                    search_table()
                    show_board()

        elif board[9]+board[10]+board[11]+board[13]+board[14]+board[15] in _65k_endgame_sums:
            sum_combo += 1
            if sum_combo >= 2:
                table_positions = [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 12]
                table_id = "65k_dpdf_from_830k_pdf"
                search_table()
                show_board()
        else:
            combo = 0
            accuracy = 1
            sum_combo = 0

if opcode == 1:
    generate_positions()

if opcode == 0:
    table_id = "32k-dpdf10-full"
    show_board()
    print("Make your move (IJKL keys)")
    while is_alive == 1:
        def wait_for_key(keys):
            while True:
                event = keyboard.read_event()
                if event.event_type == "down" and event.name in keys:
                    return keys[event.name]


        keys_to_functions = {
            'i': move_up,
            'j': move_left,
            'k': move_down,
            'l': move_right,
        }

        function_to_execute = wait_for_key(keys_to_functions)
        function_to_execute()
        detect_death()

    if is_alive == 0:
        print("you died lmao. your total accuracy was " + str(accuracy) + " and you played like you have a success rate of at most " + str(accuracy * 83.26358285319678) + "%")
    if is_alive == 2:
        print("you completed the endgame! your accuracy was " + str(accuracy * 100) + "%, and you played like you have a success rate of " + str(accuracy * 83.26358285319678) + "%")

