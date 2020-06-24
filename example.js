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
