#pragma once
#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/sinks/rotating_file_sink.h"

/* https://github.com/gabime/spdlog */

class SpdlogTest {
private:
	std::shared_ptr<spdlog::logger> _logger;
public:
	SpdlogTest() {
		auto max_size = 1024;
		auto max_files = 3;
		_logger = spdlog::rotating_logger_mt("rotating_logger", "logs/_rotating-log.txt", max_size, max_files);
		_logger->set_pattern("[%D %H:%M:%S,%e] [%^%L%$] [thread %t] %v");
	};
	void test_basic_logger();
	void test_rotating_logger();
};