var config=require("./config.js").config;
var movefile=config.movefile;
var move=require(movefile).move;
var pair=require(movefile).pair;
var moveleft=function(board){
return move(board,[0,1,2])|move(board,[3,4,5]);
};
var moveright=function(board){
return move(board,[2,1,0])|move(board,[5,4,3]);
};
var moveup=function(board){
return 0;
};
var movedown=function(board){
return pair(board,3,0)|pair(board,4,1)|pair(board,5,2);
};
exports.moveleft=moveleft;
exports.moveright=moveright;
exports.moveup=moveup;
exports.movedown=movedown;
