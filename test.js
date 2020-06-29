var SharedMemory = require("bindings")("SharedMemory");
// var SharedMemory = require("bindings")("SharedMemory", 1024);

var m = new SharedMemory.Map("test.sharememory", 64 * 1024);
// Parameter 1, is ShareMemory Name
// Set memory size, default size is 64k, minimum size is 1024 bytes. If you want to insert more data, you must set bigger memory size.
// If insert error, you must set bigger memory size.
m.insert("key0", "value0");
m.insert("key20", "value20");
m.clear(); // You can comment out this line
m.insert("key1", "value1");
m.insert("key2", "value2");
m.insert("key3", "value3");
m.insert("key3", 1234);
m.insert("key5", { x: 1, y: { z: 2 } });
m.insert(234, { x: 1, y: { z: 2 } });
m.insert({ x: 123 }, { x: 111, y: { z: 2 } });
console.log(m.empty());
console.log(m.at(234));
console.log(m.at({ x: 123 }));

xx = m.value[{ x: 123 }];
console.log("Object: " + JSON.stringify(xx));

console.log(m.value);
console.log(m.name);
console.log(m);

var v = new SharedMemory.Vector("test.sharememory", 1024);
// Same as Map
// You can use the same or different name, If the name exists, the memory size is ignored.

v.push_back(1);
v.push_back({ x: 1, y: { z: 1 } });
v.clear(); // You can comment out this line
v.push_back(2);
v.push_back({ x: 2, y: { z: 2 } });
v.push_back("1");
v.push_back("2");
v.push_back("3");
v.push_back("4");

v.erase(3);
console.log(v.empty());
console.log(v.at(0));
console.log(v.value);
// console.log(v.name);
// console.log(v);
