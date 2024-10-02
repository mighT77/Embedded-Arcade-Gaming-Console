#include <TFT_eSPI.h>
#include "menu.h"
#include "pong.h"
#include "snake.h"

#include "game_state.h"

extern TFT_eSPI tft;
extern GameState currentGameState;
int menuIndex = 0;


void displayMenu()
{
  tft.fillScreen(TFT_BLACK);
  tft.setTextSize(2);
  const char *menuItems[] = {"Start Pong", "Start Snake", "Quit"};
  int maxMenuItems = 3;

  for (int i = 0; i < maxMenuItems; i++)
  {
    tft.setCursor(10, 30 + i * 30);
    if (i == menuIndex)
    {
      tft.setTextColor(TFT_GREEN, TFT_BLACK);
      tft.print("> ");
    }
    else
    {
      tft.setTextColor(TFT_WHITE, TFT_BLACK);
    }
    tft.println(menuItems[i]);
  }
}

void handleMenuInput(int button)
{
  if (button == 2)
  {
    menuIndex = (menuIndex > 0) ? menuIndex - 1 : 2;
    menuNeedsUpdate = true;
  }
  if (button == 1)
  {
    if (menuIndex == 0)
    {
      currentGameState = PONG;
      tft.fillScreen(TFT_BLACK);
      setupPong();
    }
    else if (menuIndex == 1)
    {
      currentGameState = SNAKE;
      tft.fillScreen(TFT_BLACK);
      setupSnake();
    }
    else if (menuIndex == 2)
    {
      Serial.println("Exiting...");
    }
  }
}
