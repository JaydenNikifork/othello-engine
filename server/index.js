import { WebSocketServer } from "ws";
import http from "http";
import { spawn } from "child_process";

const PORT = process.env.PORT || 8085;

const server = http.createServer((req, res) => {
  res.writeHead(200, { 'Content-Type': 'text/plain' });
  res.end('WebSocket wss is running\n');
});

const wss = new WebSocketServer({ server });

console.log(`WebSocket wss is running on ws://localhost:${PORT}`);

const games = {};
let gameCounter = 0;

wss.on('connection', (ws, req) => {
  console.log('New client connected');

  if (req.url !== '/ws') {
    console.log(`Rejected connection with invalid path: ${req.url}`);
    ws.close();
    return;
  }

  let gameNum = gameCounter;
  gameCounter++;
  games[gameNum] = spawn('../game/a.out');
  games[gameNum].stdout.on('data', (data) => {
    console.log("got line:", data.toString());
    ws.send(data.toString());
  });
  games[gameNum].on('error', () => {
    console.log("ERROR!");
  })
  games[gameNum].on('exit', () => {
    // ws.close();
  });
  ws.send(gameNum);
  
  ws.on('message', (message) => {
    message = message.toString();
    console.log("Got message: ", message)
    const { gameId, move } = JSON.parse(message);
    const game = games[gameId];
    game.stdin.write(move.x + '\n');
    game.stdin.write(move.y + '\n');
  });

  ws.on('close', () => {
    games[gameNum].kill();
  });

  ws.on('error', () => {
    games[gameNum].kill();
  });
});

server.listen(PORT, () => {
  console.log(`Server is listening on http://localhost:${PORT}`);
});