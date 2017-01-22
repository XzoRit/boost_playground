#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <iostream>
#include <string>
#include <algorithm>
#include <iterator>

int main()
{
  std::cout << "property_tree\n\n";
  using boost::property_tree::ptree;
  {
    ptree pt;
    pt.put("menu.foo", true);
    pt.put("menu.bar", "true");
    pt.put("menu.value", 102.3E+06);
    pt.put("menu.popup.value", "new");
    pt.put("menu.popup.onClick", "createNewDoc()");
    pt.put("menu.popup.value", "open");
    pt.put("menu.popup.onClick", "openDoc()");

    write_json("config.json", pt);
  }
  {
    ptree pt;
    read_json("example_json.json", pt);
    bool const menu_foo = pt.get<bool>("menu.foo");
    std::cout << "menu.foo = " << menu_foo << '\n';
    bool const menu_bar = pt.get<bool>("menu.bar");
    std::cout << "menu.bar = " << menu_bar << '\n';
    int const menu_value = pt.get<float>("menu.value");
    std::cout << "menu.value = " << menu_value << '\n';

    ptree const& menu_child = pt.get_child("menu");
    bool const menu_child_foo = menu_child.get<bool>("foo");
    std::cout << "menu_child_foo = " << menu_child_foo << '\n';
  }
  return 0;
}
