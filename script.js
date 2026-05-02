const N = 9;
let board = Array.from({ length: N }, () => Array(N).fill(0));

const sudokuBoard = document.getElementById("sudoku-board");
const generateBtn = document.getElementById("generate");
const solveBtn = document.getElementById("solve");
const clearBtn = document.getElementById("clear");

// --- Create Sudoku Grid ---
function createBoard() {
  sudokuBoard.innerHTML = "";
  for (let r = 0; r < N; r++) {
    for (let c = 0; c < N; c++) {
      const input = document.createElement("input");
      input.type = "text";
      input.maxLength = 1;
      input.value = board[r][c] === 0 ? "" : board[r][c];
      input.dataset.row = r;
      input.dataset.col = c;
      sudokuBoard.appendChild(input);
    }
  }
}

// --- Read from HTML inputs ---
function readBoard() {
  const inputs = document.querySelectorAll("#sudoku-board input");
  inputs.forEach(input => {
    const r = parseInt(input.dataset.row);
    const c = parseInt(input.dataset.col);
    const val = parseInt(input.value);
    board[r][c] = isNaN(val) ? 0 : val;
  });
}

// --- Update UI from board[][]
function updateUI() {
  const inputs = document.querySelectorAll("#sudoku-board input");
  inputs.forEach(input => {
    const r = parseInt(input.dataset.row);
    const c = parseInt(input.dataset.col);
    input.value = board[r][c] === 0 ? "" : board[r][c];
  });
}

// --- Solver Functions ---
function getPossibleValues(board, row, col) {
  const domain = new Set([1,2,3,4,5,6,7,8,9]);
  for (let c=0;c<N;c++) if (board[row][c]!==0) domain.delete(board[row][c]);
  for (let r=0;r<N;r++) if (board[r][col]!==0) domain.delete(board[r][col]);
  const sr=row-row%3, sc=col-col%3;
  for(let i=0;i<3;i++)for(let j=0;j<3;j++){
    const v=board[sr+i][sc+j];
    if(v!==0) domain.delete(v);
  }
  return Array.from(domain);
}

function findMRVCell(board){
  let min=N+1,cell=null;
  for(let r=0;r<N;r++){
    for(let c=0;c<N;c++){
      if(board[r][c]===0){
        const options=getPossibleValues(board,r,c);
        if(options.length<min){
          min=options.length;
          cell={r,c};
        }
      }
    }
  }
  return cell;
}

function solveSudokuMRV(board){
  const cell=findMRVCell(board);
  if(!cell) return true;
  const {r,c}=cell;
  const options=getPossibleValues(board,r,c);
  for(const num of options){
    board[r][c]=num;
    if(solveSudokuMRV(board)) return true;
    board[r][c]=0;
  }
  return false;
}

function shuffle(arr){
  for(let i=arr.length-1;i>0;i--){
    const j=Math.floor(Math.random()*(i+1));
    [arr[i],arr[j]]=[arr[j],arr[i]];
  }
  return arr;
}

function fillRandomBoard(board){
  for(let r=0;r<N;r++){
    for(let c=0;c<N;c++){
      if(board[r][c]===0){
        const vals=shuffle(getPossibleValues(board,r,c));
        for(const num of vals){
          board[r][c]=num;
          if(fillRandomBoard(board)) return true;
          board[r][c]=0;
        }
        return false;
      }
    }
  }
  return true;
}

function generatePuzzle(board,k){
  const idxs=shuffle([...Array(N*N).keys()]);
  for(let i=0;i<k;i++){
    const idx=idxs[i];
    board[Math.floor(idx/N)][idx%N]=0;
  }
}

// --- Buttons ---
generateBtn.onclick=()=>{
  board=Array.from({length:N},()=>Array(N).fill(0));
  fillRandomBoard(board);
  generatePuzzle(board,55);
  updateUI();
};

solveBtn.onclick=()=>{
  readBoard();
  solveSudokuMRV(board);
  updateUI();
};

clearBtn.onclick=()=>{
  board=Array.from({length:N},()=>Array(N).fill(0));
  updateUI();
};

// Initialize grid
createBoard();
