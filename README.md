# Parcellation


#### BV INSTALLATION IN UBUNTU 18.04 LTS FROM SCRATCH


Copy the files from “INSTALLATION_BrainVisa_2020” folder and paste them somewhere in the computer.


#### ************************** bv_maker.cfg .bashrc **********************************
  
Configuration settings. These files are: bv_maker.cfg y .bashrc.

1. Make a backup of .bashrc. The path is /home/username. 

![bashrc](/INSTALLATION_BrainVisa_2020/1.png)

2. Replace file .bashrc to .bashrcBACKUP:
  - cd ~/
  - mv ~/.bashrc ~/.bashrcBACKUP

3. Copy file .bashrc from the INSTALLATION_BrainVisa_2020 folder to the path /home/username:
  - cp /path/.bashrc ~/.bashrc 

Change your username in the file.

4. Close .bashrc file and create the folder .brainvisa:
  - cd ~/
  - mkdir ~/.brainvisa

5. Copy the file bv_maker.cfg from the folder INSTALLATION_BrainVisa_2020 to (.brainvisa):
  - cp /path/bv_maker.cfg ~/.brainvisa  

Close the terminal to update.


#### **************** INSTALLATION OF NECESSARY PACKAGES****************

  
1. Install packages:
  -	sudo apt-get install subversion
  -	sudo apt-get install cmake
  -	sudo apt-get install python-pip
  -	sudo pip install python-dateutil
  -	sudo apt-get install synaptic
  -	sudo apt-get install texlive-full
  -	sudo apt-get install python-sphinx
  -	sudo apt-get install gfortran
  -	sudo pip install soma-workflow
  -	sudo apt install python-numpy python-scipy
  -	sudo apt install default-jdk
  -	sudo apt install python-matplotlib python-paramiko pyro
  -	sudo apt install swig


2. Configure the environment variable VTK in .bashrc: 

  - export VTK_DIR=/home/username/freesurfer/lib/vtk/lib/vtk-5.6  

3. Configure the environment variable soma_workflow in .bashrc:

  - export soma_workflow_DIR=/usr/local/lib/python2.7/dist-packages

Close the terminal(s) since we have modified the .bashrc file, this way the changes are updated (in principle all these changes should already appear in the .bashrc file).


#### ******************************* SYNAPTIC **********************************

  
Open synaptic and install all the following libraries.

1. Search in synaptic the library g++-4.9, right click on it and press Mark to install and then press Apply.

2. Look for:
  - libsigc++-2.0-dev
  - libXml2-dev 
  - libboost-all-dev

Mark to install and then press Apply.

3. Look for:
  -	libsphinxbase-dev, libssl-dev, libcairo2-dev, libxcb-composite0-dev, libxcb-cursor-dev, libxcb-damage0-dev, libxcb-dpms0-dev, libjpeg-dev, libpng++-dev, libtiff5-dev, libjasper-dev, doxygen, libarchive-dev, libghc-bzlib-dev, libghc-pipes-zlib-dev, gfortran-4.9, libdee-qt4-dev, libntrack-qt4-dev, libqjdns-qt4-dev, libqglviewer2-qt4, libqt4pas5, libqtgstreamer-dev, libqtweetlib1.0, libqwt5-qt4-dev, libqwtplot3d-qt4-dev, libsoqt4-dev, python-qt4, python-qt4-dev, libvtk-dicom-dev, libblas-dev, liblapack-dev, libgsl-dev, libhdf5-dev, hdf5-tools, liblinear3-dev, libsvm-dev, libhdf5-10-dev, libhdf5-mpi-dev,  libhdf5-mpich-dev, libhdf5-serial-dev, libjhdf5-java, libhdf5-openmpi-dev, libjpeg62-dev, libtiff5-dev, libminc-dev.   Mark to install and apply.


#### ************************************ SVN *************************************

  
1. Download necessary svn files. Write in console:
  - svn export https://bioproj.extra.cea.fr/neurosvn/brainvisa/development/brainvisa-cmake/branches/bug_fix /home/username/neurosvn/brainvisa-cmake
  - request user and password

2. Configure cmake. Write in console:
  - cd /home/ username /neurosvn/brainvisa-cmake
  - cmake -DCMAKE_INSTALL_PREFIX=. .         
  - make install


#### ********************************* SOURCES ************************************

  
1. Download code:
  - cd /home/username/neurosvn/brainvisa-cmake/bin/
  - ./bv_env_host bv_maker sources

If the download fails or some of the directories are not completely downloaded, you must re-launch the command. In case of error you must access the directory with problems (you have to look where it says FAILED in the terminal, that will be the path in which we will have to be to launch the svn cleanup command), to know the directory in which we are, by terminal we introduce pwd and it will show us the path in which we are, we only have to compare the path that we see in FAILED with the one that throws pwd, if we are in it we launch svn cleanup, otherwise we move to that path with the command:

  - cd /home/username/neurosvn/brainvisa-stable/folder…

and launch svn cleanup command.

2. Relaunch:
  - cd /home/username/neurosvn/brainvisa-cmake/bin/
  - ./bv_env_host bv_maker sources

If it still doesn't work you have to keep trying, sometimes it's the fault of the connection or the servers.

If everything went well the message All went good will be displayed and you can skip to the CONFIGURE section, if the error is caused by /capsul then continue with the CAPSUL section.

First of all, make sure that you have downloaded the ptk (Connect commands) folder in /home/username/neurosvn/brainvisa-stable


#### ********************************** CAPSUL *************************************

  
1. In case the error occurs through the /capsul directory, what we'll have to do is open a terminal and press
  - cd home/username/neurosvn/brainvisa-stable
  - rm -rf capsul/

2. Copy capsul folder (INSTALLATION_BrainVisa_2020) in home/username/neurosvn/brainvisa-stable

3. Copy the  file components_sources.json in /home/username/neurosvn/brainvisa-stable in case you do not find it (rare if you have downloaded well but also included in INSTALLATION_BrainVisa_2020).

4. Move to:
  - cd /home/username/neurosvn/brainvisa-stable 

and tip:
  - svn cleanup
  - svn update

When you finish updating the svn you can go to the CONFIGURE section.

Here you should finish without errors.


#### ******************************* CONFIGURE ************************************
  
1. Launch configure in terminal:
  - cd /home/username/neurosvn/brainvisa-cmake/bin/
  - ./bv_env_host bv_maker configure

For all the errors you will have to download the libraries with synaptic (it is preferable to download the libraries with -dev), most of them should already be installed at this time because at the beginning of this guide there is an installation section.

If you do the ./bv_env_host bv_maker configure and the All went good message appears at the end, you can go to the last command that performs the compilation (BUILD section).


#### *********************************** BUILD*************************************
  
Nota: Verify in file bv_maker.cfg that “build” section is not commented.

1. Compile the code:
  - cd /home/username/neurosvn/brainvisa-cmake/bin/
  - ./bv_env_host bv_maker build

Here you have to take into account the execution threads, if it gives problems when doing the build, vary the threads, for example, -j4 = 4 threads, -j1 = 1 execution thread. The threads are varied in the bv_maker.cfg file.

You have to execute ./bv_env_host bv_maker build as many times as necessary (varying the number of threads and until it reaches 100% of the installation) until All went good appears, otherwise it will not have been compiled correctly. 

At some point in the build, some libraries will fail. Usually they will be in the path shown here, if not just change the path and copy those files to the Brainvisa folder.

2. At some point you may have to create the /usr/X11R6/lib64/ directory by typing in terminal:
  - sudo mkdir /usr/X11R6
  - cd /usr/X11R6
  - sudo mkdir lib64

3. Copy these libraries:
  - sudo cp /usr/lib/x86_64-linux-gnu/libjpeg.so /usr/lib64/
  - sudo cp /usr/lib/x86_64-linux-gnu/libtiff.a /usr/lib64/
  - sudo cp /usr/lib/x86_64-linux-gnu/libexpat.a /usr/lib64/
  - sudo cp /usr/lib/x86_64-linux-gnu/libfreetype.so /usr/lib64/
  - sudo cp /usr/lib/x86_64-linux-gnu/libXt.so /usr/lib/
  - sudo cp /usr/lib/x86_64-linux-gnu/libSM.so /usr/X11R6/lib64/
  - sudo cp /usr/lib/x86_64-linux-gnu/libICE.so /usr/X11R6/lib64/
  - sudo cp /usr/lib/x86_64-linux-gnu/libX11.so /usr/X11R6/lib64/
  - sudo cp /usr/lib/x86_64-linux-gnu/libXext.so /usr/X11R6/lib64/
  - sudo cp /usr/lib/x86_64-linux-gnu/libXss.so /usr/X11R6/lib64/   (included in INSTALLATION_BrainVisa_2020, copy all the files from folder lib64 to that path)
  - sudo cp /usr/lib/x86_64-linux-gnu/libGL.so /usr/lib64/

Now launch the command ./bv_env_host bv_maker build. Don't forget to constantly throw ./bv_env_host bv_maker build changing the number of threads.

When you finish and get to 100% and put All went good, we will prove that the installation works. Therefore, you have to use the two folders from INSTALLATION_BrainVisa_2020: testAims and testConnect.

4. We move to the folder:
  - cd /home/username/neurosvn/build-brainvisa-stable-release/bin/

and open a terminal.

5. For the testAims:
  - ./AimsThreshold -i /path/testAims/t1.ima -o /path/testAims/out.ima -t 50 -m lt

6. For the testConnect:
  - ./ConnectFiberDistanceMatrix -i /path/pruebaConnect/short_2_28.bundles -outdir /path/pruebaConnect/ -outstr TEST

In addition, we will also check that soma can be imported.

7. We type in the terminal:
  - from soma import aims

If no mistakes, everything went well and aims can be imported.


...---===---...---===---...===---...---===*** END ***===---...---===---...===----...---===---...



