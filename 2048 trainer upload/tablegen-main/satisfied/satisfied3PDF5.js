var satisfied=function(board){
  if(board[1]===64 || board[3]===64){
      return true;
  }
  
  return false;
};
exports.satisfied=satisfied;