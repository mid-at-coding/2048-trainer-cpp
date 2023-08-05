var config=require("./config.js").config;
var movefile=config.movefile;
var move=require(movefile).move;
var pair=require(movefile).pair;
var moveleft=function(board){
  if(board[5]*board[6]*board[7]*board[0]===0||board[5]===board[0]){
    return 0;
  };
return move(board,[4,3,2,1]);
};
var moveright=function(board){
return move(board,[1,2,3,4])|pair(board,0,5);
};
var moveup=function(board){
  if(board[0]*board[1]*board[2]*board[3]===0||board[3]===board[0]){
    return 0;
  };
return move(board,[4,5,6,7]);
};
var movedown=function(board){
return move(board,[7,6,5,4])|pair(board,0,3);
};
exports.moveleft=moveleft;
exports.moveright=moveright;
exports.moveup=moveup;
exports.movedown=movedown;
