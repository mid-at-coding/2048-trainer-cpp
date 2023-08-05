var satisfied=function(board){
  if(board[2]===64 || board[4]===64){
      return true;
  }
  
  return false;
};
exports.satisfied=satisfied;