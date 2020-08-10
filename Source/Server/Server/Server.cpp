// Server.cpp : Defines the entry point for the console application.
#include "stdafx.h"
#include "Server.h"
#include "afxsock.h"
#include "math.h"
#include <iostream>
#include <string>
#include <string.h>
#include <fstream>
#include <Windows.h>
using namespace std;
#ifdef _DEBUG
#define new DEBUG_NEW
#endif
// The one and only application object
CWinApp theApp;
//Definition struct Ticket
struct VEXE {
	char ID[255];
	char Ten[255];
	char LoaiVe[255];
	int SoLuong;
	int GiaTien;
};
//Definition struct Request Ticket
struct RequestTicket {
	char ID[255];
	char Ten[255];
	char LoaiVe[255];
	int SoLuong;
};
struct ResponseRequest {
	int errCode;
	int slVeConLai;
	int giaTien;
};
// chuyen char[] sang int
void String_convert_int(char s[], int &sl) {
	int res = 0;
	for (int i = 0; i < strlen(s); i++)
	{
		res = res * 10 + (s[i] - '0');
	}
	sl = res;
}
int StringInt(char s[]) {
	int res = 0;
	int sl;
	for (int i = 4; i < strlen(s); i++)
	{
		res = res * 10 + (s[i] - '0');
	}
	sl = res;
	return sl;
}
//Khởi tạo
VEXE *HangVeXe;
int so_luong = 0;
bool mutexLock = false;
//THREAD
DWORD WINAPI function_cal(LPVOID arg)
{
	SOCKET* hConnected = (SOCKET*)arg;
	CSocket mysock;
	//Chuyen ve lai CSocket
	mysock.Attach(*hConnected);
	//gửi số lượng vé
	mysock.Send(&so_luong, sizeof(so_luong), 0);
	//gửi vé
	for (int j = 0; j < so_luong; j++) {
		mysock.Send(&HangVeXe[j], sizeof HangVeXe[j], 0);
	}
	int t = 1;
	while (mutexLock)
	{
	}
	mutexLock = true;
	do {
		ResponseRequest ReRes;
		RequestTicket ReciveReq;
		//Khởi tạo giá trị
		ReRes.errCode = 0;
		ReRes.slVeConLai = 0;
		ReRes.giaTien = 0;
		//Nhận yêu cầu từ khách hàng
		mysock.Receive((RequestTicket*)&ReciveReq, sizeof RequestTicket, 0);

		for (int j = 0; j < so_luong; j++) {
			if ((strcmp(HangVeXe[j].ID, ReciveReq.ID) == 0) && (strcmp(HangVeXe[j].Ten, ReciveReq.Ten) == 0)) {
				ReRes.errCode += 1;
				if (strcmp(HangVeXe[j].LoaiVe, ReciveReq.LoaiVe) == 0) {
					ReRes.errCode += 2;
					ReRes.slVeConLai = HangVeXe[j].SoLuong;
					if (HangVeXe[j].SoLuong >= ReciveReq.SoLuong) {
						ReRes.errCode += 4;
						ReRes.giaTien = ReciveReq.SoLuong * HangVeXe[j].GiaTien;
						HangVeXe[j].SoLuong -= ReciveReq.SoLuong;
					}
				}
			}
		}
		//Gửi về Client mã lỗi
		cout << "Error code: " << ReRes.errCode << "\n";
		mysock.Send(&ReRes, sizeof ReRes, 0);
		mysock.Receive((int*)&t, sizeof t, 0);
	} while (t == 1);
	mutexLock = false;
	// ĐÓNG KẾT NỐI
	delete hConnected;
	return 0;
}
// MAIN
int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int nRetCode = 0;
	HMODULE hModule = ::GetModuleHandle(NULL);
	if (hModule != NULL)
	{
		// initialize MFC and print and error on failure
		if (!AfxWinInit(hModule, NULL, ::GetCommandLine(), 0))
		{
			// TODO: change error code to suit your needs
			_tprintf(_T("Fatal Error: MFC initialization failed\n"));
			nRetCode = 1;
		}
		else
		{
			// TODO: code your application's behavior here.
			AfxSocketInit(NULL);
			CSocket server, s;
			DWORD threadID;
			HANDLE threadStatus;
			//Nap du lieu vao struct
			int count = 0;
			std::fstream fileInput("VeXe.txt", std::ios::in);
			// kIỂM TRA XEM FILE CÓ BỊ LỖI
			if (fileInput.fail()) {
				std::cout << "Failed to open this file!" << std::endl;
			}
			// lấy số lượng vé để khỏi tao struct Hang ve xe
			char temp1[255];
			fileInput.getline(temp1, 255);
			String_convert_int(temp1, so_luong);
			HangVeXe = new VEXE[so_luong];
			//Khởi tạo mảng
			//Đọc dữ liệu vô struct mặt hàng
			while (!fileInput.eof()) {
				char temp[255];
				fileInput.getline(temp, 255);
				strcpy(HangVeXe[count].ID, temp);
				fileInput.getline(temp, 255);
				strcpy(HangVeXe[count].Ten, temp);
				fileInput.getline(temp, 255);
				strcpy(HangVeXe[count].LoaiVe, temp);
				fileInput.getline(temp, 255);
				int xx;
				String_convert_int(temp, xx);
				HangVeXe[count].SoLuong = xx;
				fileInput.getline(temp, 255);
				String_convert_int(temp, xx);
				HangVeXe[count].GiaTien = xx;
				//tu tang so luong ve ve xe trong hang doi
				count++;
			}
			std::cout << std::endl;
			fileInput.close(); // ĐÓNG FILE NẠP

			server.Create(4567);
			do {
				printf("Server lang nghe ket noi tu client\n");
				server.Listen();
				server.Accept(s);
				//Khoi tao con tro Socket
				SOCKET* hConnected = new SOCKET();
				//Chuyển đỏi CSocket thanh Socket
				*hConnected = s.Detach();
				//Khoi tao thread tuong ung voi moi client Connect vao server.
				//Nhu vay moi client se doc lap nhau, khong phai cho doi tung client xu ly rieng
				threadStatus = CreateThread(NULL, 0, function_cal, hConnected, 0, &threadID);
			} while (1);
		}
	}
	else
	{
		// TODO: change error code to suit your needs
		_tprintf(_T("Fatal Error: GetModuleHandle failed\n"));
		nRetCode = 1;
	}
}