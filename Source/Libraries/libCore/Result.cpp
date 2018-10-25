///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Result.cpp
//
//  A result of an operation.
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  Copyright (c) Project Firestorm 2018
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "Result.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

OPEN_NAMESPACE(Firestorm);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Error::Error()
: _code(nullptr)
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Error::Error(const ErrorCode* errorCode, const string& details, const char* file, int line)
: _code(errorCode)
, _details(details)
{
	_formatted.append_sprintf("[%d] ERROR %s", _code->_code, _code->_message.c_str());
	if(!_details.empty())
		_formatted.append_sprintf("\n    DETAILS: %s", _details.c_str());
	if(file)
	{
		_formatted.append_sprintf("\n    File: %s", file);
	}
	if(line > 0)
	{
		_formatted.append_sprintf("\n    Line: %d", line);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Error::Error(const Error& error)
: _code(error._code)
, _details(error._details)
, _formatted(error._formatted)
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Error::Error(Error&& other)
: _code(other._code)
, _details(eastl::move(other._details))
, _formatted(eastl::move(other._formatted))
{
	other._code = nullptr;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Error::operator string() const
{
	return _formatted;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

const char* Error::Format() const
{
	return _formatted.c_str();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Error::operator uint32_t() const
{
	return _code->_code;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Error& Error::operator=(const Error& e)
{
	if(this != &e)
	{
		_code = e._code;
		_details = e._details;
		_formatted = e._formatted;
	}
	return *this;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool Error::operator==(const Error& e) const
{
	return _code->_code == e._code->_code;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Error::Set(const ErrorCode* code, const string& details)
{
	_code = code;
	_details = details;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Error::operator bool() const
{ 
	return _code != nullptr;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

uint32_t ErrorCode::s_errorCodes = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ErrorCode::ErrorCode(const string& message)
: _code(++s_errorCodes)
, _message(message)
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ErrorCode::operator string() const 
{ 
	return _message; 
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ErrorCode::operator const string&() const
{ 
	return _message; 
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ErrorCode::operator uint32_t() const
{ 
	return _code;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CLOSE_NAMESPACE(Firestorm);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////