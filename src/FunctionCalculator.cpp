
#include "FunctionCalculator.h"
#include "Add.h"
#include "Mul.h"
#include "Comp.h"
#include "Identity.h"
#include "Swap.h"
#include "Reverse.h"
#include "SubStr.h"
#include <iostream>
#include <algorithm>
#include "FileExceptions.h"
#include <iostream>



FunctionCalculator::FunctionCalculator(std::istream& istr, std::ostream& ostr)
	: m_actions(createActions()), m_operations(createOperations()), m_istr(istr), m_ostr(ostr)
{
}

void FunctionCalculator::run()
{

	//check if the input is a number then check if the number is between 3 and 100
	std::string maxFunc;
	while (true)
	{
		try {
			m_ostr << "please enter the maximum number of functions:\n";
			std::cin >> maxFunc;

			if (!checkIfNumber(maxFunc))
			{
				throw std::invalid_argument("input must be a number\n");
			}

			maxFunctions = std::stoi(maxFunc);

			if (maxFunctions >= 3 && maxFunctions <= 100)
				break;
			else
				throw std::invalid_argument("Functions num must be (min = 3 and max = 100): \n");
		}
		catch (std::invalid_argument e)
		{
			m_ostr << e.what();
		}

	}
	do
	{
		try {

			if (maxFunctions < 3 || m_operations.size() > maxFunctions)
				throw std::invalid_argument("");
		}
		catch (std::invalid_argument e)
		{
			m_ostr << "Functions num must be min: 3 and max: " << maxFunctions << std::endl;
			m_operations.erase(m_operations.end() - 1);
		}
		m_ostr << '\n';
		printOperations();
		m_ostr << "Enter command ('help' for the list of available commands): ";
		const auto action = readAction(std::cin);
		runAction(action, std::cin);
	} while (m_running);
}

void FunctionCalculator::eval(std::istream& inp)
{
	if (auto index = readOperationIndex(inp); index)
	{
		const auto& operation = m_operations[*index];
		auto input = std::string();
		try
		{
			inp >> input;
			if (input.size() > 20)
				throw std::out_of_range("");
			if (operation->compute(input).size() > 20)
				throw std::out_of_range("");
			operation->print(m_ostr, input);
			m_ostr << " = " << operation->compute(input) << '\n';
		}

		catch (std::out_of_range e)
		{
			m_ostr << "\nthe length of the string must be maximum 20\n";
		}

	}
}
void FunctionCalculator::substr(std::istream& inp)
{
	int start_index, chars_length;
	std::string s1, s2;
	try {

		inp >> s1 >> s2;
		if (!(checkIfNumber(s1) && checkIfNumber(s2)))
		{
			throw std::invalid_argument("");
		}

	}
	catch (std::invalid_argument e)
	{
		m_ostr << "please enter only numbers\n";
		return;
	}
	start_index = std::stoi(s1);
	chars_length = std::stoi(s2);
	m_operations.push_back(std::make_shared<SubStr>(start_index, chars_length));
}

void FunctionCalculator::mul(std::istream& inp)
{
	std::string s;
	int n;
	try {

		inp >> s;
		if (!checkIfNumber(s))
		{
			throw std::invalid_argument("");
		}

	}

	catch (std::invalid_argument e)
	{
		m_ostr << "wrong input, please enter a number\n";
		inp >> s;//to clear the buffer
		return;
	}
	if (auto i = readOperationIndex(inp); i)
	{
		n = std::stoi(s);
		try {
			if (n <= 0)
			{
				throw std::invalid_argument("");
			}
			m_operations.push_back(std::make_shared<Mul>(n, m_operations[*i]));
		}
		catch (std::invalid_argument e)
		{
			m_ostr << "please enter a positive number\n";
		}
	}
}

void FunctionCalculator::del(std::istream& inp)
{
	try {

		if (auto i = readOperationIndex(inp); i)
		{
			if (m_operations.size() <= 3)
				throw std::invalid_argument("");
			m_operations.erase(m_operations.begin() + *i);
		}

	}
	catch (std::invalid_argument e)
	{
		m_ostr << "Functions num must be at least 3 \n";
	}

}

void FunctionCalculator::help()
{
	m_ostr << "The available commands are:\n";
	for (const auto& action : m_actions)
	{
		m_ostr << "* " << action.command << action.description << '\n';
	}
	m_ostr << '\n';
}

void FunctionCalculator::exit()
{
	m_ostr << "Goodbye!\n";
	m_running = false;
}

void FunctionCalculator::resize(std::istream& inp)
{
	int temp = maxFunctions;
	std::string s;

	while (true)
	{
		try {
			inp >> s;
			if (!checkIfNumber(s))
			{
				throw std::invalid_argument(" wrong input, please input a number \n");
			}
			maxFunctions = std::stoi(s);
			if (maxFunctions >= 3 && maxFunctions <= 100)
			{
				m_ostr << "the new size is: " << maxFunctions << std::endl;
				break;
			}
			else
				throw std::invalid_argument("Functions num must be min : 3 and max : 100\n");
		}
		catch (std::invalid_argument e)
		{
			m_ostr << e.what();
			break;
		}
	}

	//when the new max functions is less than the current functions, then we need to resize the vector(m_operations)
	if (maxFunctions < temp && maxFunctions < m_operations.size())
	{
		m_operations.resize(maxFunctions);
	}


}

void FunctionCalculator::read()
{
	std::ifstream input;
	try
	{
		openFile(input);
	}
	catch (InvalidPath e)
	{
		std::cerr << e.what();
		return;
	}
	m_inFile = true;
	while (!input.eof() && m_continueReading)
	{
		try
		{
			const auto action = readAction(input);
			runAction(action, input);
			++m_line;
		}
		catch (FileError e)
		{
			std::cerr << e.what();
			m_continueReading = checkError();
		}

	}
	m_continueReading = true;
	input.close();
	m_inFile = false;
}

void FunctionCalculator::openFile(std::ifstream& inp)
{
	std::string path;
	std::cin >> path;

	inp.open(path);
	if (!inp.is_open())
		throw InvalidPath();
}

bool FunctionCalculator::checkError()const
{
	std::cout <<
		"\nThere is an error! would you like to "
		"continue reading the file ? \nY - yes / N - no\n";
	while (true)
	{
		try
		{
			std::string read;
			std::getline(std::cin, read);
			if (read == "Y" || read == "y")
				return true;
			else if (read == "N" || read == "n")
				return false;
			else
				throw std::istream::failure("");
		}
		catch (std::istream::failure e)
		{
			std::cout << "(" << e.what() << ")\n";
		}
	}
}

//function that check if a string is a number or not
bool FunctionCalculator::checkIfNumber(std::string s)const
{
	//the '-' means minus and thats considered a number
	if (!s[0] == '-' || !isdigit(s[0]))
	{
		return false;
	}

	for (size_t i = 1; i < s.size(); i++)
	{
		if (!isdigit(s[i]))
			return false;
	}


	return true;
}

void FunctionCalculator::printOperations() const
{
	m_ostr << "List of available string operations:\n";
	for (decltype(m_operations.size()) i = 0; i < m_operations.size(); ++i)
	{
		m_ostr << i << ".\t";
		m_operations[i]->print(m_ostr, true);
		m_ostr << '\n';
	}
	m_ostr << '\n';
}

std::optional<int> FunctionCalculator::readOperationIndex(std::istream& inp) const
{
	std::string i;
	int num;
	try
	{
		inp >> i;

		if (!is_number(i))
		{
			throw(std::iostream::failure(""));
		}
		num = std::stoi(i);
		if (num >= m_operations.size())
		{
			throw(std::out_of_range(""));
		}
	}
	catch (std::istream::failure)
	{
		m_ostr << i << " Wrong input! Please try again:\n";
		return {};
	}

	catch (std::out_of_range)
	{
		m_ostr << "Operation #" << num << " doesn't exist! Please try again:\n";
		return {};
	}
	return num;
}

bool FunctionCalculator::is_number(std::string s)const
{
	for (char c : s) {
		if (!std::isdigit(c)) {
			return false;
		}
	}
	return !s.empty();
}
FunctionCalculator::Action FunctionCalculator::readAction(std::istream& inp) const
{
	auto action = std::string();
	try {
		inp >> action;

		const auto i = std::ranges::find(m_actions, action, &ActionDetails::command);
		if (i == m_actions.end())
		{
			if (m_inFile)
				throw FileError();
			else
				throw std::iostream::failure("");
		}
		return i->action;

	}

	catch (std::istream::failure e)
	{
		return Action::Invalid;
	}
}
void FunctionCalculator::runAction(Action action, std::istream& inp)
{
	switch (action)
	{
	default:
		m_ostr << "Unknown enum entry used!\n";
		break;

	case Action::Invalid:
		m_ostr << "Command not found\n";
		break;

	case Action::Eval:         eval(inp);                     break;
	case Action::SubStr:       substr(inp);                   break;
	case Action::Add:          binaryFunc<Add>(inp);          break;
	case Action::Mul:          mul(inp);                      break;
	case Action::Comp:         binaryFunc<Comp>(inp);         break;
	case Action::Del:          del(inp);                      break;
	case Action::Help:         help();                     break;
	case Action::Exit:         exit();                     break;
	case Action::Read:         read();                     break;
	case Action::Resize:       resize(inp);                   break;
	}
}

FunctionCalculator::ActionMap FunctionCalculator::createActions() const
{
	return ActionMap
	{
		{
			"eval",
			"(uate) num x - compute the result of function #num on the "
			"following x string",
			Action::Eval
		},
		 {
			"substr",
			" x y - creates a substring starting from index x with the length of y characters "
			"following x string",
			Action::SubStr
		},
		{
			"add",
			"(on) num1 num2 - Creates an operation that is the concatenation of "
			"operation #num1 and operation #num2",
			Action::Add
		},
		{
			"mul",
			"(tiply) n num - Creates an operation that is the "
			"multiply n of operation #num",
			Action::Mul
		},
		{
			"comp",
			"(osite) num1 num2 - creates an operation that is the composition "
			"of operation #num1 and operation #num2",
			Action::Comp
		},
		{
			"del",
			"(ete) num - delete operation #num from the operation list",
			Action::Del
		},
		{
			"help",
			" - print this command list",
			Action::Help
		},
		{
			"exit",
			" - exit the program",
			Action::Exit
		},
			{
			"read",
			" - Read the commands from a file",
			Action::Read
		},
		{
			"resize",
			" - Resize the number of the max num of Functions",
			Action::Resize
		}
	};
}

FunctionCalculator::OperationList FunctionCalculator::createOperations() const
{
	return OperationList
	{
		std::make_shared<Identity>(),
		std::make_shared<Swap>(),
		std::make_shared<Reverse>()

	};
}
