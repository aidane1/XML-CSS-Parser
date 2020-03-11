#pragma once

#include <vector>
#include <iostream>
#include "./css.cpp"
#include "./xml.cpp"

#define SCREEN_WIDTH 1600
#define SCREEN_HEIGHT 1000

struct Computed_Values
{
    float width;
    float height;
    float offset_x;
    float offset_y;
    Computed_Values()
    {
    }
};

class DOM_Element
{
public:
    Computed_Values computed;
    std::vector<Rule *> rules;
    std::vector<DOM_Element *> children;
    std::vector<Attribute> attributes;
    std::string tag;
    DOM_Element *parent;

public:
    DOM_Element()
    {
    }
    DOM_Element(Element *element, std::vector<Rule *> styles) : rules(styles)
    {
        tag = element->getTag();
        attributes = element->getAttributes();
    }
    DOM_Element(Element *element, std::vector<Rule *> styles, DOM_Element *parent) : rules(styles), parent(parent)
    {
        tag = element->getTag();
        attributes = element->getAttributes();
    }
    void appendChild(DOM_Element *child)
    {
        children.push_back(child);
    }
    std::vector<DOM_Element *> getChildren()
    {
        return children;
    }
};

class DOM
{
private:
    DOM_Element *root;

public:
    static DOM_Element *assembleChildren(Element *, CSS *, DOM_Element * = nullptr);

    DOM(XML *tree, CSS *stylesheets)
    {
        root = DOM::assembleChildren(tree->getRoot(), stylesheets);

        // std::cout << root->getChildren()[0]->getChildren()[1]->getChildren()[0]->rules.size() << "\n";
    }
};

DOM_Element *DOM::assembleChildren(Element *element, CSS *stylesheets, DOM_Element *parent)
{
    DOM_Element *current = new DOM_Element(element, stylesheets->getRules(element), parent);

    for (int i = 0; i < element->getChildren().size(); i++)
    {
        current->appendChild(assembleChildren(element->getChildren()[i], stylesheets, current));
    }
    return current;
}

class DIV_Tag : public DOM_Element
{
};
