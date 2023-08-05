var config=require("./config.js").config;
var movefile=config.movefile;
var move=require(movefile).move;
var pair=require(movefile).pair;

var moveleft=function(board){
if(board[0]*board[1]*board[6]*board[7]*board[8]*board[9]===0||board[1]===board[0]||board[7]===board[6]||board[8]===board[9]){return 0;};
return move(board,[2,3,4,5])|pair(board,0,1)|pair(board,6,7)|pair(board,8,9);
};

var moveright=function(board){
return move(board,[5,4,3,2])|pair(board,1,0)|pair(board,7,6)|pair(board,9,8);
};

var moveup=function(board){
if(board[4]*board[5]===0){return 0;};
return move(board,[0,2,6,8])|move(board,[1,3,7,9]);
};

var movedown=function(board){
if(board[4]*board[5]===0){return 0;};
return move(board,[8,6,2,0])|move(board,[9,7,3,1]);
};

exports.moveleft=moveleft;
exports.moveright=moveright;
exports.moveup=moveup;
exports.movedown=movedown;

// 0 1 x x
// 2 3 4 5 
// 6 7 x x
// 8 9 x x