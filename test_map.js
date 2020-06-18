var SharedMemory = require("bindings")("SharedMemory");

var m = new SharedMemory.Map("sam");
m.clear();
m.insert("key1", "value1");
m.insert("key2", "value2");
m.insert("key3", "value3");
console.log(m.empty());
console.log(m.at("key2"));
//console.log(m.at(1));
console.log(m.value);
// console.log(m.getValue());
console.log(m.name);
console.log(m);
