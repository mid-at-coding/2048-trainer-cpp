var config=require("./config.js").config;
var movefile=config.movefile;
var move=require(movefile).move;
var pair=require(movefile).pair;
var moveleft=function(board){
return move(board,[0,1,2,3])|move(board,[4,5,6,7]);
};
var moveright=function(board){
return move(board,[3,2,1,0])|move(board,[7,6,5,4]);
};
var moveup=function(board){
return 0;
};
var movedown=function(board){
return pair(board,5,1)|pair(board,6,2)|pair(board,7,3)|pair(board,4,0);
};
exports.moveleft=moveleft;
exports.moveright=moveright;
exports.moveup=moveup;
exports.movedown=movedown;
