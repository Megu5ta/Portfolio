#pragma once

#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <stdint.h>
#include <string>

class PsInterface
{
protected:
	virtual bool init() = 0;
	virtual bool deinit() = 0;

public:
	virtual bool open() = 0;
	virtual bool close() = 0;

	virtual size_t write(const uint8_t *data, size_t size) = 0;
	virtual size_t read(uint8_t* data, size_t size) = 0;

	virtual void error(const std::string& message) = 0;
};