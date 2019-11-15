# HUOJI CSGO battle platform [under development]
# *Note, this open source project has no client-side anti-cheating. If you have real strength, you can contact me by email. Email: root@key08.com
The original intention of writing this project was to improve the atmosphere of domestic CSGO and to make some people more likely.
## Advice
> b station @automatic delivery robot https://space.bilibili.com/101385631 
> Instead of complaining about others every day. Why not experience the hard work of others?
## Donate
Your donation will give me more time to develop this platform (please send your Steamid in Alipay after donation):
![image](https://github.com/huoji120/csgo_full/blob/master/pic/alipay.jpg)
## Features
+ 5 person room system
+ Invitation code registration system
+ The server is anti-cheating, detecting self-attacking and shielding perspective (the perspective cannot pass through the wall)
+ Client anti-cheating based on VT virtualization technology * Note that this open source project has no client-side anti-cheating
+ Room chat system
+ ladder alignment system
+ Personal record system
+ Content Audit System
## TODO
+ friend system
+ demo storage and resolution system
+ Emergency shutdown system
> The anti-cheat system has written a separate set, but it has not been docked with this platform. Based on VT virtualization technology. It is reserved for friends who have the strength to commercialize. So it is not open source.
> This platform only writes a month's demo. If you build a commercial, you should refactor it. Individuals can't take care of it because of life. There are bugs that can be sent to improve my mailbox.
## Build:
Please be patient. Easy to build
## Pre-preparation:
Sign up: https://www.vaptcha.com and buy a VIP service (I don't even talk about setting up a platform for VIP money?)
Register: https://console.bce.baidu.com/ Create a content review service
Install electron and install the dependencies required by the client
## Start building
Register Sina Cloud -> Cloud Application -> Create application, recommended configuration as shown:
![image](https://github.com/huoji120/csgo_full/blob/master/pic/1.png)
Purchase the corresponding redis and mysql services:
![image](https://github.com/huoji120/csgo_full/blob/master/pic/2.png)
Modify the mysql configuration file, redis configuration, vaptcha, Baidu ak, server communication key (important) in the django_www/www/www/url.py code to your registered
Change the mysql and redis connection configuration of the django_www/EzServer.py code to the same thing as above.
![image](https://github.com/huoji120/csgo_full/blob/master/pic/3.png)
Enter phpadmin to restore the backup file. The backup file is backup.sql
Then modify the vaptcha_id of client/electron/main.js to your registered vaptcha information, and the WebsiteDomain and Domain correspond to your domain name.
Modify the admin login account password in django_www/www/www/view.py (Hint: Search huoji120)
## last step
Enter the SAE LINUX terminal Upload your changed code, then execute the django/www/install_lib.sh code to install python and support library.
Create a background service with the built-in mint Create two EzServer.py with python3 /home/django_www/www/manage.py runserver 0:5050
Execute electron --no-sandbox in the client directory. / If the black screen please add --disable-gpu to turn off the graphics card rendering (white screen black screen is often related to the graphics card)
The client directory has pack.cmd which is the packaging command.
success:
![image](https://github.com/huoji120/csgo_full/blob/master/pic/4.png)
![image](https://github.com/huoji120/csgo_full/blob/master/pic/5.png)
![image](https://github.com/huoji120/csgo_full/blob/master/pic/6.png)
![image](https://github.com/huoji120/csgo_full/blob/master/pic/7.png)
![image](https://github.com/huoji120/csgo_full/blob/master/pic/8.png)
![image](https://github.com/huoji120/csgo_full/blob/master/pic/9.png)
![image](https://github.com/huoji120/csgo_full/blob/master/pic/10.png)
![image](https://github.com/huoji120/csgo_full/blob/master/pic/11.png)
![image](https://github.com/huoji120/csgo_full/blob/master/pic/12.png)
![image](https://github.com/huoji120/csgo_full/blob/master/pic/13.png)
![image](https://github.com/huoji120/csgo_full/blob/master/pic/14.png)
![image](https://github.com/huoji120/csgo_full/blob/master/pic/15.png)
![image](https://github.com/huoji120/csgo_full/blob/master/pic/16.png)
![image](https://github.com/huoji120/csgo_full/blob/master/pic/17.png)
![image](https://github.com/huoji120/csgo_full/blob/master/pic/18.png)
![image](https://github.com/huoji120/csgo_full/blob/master/pic/19.png)
![image](https://github.com/huoji120/csgo_full/blob/master/pic/20.png)
![image](https://github.com/huoji120/csgo_full/blob/master/pic/21.png)
![image](https://github.com/huoji120/csgo_full/blob/master/pic/22.png)
## The last step
Upload the sourcemod server to your CSGO server
Open the sourcemode/addons/cfg/crow.ini file
Modify the server id and server communication key inside
Visit http://csgo.applinzi.com/ha4k1r_admin/ Click Add Server, fill in the server id you just added to have the game server!
Run.sh is the open service script. Modify the steamkey inside for you. Put it in the csgo server directory and open it directly. /run.sh
## Acknowledgement list
Mav. GZ. Sixth person Ashiterui TwiLight Sibene LeaderHK 16 nights*night HETAO:P Matcha control
WWXD Beibei Speturm Ghost Island My little gray is to give white nine deemder zzz Lappland Mo Chen q Rainbowcolt
Rvcyle is back to Mai Mai Ha4k1r President of the Dry Pull Association and all the friends who have supported this project.
