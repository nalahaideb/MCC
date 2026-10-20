#ifndef ERROR_H
#define ERROR_H
class Error
{
	public:
	Error(const std::string m)
	:message(m) {}
	
	const std::string message;
};

#endif
