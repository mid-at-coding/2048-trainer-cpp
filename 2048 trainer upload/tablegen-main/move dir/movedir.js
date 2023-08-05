var config=require("./config.js").config;
var movefile=config.movefile;
var move=require(movefile).move;
var pair=require(movefile).pair;
var moveleft=function(board){
  var e=board[4];
  var f=board[5];
  var g=board[6]
  if(e*f*g*board[7]*board[8]===0||e===f||f===g){
    return 0;
  }
  return move(board,[0,1,2,3]);
};
var moveright=function(board){
  if(board[7]*board[8]===0){
    return 0;
  }
  return move(board,[3,2,1,0])|move(board,[6,5,4]);
};
var moveup=function(board){
  var a=board[0];
  var e=board[4];
  var c=board[2];
  var g=board[6];
  if(a*e*c*g*board[3]===0||a===e||c===g){
    return 0;
  }
  return move(board,[1,5,7,8]);
};
var movedown=function(board){
  return move(board,[8,7,5,1])|pair(board,4,0)|pair(board,6,2);
};
exports.moveleft=moveleft;
exports.moveright=moveright;
exports.moveup=moveup;
exports.movedown=movedown;