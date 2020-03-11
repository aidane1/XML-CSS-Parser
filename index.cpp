#include <iostream>
#include <vector>
#include <fstream>
#include "./xml.cpp"
#include "./css.cpp"
#include "./dom.cpp"

char *openFile(const char *file)
{
    std::streampos begin, end;
    std::ifstream fileStream;
    fileStream.open(file);
    begin = fileStream.tellg();
    fileStream.seekg(0, std::ios::end);
    end = fileStream.tellg();
    int size = (end - begin);
    char *memblock = new char[size];
    fileStream.seekg(0, std::ios::beg);
    fileStream.read(memblock, size);
    fileStream.close();
    return memblock;
}

int main()
{
    std::cout << sizeof(__int128_t) << "\n";

    char *xml_data = openFile("XML-HTML/test.html");
    char *css_data = openFile("CSS/test.css");

    // std::cout << strlen(xml_data) << "\n";

    XML *parsed_xml = new XML(xml_data);
    CSS *parsed_css = new CSS(css_data);

    DOM *dom_tree = new DOM(parsed_xml, parsed_css);
    

    
    delete parsed_xml;
    delete parsed_css;
    delete dom_tree;
    
    delete xml_data;
    delete css_data;

    return 0;
}