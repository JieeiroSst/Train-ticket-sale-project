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
//nickname
struct NickNmae {
	char  *Ten;
};
//Definition struct Ticket
struct VEXE{
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
void String_convert_int(char s[], int &sl){
		int res = 0;
		for (int i = 0; i < strlen(s); i++)
		{
			res = res * 10 + (s[i] -'0');
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

// MAIN
int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int MuaDuocVe[20];
	int run;
	int nRetCode=0;
	//Nap du lieu vao struct
	char LINE[255];
	int count=0;
	VEXE *HangVeXe;
	string mathangx[3];
	int so_luong = 0;
	int mhx = 0;
	int _yes = 0;
	std::fstream fileInput("VeXe.txt", std::ios::in);
	// kIỂM TRA XEM FILE CÓ BỊ LỖI
	if (fileInput.fail()) {
		std::cout << "Failed to open this file!" << std::endl;
	}
	// lấy số lượng hàng để khỏi tao struct Hang ve xe
	char temp1[255];
	fileInput.getline(temp1, 255);
	String_convert_int(temp1, so_luong);
	HangVeXe = new VEXE[so_luong];//khoi dong mang
	// Đọc dữ liệu vô struct mặt hàng
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
	// initialize MFC and print and error on failure
	// initialize MFC and print and error on failure
	if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0)){
		// TODO: change error code to suit your needs
		_tprintf(_T("Fatal Error: MFC initialization failed\n"));
		nRetCode = 1;
	}else {
		// TODO: code your application's behavior here.
		CSocket server;
		unsigned int port = 1234;
		int i;
		AfxSocketInit(NULL);
		server.Create(port);
		server.Listen(5);
		//Nhap so luong client
		int num_client = 1;
		do {
			printf("\n Nhap so luong khach hang: ");
			scanf_s("%d", &num_client);
		} while (num_client < 1);
		//Khởi tạo mảng nickname
		NickNmae  * nickname;
		nickname = new NickNmae[num_client];
		printf("\n Dang lang nghe ket noi tu khach hang...\n");
		//Tao mang chua cac socket client
		CSocket * sockClients = new CSocket[num_client];
		int *money;
		money = new int[num_client];
		for (i = 0; i < num_client; i++) {
			server.Accept(sockClients[i]);
			printf("Da tiep nhan khach hang %d/%d\n", i + 1, num_client);
			//Bao cho client biet minh la client thu may
			sockClients[i].Send((char*)&i, sizeof(int), 0);
			// Nhận nick name từ client
			int MsgSize;
			char *temp;
			do {
				sockClients[i].Receive((char*)&MsgSize, sizeof(int), 0); // Neu nhan loi thi tra ve la SOCKET_ERROR.		
				temp = new char[MsgSize + 1];
				sockClients[i].Receive((char*)temp, MsgSize, 0);
				temp[MsgSize] = '\0';
				//Kiểm tre nick name có bị trùng không
				for (int j = 0; j < i; j++) {
					if (strcmp(nickname[j].Ten, temp) == 0)
					{
						MsgSize = -1;
						// gửi tin nhắn báo cho client biet nickname bị trùng
						sockClients[i].Send(&MsgSize, sizeof(MsgSize), 0);
					}
				}
				if (MsgSize != -1)
				{
					MsgSize = 0;
					sockClients[i].Send(&MsgSize, sizeof(MsgSize), 0);
					nickname[i].Ten = temp;
				}
			} while (MsgSize == -1);//lặp lại khi nick name bị trùng
		}

		// DANH SACH NICKNAME HỢP LỆ
		cout << "Danh sach cac nickname hop le:\n";
		for (int i = 0; i < num_client; i++)
		{
			cout << nickname[i].Ten << endl;
		}
		//Gửi vé xe cho các cilent

		for (int i = 0; i < num_client; i++)
		{
			//gửi số lượng vé
			sockClients[i].Send(&so_luong, sizeof(so_luong), 0);

			//gửi vé
			for (int j = 0; j < so_luong; j++) {
				sockClients[i].Send(&HangVeXe[j], sizeof HangVeXe[j], 0);
			}
		}

		for (int i = 0; i < num_client; i++) {
			int t = 1;
			do {
				ResponseRequest ReRes;
				RequestTicket ReciveReq;
				//Khởi tạo giá trị
				ReRes.errCode = 0;
				ReRes.slVeConLai = 0;
				ReRes.giaTien = 0;
				//Nhận yêu cầu từ khách hàng
				sockClients[i].Receive((RequestTicket*)&ReciveReq, sizeof RequestTicket, 0);
				for (int j = 0; j < so_luong; j++) {
					if (strcmp(HangVeXe[j].ID, ReciveReq.ID) != 1 && strcmp(HangVeXe[j].Ten, ReciveReq.Ten) != 1) {
						ReRes.errCode += 1;
						if (strcmp(HangVeXe[j].LoaiVe, ReciveReq.LoaiVe) != 1) {
							ReRes.errCode += 2;
							ReRes.slVeConLai = HangVeXe[j].SoLuong;
							if (HangVeXe[j].SoLuong >= ReciveReq.SoLuong) {
								ReRes.errCode += 4;
								ReRes.giaTien= ReciveReq.SoLuong * HangVeXe[j].GiaTien;
								HangVeXe[j].SoLuong -= ReciveReq.SoLuong;
							}
						}
					}
				}
					//Gửi về Client mã lỗi
					cout << ReRes.errCode;
					sockClients[i].Send(&ReRes, sizeof ReRes, 0);
				sockClients[i].Receive((int*)&t, sizeof t, 0);
			} while (t==1);
		}

		// ĐÓNG KẾT NỐI
		for (int i = 0; i < num_client; i++)
		{
			sockClients[i].ShutDown(2); //Ngat ca chieu Gui va Nhan
			sockClients[i].Close();
		}
		getchar();
		server.Close();
	}
	getchar();
	return nRetCode;
}
