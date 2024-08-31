#include <Windows.h>
#include <iostream>
#include <string>
#pragma comment(lib,"winmm.lib")
using namespace std;

int main()
{
    MCI_OPEN_PARMS mciOpen;
    MCI_STATUS_PARMS mciStatus;
    TCHAR szPath[MAX_PATH] = TEXT(R"(D:\Wormwaker\PROGRAMS\C¡¢C++\Amusement\DarkVoxel2\Audio\snd_eat.mp3)"); //mp3 file path
    memset(&mciOpen, 0, sizeof(mciOpen));
    memset(&mciStatus, 0, sizeof(mciStatus));

    //open the mp3 file
    mciOpen.lpstrDeviceType = TEXT("mpegvideo");
    mciOpen.lpstrElementName = szPath;
    if (mciSendCommand(NULL, MCI_OPEN, MCI_OPEN_TYPE | MCI_OPEN_ELEMENT, (DWORD)&mciOpen))
    {
        cout << "Error in opening file\n";
        return 0;
    }

    //get the length of the mp3 file in milliseconds
    mciStatus.dwItem = MCI_STATUS_LENGTH;
    if (mciSendCommand(mciOpen.wDeviceID, MCI_STATUS, MCI_STATUS_ITEM, (DWORD)&mciStatus))
    {
        cout << "Error getting file length\n";
        return 0;
    }

    //print the length of the mp3 file in milliseconds
    cout << "File length in milliseconds: " << mciStatus.dwReturn << endl;

    //close the mp3 file
    mciSendCommand(mciOpen.wDeviceID, MCI_CLOSE, 0, NULL);

    return 0;
}
