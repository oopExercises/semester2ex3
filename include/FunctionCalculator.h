#pragma once

#include <vector>
#include <memory>
#include <string>
#include <iosfwd>
#include <optional>
#include <iostream>
#include "FileExceptions.h"
#include <fstream>
class Operation;

class FunctionCalculator
{
public:
    FunctionCalculator(std::istream& istr, std::ostream& ostr);
    void run();

private:
    void eval(std::istream& inp);
    void substr(std::istream& inp);
    void mul(std::istream& inp);
    void del(std::istream& inp);
    void help();
    void exit();
    void resize(std::istream& inp);
    void read();
    void openFile(std::ifstream& inp);
    bool checkError()const;
    bool checkIfNumber(std::string)const;

    template <typename FuncType>
    void binaryFunc(std::istream &inp)
    {
        if (auto f0 = readOperationIndex(inp), f1 = readOperationIndex(inp); f0 && f1)
        {
            m_operations.push_back(std::make_shared<FuncType>(m_operations[*f0], m_operations[*f1]));
        }
    }

    void printOperations() const;

    enum class Action
    {
        Invalid,
        Eval,
        SubStr,
        Add,
        Mul,
        Comp,
        Del,
        Help,
        Exit,
        Read,
        Resize,
    };

    struct ActionDetails
    {
        std::string command;
        std::string description;
        Action action;
    };

    using ActionMap = std::vector<ActionDetails>;
    using OperationList = std::vector<std::shared_ptr<Operation>>;

    const ActionMap m_actions;
    OperationList m_operations;
    bool m_running = true;
    std::istream& m_istr;
    std::ostream& m_ostr;

    std::optional<int> readOperationIndex(std::istream& inp) const;
    bool is_number( std::string s)const;
    Action readAction(std::istream& inp) const;
    void runAction(Action action, std::istream& inp);

    ActionMap createActions() const;
    OperationList createOperations() const ;

    int maxFunctions = 0;
    bool m_inFile = false;
    int m_line;
    bool m_continueReading = true;
    //bool m_readMode = false;
    //int m_FuncCounter = 2;
    //std::string dtos(double d);
    int length = 3;
};
