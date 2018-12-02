//Send sensor data to React server
//by using the socket.io engine

'use strict';
const express = require("express");
const http = require("http");
const socketIo = require("socket.io");
const port = process.env.PORT || 4001;
const index = require("./routes/index");
let addon = require('./build/Release/integer');

//Create Socket.io server
const app = express();
app.use(index);

const server = http.createServer(app);
const io = socketIo(server);

//Buffer variables
let nrOne = new Buffer("1");
let nrTwo = new Buffer("40000");

//	Call the C++ function with our numbers, and store the result in a new
//	variable
let sum = addon.sum(nrOne, nrTwo);

//Sends data to client on connection
io.on("connection", socket => {
  console.log("New client connected"), setInterval(
    () => emitData(socket),
    10000
  );
  socket.on("disconnect", () => console.log("Client disconnected"));
});
const emitData = async socket => {
  try {
    socket.emit("FromAPI", sum);
  } catch (error) {
    console.error(`Error: ${error.code}`);
  }
};
server.listen(port, () => console.log(`Listening on port ${port}`));
