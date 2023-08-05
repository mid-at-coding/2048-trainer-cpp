var satisfied=function(board){
  if(board[9]===1024){
      return true;
  }
  
  return false;
};
exports.satisfied=satisfied;