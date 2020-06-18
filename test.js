var SharedMemory = require("bindings")("SharedMemory");

var m = new SharedMemory.Map("test.sharememory");
m.clear();
m.insert("key1", "value1");
m.insert("key2", "value2");
m.insert("key3", "value3");
console.log(m.empty());
console.log(m.at("key2"));
console.log(m.value);
console.log(m.name);
console.log(m);

var v = new SharedMemory.Vector("test.sharememory");
v.clear();
v.push_back("1");
v.push_back("2");
v.push_back("3");
v.push_back("4");
v.erase(3);
console.log(v.empty());
console.log(v.at(2));
console.log(v.value);
console.log(v.name);
console.log(v);
