var config=require("./config.js").config;
var movefile=config.movefile;
var move=require(movefile).move;
var pair=require(movefile).pair;
var moveleft=function(board){
  if(board[6]*board[7]*board[8]*board[0]*board[1]===0||board[6]===board[1]||board[1]===board[0]){
    return 0;
  };
return move(board,[5,4,3,2]);
};
var moveright=function(board){
return move(board,[2,3,4,5])|move(board,[0,1,6]);
};
var moveup=function(board){
  if(board[0]*board[1]*board[2]*board[3]*board[4]===0||board[3]===board[0]||board[4]===board[1]){
    return 0;
  };
return move(board,[5,6,7,8]);
};
var movedown=function(board){
return move(board,[8,7,6,5])|pair(board,1,4)|pair(board,0,3);
};
exports.moveleft=moveleft;
exports.moveright=moveright;
exports.moveup=moveup;
exports.movedown=movedown;
