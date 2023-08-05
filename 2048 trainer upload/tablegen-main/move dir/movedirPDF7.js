var config=require("./config.js").config;
var movefile=config.movefile;
var move=require(movefile).move;
var pair=require(movefile).pair;
var moveleft=function(board){
  if(board[4]*board[5]*board[6]===0){
    return 0;
  };
return move(board,[3,2,1,0]);
};
var moveright=function(board){
return move(board,[0,1,2,3]);
};
var moveup=function(board){
  if(board[0]*board[1]*board[2]===0){
    return 0;
  };
return move(board,[3,4,5,6]);
};
var movedown=function(board){
return move(board,[6,5,4,3]);
};
exports.moveleft=moveleft;
exports.moveright=moveright;
exports.moveup=moveup;
exports.movedown=movedown;
