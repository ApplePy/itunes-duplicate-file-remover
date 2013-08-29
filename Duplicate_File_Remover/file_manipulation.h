#pragma once
#include <string>
#include <fstream>
#include <vector>
#include "windows.h"

class file_manipulation
{
private:
	const std::string filepath;
	std::ifstream read;
	std::ofstream write;
	std::string input;

protected:
	std::vector <std::string> processing;

	int create_folder()
	{
		int result=0;
		input=filepath;

		parse_input('\\','/',true);

		if (parsedInput.size()>=1)
		{
			for (unsigned int iterator=1; iterator<parsedInput.size()-1;iterator++)
			{
				parsedInput[iterator]=parsedInput[iterator-1]+"\\"+parsedInput[iterator];
				result=WIN32::CreateDirectory(static_cast<LPCTSTR>(parsedInput[iterator].c_str()), NULL);
			}
		}
		else
			return 3;

		if (result==ERROR_ALREADY_EXISTS)
			return 1;
		else if (result==ERROR_PATH_NOT_FOUND)
			return 2;
		else
			return 0;
	}

public:
	std::vector <std::vector <std::string>> parsedInputMulti;
	std::vector<std::string> parsedInput;

	file_manipulation(std::string filename):filepath(filename),input("")
	{
	}
	~file_manipulation()
	{
		if (read.is_open())
			read.close();
		if (write.is_open())
			write.close();
	}
	bool if_exists ()
	{
		read.open (filepath);
		if (read.is_open())
		{
			read.close();
			return true;
		}
		else
			return false;
	} 
	int write_file(std::string putin="false",std::string arg="overwrite")
	{
		if (arg=="overwrite")
			write.open(filepath, std::ios::trunc);
		else if (arg=="append")
			write.open(filepath, std::ios::app);
		if (write.is_open())
		{
			write << putin;
			write.close();
			return 0;
		}
		else
		{
			create_folder();

			if (arg=="overwrite")
				write.open(filepath, std::ios::trunc);
			else if (arg=="append")
				write.open(filepath, std::ios::app);
			if (write.is_open())
			{
				write << putin;
				write.close();
				return 0;
			}
			else
			{
				return 1;
			}
		}
	}
	int read_file (std::string arg="all")
	{
		read.open(filepath);
		input="";
		if (read.is_open())
		{
			if (arg=="word") //reads one word; end of word signalled by a space
			{
				std::getline(read,input,' ');
			}
			else if (arg=="line") //reads only one line, '\n' default delimiter
			{
				std::getline(read,input);
			}
			else if (arg=="all") //reads whole file
			{
				std::string tempInput;
				while (read.good())
				{
					getline (read,tempInput);
					input+=tempInput;
					input+='\n';
				}
				input.erase(input.cend()-1);
			}
			read.close();
			return 0;
		}
		else
			return 1;
	}
	int parse_input(char x='\n', bool parsedInputSwap=true)
	{
		if (input.empty()==true)
		{
			return 1;
		}
		processing.clear();
		std::string tempinput="";
		for (unsigned int i=0;i<(input.length());i++)
		{
			if (input.at(i)==x)
			{
				processing.push_back(tempinput);
				tempinput="";
			}
			else
				tempinput+=input.at(i);
		}
		processing.push_back(tempinput);
		if (parsedInputSwap==true)
		{
			parsedInput.clear();
			parsedInput.swap(processing);
		}
		return 0;
	}
	int parse_input(char x, char y, bool parsedInputSwap=true)
	{
		if (input.empty()==true)
		{
			return 1;
		}
		processing.clear();
		std::string tempinput="";
		for (unsigned int i=0;i<(input.length());i++)
		{
			if (input.at(i)==x || input.at(i)==y)
			{
				processing.push_back(tempinput);
				tempinput="";
			}
			else
				tempinput+=input.at(i);
		}
		processing.push_back(tempinput);
		if (parsedInputSwap==true)
		{
			parsedInput.clear();
			parsedInput.swap(processing);
		}
		return 0;
	}
	int parse_input_multiLine(char x=' ')
	{
		parsedInputMulti.clear();
		int result=parse_input('\n',false);
		if (result==1)
		{
			return 1;
		}
		const std::vector <std::string> lines (processing.cbegin (), processing.cend ());
		for (unsigned int n=0;n<lines.size();n++)
		{
			input=lines[n];
			parse_input (x,false);
			parsedInputMulti.push_back(processing);
		}

		return 0;
	}
	int const count_input_lines()
	{
		if (input.size()==0)
			return -1;
		int count = 0; 
		for (unsigned int i = 0; i < input.size(); i++)
		{
			if (input[i] == '\n')
				count++;
		}
		if (input.at(input.size()-1)!='\n')
			count++;
		return count; 
	}
	std::string const get_filepath()
	{
		return filepath;
	}
	std::string const get_input()
	{
		return input;
	}
};