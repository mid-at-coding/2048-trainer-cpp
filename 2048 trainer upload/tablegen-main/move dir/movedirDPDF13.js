var config=require("./config.js").config;
var movefile=config.movefile;
var move=require(movefile).move;
var pair=require(movefile).pair;
var moveleft=function(board){
if(board[8]===0||board[8]===board[9]||board[10]===board[9]||board[10]===board[11]){return 0;};
return move(board,[0,1,2,3])|move(board,[4,5,6,7]);
};
var moveright=function(board){
return move(board,[3,2,1,0])|move(board,[7,6,5,4]|move(board,[11,10,9,8]);
};
var moveup=function(board){
if(board[1]*board[2]*board[3]*board[5]*board[6]*board[7]*board[9]*board[10]*board[1]===0||board[1]===board[5]||board[2]===board[6]||board[3]===board[7]||board[9]===board[5]||board[10]===board[6]||board[11]===board[7]){return 0;};
return move(board,[0,4,8,9]);
};
var movedown=function(board){
return move(board,[12,8,4,0])|move(board,[9,5,1])|move(board,[10,6,2])|move(board,[11,7,3]);
};
exports.moveleft=moveleft;
exports.moveright=moveright;
exports.moveup=moveup;
exports.movedown=movedown;

// 0  1  2  3
// 4  5  6  7
// 8  9  10 11
// 12 x  x  x