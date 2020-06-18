var SharedMemory = require("bindings")("SharedMemory");

var v = new SharedMemory.Vector("sam");
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
