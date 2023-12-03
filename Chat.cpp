#include "Chat.h"

Chat::Chat(){
	greeting();
}

Chat::Chat(std::string active_user_login, std::string active_recipient_login, std::string active_user_name) :
	_active_user_login(active_user_login), _active_recipient_login(active_recipient_login), _active_user_name(active_user_name) {}


//----------- Создание сокета ---------------------------
void Chat::socket_file() {
	_socket_file_descriptor = socket(AF_INET, SOCK_STREAM, 0);
	if (_socket_file_descriptor == -1) {
		std::cout << " Не удалось создать сокет!" << std::endl;
		exit(1);
	}
}

//----------- Закрытие сокета ------------------------------
void Chat::close_socket() {
	close(_socket_file_descriptor);
	auto now = std::chrono::system_clock::now();
	std::time_t end_time = std::chrono::system_clock::to_time_t(now);
	std::cout << std::ctime(&end_time) << " сокет закрыт" << std::endl;

}

//------------ Настройка порта и привязка сокета -------------
void Chat::server_address() {
	// 
	_server_address.sin_addr.s_addr = htonl(INADDR_ANY);
	// Зададим номер порта для связи
	_server_address.sin_port = htons(PORT);
	// Используем IPv4
	_server_address.sin_family = AF_INET;
	// Привяжем сокет
	_bind_status = bind(_socket_file_descriptor, (struct sockaddr*)&_server_address,
		sizeof(_server_address));
	if (_bind_status == -1) {
		std::cout << " Не удалось выполнить привязку сокета!" << std::endl;
		exit(1);
	}
}

//------------ Постановка сервера на прием ----------------------
void Chat::connect() {
	_connection_status = listen(_socket_file_descriptor, 5);
	if (_connection_status == -1) {
		std::cout << " Сокет не может прослушивать новые подключения!" << "\n";
		exit(1);
	}
	else {
		std::cout << " Ожидание данных: " << "\n";
	}
	_length = sizeof(_client);
	_connection = accept(_socket_file_descriptor, (struct sockaddr*)&_client, &_length);
	if (_connection == -1) {
		std::cout << " Сервер не может принять данные от клиента!" << "\n";
		exit(1);
	}
}

//----------- Обмен данными ------------------------------
void Chat::exchange(const std::string& mess) {
	transmitting(mess);
	std::cout << " Ожидание данных \n";
	receiving_user();
}


//---------- Парсинг сообщения -------------------------------
std::string Chat::get_parsing(const std::string& mess, std::string word, int count) {
	int counter{ 0 };
	for (int i = 0; i < mess.size(); i++) {
		if (mess[i] != ' ' && counter == count) {
			word += mess[i];
			continue;
		}
		else if (mess[i] == ' ') counter++;
		if (counter > count) break;
	}
}

//-------------- Отправка данных -----------------------------------
void Chat::transmitting(const std::string& mess){
	memset(_message, 0, sizeof(_message));
	strcpy(_message, mess.c_str());
	ssize_t bytes = write(_connection, _message, sizeof(_message));
	transmitted(message());
}

//-------------- Прием данных от пользователе --------------------
void Chat::receiving_user() {
	memset(_message, 0, sizeof(_message));
	read(_connection, _message, sizeof(_message));
	accepted(message());
}

//-------------- Возврат первого символа сообщения --------------------
char Chat::message0() {
	return _message[0];
}

std::string Chat::message() {
	return std::string(_message);
}

void Chat::accepted(std::string mess){
	auto now = std::chrono::system_clock::now();
	std::time_t end_time = std::chrono::system_clock::to_time_t(now);
	std::cout << std::ctime(&end_time) << " принято: " << mess << std::endl;
}

void Chat::transmitted(std::string mess){
	auto now = std::chrono::system_clock::now();
	std::time_t end_time = std::chrono::system_clock::to_time_t(now);
	std::cout << std::ctime(&end_time) << " передано: " << mess << std::endl;
}

//---------------- Приветствие ------------------------------------------------------------
void Chat::greeting() {
	clean_console();
	std::cout << "\n          Сервер включён!\n\n";
}

//----------------- Прощание --------------------------------------------------------------
void Chat::farewell() {
	std::cout << " Пользователь отключился от сети.\n";
}

//--------------- Проверка логина -------------------------------------------------------
bool Chat::finding(std::string login) {
	if (_users.find(login) == _users.end()) return true;
	else return false;
}

//--------------- Проверка пароля -------------------------------------------------------
bool Chat::check_password(std::string password, std::string login) {
	if (_users.at(login).user_password() == password) return true;
	else return false;
}

//--------------- Вход и регистрация ----------------------------------------------------
void Chat::registration(char menu, bool* check_user) {
	User user;

	*check_user = false;
	// Вход в чат
	if (menu == '1') {
		exchange(" Введите логин(латинский алфавит, цифры, символы): ");
		user.get_user_login(message());
		exchange(" Введите пароль(латинский алфавит, цифры, символы): ");
		user.get_user_password(message());
		int counter = 0;

		if (!finding(user.user_login()) && check_password(user.user_password(), user.user_login())) {
			get_user(user.user_login(), _users.at(user.user_login()).user_name());
			transmitting("вход выполнен");
			*check_user = true;
		}
		else {
			transmitting("false");
			return;
		}

	}
	// регистрация нового пользователя
	else {
		*check_user = true;
		user.get_user_name(message());
		exchange(user.user_name());
		exchange(" Введите логин (латинский алфавит, цифры, символы): ");
		bool check_login;
		do {
			check_login = true;
			user.get_user_login(message());
			if (!finding(user.user_login())) {
				user.clear_login();
				exchange("false");
				check_login = false;
			}
		} while (!check_login);
		exchange(" логин принят");
		exchange( " Введите пароль (латинский алфавит, цифры, символы): ");
		user.get_user_password(message());

		_users.emplace(user.user_login(), user);
		get_user(user.user_login(), user.user_name());
		exchange("регистрация прошла успешно");
	}
}

//----------------- Регистрация общего чата ---------------------------------------
void Chat::reg_all_user() {
	User user;
	user.get_user_login("ALL_USERS");
	user.get_user_password("admin");
	user.get_user_name("общий чат");
	_users.emplace(user.user_login(), user);
}

//--------------- Возвращает логин активного пользователя --------------------------
std::string Chat::active_user_login() {
	return _active_user_login;
}

//--------------- Возвращает имя активного пользователя -----------------------------
std::string Chat::active_user_name() {
	return _active_user_name;
}

//--------------- Возвращает логин получателя ---------------------------------------
std::string Chat::active_recipient_login() {
	return _active_recipient_login;
}

//--------------- Активация пользователя --------------------------------------------
void Chat::get_user(std::string login, std::string name)
{
	_active_user_login = login;
	_active_user_name = name;
}

//--------------- Деактивация пользователя ------------------------------------------
void Chat::out_user() {
	_active_user_login = '\0';
	_active_user_name = '\0';

}

//--------------- Выбор получателя -------------------------------------------------
void Chat::get_recipient(char menu) {

	int counter = 0;
	if (menu == '2') _active_recipient_login = "ALL_USERS";
	else {
		int id{ 0 };
		do {
			receiving_user();
			id = stoi(message());
			if (id < 1 || id > _users.size()) {
				transmitting(" Не верный ID, повторите выбор: ");
			}
			else {
				transmitting(" ID принят");
				break;
			}

		} while (true);

		std::map<std::string, User>::iterator it = _users.begin();
		for (; it != _users.end(); it++) {
			counter++;
			if (counter == id) break;
		}

		_active_recipient_login = it->second.user_login();
	}
}

//------------------ Отправка сообщение об отсутствии других пользователей
void Chat::one_user(){
	transmitting(" Вы пока единственный пользователь.\n Дождитесь регистрации других пользователей.\n");
}

//------------- Определение количества пользователей --------------------------------
int Chat::sizeList() {
	return _users.size();
}

//------------- Создание и отправка сообщения ---------------------------------------
void Chat::send_message() {
	Message messages;
	std::string mess{};
	char menu{ ' ' };
	while (true) {
		exchange("запрос на действия");

		if (message() == "Esc") {
			transmitting("очистка");
			break;
		}
		else if (message() == "enter") {
			exchange("сообщение:");
			mess = message();
			messages.create_message(mess, _active_user_name, _active_user_login, _active_recipient_login);
			_messages.push_back(messages);
			std::cout << "\n Сообщение ";
			if (_active_recipient_login == "ALL_USERS") std::cout << " в общий чат\n";
			else {
				std::cout << " для ";
				_users.at(_active_recipient_login).showUserName();
			}
			transmitting(" отправлено");
			break;
		}
		else transmitting("хм, можно повторить ?");
	}
}

//----------------- Основная функция работы чата -------------------------------------------
void Chat::chat_work(){
	reg_all_user();

	socket_file();
	server_address();
	connect();
	exchange(" проверка связи");

	while(_work) {
		std::cout << " ожидание данных\n";
		receiving_user();
		_menu = message0();
		if (_menu == '3') {
			transmitting(" сервер завершил работу ");
			close_socket();
			farewell();
			exit(0);
		}
		// вход в аккаунт
		else if (_menu == '1') {
			if (sizeList() == 1) {
				transmitting("error");
				continue;
			}
			if (sizeList() == 2) {
				transmitting("error2");
				one_user();
				continue;
			}
			transmitting(" Вход");
			registration(_menu, &_check_user);
			if (_check_user == false) {
				continue;
			}
		}
		//регистрация нового пользователя
		else {
			exchange(" Регистрация:");
			registration(_menu, &_check_user);
		}
		// проверка кол-ва зарегистрированных
		if (sizeList() == 2) {
			transmitting("error2");
			one_user();
			continue;
		}
		else transmitting("список доступен");
		//работа аккаунта
		account_work();
	}
}

//--------------------- Функция работы аккаунта ------------------------------------
void Chat::account_work(){

	do {
		receiving_user();
		_menu = message0();
		_discussion = true;
		switch (_menu) {
		case '1': // личная беседа
			transmitting(" ID собеседника: ");
			get_recipient(_menu);
			send_message();
			break;
		case '2': // сообщение всем
			get_recipient(_menu);
			send_message();
			break;
		case '3': // выход
			transmitting(" Вы покинули аккаунт.");
			out_user();
			_discussion = false;
			break;

		default:
			break;
		}

	} while (_discussion);
}

//----------------- Очистка консоли -------------------------------------------------------
void Chat::clean_console() {
#ifdef _WIN32
	std::system("cls");  // Очистка экрана в системе Windows
#else
	std::system("clear");  // Очистка экрана в системах UNIX и Linux
#endif
}
