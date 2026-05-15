#include <iostream>

namespace blueprints {
class object {
private:
  int inventory_size;
  int entry_num;
  bool heavy;
  std::string name;
  object *next = nullptr;

public:
  const std::string &get_name() const { return name; }
  void set_name(std::string &new_name) { name = new_name; }
  const int &get_inventory_size() const { return inventory_size; }
  void set_inventory_size(int &size) { inventory_size = size; }
  void set_next(blueprints::object &next_object) { next = &next_object; }
};

} // namespace blueprints
