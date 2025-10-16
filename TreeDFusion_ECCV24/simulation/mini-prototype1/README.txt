This prototype outputs the logs for making a rectangular prism rise in height over time. 

To run this simulation: Be sure to download both include, src, and CMakeLists.txt into a contained folder. Then cd into that folder 
on your terminal and run the following commands. 

mkdir -p build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j
./shape_growth

Output should look something like this: 

{"jobId":"job-001","t":0.0333333,"width":1,"depth":1,"height":1.00666667,"shape":"rect_prism"}
{"jobId":"job-001","t":0.0666667,"width":1,"depth":1,"height":1.01333333,"shape":"rect_prism"}
{"jobId":"job-001","t":0.1,"width":1,"depth":1,"height":1.02,"shape":"rect_prism"}
{"jobId":"job-001","t":0.133333,"width":1,"depth":1,"height":1.02666667,"shape":"rect_prism"}
{"jobId":"job-001","t":0.166667,"width":1,"depth":1,"height":1.03333333,"shape":"rect_prism"}

Eventually, this prototype will begin to take in a tree type from user input, then based off the tree type, scale the growth over a period of time. 
This will be the next mini-prototype.


