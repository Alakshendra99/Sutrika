#include <windows.h>
#include <fstream>
#include <chrono>
#include <stdio.h>
#include <conio.h>
#include <iomanip>
#include <iostream>
#include "PCANBasic.h"
#include "DriverPCAN.hpp"

using namespace std;


void ClearScreen (void) {
  HANDLE ConsoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
  CONSOLE_SCREEN_BUFFER_INFO ConsoleBuffer;
  GetConsoleScreenBufferInfo(ConsoleHandle, &ConsoleBuffer);
  WORD DefaultSettings = ConsoleBuffer.wAttributes;
  SetConsoleTextAttribute(ConsoleHandle, DefaultSettings);
  DWORD ConsoleSize = ConsoleBuffer.dwSize.X * ConsoleBuffer.dwSize.Y;
  COORD Origin = {0, 0};
  DWORD CharsWritten;
  FillConsoleOutputCharacter(ConsoleHandle, ' ', ConsoleSize, Origin, &CharsWritten);
  FillConsoleOutputAttribute(ConsoleHandle, DefaultSettings, ConsoleSize, Origin, &CharsWritten);
  SetConsoleCursorPosition(ConsoleHandle, Origin);
}

enum CONSOLECOLOURS {
      CONSOLECOLOUR_BLACK = 0, CONSOLECOLOUR_BLUE = 1, CONSOLECOLOUR_GREEN = 2,
      CONSOLECOLOUR_AQUA = 3, CONSOLECOLOUR_RED = 4, CONSOLECOLOUR_PURPLE = 5,
      CONSOLECOLOUR_YELLOW = 6, CONSOLECOLOUR_WHITE = 7, CONSOLECOLOUR_GRAY = 8,
      CONSOLECOLOUR_LIGHTBLUE = 9, CONSOLECOLOUR_LIGHTGREEN = 10, CONSOLECOLOUR_LIGHTAQUA = 11,
      CONSOLECOLOUR_LIGHTRED = 12, CONSOLECOLOUR_LIGHTPURPLE = 13, CONSOLECOLOUR_LIGHTYELLOW = 14,
      CONSOLECOLOUR_BRIGHTWHITE = 15,
};

void ConsoleColor (CONSOLECOLOURS Colour = CONSOLECOLOUR_WHITE, CONSOLECOLOURS Background = CONSOLECOLOUR_BLACK) {
  HANDLE ConsoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
  int SafeColour = (uint8_t)(Colour & 0x0F);
  int SafeBackground = (uint8_t)(Background & 0x0F);
  int ConsoleColour = (SafeColour) | (SafeBackground << 4);
  SetConsoleTextAttribute(ConsoleHandle, ConsoleColour);
}

std::string ShowCAN (TPCANMsg MSG) {
  cout << "CAN : ";
  cout << std::hex << std::uppercase << std::setw(3) << std::setfill('0') << MSG.ID;
  cout << std::dec;
  cout << (( MSG.MSGTYPE == PCAN_MESSAGE_STANDARD ) ? " : STD : " : " : EXT : ");
  cout << (int)MSG.LEN;
  cout << " : ";
  for (int I = 0; I < MSG.LEN; I++) {
    cout << " ";
    cout << std::hex << std::uppercase << std::setw(2) << std::setfill('0') << (int)MSG.DATA[I];
  }
  cout << std::dec;
}

int main (void) {
  DriverPCAN PCAN;
  PCAN.Initialize();
  PCAN.Filter(0x096, 0x096, PCAN_STD);
  PCAN_Message MSG;
  ClearScreen();
  cout << "\nLogs Via CAN\n- - -\n\n";
  while (1) {
    if (PCAN.Read(MSG) == PCAN_ERROR_OK) {
      for (uint8_t I = 0; I < MSG.LEN; I++) {
        cout << (char)(MSG.DATA[I]);
      }
    }
  }
  cout << "\n\n\nExiting ...";
  cout << "\nExited\n\n";
  return 0;
}
