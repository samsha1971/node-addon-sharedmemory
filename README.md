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

cluster/worker:

```javascript
const cluster = require("cluster");
const SharedMemory = require("./");
//const SharedMemory = require("bindings")("SharedMemory");
const m = new SharedMemory.Map("test.sharememory");
const v = new SharedMemory.Vector("test.sharememory");

if (cluster.isMaster) {
  console.log(`Cluster ${process.pid} is running.`);
  m.clear();
  m.insert("process_" + `${process.pid}`, `${process.pid}`);
  v.clear();
  v.push_back(`process : ${process.pid}`);

  for (let i = 0; i < 2; i++) {
    const worker = cluster.fork();
    worker.on("message", (msg) => {
      console.log(`Receive worker ${process.pid} message.`);
      console.log("    Map:" + JSON.stringify(m.value));
      console.log("    Vector:" + JSON.stringify(v.value));
    });
  }

  cluster.on("exit", (worker, code, signal) => {
    console.log(`Worker ${worker.process.pid} exits.`);
  });
} else {
  console.log(`Worker ${process.pid} start.`);
  console.log("    Map:" + JSON.stringify(m.value));
  console.log("    Vector:" + JSON.stringify(v.value));

  m.insert("process_" + `${process.pid}`, `${process.pid}`);
  v.push_back(`process : ${process.pid}`);
  process.send("");
}

```

test:

```javascript
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

```

