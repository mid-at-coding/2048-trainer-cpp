var config=require("./config.js").config;
var movefile=config.movefile;
var move=require(movefile).move;
var pair=require(movefile).pair;
var moveleft=function(board){
  if(board[3]*board[4]*board[5]===0||board[5]===board[3]){
    return 0;
  };
return move(board,[2,1,0]);
};
var moveright=function(board){
return move(board,[0,1,2])|pair(board,5,3);
};
var moveup=function(board){
  if(board[0]*board[1]*board[5]===0||board[5]===board[1]){
    return 0;
  };
return move(board,[2,3,4]);
};
var movedown=function(board){
return move(board,[4,3,2])|pair(board,5,1);
};
exports.moveleft=moveleft;
exports.moveright=moveright;
exports.moveup=moveup;
exports.movedown=movedown;
