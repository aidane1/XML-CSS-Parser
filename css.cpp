#pragma once

#include <iostream>
#include <vector>
#include "./xml.cpp"

const std::string WHITESPACE = " \n\r\t\f\v";

std::string ltrim(const std::string &s)
{
    size_t start = s.find_first_not_of(WHITESPACE);
    return (start == std::string::npos) ? "" : s.substr(start);
}

std::string rtrim(const std::string &s)
{
    size_t end = s.find_last_not_of(WHITESPACE);
    return (end == std::string::npos) ? "" : s.substr(0, end + 1);
}

std::string trim(const std::string &s)
{
    return rtrim(ltrim(s));
}

struct Declaration
{
    std::string property;
    std::string value;
    void display()
    {
        std::cout << "    " << property << ":" << value << "\n";
    }
    Declaration(std::string data)
    {
        int start_index;
        int end_index;
        bool declaration_is_open = false;
        int last_char_index;
        bool is_value = false;

        for (int i = 0; i < data.length(); i++)
        {
            if (isspace(data[i]))
            {
                if (!is_value)
                {
                    end_index = last_char_index;
                    declaration_is_open = false;
                }
            }
            else if (data[i] == ';')
            {
                end_index = last_char_index;
                value = data.substr(start_index, end_index - start_index + 1);
            }
            else if (data[i] == ':')
            {
                if (declaration_is_open)
                {
                    end_index = last_char_index;
                }
                is_value = true;
                declaration_is_open = false;
                property = data.substr(start_index, end_index - start_index + 1);
            }
            else
            {
                last_char_index = i;
                if (!declaration_is_open)
                {
                    declaration_is_open = true;
                    start_index = i;
                }
            }
        }
    }
    Declaration()
    {
    }
};

class Selector
{
private:
    std::string selector;

public:
    void display()
    {
        std::cout << selector << " {"
                  << "\n";
    }
    bool matchesSelector(Element *element)
    {
        switch (selector[0])
        {
        case '.':
            if (
                element->hasAttribute("class") &&
                element->getAttribute("class")->value.compare(selector.substr(1, selector.length() - 1)) == 0)
            {
                return true;
            }
            break;
        case '#':
            if (
                element->hasAttribute("id") &&
                element->getAttribute("id")->value.compare(selector.substr(1, selector.length() - 1)) == 0)
            {
                return true;
            }
            break;
        default:
            break;
        }
        return false;
    }
    Selector(std::string data) : selector(trim(data))
    {
    }
    Selector()
    {
    }
};

class Rule
{
private:
    std::vector<Declaration> declarations;
    Selector selector;

public:
    static std::vector<Declaration> getDeclarationsFromString(std::string);
    void display()
    {
        selector.display();
        for (int i = 0; i < declarations.size(); i++)
        {
            declarations[i].display();
        }
        std::cout << "}\n\n";
    }
    Selector getSelector()
    {
        return selector;
    }
    Rule(std::string data)
    {
        bool body_is_open = false;
        int start_index;
        int end_index;

        for (int i = 0; i < data.length(); i++)
        {
            if (data[i] == '{' && !body_is_open)
            {
                body_is_open = true;
                this->selector = Selector(data.substr(0, i));
                start_index = i + 1;
            }
            else if (data[i] == '}')
            {
                if (body_is_open)
                {
                    body_is_open = false;
                    end_index = i;
                }
            }
        }

        std::vector<Declaration> found_declarations = Rule::getDeclarationsFromString(data.substr(start_index, end_index - start_index));
        for (int i = 0; i < found_declarations.size(); i++)
        {
            declarations.push_back(found_declarations[i]);
        }
    }
};

std::vector<Declaration> Rule::getDeclarationsFromString(std::string data)
{
    std::vector<Declaration> declarations;

    int start_index;
    int end_index;
    bool declaration_is_open = false;
    for (int i = 0; i < data.length(); i++)
    {
        if (isspace(data[i]))
        {
        }
        else if (data[i] == ';')
        {
            end_index = i;
            declaration_is_open = false;
            declarations.push_back(Declaration(data.substr(start_index, end_index - start_index + 1)));
        }
        else
        {
            if (!declaration_is_open)
            {
                declaration_is_open = true;
                start_index = i;
            }
        }
    }
    return declarations;
}

class CSS
{
private:
    std::vector<Rule *> rules;

public:
    void display()
    {
        for (int i = 0; i < rules.size(); i++)
        {
            rules[i]->display();
        }
    }

    std::vector<Rule *> getRules(Element *element)
    {
        std::vector<Rule *> rules;
        for (auto &rule : this->rules)
        {
            if (rule->getSelector().matchesSelector(element))
            {
                rules.push_back(rule);
            }
        }
        return rules;
    }

    CSS(std::string data)
    {

        bool rule_is_open = false;
        int start_index;
        int end_index;
        for (int i = 0; i < data.length(); i++)
        {
            if (isspace(data[i]))
            {
            }
            else if (data[i] == '}')
            {
                if (rule_is_open)
                {
                    rule_is_open = false;
                    end_index = i;
                    Rule *rule = new Rule(data.substr(start_index, end_index - start_index + 1));
                    this->rules.push_back(rule);
                }
            }
            else
            {
                if (!rule_is_open)
                {
                    rule_is_open = true;
                    start_index = i;
                }
            }
        }
    }
};