## node-addon-sharedmemory

### 程序说明

利用c++ boost的跨进程共享内存技术，为nodejs提供跨进程的共享内存的插件。

### 依赖

#### Windows

- Visual Studio 2017

- Boost 1.69.0
- nodejs 12.18.0
- node-addon-api  
- cmake-js

#### Linux

没有测试。


### 使用

配置基本编译环境，主要参考：https://github.com/nodejs/node-addon-api，然后运行：

```shell
npm install
node ./test.js
```

### 举例

```javascript
var SharedMemory = require("node-addon-sharememory");
// var SharedMemory = require("bindings")("SharedMemory");
// Set memory size, default size is 64k, minimum size is 1024 bytes.
// var SharedMemory = require("bindings")("SharedMemory", 1024);

var m = new SharedMemory.Map("test.sharememory");
// Use the same or different name
// var m = new SharedMemory.Map("test1.sharememory");
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
// console.log(m.name);
// console.log(m);

var v = new SharedMemory.Vector("test.sharememory");
// Use the same or different name
// var m = new SharedMemory.Vector("test1.sharememory");
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

```

