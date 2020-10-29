#include "SpdlogTest.h"
#include <iostream>

/* basic_logger_mt */

void SpdlogTest::test_basic_logger()
{
	auto basic_logger = spdlog::basic_logger_mt("basic_logger", "logs/_basic-log.txt");
	basic_logger->info("info basic_logger_mt.");
}

/* rotating_logger_mt */

void independentThread()
{
	std::shared_ptr<spdlog::logger> _logger = spdlog::get("rotating_logger");
	_logger->info("Starting concurrent thread.\n");
	for (int i = 0; i < 20; i++)
	{
		_logger->info("rotating_logger msg {}", i);
		std::this_thread::sleep_for(std::chrono::microseconds(20));
	}
	_logger->info("Exiting concurrent thread.\n");
}

void SpdlogTest::test_rotating_logger()
{
	//auto max_size = 1024;
	//auto max_files = 3;
	//std::shared_ptr<spdlog::logger> _logger = spdlog::rotating_logger_mt("rotating_logger", "logs/_rotating-log.txt", max_size, max_files);
	//_logger->set_pattern("[%D %H:%M:%S,%e] [%^%L%$] [thread %t] %v");

	_logger->info("Starting thread caller.\n");
	std::thread t1(independentThread);
	std::thread t2(independentThread);
	t1.detach();
	t2.detach();
	std::this_thread::sleep_for(std::chrono::seconds(3));
	_logger->info("Exiting thread caller.\n");
}