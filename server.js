const express = require('express');
const app = express();
const http = require('http').Server(app);
const io = require('socket.io')(http);
const mqtt = require('mqtt');
const voiceSensing = require('./voiceSensing');

const mqttServer = 'broker.hivemq.com';
const mqttTopic = 'health_monitor_system';
const mqttControl = 'health_monitor_control';

// Connect to MQTT broker
const client = mqtt.connect(`mqtt://${mqttServer}`);

// Serve the HTML page
app.get('/', (req, res) => {
  res.sendFile(__dirname + '/index.html');
});

// Handle MQTT messages
client.on('connect', () => {
  console.log('Connected to MQTT');
  client.subscribe(mqttTopic);
});

client.on('message', (topic, message) => {
  const data = JSON.parse(message.toString());
  io.sockets.emit('sensorData', data);
});

// Handle Socket.io connections
io.on('connection', (socket) => {
  console.log('A user connected');

  socket.on('disconnect', () => {
    console.log('A user disconnected');
  });
});

// Initialize the Picovoice-based voice sensing
voiceSensing.initializeVoiceSensing(io, client, mqttControl);

const port = process.env.PORT || 3000;

http.listen(port, () => {
  console.log(`Server is running on port ${port}`);
});
