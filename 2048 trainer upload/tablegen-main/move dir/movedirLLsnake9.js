var config=require("./config.js").config;
var movefile=config.movefile;
var move=require(movefile).move;
var pair=require(movefile).pair;
var moveleft=function(board){
return 0;
};
var moveright=function(board){
return move(board,[4,3,2])|pair(board,1,0)|pair(board,6,5)|pair(board,8,7);
};
var moveup=function(board){
if(board[4]===0){return 0;};
return move(board,[0,2,5,7])|move(board,[1,3,6,8]);
};
var movedown=function(board){
if(board[4]===0){return 0;};
return move(board,[7,5,2,0])|move(board,[8,6,3,1]);
};
exports.moveleft=moveleft;
exports.moveright=moveright;
exports.moveup=moveup;
exports.movedown=movedown;

// 0 1
// 2 3 4
// 5 6
// 7 8
