var SharedMemory = require("bindings")("SharedMemory");
// Set memory size, default size is 64k, minimum size is 1024 bytes.
// var SharedMemory = require("bindings")("SharedMemory", 1024);

var m = new SharedMemory.Map("test.sharememory");
// Use the same or different name
// var m = new SharedMemory.Map("test1.sharememory");
m.insert("key0", "value0");
m.insert("key20", "value20");
m.clear();
m.insert("key1", "value1");
m.insert("key2", "value2");
m.insert("key3", "value3");
m.insert("key3", 1234);
m.insert("key5", { x: 1, y: { z: 2 } });
console.log(m.empty());
console.log(m.at("key2"));
console.log(m.value);
console.log(m.name);
console.log(m);

var v = new SharedMemory.Vector("test.sharememory");
// Use the same or different name
// var m = new SharedMemory.Map("test2.sharememory");
v.push_back("1234");
v.push_back({ x: 1, y: { z: 2 } });
v.clear();
v.push_back("1");
v.push_back("2");
v.push_back("3");
v.push_back("4");
v.erase(3);
console.log(v.empty());
console.log(v.at(0));
console.log(v.value);
console.log(v.name);
console.log(v);
