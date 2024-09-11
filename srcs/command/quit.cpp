#include <iostream>
#include <set>
#include "Command.hpp"

/*
? 전달 메시지
* 유저에게 전달 -> "ERROR :Closing link: (root@127.0.0.1) [Quit: <message>]"
* 해당 유저가 속해있는 채널의 다른 유저들에게 전달 ":<target>!root@127.0.0.1 QUIT :Quit: <message>"
* <message>가 없거나 비어있을 경우, "leaving"이 자동으로 들어감

! 유의사항
	* rpl_no 없음
	* 인자 개수가 여러개가 들어와도 작동
	* user1이 QUIT 할 때, user1와 채널이 2개 이상 겹치는 다른 유저에게 메시지가 1번만 출력되도록 주의

todo->
	* client가 속해있던 채널에서 client 전부 제거
	* Server._clients에서 제거 (deleteClnt())
	* Client._connected 값 false로 변경 (setConnected(false))
	* Client 객체 자체는 나중에 Server에서 삭제하기 때문에, setConnected() 함수를 통해 false로 처리
*/
void Command::quit(Server& serv)
{
	std::string prefix;
	std::string msg;

	std::map< int, Client* > user_list = serv.getClients();
	std::map< int, Client* >::iterator it = user_list.begin();
	while (it != user_list.end())
	{
		if (it->second->getNickname() == _sender)
			break ;
		it++;
	}
	if (it == user_list.end())
	{
		//! 불가능한 상황... sender가 서버 유저리스트에 없다고?
		std::cout << "!!!!!!! quit.cpp에서 매우 치명적인 상황 발생1 !!!!!!!" << std::endl;
		return ;
	}
	Client* target = it->second;
	std::vector< std::string > clnt_chan_list = target->getCurrChannel();
	std::map< std::string, Channel* > serv_chan_list = serv.getChannels();
	//todo [Step1] target이 QUIT 했다는 알림을 받을 유저들을 탐색하여 저장
	//todo [Step2] 이와 동시에 해당 channel에서 client 제거
	std::set< std::string > related_clients;
	int joined_chan_size = clnt_chan_list.size();
	for (int i = 0; i < joined_chan_size; i++)
	{
		std::map< std::string, Channel* >::iterator it_chan = serv_chan_list.find(clnt_chan_list[i]);
		if (it_chan == serv_chan_list.end())
		{
			//! 이것도 불가능한 상황.. 무언가 단단히 잘못된 상황이라는 뜻
			std::cout << "!!!!!!! quit.cpp에서 매우 치명적인 상황 발생2 !!!!!!!" << std::endl;
			return ;
		}
		std::vector< std::pair< bool, Client* > > client_list = it_chan->second->getClients();
		int curr_clnt_size = client_list.size();
		for (int j = 0; j < curr_clnt_size; j++)
		{
			std::string tmp = client_list[j].second->getNickname();
			// 본인이 아닐 경우에만 추가
			if (tmp != _sender)
				related_clients.insert(tmp);

		}
		it_chan->second->deleteClient(*target);
	}
	//todo [Step3] server._clients에서 제거
	// serv.deleteClnt(target->getSockFd());
	//todo [Step4] Client 멤버 변수 _connected를 false로 변경
	target->setConnected(false);
	//todo [Step5] 최종적으로 메시지 전달
	std::string quit_msg = _appendRemaining();
	if (quit_msg.empty())
		quit_msg = "leaving";
	msg = "ERROR :Closing link: (" + target->getUsername() + "@localhost) [Quit: " + quit_msg + "]\n";
	setMsgs(_sender, msg);
	for (std::set< std::string >::iterator it = related_clients.begin(); it != related_clients.end(); it++)
	{
		prefix = serv.genPrefix(_sender, 0);
		msg = prefix + "QUIT :Quit: " + quit_msg + "\n";
		setMsgs(*it, msg);
	}
}