var move=function(board,positions){//This attempts to make a move on the board over some "row" with positions specified in the second argument. If successful, it returns some nonzero number. Otherwise, it returns zero. Modifies in-place.
  //Note: From what I can tell, the original 2048 implementation runs in quadratic time per row. This should be linear time per row.
  //I know that this works but I struggle to explain it. I've tried anyways.
  //We are looking at two tiles: curr and now.
  //now is the tile that we're currently looking at in the "row".
  //curr is some previous tile that we're currently looking at; we are considering merging into it.
  var curr=0;
  var j=0;//This lets us modify in-place.
  var moved=0;
  var a=board[positions[0]];
  for(var i=1;i<positions.length;i++){
    var b=board[positions[i]];
    if((a===b&&a>0)||(a===0&&b>0)){
      moved=1;
      break;
    }
    a=b;
  }
  for(var i=0;i<positions.length;i++){
    var now=board[positions[i]];//Look at this tile
    if(!now){//Blank space; don't do anything
      continue;
    }else if(!curr){//curr is blank. As a result, now is the next tile that we might merge into.
      curr=now;
      //false||nonzero becomes nonzero, which is not a boolean. So we might as well use the same datatype: 0|nonzero is still a number.
      continue;
    }//At this point, we know that both now and curr aren't blank.
    var p=positions[j];
    board[p]=curr;//Put this into the board
    if(curr===now){//Time to merge.
      board[p]<<=1;//Double this value
      curr=0;//We've merged, so drop this.
    }else{
      curr=now;//We can't merge, so we go to the next tile.
    }
    j+=1;//Now go to the next space. Observe that j can never get ahead of i, so we'll be fine.
  }
  if(curr){//We may not have put the last curr back into the board, so let's do it now.
    board[positions[j]]=curr;
    j+=1;
  }
  while(j<positions.length){//Clear the rest of the "row"
    board[positions[j]]=0;
    j+=1;
  }
  return moved;
};
exports.move=move;
var pair=function(board,a,b){
  var c=board[a];
  var d=board[b];
  if(c+d===0){
    return 0;
  }
  if(c===0||c===d){
    board[a]+=d;
    board[b]=0;
    return 1;
  }
  return 0;
};
exports.pair=pair;