var config=require("./config.js").config;
var movefile=config.movefile;
var move=require(movefile).move;
var pair=require(movefile).pair;
var moveleft=function(board){
  var e=board[4];
  var f=board[5];
  if(e*f*board[6]*board[7]===0||e===f){
    return 0;
  }
  return move(board,[0,1,2,3]);
};
var moveright=function(board){
  if(board[6]*board[7]===0){
    return 0;
  }
  return move(board,[3,2,1,0])|pair(board,5,4);
};
var moveup=function(board){
  var a=board[0];
  var e=board[4];
  if(a*e*board[2]*board[3]===0||a===e){
    return 0;
  }
  return move(board,[1,5,6,7]);
};
var movedown=function(board){
  return move(board,[7,6,5,1])|pair(board,4,0);
};
exports.moveleft=moveleft;
exports.moveright=moveright;
exports.moveup=moveup;
exports.movedown=movedown;