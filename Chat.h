#pragma once

#include<iostream>
#include<string>
#include<string.h>
#include<unistd.h>
#include<sys/socket.h>
#include<netinet/in.h>


#include <vector>
#include <map>
#include <cstdlib>
#include <thread>
#include <chrono>

#include "User.h"
#include "Message.h"
#include "Hash.h"

#define MESSAGE_LENGTH 1024 // Максимальный размер буфера для данных
#define PORT 7777 // Будем использовать этот номер порта

class Chat
{
public:
	Chat();
	Chat(std::string, std::string, std::string);

	void socket_file(); // Создание сокета
	void close_socket(); // Закрытие сокета
	void server_address(); // Настройка порта и привязка сокета
	void connect(); // Постановка сервера на приём
	void exchange(const std::string& mess); // Обмен данными
	std::string get_parsing(const std::string& mess, std::string word, int count); // Парсинг
	void transmitting(const std::string& mess); //отправка данных
	void receiving_user(); // Прием данных
	char message0();// Возврат первого символа сообщения
	std::string message();// Перевод сообщения в стринг
	void accepted(std::string mess);
	void transmitted(std::string mess);

	void greeting();
	void farewell();

	bool finding(std::string);
	bool check_password(std::string, std::string);
	void registration(char, bool*);
	void reg_all_user();

	std::string active_user_login();
	std::string active_user_name();
	std::string active_recipient_login();


	void get_user(std::string, std::string);
	void out_user();
	void get_recipient(char);

	void one_user();

	int sizeList();

	void send_message();

	void chat_work();
	void account_work();

	void clean_console();

private:

	int _socket_file_descriptor, _connection, _bind_status, _connection_status;
	struct sockaddr_in _server_address, _client;
	socklen_t _length;
	char _message[MESSAGE_LENGTH];


	std::map <std::string, User> _users;
	std::vector<Message> _messages;
	std::string _active_user_login;
	std::string _active_recipient_login;
	std::string _active_user_name;

	char _menu;

	bool _work{ true };
	bool _check_user{ false };
	bool _discussion{ true };
};
