#pragma once

struct Client
{
private:
	char* name;
	char* billFilename;
public:
	Client(const char* _name, const char* _billFilename);
	~Client();
	const char* getName() const;
	const char* getBillFilename() const;
};
