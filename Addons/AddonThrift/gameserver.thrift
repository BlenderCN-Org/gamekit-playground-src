namespace java gs 
namespace cpp gs
namespace java org.tt.gamedb.admin.shared

const i32 SERVER_VERSION = 1
const i32 MINIMAL_CLIENT_VERSION = 1

const i32 os_all = 0;
const i32 os_ios=1,
const i32 os_android=2,
const i32 os_bb=3,
const i32 os_windows=4,
const i32 os_mac=5,
const i32 os_linux=6, 
const i32 os_mobile=7, 
const i32 os_desktop=8

struct TUser {
  1: string username,
  2: string password,
  3: string email,
  10: optional binary data
}

struct TScore {
  1: double score = 0,
  2: i64 time = 0,
  3: optional binary data,
  4: string username,
  5: string gamename,
  6: i32 mode,
  7: i32 os
}

struct TGame {
  1: string name,
  2: optional binary data
} 

struct TMode {
	1: i32 modeNr
}

struct TRegInfo {
1: i64 register_date,
2: i32 amount_logins
}

enum TAddUserResultCode {
	ok,
	username_exists,
	email_exists,
	not_allowed_operation
}

enum TAddScoreResultCode {
	nothing_special=0,
	player_highscore=1,
	current_highscore=2,
	alltime_highscore=3
}

struct TAddScoreResponse {
1:	i32 pos,
2:  TAddScoreResultCode result_code,
3:  i32 os
}

enum TLoginResultCode {
	ok=0,
	no_game=1,
	player_not_allowed_to_register_for_game=2,
	player_password_invalid=3,
	player_suspended=4,
	no_salt_requested=5
}

struct TLoginResponse {
1: TLoginResultCode result_code,
2: optional TUser user
}

enum TGetUserResultCode {
	ok,
	unknown
}

struct TGetUserResponse {
1: TGetUserResultCode result_code,
2: optional TUser user
}



enum ErrorCode {
	no_user,
	no_game,
	unknown_mode,
	no_os
}

exception TErrorException {
  1: ErrorCode error_code,
  2: optional string error_msg
}

enum TUserDataResponse {
	ok,
	email_used,
	username_used,
	both
}
enum TSetGameResult {
	ok,
	unknown,
	client_not_supported
}

struct TSessionCheck {
1: bool hasGame,
2: bool hasPlayer
}

struct TSigninToken {
1: string tokenName,
2: string tokenHash
}

enum TAutoLoginResultCode {
	ok,invalid,wrong_meta
}

struct TAutoLoginResponse {
	1: TAutoLoginResultCode resultCode 
	2: optional string tokenName,
	3: optional string tokenHash,
	4: optional TUser user
}


struct TBeforeLevelData {
	1: list<TScore> top,
	2: list<TScore>	around_player,
	3: i32 around_player_offset,
	4: i32 player_pos,
	5: TScore player_score
}

service TGameServerService {
  
  void ping(),

  TSetGameResult 	set_game(1:string gameid, 2:i32 os, 3: i32 version),
  void 				create_game(1: string name,2: i32 modeAmount,3: bool ascOrdering),
  void 				create_modes(1: string gameid,2:i32 amount, 3: bool ascOrdering),
  set<TGame> 		get_all_games(),
  TGame				get_game(1: string name) throws (1: TErrorException e),
  void              set_mode_order_ascending(1:i32 mode, 2:bool asc) throws (1: TErrorException e),	

  TAddUserResultCode 	add_user(1:TUser u) ,
  TGetUserResponse 		get_user_by_username(1:string username),

  set<TUser> 		get_user_list(),
  string 			request_logintoken();
  TLoginResponse 	login_user(1:string username,2: string password),
  TSigninToken      request_signintoken(1: string meta) throws (1:TErrorException e), 
  TAutoLoginResponse login_by_token(1:string tokenUser, 2: string tokenHash,3: string meta),
  TUserDataResponse check_userdata(1:string username, 2: string email),
  TGetUserResponse 	get_login_user(),
 
  TSessionCheck 	check_session(),
 

  list<TAddScoreResponse> add_score(1:i32 mode,2:double score) throws (1: TErrorException e),
  list<TScore> 		get_player_scores(1:i32 mode,2:i32 amount, 3:i32 offset) throws (1: TErrorException e),
  list<TScore> 		get_highscores(1:i32 mode,2:i32 amount, 3:i32 offset)  throws (1: TErrorException e),
  list<TScore>		get_player_highscores() throws (1:TErrorException e),
  TScore            get_player_highscore(1:i32 mode) throws (1: TErrorException e),
  i32 				get_player_pos(1:i32 mode) throws (1: TErrorException e),
  i32 				get_pos_by_score(1:i32 mode, 2:double score) throws (1: TErrorException e),
  TBeforeLevelData  get_before_level_data(1:i32 mode,2:i32 top_amount,3:i32 me_amount) throws (1: TErrorException e)


}

