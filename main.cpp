#include <iostream>
#include "sqm.hpp"

int main()
{
	auto sqm = c_sqm::singleton();
	sqm->initialize("176.32.37.93", 7777);

	auto srv_info = sqm->handle<s_server_info>(sqm->query(QUERYTYPE_SERVERINFO, 1));
	sqm->shutdown();
	
	std::cout << srv_info.players << " / " << srv_info.max_players << std::endl;
	return 1;
}
