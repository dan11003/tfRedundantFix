
# tfRedundantFix
Rosbags may contain multiple tf messages (transformations) with the same timestamp. This packet removes redundant transformations.
## Do I have redundancies
if you get something like when playbacking a file you may
```
TF_REPEATED_DATA ignoring data with redundant timestamp for frame base_link at time 1686147047,560185 according to authority /play_1686565779400816854
```


```
cd data
rosrun tfRedundantFix tfFix --input `pwd`/bag.bag --output `pwd`/output.bag
```
