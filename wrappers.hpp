#pragma once

#include <stdint.h>
#include <sstream>
#include <map>
#include <cp1251_utf8.h>

enum e_query_type: uint8_t
{
	QUERYTYPE_SERVERINFO = 0,
	QUERYTYPE_SERVERRULES,
	QUERYTYPE_PSEUDORANDOM,
	QUERYTYPE_PLAYERLIST,
	QUERYTYPE_DETAILPLAYERLIST,
	QUERYTYPE_RCON,

	QUERYTYPES_COUNT
};

static char e_type_to_rpc[QUERYTYPES_COUNT] = { 'i', 'r', 'p', 'c', 'd', 'x' };

/*
*	For fast read from sstream
*/
class c_fast_reader 
{
	std::stringstream *_ss;
public:
	void set_stream(std::stringstream &ss)
	{
		_ss = &ss;
	}

	template<typename T>
	T read_num(std::streampos pos = -1)
	{
		if (_ss->str().empty()) return 0;
		T result;
		if (pos != -1) {
			_ss->seekg(pos);
		}
		_ss->read(reinterpret_cast<char *>(&result), sizeof(T));
		return result;
	}

	template<typename T = uint32_t>
	std::string read_str(std::streampos pos = -1)
	{
		std::string result;
		T result_length = read_num<T>();
		result.resize(result_length);
		_ss->read(&result[0], result_length);
		return result;
	}

	template<typename T = uint32_t>
	std::string read_str_utf8(std::streampos pos = -1)
	{
		char s_cp1251[512], s_utf8[512];
		T result_length = read_num<T>();
		_ss->read(&s_cp1251[0], result_length);
		s_cp1251[result_length] = 0;
		CP1251_UTF8(s_utf8, s_cp1251, result_length);
		return std::string(s_utf8);
	}
};

struct s_query_base : public c_fast_reader
{
	e_query_type type;
};

struct s_server_info : public s_query_base
{
	uint8_t is_locked;
	uint16_t players;
	uint16_t max_players;
	std::string host_name;
	std::string game_mode;
	std::string language;

	s_server_info(std::stringstream &ss)
	{
		type = QUERYTYPE_SERVERINFO;

		ss.seekg(11);
		set_stream(ss);
		is_locked = read_num<uint8_t>();
		players = read_num<uint16_t>();
		max_players = read_num<uint16_t>();
		host_name = read_str_utf8();
		game_mode = read_str_utf8();
		language = read_str_utf8();
	}
};

struct s_pseudo_random : public s_query_base
{
	uint8_t first_ping, second_ping, third_ping, fourth_ping;

	s_pseudo_random(std::stringstream &ss)
	{
		type = QUERYTYPE_PSEUDORANDOM;
		
		ss.seekg(11);
		set_stream(ss);
		first_ping = read_num<uint8_t>();
		second_ping = read_num<uint8_t>();
		third_ping = read_num<uint8_t>();
		fourth_ping = read_num<uint8_t>();
	}
};

struct s_server_rules : public s_query_base
{
	std::map<std::string, std::string> rules;

	s_server_rules(std::stringstream &ss)
	{
		type = QUERYTYPE_SERVERRULES;
		
		ss.seekg(11);
		set_stream(ss);
		uint16_t rule_count = read_num<uint16_t>();
		for (uint16_t i = 0; i < rule_count; ++i) {
			std::string rule, value;
			rule = read_str<uint8_t>();
			value = read_str<uint8_t>();
			rules.emplace(rule, value);
		}
	}
};

struct s_player_list : public s_query_base
{
	std::map<std::string, int> players;

	s_player_list(std::stringstream &ss)
	{
		type = QUERYTYPE_PLAYERLIST;
		
		ss.seekg(11);
		set_stream(ss);
		uint16_t players_count = read_num<uint16_t>();

		if(players_count >= 100) return; // the server will not return anything, anyway

		for (uint16_t i = 0; i < players_count; ++i)
		{
			std::string name = read_str_utf8<uint8_t>();
			int score = read_num<int>();
			players.emplace(name, score);
		}
	}
};

struct s_detailed_player_list : public s_query_base
{
	struct s_player_detail
	{
		uint8_t id;
		int score;
		int ping;
	};

	s_player_detail* details_list;
	std::map<std::string, s_player_detail*> players;

	s_detailed_player_list(std::stringstream &ss)
	{
		type = QUERYTYPE_DETAILPLAYERLIST;
		
		ss.seekg(11);
		set_stream(ss);

		int currentDetail = 0;
		uint16_t players_count = read_num<uint16_t>();
		if(players_count >= 100) return; // the server will not return anything, anyway

		details_list = new s_player_detail[players_count];
		for (uint16_t i = 0; i < players_count; ++i)
		{
			std::string name = read_str_utf8<uint8_t>();
			details_list[currentDetail].id = read_num<uint8_t>();
			details_list[currentDetail].score = read_num<int>();
			details_list[currentDetail].ping = read_num<int>();
			players.emplace(name, &details_list[currentDetail]);
			++currentDetail;
		}
	}
	~s_detailed_player_list()
	{
		if(details_list)
		{
			delete[](details_list);
			details_list = 0;
		}
	}
};