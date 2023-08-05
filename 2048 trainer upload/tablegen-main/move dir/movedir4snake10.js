var config=require("./config.js").config;
var movefile=config.movefile;
var move=require(movefile).move;
var pair=require(movefile).pair;
var moveleft=function(board){
return 0;
};
var moveright=function(board){
return move(board,[2,1,0])|move(board,[5,4,3])|pair(board,7,6)|pair(board,9,8);
};
var moveup=function(board){
if(board[2]===0||board[5]===0||board[2]===board[5]){return 0;};
return move(board,[0,3,6,8])|move(board,[1,4,7,9]);
};
var movedown=function(board){
return move(board,[8,6,3,0])|move(board,[9,7,4,1])|pair(board,5,2);
};
exports.moveleft=moveleft;
exports.moveright=moveright;
exports.moveup=moveup;
exports.movedown=movedown;

// 0 1 2
// 3 4 5
// 6 7
// 8 9


// 32k backfilling monster example(mmm)
// 2   32   64
// 0   16   128
// 0   8
// 2   4