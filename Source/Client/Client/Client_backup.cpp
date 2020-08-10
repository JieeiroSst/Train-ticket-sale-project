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
using namespace std;

int _tmain(int argc, TCHAR* argv[], TCHAR* envp[]) {
	int nRetCode = 0;
	// initialize MFC and print and error on failure
	if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0)) {
		// TODO: change error code to suit your needs
		_tprintf(_T("Fatal Error: MFC initialization failed\n"));
		nRetCode = 1;
	}
	else {
		// TODO: code your application's behavior here.
		CSocket client;
		char sAdd[1000];
		unsigned int port = 1234; //Cung port voi server
		AfxSocketInit(NULL);
		//1. Tao socket
		client.Create();
		// Nhap dic chi IP cua server
		printf("\n Nhap dia chi IP cua client: ");
		gets_s(sAdd);
		if (client.Connect(CA2W(sAdd), port)) {
			printf("\nBan da da ket noi toi server thanh cong.");
			//Nhan tu server, cho biet day la client thu may
			int id;
			client.Receive((char*)&id, sizeof(id), 0);
			printf(" \nServer: Day la khac hang thu %d\n", id + 1);
			srand((unsigned)time(NULL));

			//ĐĂNG KÝ NICK NAME
			int MsgSize;
			do {
				cout << "Nhap nick nanme:(alainluu): ";
				char nickname[100];
				gets_s(nickname);
				MsgSize = strlen(nickname);
				// Gui di do dai thong diep de Server biet duoc do dai nay
				client.Send(&MsgSize, sizeof(MsgSize), 0);
				// Gui di thong diep voi do dai la MsgSize
				client.Send(nickname, MsgSize, 0);
				// Nhận con số báo thành công hay thất bại đến từ server
				client.Receive((char*)&MsgSize, sizeof(int), 0);
				if (MsgSize != -1)
				{
					cout << "Server: Nickname dang ki thanh cong.\n";
				}
				else
				{
					cout << "Nickname bi trung. Moi nhap nickname khac.\n";
				}
			} while (MsgSize == -1);
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
				cout << "< So luong ve: " << HangVeXe[i].SoLuong << " >\n";
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
				cout << "Nhap ten ve ";
				gets_s(Req.Ten);
				cout << "Nhap loai ve ";
				gets_s(Req.LoaiVe);
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
		else {
			printf("Khong connect duoc toi server....");
		}
	}
	getchar();
	return nRetCode;
}
