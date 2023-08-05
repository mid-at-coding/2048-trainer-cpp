var config=require("./config.js").config;
var movefile=config.movefile;
var move=require(movefile).move;
var pair=require(movefile).pair;
var moveleft=function(board){
if(board[0]*board[5]*board[6]*board[7]*board[8]===0&&board[5]!==board[6]&&board[7]!==board[8]){return 0;};
return move(board,[1,2,3,4]);
};
var moveright=function(board){
return move(board,[4,3,2,1])|pair(board,6,5)|pair(board,8,7);
};
var moveup=function(board){
if(board[3]*board[4]===0){return 0;};
return move(board[0,1,5,7])|move(board[2,6,8]);
}
var movedown=function(board){
if(board[2]*board[3]*board[6]*board[4]*board[8]===0 && board[2]!==board[6] &&board[6]!==board[8]){return 0;};
return move(board,[7,5,1,0]);
};
exports.moveleft=moveleft;
exports.moveright=moveright;
exports.moveup=moveup;
exports.movedown=movedown;

// 0 x x x
// 1 2 3 4
// 5 6 x x
// 7 8 x x