#include "userpref.h"


userpref::userpref(std::string initializingDirectoryInput, std::string initializingMaskInput):directoryInput(internalDirectoryInput), maskInput(internalMaskInput), searchTerm(internalSearchTerm), internalMaskInput (initializingMaskInput), initialMaskInput (initializingMaskInput)
{
	if (initializingDirectoryInput.back() != '\\') //If the user forgot to end the directory path with a slash, add it in
		initializingDirectoryInput.append("\\");

	internalDirectoryInput = initializingDirectoryInput;
	initialDirectoryInput = internalDirectoryInput;

	setSearchTerm(); //concatenates the directory and mask to make the search term
	initialSearchTerm = internalSearchTerm;

	confirmAdd = 0;
	confirmDelete = 0;
}


userpref::~userpref(void)
{
}

ushort userpref::setSearchTerm(initialReset resetToInital)
{
	if (resetToInital == reset)
	{
		internalSearchTerm = initialSearchTerm;
		return 0;
	}
	return 1;
}

void userpref::setSearchTerm()
{
	internalSearchTerm = internalDirectoryInput + internalMaskInput;
}

void userpref::setSearchTerm(std::string input)
{
	internalSearchTerm = input;
}

void userpref::setMaskInput(std::string input)
{
	internalMaskInput = input;
}

ushort userpref::setMaskInput(initialReset resetToInitial = leave)
{
	if (resetToInitial == reset)
	{
		internalMaskInput = initialMaskInput;
		return 0;
	}
	return 1;
}

void userpref::setDirectoryInput(std::string input)
{
	if (input.back() != '\\')
		input.append("\\");
	internalDirectoryInput = input;
}

ushort userpref::setDirectoryInput(initialReset resetToInitial = leave)
{
	if (resetToInitial == reset)
	{
		internalDirectoryInput = initialDirectoryInput;
		return 0;
	}
	return 1;
}