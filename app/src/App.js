import logo from './logo.svg';
import './App.css';
import { useState, useEffect } from 'react';
import { openWs } from './socket';

function App() {
  const [gameId, setGameId] = useState(null);
  const [move, setMove] = useState(null);
  const [board, setBoard] = useState(null);
  const [ws, setWs] = useState(null);

  const Piece = ({ piece }) => {
    const className = `piece ${piece === '0' ? 'hidden' : piece === '1' ? 'white' : 'black'}`

    return (
      <div className={className}></div>
    );
  }

  const Cell = ({ x, y, piece }) => {
    const makeMove = () => {
      ws.send(JSON.stringify({
        gameId,
        move: {
          x, y
        }
      }));
    }

    return (
      <div className="cell" onClick={makeMove}>
        <Piece piece={piece} />
      </div>
    );
  }
  
  useEffect(() => {
    const ws = new WebSocket(process.env.REACT_APP_SERVER_URL);
    ws.onmessage = (event) => {
      const message = event.data;
      const [move, board] = message.split('-');
      console.log(move, board);
      if (board === undefined) {
        setGameId(move);
        return;
      }
      setMove(move);
      setBoard(board.split("").slice(0, 64));
    };
    ws.onopen = () => {
      console.log("Connection is open!");
      setWs(ws);
    }

    // return () => {
    //   ws.close();
    // }
  }, []);

  return (
    <div className="App">
      <div className='board'>
        {
          board !== null && board.map((sqr, idx) => {
            let y = String(Math.floor(idx / 8));
            let x = String(idx % 8);
            return (
              <Cell x={x} y={y} piece={sqr} />
            );
          })
        }
      </div>
    </div>
  );
}

export default App;
