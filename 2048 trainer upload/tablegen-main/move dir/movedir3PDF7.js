var config=require("./config.js").config;
var movefile=config.movefile;
var move=require(movefile).move;
var pair=require(movefile).pair;
var moveleft=function(board){
  if(board[6]===0{
    return 0;
  };
return move(board,[0,1,2])|move(board,[3,4,5]);
};
var moveright=function(board){
return move(board,[2,1,0])|move(board,[5,4,3]);
};
var moveup=function(board){
  if(board[1]*board[2]*board[4]*board[5]===0||board[4]===board[1]||board[5]===board[2]){
    return 0;
  };
return move(board,[0,3,6]);
};
var movedown=function(board){
return move(board,[6,3,0])|pair(board,4,1)|pair(board,5,2);
};
exports.moveleft=moveleft;
exports.moveright=moveright;
exports.moveup=moveup;
exports.movedown=movedown;

// 0 1 2
// 3 4 5
// 6 x x
