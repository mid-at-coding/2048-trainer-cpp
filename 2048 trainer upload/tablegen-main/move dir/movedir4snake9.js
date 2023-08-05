var config=require("./config.js").config;
var movefile=config.movefile;
var move=require(movefile).move;
var pair=require(movefile).pair;
var moveleft=function(board){
return 0;
};
var moveright=function(board){
return move(board,[2,1,0])|pair(board,4,3)|pair(board,6,5)|pair(board,8,7);
};
var moveup=function(board){
if(board[2]===0){return 0;};
return move(board,[0,3,5,7])|move(board,[1,4,6,8]);
};
var movedown=function(board){
return move(board,[7,5,3,0])|move(board,[8,6,4,1]);
};
exports.moveleft=moveleft;
exports.moveright=moveright;
exports.moveup=moveup;
exports.movedown=movedown;

// 0 1 2
// 3 4
// 5 6
// 7 8
