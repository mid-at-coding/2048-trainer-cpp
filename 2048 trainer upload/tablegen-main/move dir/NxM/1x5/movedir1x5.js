var config=require("./config.js").config;
var movefile=config.movefile;
var move=require(movefile).move;
var pair=require(movefile).pair;
var moveleft=function(board){
return move(board,[0,1,2,3,4]);
};
var moveright=function(board){
return move(board,[4,3,2,1,0]);
};
var moveup=function(board){
return 0;
};
var movedown=function(board){
return 0;
};
exports.moveleft=moveleft;
exports.moveright=moveright;
exports.moveup=moveup;
exports.movedown=movedown;
