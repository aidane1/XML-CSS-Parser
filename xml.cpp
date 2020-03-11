#pragma once

#include <iostream>
#include <vector>

struct Attribute
{
    std::string name;
    std::string value;
    Attribute(std::string name, std::string value) : name(name), value(value){};
    Attribute()
    {
    }
};

class Element
{
private:
    std::vector<Attribute> attributes;
    std::string tag;
    std::vector<Element *> children;
    Element *parent;

public:
    Element *getParent()
    {
        return parent;
    }
    bool hasAttribute(std::string attribute_name)
    {
        for (auto &attribute : attributes)
        {
            if (attribute.name.compare(attribute_name) == 0)
            {
                return true;
            }
        }
        return false;
    }
    Attribute *getAttribute(std::string attribute_name)
    {
        for (auto &attribute : attributes)
        {
            if (attribute.name.compare(attribute_name) == 0)
            {
                return &attribute;
            }
        }
        return nullptr;
    }
    void setTag(std::string tag_name)
    {
        tag = tag_name;
    }
    std::string getTag()
    {
        return tag;
    }
    static std::vector<Attribute> getAttributesFromString(std::string);
    void setMarkup(std::string markup)
    {
        // tag = markup;
        bool found_tag = false;
        bool found_start_of_tag = false;
        int start_index;
        int end_index;
        for (int i = 0; i < markup.length(); i++)
        {
            if (isspace(markup[i]))
            {
                if (!found_tag && found_start_of_tag)
                {
                    end_index = i;
                    tag = markup.substr(start_index, end_index - start_index);
                    found_tag = true;
                    break;
                }
            }
            else if (i == markup.length() - 1)
            {
                end_index = i;
                tag = markup.substr(start_index, end_index - start_index + 1);
                found_tag = true;
            }
            else
            {
                if (!found_tag && !found_start_of_tag)
                {
                    found_start_of_tag = true;
                    start_index = i;
                }
            }
        }
        std::vector<Attribute> found_attributes = Element::getAttributesFromString(markup.substr(end_index + 1, markup.length() - end_index - 1));
        for (int i = 0; i < found_attributes.size(); i++)
        {
            attributes.push_back(found_attributes[i]);
        }
    }
    void appendChild(Element *child)
    {
        children.push_back(child);
    }
    std::vector<Element *> getChildren()
    {
        return children;
    }
    void appendAttribute(Attribute new_attribute)
    {
        attributes.push_back(new_attribute);
    }
    std::vector<Attribute> getAttributes()
    {
        return attributes;
    }
    void display(int depth)
    {
        for (int i = 0; i < depth; i++)
        {
            std::cout << "    ";
        }
        std::cout << " - " << tag;
        for (int i = 0; i < attributes.size(); i++)
        {
            std::cout << " (" << attributes[i].name << ":" << attributes[i].value << ")";
        }
        std::cout << "\n\n";
        for (int i = 0; i < children.size(); i++)
        {
            children[i]->display(depth + 1);
        }
    }

    Element(Element *parent) : parent(parent){};

    Element(Element *parent, std::string markup) : parent(parent)
    {
        tag = markup;
    };

    Element(Element *parent, std::string tag, std::vector<Attribute> attributes) : parent(parent), tag(tag), attributes(attributes)
    {
        //s
    }
};

std::vector<Attribute> Element::getAttributesFromString(std::string data)
{
    std::vector<Attribute> attributes;
    bool attribute_is_open;
    int start_index;
    int end_index;

    std::string name;
    std::string value;

    bool name_is_complete = false;
    char delim_char;
    for (int i = 0; i < data.length(); i++)
    {
        if (isspace(data[i]))
        {
            if (attribute_is_open && !name_is_complete)
            {
                end_index = i;
                attribute_is_open = false;
                name_is_complete = true;
                name = data.substr(start_index, end_index - start_index);
            }
        }
        else if (data[i] == '=')
        {
            if (attribute_is_open)
            {
                if (!name_is_complete)
                {
                    end_index = i;
                    attribute_is_open = false;
                    name_is_complete = true;
                    name = data.substr(start_index, end_index - start_index);
                }
            }
        }
        else if (data[i] == '\"' || data[i] == '\'')
        {
            if (!attribute_is_open)
            {
                attribute_is_open = true;
                start_index = i + 1;
                delim_char = data[i];
            }
            else
            {
                if (data[i] == delim_char)
                {
                    end_index = i;
                    attribute_is_open = false;
                    name_is_complete = false;
                    value = data.substr(start_index, end_index - start_index);
                    attributes.push_back(Attribute(name, value));
                }
            }
        }
        else if (i == data.length() - 1)
        {
        }
        else
        {
            if (!attribute_is_open)
            {
                attribute_is_open = true;
                start_index = i;
            }
        }
    }
    return attributes;
}

class XML
{
private:
    Element *root;

public:
    Element *getRoot()
    {
        return root;
    }
    void display()
    {
        Element *current_node = root;
        root->display(0);
    }
    XML(std::string data)
    {
        Element *root_node = new Element(nullptr, "root");

        root = root_node;

        Element *current_node = root_node;

        bool markup_is_open;
        bool text_is_open;
        bool is_opening_tag;
        int start_index;
        int end_index;
        bool is_comment = false;

        for (int i = 0; i < data.length(); i++)
        {
            if (!is_comment)
            {
                if (data[i] == '<')
                {
                    if (data.substr(i, 4) == "<!--")
                    {
                        is_comment = true;
                    }
                    else
                    {
                        if (text_is_open)
                        {
                            text_is_open = false;
                            end_index = i;
                            current_node->appendAttribute(Attribute("content", data.substr(start_index, end_index - start_index)));
                            current_node = current_node->getParent();
                        }
                        if (data[i + 1] == '/')
                        {
                            is_opening_tag = false;
                            markup_is_open = true;
                            start_index = i + 1;
                            current_node = current_node->getParent();
                        }
                        else
                        {
                            is_opening_tag = true;
                            markup_is_open = true;
                            start_index = i;
                            Element *node = new Element(current_node);
                            current_node->appendChild(node);
                            current_node = node;
                            // std::cout << i << "\n";
                        }
                    }
                }
                else if (data[i] == '>')
                {
                    markup_is_open = false;
                    end_index = i;
                    if (is_opening_tag)
                    {
                        current_node->setMarkup(data.substr(start_index + 1, end_index - start_index - 1));
                    }
                }
                else
                {
                    if (!markup_is_open && !text_is_open && !isspace(data[i]))
                    {
                        text_is_open = true;
                        Element *node = new Element(current_node, "text", std::vector<Attribute>());
                        current_node->appendChild(node);
                        current_node = node;
                        start_index = i;
                    }
                }
            }
            else
            {
                if (data.substr(i - 3, 3) == "-->")
                {
                    is_comment = false;
                }
            }
        }
    }
};