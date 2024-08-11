#include <SDL2/SDL.h>
#include <stdbool.h>

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;
const int PADDLE_WIDTH = 10;
const int PADDLE_HEIGHT = 100;
const int BALL_SIZE = 10;
const int PADDLE_SPEED = 10;
const int BALL_SPEED = 5;

unsigned int vollyCount = 0;

typedef struct
{
  int x, y, w, h;
  int dx, dy;
} Paddle;

typedef struct
{
  int x, y, w, h;
  int dx, dy;
} Ball;

void initialize(SDL_Window **window, SDL_Renderer **renderer)
{
  SDL_Init(SDL_INIT_VIDEO);
  *window = SDL_CreateWindow("Pong", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
  *renderer = SDL_CreateRenderer(*window, -1, SDL_RENDERER_ACCELERATED);
}

void handle_input(bool *running, Paddle *left_paddle, Paddle *right_paddle)
{
  SDL_Event event;
  while (SDL_PollEvent(&event))
  {
    if (event.type == SDL_QUIT)
    {
      *running = false;
    }
  }
  const Uint8 *state = SDL_GetKeyboardState(NULL);
  if (state[SDL_SCANCODE_W] && left_paddle->y > 0)
  {
    left_paddle->y -= PADDLE_SPEED;
  }
  if (state[SDL_SCANCODE_S] && left_paddle->y < WINDOW_HEIGHT - PADDLE_HEIGHT)
  {
    left_paddle->y += PADDLE_SPEED;
  }
  if (state[SDL_SCANCODE_UP] && right_paddle->y > 0)
  {
    right_paddle->y -= PADDLE_SPEED;
  }
  if (state[SDL_SCANCODE_DOWN] && right_paddle->y < WINDOW_HEIGHT - PADDLE_HEIGHT)
  {
    right_paddle->y += PADDLE_SPEED;
  }
}

void reset_ball(Ball *ball)
{
  ball->x = WINDOW_WIDTH / 2;
  ball->y = WINDOW_HEIGHT / 2;
  ball->dx = BALL_SPEED * (ball->dx < 0 ? 1 : -1);
  ball->dy = BALL_SPEED * (ball->dy < 0 ? 1 : -1);
  ball->dx = ball->dx > 0 ? BALL_SPEED : -BALL_SPEED;
  vollyCount = 0;
}

void update_ball(Ball *ball, Paddle *left_paddle, Paddle *right_paddle)
{
  ball->x += ball->dx;
  ball->y += ball->dy;
  // Check for collision with the top and bottom window boundaries
  if (ball->y <= 0 || ball->y >= WINDOW_HEIGHT - BALL_SIZE)
  {
    ball->dy = -ball->dy;
  }
  // Check for collision with the paddles
  if (SDL_HasIntersection((SDL_Rect *)ball, (SDL_Rect *)left_paddle) || SDL_HasIntersection((SDL_Rect *)ball, (SDL_Rect *)right_paddle))
  {
    ball->dx = -ball->dx;
    vollyCount += 1;
  }
  // Check if the ball has passed the paddles
  if (ball->x < 0 || ball->x > WINDOW_WIDTH)
  {
    reset_ball(ball);
  }
}

void update_ai(Paddle *right_paddle, Ball *ball)
{
  if (ball->dy > 0 && ball->y > right_paddle->y + right_paddle->h / 2 && right_paddle->y < WINDOW_HEIGHT - PADDLE_HEIGHT)
  {
    right_paddle->y += PADDLE_SPEED;
  }
  else if (ball->dy < 0 && ball->y < right_paddle->y + right_paddle->h / 2 && right_paddle->y > 0)
  {
    right_paddle->y -= PADDLE_SPEED;
  }
}

void render(SDL_Renderer *renderer, Paddle *left_paddle, Paddle *right_paddle, Ball *ball)
{
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
  SDL_RenderClear(renderer);

  SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
  SDL_RenderFillRect(renderer, (SDL_Rect *)left_paddle);
  SDL_RenderFillRect(renderer, (SDL_Rect *)right_paddle);
  SDL_RenderFillRect(renderer, (SDL_Rect *)ball);

  SDL_RenderPresent(renderer);
}

int main()
{
  SDL_Window *window = NULL;
  SDL_Renderer *renderer = NULL;
  initialize(&window, &renderer);

  Paddle left_paddle = {10, WINDOW_HEIGHT / 2 - PADDLE_HEIGHT / 2, PADDLE_WIDTH, PADDLE_HEIGHT};
  Paddle right_paddle = {WINDOW_WIDTH - 20, WINDOW_HEIGHT / 2 - PADDLE_HEIGHT / 2, PADDLE_WIDTH, PADDLE_HEIGHT};
  Ball ball = {WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2, BALL_SIZE, BALL_SIZE, BALL_SPEED, BALL_SPEED};

  bool running = true;
  while (running)
  {
    handle_input(&running, &left_paddle, &right_paddle);
    update_ball(&ball, &left_paddle, &right_paddle);
    update_ai(&right_paddle, &ball);
    render(renderer, &left_paddle, &right_paddle, &ball);
    ball.dx = ball.dx > 0 ? BALL_SPEED + vollyCount : -BALL_SPEED - vollyCount;
    SDL_Delay(16); // ~60 FPS
  }

  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
  return 0;
}
