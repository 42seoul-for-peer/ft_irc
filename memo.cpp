// struct sockaddr_in {
// 	sa_family_t	sin_family; // 주소 체계 (address family)
// 	uint16_t	sin_port; // 16비트 포트 번호
// 	struct		sin_addr; // 32비트 IP 주소
// 	char		sin_zero[8]; // 사용X
// };
// struct in_addr {
// 	in_addr_t	s_addr; // 32비트 IPv4 인터넷 주소
// };

// sockaddr_in
// sin_family: 프로토콜 체계마다 적용하는 주소체계에 차이가 있어 정보를 저장해야 함
// sin_port: 네트워크 바이트 순서로 저장된 PORT 번호
// sin_Addr: 네트워크 바이트 순서로 저장된 32비트 IP 주소
// sin_zero: 크기를 sockaddr 구조체의 크기와 일치 시키기 위해 사용되는 0으로 초기화 된 비트

// struct sockaddr {
// 	sa_family_t	sin_family;
// 	char		sa_data[14];
// };
// sock addr가 주소 정보를 담기에 불편해서 sockaddr_in 구조체가 등장하게 됨

// 네트워크 상으로 데이터를 전송할 때는 빅 엔디안 기준으로 변경해서 송수신함

// 바이트 순서의 변환(Endian Conversions)
// unsigned short htons(unsigned short);

