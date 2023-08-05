var satisfied=function(board){
  return board[6]===256||(board[5]===256&&board[6]*board[7]);
};
exports.satisfied=satisfied;