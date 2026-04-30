#include "lib\DriverPCAN.hpp"
#include "lib\UDS.hpp"

int main(void) {
  cout << "\n\n\nPeak System's PCAN Testing";
  cout << "\n - Alakshendra Singh\n";
  ISO_UDS UDS;
  UDS.Start();
  // uint64_t Entry = UDS.MicroClock();
  // uint64_t Exit = UDS.MicroClock();

  // while ((Exit-Entry) < 10) {
  //   Exit = UDS.MicroClock();
  // }
  // cout << "\nTime (us) : " << Entry;
  // cout << "\nTime (us) : " << Exit;






  // DriverPCAN PCAN;
  // PCAN.Initialize();
  //   PCAN.MESSAGE.MSGTYPE = PCAN_MESSAGE_STANDARD;
  //   PCAN.MESSAGE.ID = 0x785;
  //   PCAN.MESSAGE.LEN = 8;
  //   PCAN.MESSAGE.DATA[0] = 0x03;
  //   PCAN.MESSAGE.DATA[1] = 0x22;
  //   PCAN.MESSAGE.DATA[2] = 0xF1;
  //   PCAN.MESSAGE.DATA[3] = 0x95;
  //   PCAN.MESSAGE.DATA[4] = 0x00;
  //   PCAN.MESSAGE.DATA[5] = 0x00;
  //   PCAN.MESSAGE.DATA[6] = 0x00;
  //   PCAN.MESSAGE.DATA[7] = 0x00;

  

  // // cout << "\n\nSending CAN Signals . . .";
  // // for (int I = 0; I < 10; I++) {
  // //   PCAN.Write (PCAN.MESSAGE.ID, PCAN.MESSAGE.DATA);
  // //   Sleep(1000);
  // // }

  // PCAN.Filter (0x78D);
  // Sleep(1000);
  // PCAN.Write (PCAN.MESSAGE.ID, PCAN.MESSAGE.DATA);

  // cout << "\n\nListening Mode Started\nPress SPACE to Exit";
  // cout << "\nListening CAN Signals . . .\n";

  // do {
  //   TPCANStatus Status;
  //   TPCANMsg MSG;
  //   TPCANTimestamp Time;
  //   Status = PCAN.Read (MSG, Time);
  //   if (PCAN_ERROR_OK == Status) {
  //     PCAN.Show (MSG);
  //   }
  //   if (GetAsyncKeyState(VK_SPACE) & 0x8000) {
  //     break;
  //   }
  // } while (true);

  cout << "\n\nListening Mode Stopped\nPCAN in Idle State\n";
  getchar();
  return 0;
}
