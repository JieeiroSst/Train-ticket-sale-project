// Client.cpp : Defines the entry point for the console application.
#include "stdafx.h"
#include "Client.h"
#include "afxsock.h"
#include "math.h"
#include <iostream>
#include <string>
#include <string.h>
#include <fstream>
#include <Windows.h>
#ifdef _DEBUG
#define new DEBUG_NEW
#endif
// The one and only application object
CWinApp theApp;
using namespace std;
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
void String_to_upper(char s[]) {
	for (int i = 0; i < strlen(s); i++)
	{
		s[i]=toupper(s[i]);
	}
}
int test;
int _tmain(int argc, TCHAR* argv[], TCHAR* envp[]) {
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
			CSocket client;

			client.Create();
			client.Connect(_T("127.0.0.1"), 4567);
			//Khai báo các biến
			int sovexe = 0;
			VEXE *HangVeXe;
			//Nhận số lượng vé xe
			client.Receive(&sovexe, sizeof(int), 0);
			//Khởi tạo mảng vé xe
			HangVeXe = new VEXE[sovexe];
			for (int j = 0; j < sovexe; j++) {
				client.Receive(&HangVeXe[j], sizeof HangVeXe[j], 0);
			}
			cout << "Danh sach ve xe" << endl;
			//In danh sách vé xe
			for (int i = 0; i < sovexe; i++)
			{
				cout << "< ID: " << HangVeXe[i].ID << " >\t";
				cout << "< Ten Ve Xe: " << HangVeXe[i].Ten << " >\t";
				cout << "< Loai Ve Xe: " << HangVeXe[i].LoaiVe << " >\t";
				cout << "< So luong ve: " << HangVeXe[i].SoLuong << " >\t";
				cout << "< Gia ve: " << HangVeXe[i].GiaTien << " >\n";
			}
			cout << "Vui long nhap thong tin ve can tim kiem \n";
			char select = 'y';
			do {
				RequestTicket Req;
				ResponseRequest ResFromServer;
				string MessageError;
				//Nhập vé cần tìm kiếm
				cout << "Nhap id ve ";
				gets_s(Req.ID);
				String_to_upper(Req.ID);
				cout << "Nhap ten ve ";
				gets_s(Req.Ten);
				String_to_upper(Req.Ten);
				cout << "Nhap loai ve ";
				gets_s(Req.LoaiVe);
				String_to_upper(Req.LoaiVe);
				cout << "Nhap so luong ve ";
				cin >> Req.SoLuong;
				//Gửi yêu cầu truy vấn cho server
				client.Send(&Req, sizeof Req, 0);
				//Nhận phản hồi từ server
				client.Receive((ResponseRequest*)&ResFromServer, sizeof ResFromServer, 0);
				//In thông báo dựa trên errCode gửi về từ server
				switch (ResFromServer.errCode)
				{
				case 0:
				{
					MessageError = "Khong co ve can tim\n";
					break;
				}
				case 1:
				{
					MessageError = "Khong co loai ve nay\n";
					break;
				}
				case 3:
				{
					MessageError = "Vuot qua so ve con lai\n";
					break;
				}
				case 7:
				{
					MessageError = "Loai ve nay dang co sang. ";
					break;
				}
				}
				cout << MessageError;
				//In ra giá tiền hoặc số lượng vé còn lại
				if (ResFromServer.errCode == 7)
				{
					cout << "So tien can phai tra la: " << ResFromServer.giaTien;
				}
				else if (ResFromServer.errCode == 3)
				{
					cout << "So ve con lai la: " << ResFromServer.slVeConLai;
				}
				//Hỏi có tiếp tục mua vé không
				cout << "\nQuy khach co muon tiep tuc mua ve khong? (Y/N)";
				getchar();
				select = getchar();
				getchar();
				//Gửi cho server thông báo có tiếp tục hay không
				int t = 0;
				if (select == 89 || select == 121)
				{
					t = 1;
				}
				client.Send(&t, sizeof t, 0);
			} while (select == 89 || select == 121);
		}
	}
	else
	{
		// TODO: change error code to suit your needs
		_tprintf(_T("Fatal Error: GetModuleHandle failed\n"));
		nRetCode = 1;
	}
	return nRetCode;
}