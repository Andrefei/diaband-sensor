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

//Call the C++ function and store the result in a new variable
function getNfcData() {
  let nfc_string = "" + addon.readData(nrOne, nrTwo);
  //TODO Check if string is emtpy
  var hex_array = nfc_string.split(" ");
  hex_array.shift();
  var nfc_parsed = [];
  //Fabricate sensor data
  hex_array.forEach(function(entry) {
    nfc_parsed.push(parseInt(entry, 16));
    nfc_parsed.unshift(parseInt(entry, 16));
  });
  hex_array.forEach(function(entry) {
    nfc_parsed.push(parseInt(entry, 16));
    nfc_parsed.unshift(parseInt(entry, 16));
  });
  //--------
  return nfc_parsed;
}

//Sends data to client on connection
io.on("connection", socket => {
  console.log("New client connected"), setInterval(
    () => emitData(socket),
    1000
  );
  socket.on("disconnect", () => console.log("Client disconnected"));
});
const emitData = async socket => {
  try {
    socket.emit("FromAPI", getNfcData());
  } catch (error) {
    console.error(`Error: ${error.code}`);
  }
};
server.listen(port, () => console.log(`Listening on port ${port}`));
