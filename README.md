# Zhongkong-Robot-Competition
Code library of 2018 Zhongkong Robot Competition for Supermarket Challenge

# Description 
The competition venue simulates a supermarket. There are 12 cargo windows on each of the four shelves A, B, C, and D. Each cargo window can hold up to 1 piece of goods. Now each shelf has at least 3 cargo windows. Lack of goods. The team needs to design and build on
Autonomous robots participate in the competition. The robot starts from the red "starting area" of the field and completes the replenishment operation of 12 pieces of goods within 12 minutes. The robot needs to obtain the goods on the platform of the E warehouse and place them in the empty window of the designated shelf. 5 points are awarded for each item placed correctly, and 5 points are deducted for the wrong shelf or non-empty window.  

![image](https://github.com/YuqingZhangMirror12/Zhongkong-Robot-Competition/blob/main/Competition%20Map.png)
![image](https://github.com/YuqingZhangMirror12/Zhongkong-Robot-Competition/blob/main/Final%20Design.jpg)

# Code
CV_Py contains the Vision Code for Object goods detection.  
CV01 contains all train, test, validation dataset created by us.  
Base Code contains the moving motor control, grasping control, photoelectric sensor recognition and line patrol code.  
