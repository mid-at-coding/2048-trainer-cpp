var config=require("./config.js").config;
var bfsqueue=[];
var spaces=config.spaces;
//console.log(spaces);
var spaces1=spaces-1;
var spaces2=spaces-2;
for(var i=0;i<=spaces+1;i++){
  bfsqueue.push([i]);
}
for(var i=0;i<spaces1;i++){
  var k=spaces-i;
  var newqueue=[];
  for(var j=0;j<bfsqueue.length;j++){
    var item=bfsqueue[j];
    var smallest=Math.min(item[i],k);
    for(var x=0;x<=smallest;x++){
      var item1=item.slice();
      item1.push(x);
      newqueue.push(item1);
    }
  }
  bfsqueue=newqueue;
}
var isvalid=function(list){
  for(var i=0;i<spaces;i++){
    var j=list[i];
    if(j===1||j===2){
      return true;
    }
  }
  return false;
};
var filtered=[];
//var positions=0;
var permute=function(list){
  var permutations=list.join("")+"\n";
  while(true){//Narayana algorithm
    var k=spaces1;
    for(var i=spaces2;i>=0;i--){
      if(list[i]<list[i+1]){
        k=i;
        break;
      }
    }
    if(k===spaces1){
      break;
    }
    var l=spaces1;
    while(list[k]>=list[l]){
      l-=1;
    }
    list[l]=[list[k],list[k]=list[l]][0];
    k+=1;
    var start=list.slice(0,k);
    var end=list.slice(k).reverse();
    list=start.concat(end);
    permutations+=list.join("")+"\n";
  }
  //positions+=permutations.length/9;
  //console.log(permutations);
  return permutations;
};

var factorial=function(n){
  var p=1;
  for(var i=1;i<=n;i++){
    p*=i;
  }
  return p;
};
var validconfigurations=0;
var buckets=[];
var values=require("./maps.js").f;
var mappings=require("./maps.js").b;
for(var i=0;i<bfsqueue.length;i++){
  var item=bfsqueue[i];
  if(isvalid(item)){
    var sum=0;
    item.map(x=>sum+=values[x]);
    if(!buckets[sum]){
      buckets[sum]=[];
    }
    buckets[sum].push(item);
    //buckets[sum]+=permute(item.map(x=>mappings[x]).reverse());
  }
}
//console.log(buckets);
//console.log(validconfigurations);
var fs=require("fs");
var n=(1<<(spaces+2))-4;
//console.log(n);
for(var i=2;i<=n;i+=2){
  console.log(i);
  var perms="";
  var positions=buckets[i];
  for(var j=0;j<positions.length;j++){
    var item=positions[j];
    perms+=permute(item.map(x=>mappings[x]).reverse());
  }
  fs.writeFileSync("./"+config.positionsdir+"/"+i+".txt",perms);
}
//console.log(positions);
//permute(["a","a","a","a","e","f","g","h","i","j"]);

//console.log(filtered);