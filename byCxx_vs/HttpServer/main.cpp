#include <winsock2.h>
#include <Windows.h>
#include <WS2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <wchar.h>
#include <tchar.h>
#include <fstream>

using namespace std;

#define WIN32_LEAN_AND_MEAN
#pragma comment(lib, "Ws2_32.lib")

const int defaultBuffLen = 512;
const char* defaultPort = "12345";
const wstring webPath = _T("../webroot");

wstring s2ws(const string& s)
{
	int len;
	int slength = (int)s.length() + 1;
	len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0);
	wchar_t* buf = new wchar_t[len];
	MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len);
	wstring r(buf);
	delete[] buf;
	return r;
}

string ws2s(const wstring& str)
{
	char* pElementText;
	int   iTextLen;
	// wide char to multi char
	iTextLen = WideCharToMultiByte(CP_ACP,
		0, str.c_str(), -1, NULL, 0, NULL, NULL);
	pElementText = new char[iTextLen + 1];
	memset((void*)pElementText, 0, sizeof(char) * (iTextLen + 1));
	::WideCharToMultiByte(CP_ACP,
		0, str.c_str(),	-1,	pElementText, iTextLen, NULL, NULL);
	string strText;
	strText = pElementText;
	delete[] pElementText;
	return strText;
}

void split(const string s, char c, vector<string> &elems)
{
	stringstream ss(s);
	string item;
	while (getline(ss, item, c))
	{
		if (item.length() > 0)
		{
			elems.push_back(item);
		}
	}
}

void wsplit(const wstring s, wchar_t c, vector<wstring> &elems)
{
	wstringstream ss(s);
	wstring item;
	while (getline(ss, item, c))
	{
		if (item.length() > 0)
		{
			elems.push_back(item);
		}
	}
}

int main()
{
	WSADATA wsaData;
	int iResult;

	SOCKET listenSocket = INVALID_SOCKET;
	SOCKET clientSocket = INVALID_SOCKET;

	struct addrinfo* result = NULL;
	struct addrinfo hints;

	int iSearchRult;
	char recvBuf[defaultBuffLen];
	int recvBufLen = defaultBuffLen;

	cout << "\tSERVER" << endl;

	int errorExit = 0;
	int isFirst = 1;
	// Main loop
	while (true)
	{
		// If the last one connection is an error exit
		if (errorExit || isFirst)
		{
			iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
			errorExit = 0;
			if (iResult != 0)
			{
				cout << "WSAStartup failed with error: " << iResult << endl;
				return 0;
			}

			ZeroMemory(&hints, sizeof(hints));
			hints.ai_family = AF_INET;
			hints.ai_socktype = SOCK_STREAM;
			hints.ai_protocol = IPPROTO_TCP;
			hints.ai_flags = AI_PASSIVE;

			// Resolve and binding the server address and port
			iResult = getaddrinfo(NULL, defaultPort, &hints, &result);
			if (iResult != 0)
			{
				cout << "getaddrinfo failed with error: " << iResult << endl;
				WSACleanup();
				return 0;
			}

			// Create a Socket for connection to server
			listenSocket = socket(
				result->ai_family,
				result->ai_socktype,
				result->ai_protocol);
			if (listenSocket == INVALID_SOCKET)
			{
				cout << "socket failed with error: " << WSAGetLastError() << endl;
				freeaddrinfo(result);
				WSACleanup();
				return 0;
			}

			iResult = bind(
				listenSocket,
				result->ai_addr,
				(int)result->ai_addrlen);
			if (iResult == SOCKET_ERROR)
			{
				cout << "bind failed with error: " << WSAGetLastError() << endl;
				closesocket(listenSocket);
				WSACleanup();
				return 0;
			}

			freeaddrinfo(result);
			// OK, let's listening sockets

			iResult = listen(listenSocket, SOMAXCONN);
			if (iResult == SOCKET_ERROR)
			{
				cout << "listen failed with error: " << WSAGetLastError() << endl;
				closesocket(listenSocket);
				WSACleanup();
				errorExit = 1;
				continue;
			}
		}
		// Accept a client socket
		isFirst = 0;
		cout << "Waitting for connection..." << endl;
		clientSocket = accept(listenSocket, NULL, NULL);
		if (clientSocket == INVALID_SOCKET)
		{
			cout << "accept failed with error: " << WSAGetLastError() << endl;
			closesocket(listenSocket);
			WSACleanup();
			errorExit = 1;
			continue;
		}
		cout << "Client " << clientSocket << " has been connected!" << endl;
		cout << "*************************************************" << endl;
		// No longer need server socket --TODO
		closesocket(listenSocket);

		// receive until one peer shuts down the connection
		do
		{
			memset(recvBuf, '\0', recvBufLen);
			iResult = recv(clientSocket, recvBuf, recvBufLen, 0);
			if (iResult > 0)
			{
				cout << iResult << " bytes received" << endl;
				cout << "Client: " << recvBuf << endl;

				// Parse the recvBuf and respond something
				vector<string> elems;
				split(recvBuf, ' ', elems);
				if (elems.size() > 1)
				{
					
					wstring wPostDir = s2ws(elems[1]);
					wcout << "post path: " << wPostDir << endl;
					wstring wPreDir = _T("");
					TCHAR prePath[MAX_PATH];
					GetModuleFileName(NULL, prePath, MAX_PATH);
					wcout << "pre path: " << prePath << endl;
					vector<wstring> welems;
					wsplit(prePath, _T('\\'), welems);
					for (int i = 0; i < welems.size() - 1; i++)
					{
						wPreDir += welems[i];
						wPreDir += _T("\\");
					}
					wstring wWebSrcDir = wPreDir + webPath + wPostDir;
					wcout << wWebSrcDir << endl;
					string htmlSrcDir = ws2s(wWebSrcDir);
					cout <<"html:  "<< htmlSrcDir << endl;
					ifstream httpFile;
					httpFile.open(htmlSrcDir);
					if (httpFile.is_open())
					{
						string outputInfo = "";
						while (!httpFile.eof())
						{
							char outputBuf[1024];
							httpFile.getline(outputBuf, 1024);
							int len = strlen(outputBuf);
							len = len >= 1023 ? 1023 : len;
							outputBuf[len] = 0;
							outputInfo += outputBuf;
						}
						
						iSearchRult = send(clientSocket, outputInfo.c_str(), outputInfo.length(), 0);
						if (iSearchRult == SOCKET_ERROR)
						{
							cout << "send failed with error: " << WSAGetLastError() << endl;
							closesocket(listenSocket);
							WSACleanup();
							errorExit = 1;
							continue;
						}
						httpFile.close();
					}
					else
					{
						cout << "File cannot open!" << endl;
						string errorInfo = "HTTP/1.1 404 File Not Found\r\nContent-Type: text/html\r\n \
								Content-Length: 23\r\n\r\n<h1>File Not Found</h1>";
						iSearchRult = send(clientSocket, errorInfo.c_str(), errorInfo.length(), 0);
						if (iSearchRult == SOCKET_ERROR)
						{
							cout << "send failed with error: " << WSAGetLastError() << endl;
							closesocket(listenSocket);
							WSACleanup();
							errorExit = 1;
							continue;
						}
					}
				}

				//string callBack = "Server have received: ";
				//callBack += recvBuf;
				//iSearchRult = send(clientSocket, callBack.c_str(), callBack.length(), 0);
				//if (iSearchRult == SOCKET_ERROR)
				//{
				//	cout << "send failed with error: " << WSAGetLastError() << endl;
				//	closesocket(listenSocket);
				//	WSACleanup();
				//	errorExit = 1;
				//	continue;
				//}
				cout << "-------------------------" << endl;
			}
			else if (iResult == 0)
			{
				cout << "Connection with " << clientSocket << " closed." << endl << endl;
				break;
			}
			else
			{
				cout << "Connection with " << clientSocket << " closed." << endl << endl;
				errorExit = 1;
				/*closesocket(listenSocket);
				WSACleanup();*/
				break;
			}

		} while (iResult > 0);

		if (!errorExit)
		{
			iResult = shutdown(clientSocket, SD_SEND);
			if (iResult == SOCKET_ERROR)
			{
				cout << "shutdown failed with error: " << WSAGetLastError() << endl;
				closesocket(clientSocket);
				WSACleanup();
				continue;
			}
		}
	}

	// Clean up
	closesocket(clientSocket);
	WSACleanup();

	return 0;
}
