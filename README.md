# tfRedundantFix
Rosbags may contain multiple tf messages (transformations) with the same timestamp. This packet removes redundant transfomraitons

```
cd data
rosrun tfRedundantFix tfFix --input `pwd`/bag.bag --output `pwd`/output.bag
```
