## node-addon-sharedmemory

### Program description

Use c++ boost's cross-process Shared memory technology to provide nodejs with plug-ins that share memory across processes.

### Dependencies

#### Windows

- Visual Studio 2017

- Boost 1.69.0
- nodejs 12.18.0
- node-addon-api  
- cmake-js

#### Linux

No Test


### Usages

Basic compile environment configuration, reference: https://github.com/nodejs/node-addon-api, then run:

```shell
npm install
node ./test.js
```

### Example

```javascript
var SharedMemory = require("bindings")("SharedMemory");
// Set memory size, default size is 64k, minimum size is 1024 bytes.
// var SharedMemory = require("bindings")("SharedMemory", 1024);

var m = new SharedMemory.Map("test.sharememory");
// Use the same or different name
// var m = new SharedMemory.Map("test1.sharememory");
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
// Use the same or different name
// var m = new SharedMemory.Map("test2.sharememory");
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
```

