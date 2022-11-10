#include <windows.h>
#include "resource.h"

#define DEFAULT_STR_SIZE 100

//��������� �����
const int MAP_SIZE = 3;
HWND MapMatrix[MAP_SIZE][MAP_SIZE];
HBITMAP PIC_V, PIC_X, PIC_O;
short CELL_SIZE;

HWND TurnIs; //����� ��� ���
BOOL TurnIsFirstP = true;
int turn = 0;

BOOL CALLBACK DlgProc(HWND, UINT, WPARAM, LPARAM);

void PrintWhoseTurn();
bool CheckEndGame(COORD);
void ClearMap();

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInst, LPTSTR lpszCmdLine, int nCmdShow)
{
	return DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG), NULL, (DLGPROC)DlgProc);
}

BOOL CALLBACK DlgProc(HWND hWnd, UINT mess, WPARAM wParam, LPARAM lParam)
{
	switch (mess)
	{
	case WM_INITDIALOG:
		//�������� ����
		PIC_V = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_BITMAP_V));
		PIC_X = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_BITMAP_X));
		PIC_O = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_BITMAP_O));

		//���������� �������� ��������
		for (int x = 0; x < MAP_SIZE; ++x)
			for (int y = 0; y < MAP_SIZE; ++y)
			{
				MapMatrix[x][y] = GetDlgItem(hWnd, IDC_STATIC1 + x + y * MAP_SIZE);

				SendMessage(
					MapMatrix[x][y],
					STM_SETIMAGE,
					WPARAM(IMAGE_BITMAP),
					LPARAM(PIC_V)
				);
			}

		TurnIs = GetDlgItem(hWnd, IDC_TURNIS2); 

		//������ �������� �� ������ ������ ����
		RECT rect;
		GetClientRect(hWnd, &rect);
		CELL_SIZE = rect.right / MAP_SIZE;

		PrintWhoseTurn();

		return TRUE;

	case WM_LBUTTONDBLCLK:
	{
		short m_x = -1,
			  m_y = -1;

		//���������� �������� �� �
		for (int i = 0; i < MAP_SIZE; ++i)
			if (LOWORD(lParam) < CELL_SIZE * (i + 1))
			{
				m_x = i;
				break;
			}

		//���������� �������� �� y
		for (int i = 0; i < MAP_SIZE; ++i)
			if (HIWORD(lParam) < CELL_SIZE * (i + 1))
			{
				m_y = i;
				break;
			}

		//���� ���� ������ � ���������� ����������
		if (m_x > -1 && m_y > -1 &&
		HBITMAP(SendMessage(
			MapMatrix[m_x][m_y], 
			STM_GETIMAGE, 
			(WPARAM)IMAGE_BITMAP, 
			NULL)
		) == PIC_V)
		{
			SendMessage(
				MapMatrix[m_x][m_y],
				STM_SETIMAGE,
				WPARAM(IMAGE_BITMAP),
				LPARAM(TurnIsFirstP ? PIC_X : PIC_O)
			);

			//���������� �� ������
			if(CheckEndGame({ m_x, m_y }))
			{
				TCHAR WinerStr[DEFAULT_STR_SIZE] = TEXT("����������: ");
				wcscat_s(WinerStr, TurnIsFirstP ? TEXT("��������") : TEXT("������"));

				if (MessageBox(hWnd, WinerStr, TEXT("������� ����������!"), MB_YESNO) == IDYES) ClearMap();
				else { EndDialog(hWnd, NULL); return TRUE; } //�����
			}
			else if (turn >= MAP_SIZE * MAP_SIZE - 1)
			{
				if (MessageBox(hWnd, TEXT("�����"), TEXT("���������� �� �������!"), MB_YESNO) == IDYES) ClearMap();
				else { EndDialog(hWnd, NULL); return TRUE; } //�����
			}
			
			//����� ����
			TurnIsFirstP = !TurnIsFirstP;
			PrintWhoseTurn();
			turn++;
		}
	}
	return TRUE;

	case WM_CLOSE:
		EndDialog(hWnd, NULL);
		return TRUE;
	}
	
	return FALSE;
}

void PrintWhoseTurn()
{
	if (TurnIsFirstP) SendMessage(TurnIs, WM_SETTEXT, NULL, LPARAM(TEXT("��������")));
	else SendMessage(TurnIs, WM_SETTEXT, NULL, LPARAM(TEXT("������")));
}

bool CheckEndGame(COORD ClickUp)
{
	HBITMAP PLAYER_CH = TurnIsFirstP ? PIC_X : PIC_O;

	//�������� �� ����������� �� �����
	bool WinFlag = true;
	for (int i = 0; i < MAP_SIZE; ++i)
		if (HBITMAP(SendMessage(
			MapMatrix[i][ClickUp.Y],
			STM_GETIMAGE,
			(WPARAM)IMAGE_BITMAP,
			NULL)
		) != PLAYER_CH)
		{
			WinFlag = false;
			break;
		}

	if (WinFlag == true) 
		return true;
	WinFlag = true;

	//�������� �� ��������� �� �����
	for (int i = 0; i < MAP_SIZE; ++i)
		if (HBITMAP(SendMessage(
			MapMatrix[ClickUp.X][i],
			STM_GETIMAGE,
			(WPARAM)IMAGE_BITMAP,
			NULL)
		) != PLAYER_CH)
		{
			WinFlag = false;
			break;
		}

	if (WinFlag == true) 
		return true;
	WinFlag = true;

	//�������� ��������� (����-������)
	for (int i = 0; i < MAP_SIZE; ++i)
		if (HBITMAP(SendMessage(
			MapMatrix[i][i],
			STM_GETIMAGE,
			(WPARAM)IMAGE_BITMAP,
			NULL)
		) != PLAYER_CH)
		{
			WinFlag = false;
			break;
		}

	if (WinFlag == true)
		return true;
	WinFlag = true;

	//�������� ��������� (�����-�����)
	for (int i = 0; i < MAP_SIZE; ++i)
		if (HBITMAP(SendMessage(
			MapMatrix[MAP_SIZE - i - 1][i],
			STM_GETIMAGE,
			(WPARAM)IMAGE_BITMAP,
			NULL)
		) != PLAYER_CH)
		{
			WinFlag = false;
			break;
		}

	if (WinFlag == true)
		return true;

	return false;
}

void ClearMap()
{
	turn = -1;
	TurnIsFirstP = false;
	for (int x = 0; x < MAP_SIZE; ++x)
		for (int y = 0; y < MAP_SIZE; ++y)
		{
			SendMessage(
				MapMatrix[x][y],
				STM_SETIMAGE,
				WPARAM(IMAGE_BITMAP),
				LPARAM(PIC_V)
			);
		}
}