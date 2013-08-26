#pragma once
#include <string>

typedef unsigned short ushort;

enum initialReset {reset, leave};

class userpref
{
public:
	userpref(std::string initializingDirectoryInput, std::string initializingMaskInput);
	~userpref(void);
	void setDirectoryInput(std::string input);
	ushort setDirectoryInput(initialReset resetToInitial);
	void setMaskInput(std::string input);
	ushort setMaskInput(initialReset resetToInitial);
	void setSearchTerm();
	ushort setSearchTerm(initialReset resetToInital);
	void setSearchTerm(std::string input);

	const std::string &directoryInput; //public-facing uneditable versions of class variables
	const std::string &maskInput; //public-facing uneditable versions of class variables
	const std::string &searchTerm; //public-facing uneditable versions of class variables
	ushort confirmDelete;
	ushort confirmAdd;

protected:
	std::string internalDirectoryInput;
	std::string internalMaskInput;
	std::string internalSearchTerm;

private:
	std::string initialDirectoryInput;
	std::string initialMaskInput;
	std::string initialSearchTerm;
};

