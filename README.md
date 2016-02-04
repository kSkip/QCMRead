# QCMRead

I wrote this program because the display on the STM100 film deposition controller, that I used in my lab, was broken, 
but the controller worked fine otherwise. So I wrote this code to see the numbers 
on the display and set certain values in the controller. After writing it, I 
decided that it was useful for others and should be improved upon. This could be especially 
useful for automation purposes as the controller can operate deposition shutters. I plan on future version supporting all the features of these controllers adn related models as well.

## Compiling

The QCMRead repository is provided as a self-contained project, requiring only linking the windows development libraries and providing the header files for the Win32 API. A C++ compiler for Windows is required. A Visual Studio 6.0 project file is provided thus making compiling easy if you wish to use Visual C++.  

A binary for QCMRead is also provided. It was compiled on 32bit Windows XP with Visual Studio 6.0.

## How to use
#### Prerequisites
You must have Windows to use QCMRead. It has only been tested on Windows XP, but should compile on Windows 95 or higher. You need an availble RS232 port for the datalink to the controller. A crossover RS232 cable is required. Insure the baud rate 
of the controller is set 9600 (this is the max for this model), and the hard coded rate in the source.

#### Starting
Then upon starting you with be greeted with a dialog prompting 
you for the name of the com port (i.e. COM1, COM2, etc.).  

![Alt text](https://github.com/kSkip/QCMRead/raw/master/CommSetting.bmp)  

After you enter this information, 
the main window will open

#### Measuring 
You will see several values being displayed. They are updated every 500ms. The thickness is an accumulated deposition on the quartz crystal in the vacuum system. This is an accumulation over the time indicated next to time. For some reason the time counts down from 60:00 mins:sec to 00:00 and then back up. This is not a software bug as the controller also reads this value. In any case, pressing the "zero" button on hte controllers zeros the time and thickness. 

You should see:

  * Rate           (time derivative of thickness)
  * Thickness      (measured thickness)
  * Density        (density set for "current film")
  * Z-factor       (Z-factor set for "current film")
  * Remaining Life (percentage of remaining life of the quartz crystal)
  * Frequency      (resonant frequency of the quartz crystal)
  * Time           (time since the thickness was zeroed)

The main window is shown below.  

![Alt text](https://github.com/kSkip/QCMRead/raw/master/MainWindow.bmp)  

#### Film Properties

To change the density and z-factor of the current film, go to Settings->Film Properties

![Alt text](https://github.com/kSkip/QCMRead/raw/master/FilmProperties.bmp)  
