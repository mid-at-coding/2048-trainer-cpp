var satisfied=function(board){
  if(board[2] === 256 || (board[4] === 32 && board[6] === 64 && board[8] === 128 && board[3] !== 32 && board[5] !== 64 && board[7] !== 128 && board[2] === 32 && board[3] !== 0 && board[5] !== 0 && board[7] !==0 && (board[0] === 0 || board[1] === 0 || board[0] === board[1]))){return true;}; // slot is either 256 (normal) or has a setup to go left to merge
return false;
};
exports.satisfied=satisfied;

