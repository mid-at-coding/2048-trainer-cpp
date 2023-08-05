var config=require("./config.js").config;
var movefile=config.movefile;
var move=require(movefile).move;
var pair=require(movefile).pair;
var moveleft=function(board){
return move(board,[0,1,2])|move(board,[3,4,5])|move(board,[6,7,8]);
};
var moveright=function(board){
return move(board,[2,1,0])|move(board,[5,4,3])|move(board,[8,7,6]);
};
var moveup=function(board){
return move(board,[0,3,6])|move(board,[1,4,7])|move(board,[2,5,8]);
};
var movedown=function(board){
return move(board,[6,3,0])|move(board,[7,4,1])|move(board,[8,5,2]);
};
exports.moveleft=moveleft;
exports.moveright=moveright;
exports.moveup=moveup;
exports.movedown=movedown;
