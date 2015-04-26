wareablematrixmin
=================

Wareable Matrix LED min for LPC1114

see [http://fukuno.jig.jp/822](http://fukuno.jig.jp/822)


#Install to Mac
##Install ARM cross compiler
Download *Mac installation  tarball* from [GNU Tools for ARM Embedded Processors](https://launchpad.net/gcc-arm-embedded/).  
As of 2015/04/26, direct url is [https://launchpad.net/gcc-arm-embedded/4.9/4.9-2015-q1-update/+download/gcc-arm-none-eabi-4_9-2015q1-20150306-src.tar.bz2](https://launchpad.net/gcc-arm-embedded/4.9/4.9-2015-q1-update/+download/gcc-arm-none-eabi-4_9-2015q1-20150306-src.tar.bz2)

##Unpack the tarball to the install directory
For example,

```bash:
sudo mv ~/Downloads/gcc-arm-none-eabi-4_9-2015q1-20150306-mac.tar.bz2 /usr/local/
cd /usr/local/
sudo tar xjf gcc-arm-none-eabi-4_9-2015q1-20150306-mac.tar.bz2 
```

You may want to install to /opt/local or ~/

##Make!

```bash:
PATH=$PATH:/usr/gcc-arm-none-eabi-4_9-2015q1/bin # Add GCC ARM Embedded toolchain
cd ~/Downloads/wareablematrixmin-master
make
```

out.hex should be created in ~/Downloads/wareablematrixmin-master/obj/

##(Optional) Add to path
vi ~/.bashrc  
Add following

```bash:
PATH=$PATH:/usr/gcc-arm-none-eabi-4_9-2015q1/bin # Add GCC ARM Embedded toolchain
```
##Write to LPC1114

Please reffer [IchigoJamのファームを書き換える](http://d.hatena.ne.jp/tarosay/20150401/1427917153) and [Mac OS Xでシリアル通信](http://mits-whisper.info/post/86784532536/lpc810-15-mac-os-x)
