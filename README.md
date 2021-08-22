# re_samp_query_mechanism
SA:MP Query Mechanism with Linux support

```C++
auto sqm = c_sqm::singleton();
sqm->initialize("176.32.37.93", 7777);

auto srv_info = sqm->handle<s_server_info>(sqm->query(QUERYTYPE_SERVERINFO, 1));
// Now srv_info contains: is_locked, players, max_players, host_name, game_mode, language
```
