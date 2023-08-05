# 2048-trainer
All the tools you'll need to improve your 2048 endgame skills

### If you have any questions, comments, or concerns, don't hesitate to ask on [Discord](https://discord.gg/8mmVNCP3a6) or [Twitter](https://twitter.com/h0si)

## tablegen
### WARNING: this tool can make extremely large tables. 100 GB of free storage and 16 GB of memory is strongly recommended.

To prepare to make a table:
1. Find the file in the `move dir` folder that corresponds to your desired table
2. copy and paste the contents of that file into the `movedir.js` file. The arrangement of tiles may be 
3. open `satisfied.js`
4. change the list index (left) to whichever tile on the board needs to be the goal tile (zero-indexed), then change the other number (right) to the goal tile
5. open `seqlistfromposition.js` and find `var start=[]`. This should be on line 61.
6. change it to whatever position you want to start with

Now to make the table:
1. Make sure node.js is installed.
2. find your way to the tablegen folder in command prompt
3. `node seqlistfromposition.js`
4. wait until either the positions are completely finished generating or quit after the sum of tiles is at least 64 greater than your goal tile
5. open `config.txt`
6. set `s0` to 4 lower than the highest sum
7. set `s1` to the lowest sum (the first line after you ran `seqlistfromposition.js`
8. run `node tablegen.js` and wait

## The trainer

Before running the trainer, make sure it's configured for your PC. 
1. Make sure that the tool looks in the right place for your table. Change line 181 to where you store your tables; I store them on the F: drive
2. Set `table_id` (line 504) to the folder(s) you store the desired table
3. On line 184 (`[x:10]`), make sure that the "10" is replaced with the exact number of spaces used in your table.
4. `python "2048-trainer.py"` to run
