#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#ifdef _WIN32

#include <conio.h>
#include <windows.h>

void get_terminal_size(int *rows, int *cols)
{
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi))
    {
        *cols = csbi.srWindow.Right - csbi.srWindow.Left;
        *rows = csbi.srWindow.Bottom - csbi.srWindow.Top - 3;
    }
    else
    {
        *cols = 0;
        *rows = 0;
    }
}

void enable_raw_mode()
{
}

void disable_raw_mode()
{
    printf("\033[?25h\033[0m");
}

#else

#include <sys/select.h>
#include <sys/ioctl.h>

void get_terminal_size(int *rows, int *cols)
{
    struct winsize w;

    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == -1)
    {
        *cols = 0;
        *rows = 0;
    }
    else
    {
        *rows = w.ws_row - 3;
        *cols = w.ws_col;
    }
}

void enable_raw_mode()
{
    system("stty -icanon -echo -icrnl -ixon opost min 1 time 0");
}

void disable_raw_mode()
{
    system("stty sane");
    printf("\033[?25h\033[0m");
}

static int kbhit(void)
{
    struct timeval tv = {0L, 0L};
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(STDIN_FILENO, &fds);
    int r = select(STDIN_FILENO + 1, &fds, NULL, NULL, &tv);
    if (r > 0)
        return 1;
    return 0;
}

int _getch()
{
    unsigned char c;
    ssize_t n = read(STDIN_FILENO, &c, 1);
    if (n <= 0)
        return -1;
    return (int)c;
}

#endif

typedef struct
{
    int *arr;
    int shape;
    int front, rear, removed;
    int x, y, dir;
    int score;
    int invincible, useCount;
} snake;

typedef struct
{
    int x, y;
    int flicker;
    int bonus;
} ball;

void init_buffer(char **buffer, char **temp_buffer, int rows, int cols)
{
    int size = sizeof(char) * rows * cols;
    int shape = rows * cols;
    *buffer = (char *)malloc(size);
    *temp_buffer = (char *)malloc(size);
    for (int i = 0; i < shape; i++)
    {
        printf("\033[42m");
        (*buffer)[i] = ' ';
        (*temp_buffer)[i] = ' ';
        printf(" ");
        if ((i + 1) % cols == 0)
            printf("\033[0m\n");
    }
}

void destroy_buffer(char *buffer, char *temp_buffer)
{
    free(buffer);
    free(temp_buffer);
}

void init_snake(snake *s, int rows, int cols)
{
    int shape = rows * cols;
    s->arr = (int *)calloc(shape, sizeof(int));
    s->x = (cols - 1) / 2;
    s->y = (rows - 1) / 2;
    s->arr[0] = s->y * cols + s->x;
    s->front = 0;
    s->rear = 0;
    s->dir = 1;
    s->shape = shape;
    s->score = 0;
    s->invincible = 0;
    s->useCount = 0;
}

void increase_snake(snake *s, int rows, int cols)
{
    switch (s->dir)
    {
    case 1:
        s->y -= 1;
        break;
    case 2:
        s->x += 1;
        break;
    case 3:
        s->x -= 1;
        break;
    case 4:
        s->y += 1;
        break;
    default:
        break;
    }
    if (s->y < 0)
        s->y = rows - 1;
    if (s->y >= rows)
        s->y = 0;
    if (s->x < 0)
        s->x = cols - 1;
    if (s->x >= cols)
        s->x = 0;

    s->rear = (s->rear + 1) % s->shape;
    s->arr[s->rear] = s->y * cols + s->x;
}

void decrease_snake(snake *s, int rows, int cols, char *temp_buffer)
{
    s->removed = s->arr[s->front];
    s->arr[s->front] = 0;
    s->front = (s->front + 1) % s->shape;
}

int handle_input(snake *s)
{
    if (kbhit())
    {
        char c = _getch();
        switch (c)
        {
        case 'w':
            if (s->dir != 4)
                s->dir = 1;
            break;
        case 'a':
            if (s->dir != 2)
                s->dir = 3;
            break;
        case 's':
            if (s->dir != 1)
                s->dir = 4;
            break;
        case 'd':
            if (s->dir != 3)
                s->dir = 2;
            break;
        case 'q':
            return 0;

        default:
            break;
        }
    }
    return 1;
}

void destroy_snake(snake *s)
{
    free(s->arr);
}

void generate_ball(char *temp_buffer, ball *b, int rows, int cols)
{
    int randB = rand() % 10;
    if (randB == 0)
        b->bonus = 1;
    else
        b->bonus = 0;
    int randX, randY;
    do
    {
        randX = rand() % cols;
        randY = rand() % rows;
    } while (temp_buffer[randY * cols + randX] == 'x' || temp_buffer[randY * cols + randX] == 'X' || temp_buffer[randY * cols + randX] == 'o' || temp_buffer[randY * cols + randX] == 'O' || temp_buffer[randY * cols + randX] == 'Q' || temp_buffer[randY * cols + randX] == 'z');
    b->x = randX;
    b->y = randY;
}

void update_buffer(char *temp_buffer, snake *s, ball *b, int rows, int cols)
{
    char sn = s->invincible ? 'z' : 'x';
    temp_buffer[s->arr[s->rear]] = sn;
    if (s->rear >= s->front)
        for (int i = s->rear; i >= s->front; i--)
        {
            temp_buffer[s->arr[i]] = sn;
        }
    else
    {
        for (int i = s->rear; i >= 0; i--)
        {
            temp_buffer[s->arr[i]] = sn;
        }
        for (int i = s->shape - 1; i >= s->front; i--)
        {
            temp_buffer[s->arr[i]] = sn;
        }
    }
    temp_buffer[s->arr[s->rear]] = 'X';
    char c;
    if (b->bonus)
        c = b->flicker ? 'q' : 'Q';
    else
        c = b->flicker ? 'o' : 'O';
    temp_buffer[b->y * cols + b->x] = c;
    temp_buffer[s->removed] = ' ';
    b->flicker = !b->flicker;
}

void display_buffer(char *buffer, char *temp_buffer, int rows, int cols, int score, float difficulty, snake *s)
{
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            int ind = i * cols + j;
            char *b = &buffer[ind];
            char *tb = &temp_buffer[ind];
            if (*b != *tb)
            {
                int color;
                switch (*tb)
                {
                case ' ':
                    color = 42;
                    break;
                case 'X':
                    color = 40;
                    break;
                case 'x':
                    color = 41;
                    break;
                case 'z':
                    color = 47;
                    break;
                case 'O':
                    color = 44;
                    break;
                case 'o':
                    color = 43;
                    break;
                case 'Q':
                    color = 47;
                    break;
                case 'q':
                    color = 40;
                    break;

                default:
                    break;
                }
                printf("\033[%d;%dH\033[%dm \033", i + 1, j + 1, color);
                *b = *tb;
            }
        }
    }
    printf("\033[%d;0H\033[1;37;40m%*s", rows + 1, cols, " ");
    printf("\033[%d;0HScore: %d | Difficulty: %.2f\033[0m", rows + 1, score, difficulty);
    if (s->invincible)
        printf("\033[1;33;40m | Invincible Count: %d\033[m", s->useCount);
    fflush(stdout);
}

void game_over(int rows, int cols, snake *s)
{
    printf("\033[1;30;41m\033[%d;%dHGAME OVER: Score = %d\033[0m", rows / 2, cols / 2 - 10, s->score);
    fflush(stdout);
    _getch();
    printf("\033[2J");
    fflush(stdout);
}

void init_ball(ball *b)
{
    b->flicker = 0;
    b->bonus = 0;
    b->x = 0;
    b->y = 0;
}

void welcome_screen(int rows, int cols)
{
    printf("\033[2J");
    for (int i = 0; i < rows; i++)
    {
        printf("\033[43m%*s", cols, " ");
        printf("\033[0m\n");
    }
    int indx = cols / 2, indy = rows / 2;
    printf("\033[%d;0H\033[1;31mS N A K E\033[0m", indy - 1);
    printf("\033[%d;0H\033[31mby", indy);
    printf("\033[%d;0H\033[3;31mJyotismoyKalita", indy + 1);
    printf("\033[%d;0H\033[23;31mW,A,S,D - Controls | q - quit", indy + 2);
    printf("\033[%d;0H\033[31mBonus Ball: Invincible for 5 non-linear collisions", indy + 3);
    fflush(stdout);
    _getch();
    printf("\033[0m\033[0;0H\033[2J");
    fflush(stdout);
}

int main(int argc, char *argv[])
{
    srand(time(0));
    enable_raw_mode();
    atexit(disable_raw_mode);
    int rows, cols;
    if (argc <= 1)
    {
        get_terminal_size(&rows, &cols);
    }
    else
    {
        rows = atoi(argv[1]);
        cols = atoi(argv[2]);
    }
    if (rows < 20)
        rows = 20;
    if (cols < 55)
        cols = 55;

    int delay = 100000;
    int exit = 1;
    char *buffer, *temp_buffer;
    float difficulty = 0;
    snake s;
    ball b;
    printf("\033[?25l\033[2J\033[0;0H");
    welcome_screen(rows, cols);
    init_buffer(&buffer, &temp_buffer, rows, cols);
    init_snake(&s, rows, cols);
    init_ball(&b);
    generate_ball(temp_buffer, &b, rows, cols);
    _getch();
    while (exit)
    {
        exit = handle_input(&s);
        increase_snake(&s, rows, cols);
        decrease_snake(&s, rows, cols, temp_buffer);
        if (s.x == b.x && s.y == b.y)
        {
            if (b.bonus)
            {
                s.score += 100;
                for (int i = 0; i < 5; i++)
                    increase_snake(&s, rows, cols);
                delay += 6000;
                difficulty -= 0.6;
                s.invincible = 1;
                s.useCount = 5;
            }
            else
            {
                s.score += 50;
                increase_snake(&s, rows, cols);
                if (delay > 30000)
                {
                    delay -= 3000;
                    difficulty += 0.3;
                }
            }
            generate_ball(temp_buffer, &b, rows, cols);
        }
        if (temp_buffer[s.arr[s.rear]] == 'x' || temp_buffer[s.arr[s.rear]] == 'z')
        {
            if (s.invincible)
            {
                if (s.useCount > 1)
                {
                    s.useCount--;
                }
                else
                {
                    s.useCount = 0;
                    s.invincible = 0;
                }
            }
            else
            {
                game_over(rows, cols, &s);
                break;
            }
        }
        update_buffer(temp_buffer, &s, &b, rows, cols);
        display_buffer(buffer, temp_buffer, rows, cols, s.score, difficulty, &s);
        if (s.dir == 1 || s.dir == 4)
            usleep(delay);
        else
            usleep(delay - 15000);
    }
    printf("\033[?25h\033[0m");
    destroy_buffer(buffer, temp_buffer);
    destroy_snake(&s);
    return 0;
}