// PURPOSE
// ??

var config=require("./config.js").config;
var movefile=config.movefile;
var movedirfile=config.movedirfile; 
var move=require(movefile).move;
var pair=require(movefile).pair;
var moveleft=require(movedirfile).moveleft;
var moveright=require(movedirfile).moveright;
var moveup=require(movedirfile).moveup;
var movedown=require(movedirfile).movedown;
var maps=require("./maps.js").maps; // num   -> alpha
var spam=require("./maps.js").spam; // alpha -> num
var satisfied=require(config.satisfiedfile).satisfied;
var positionsdir=config.positionsdir;
var tablesdir=config.tablesdir;
// this function turns a given string into a board
// a board seems to be a stack of numbers.
var str2board=function(str){
  var board=[];
  for(var i=0;i<str.length;i++){
    board.push(spam[str[i]]); // TODO: change to log
  }
  return board;
};
var fs=require("fs");
var s0=config.s0;
var s1=config.s1;
// s0: ending sum of tiles
// s1: starting sum of tiles
for(var i=s0+2;i<=s0+4;i+=2){
  var positions=fs.readFileSync("./"+positionsdir+"/"+i+".txt","utf-8").split("\n");
  // get a position (what the fuck is a position??) from some 'positionsdir' which is specified in config.txt and is by default set to 'positionstest'
  // it is worth noting ./positionstest is not a directory that exists(indeed, there is no directory named positionstest in this repo)
  if(positions[positions.length-1]===""){ // if the last line of the raw data for this position is empty, remove it
    positions.pop();
  }
  positions=positions.map(x=>x+" "+1*satisfied(str2board(x))); // turn 'positions' into a map ...
  // where every line is mapped onto a binary value that's true if str2board(x)[9] == 1024 (DPDF win condition)
  // in essence: map every position to a binary value based on if it wins DPDF or not
  fs.writeFileSync("./"+tablesdir+"/"+i+".txt",positions.join("\n"));
  // write to some tablesdir, specified in config.txt. like the positionsdir, this is not a directory that exists by default
} 

// TODO: find out what the args to this mean
// find the "utility" of a given position using expectimax
var evalprob=function(moved,v,w){ // TODO: change to reference
  var emptytiles = 0; // number of empty tiles
  var prob2 = 0; // ??
  var prob4 = 0; // ??
  for(var i=0;i<moved.length;i++){ // go through the board
    if(moved[i]){  		// if there's a tile in this position
      continue;
    }
    emptytiles+=1; 		// there's no tile here, increment emptytiles 
    moved[i]=2;			// set the current (empty) tile to a 2
    prob2+=v[moved];	// add v[the board] (??? wtf does that mean) to prob2 TODO: come back to this: what the fuck is v
    moved[i]=4;
    prob4+=w[moved];
    moved[i]=0;
  }
  return (9*prob2+1*prob4)/10/emptytiles;
};
var n4; // two spawn cache
// the motherfuck unlimited. dig in folks
var solve=function(s){
  var t=s+2; // sum of the tiles if a 2 spawns
  var u=s+4; // sum of the tiles if a 4 spawns
  var a=fs.readFileSync("./"+positionsdir+"/"+s+".txt","utf-8").split("\n"); 
  // get all the positions that have a specific sum of tiles
  if(a[a.length-1]===""){
    // remove the last line if it's empty
    a.pop();
  }
  var d=a.map(x=>str2board(x));
  // convert raw data from a into multiple boards
  var v={}; // v[board] = probability of board (for tile sum +2)
  var w={}; // w[board] = probability of board (for tile sum +4)
  if(n4){ // if s+4 is already calculated, set w equal to the cache
    w=n4;
  }else{
    console.log("Processing "+u+".txt..."); // processing the boards with a sum of tiles that assume a 4 spawn
    var c=fs.readFileSync("./"+tablesdir+"/"+u+".txt","utf-8").split("\n").map(x=>x.split(" ").slice(0,2)).map(x=>[str2board(x[0]),1*x[1]]); // uhhhhhh
	// read from the tables dir assuming a four spawn
	// map this to a probability
	// c[n][0] = nth board in this file
	// c[n][1] = probability of reaching that board
    c.map(x=>w[x[0]]=x[1]); // put these results in w
    c=[]; // reset c (why??)
  }
  console.log("Processing "+t+".txt...");
  var b=fs.readFileSync("./"+tablesdir+"/"+t+".txt","utf-8").split("\n").map(x=>x.split(" ").slice(0,2)).map(x=>[str2board(x[0]),1*x[1]]);
  // read from the tables dir assuming a two spawn
  // map this to a probability
  // b[n][0] = nth board in this file
  // b[n][1] = probability of reaching that board
  b.map(x=>v[x[0]]=x[1]); // put these results in v
  n4=v; // cache s + 2 case for n + 4
  b=[]; // reset b
  console.log("Solving s="+s);
  var u=""; // boards + probability (?)
  for(var i=0;i<d.length;i++){
    // go through all the boards in d
    var currentBoard=d[i];
	// set board equal to the current board from d
    if(satisfied(currentBoard)){
	  // if this board is a winstate
      //u.push([a[i],1]);
      u+=a[i]+" 1\n"; // TODO: what the fuck does this mean
	  // a[i]: raw string data for ith board
	  // 1: probability of winning from this boardstate
      continue;
    }
    var probs=[]; // probability of winning from a given move from a given board
    var bestprob=0;
    var moved=currentBoard.slice(); 	// copy currentBoard into moved
    if(moveleft(moved)){ 				// if board can move left
      probs.push("L");   				// add "L" to the probability(identifier for the move)
      var prob=evalprob(moved,v,w); 	// add the actual probability of winning
      probs.push(prob);					// add this to the master list
      bestprob=Math.max(prob,bestprob); // set maximum if it's changed
    }
    moved=currentBoard.slice(); 		// repeat for all directiosn
    if(moveright(moved)){
      probs.push("R");
      var prob=evalprob(moved,v,w);
      probs.push(prob);
      bestprob=Math.max(prob,bestprob);
    }
    moved=currentBoard.slice();
    if(moveup(moved)){
      probs.push("U");
      var prob=evalprob(moved,v,w);
      probs.push(prob);
      bestprob=Math.max(prob,bestprob);
    }
    moved=currentBoard.slice();
    if(movedown(moved)){
      probs.push("D");
      var prob=evalprob(moved,v,w);
      probs.push(prob);
      bestprob=Math.max(prob,bestprob);
    }
    probs=[a[i],bestprob].concat(probs);
	// probs += {board, best probability}
    u+=probs.join(" ")+"\n";
	// seperate out the probabiltiies of the subboards by a space and the root boards by a newline
  }
  //u=u.join("\n");
  fs.writeFileSync("./"+tablesdir+"/"+s+".txt",u);
};
for(var i=s0;i>=s1;i-=2){
  solve(i);
}
