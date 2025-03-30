// import WebSocket from "ws";

export function openWs() {
  const ws = new WebSocket('ws://localhost:8085');
  return ws;
}