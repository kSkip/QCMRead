# QCMRead

I wrote this program because the display on the STM100 I used in my lab was totally broken, 
but the controller worked fine otherwise. So I needed to write this code to see the numbers 
on the display and set certain values in the controller. After writing this program, I 
noticed that it could be useful for others and should be improved upon. This could be especially 
useful for automation purposes.

How to use:

You must have Windows xp, or higher to use QCMRead. You need an availble serial com port 
for connecting the controller. A crossover RS232 cable is required. Insure the baud rate 
of the controller is set 9600. Then upon starting you with be greeted with a dialog prompting 
you for the name of the com port (i.e. COM1, COM2, etc.). After you enter this information, 
the main window will open and you will see several values being displayed. 
They are updated ever 500ms.

You should see:
Rate  (the time derivative of thickness)
Thickness (the measured thickness)
Density (the density set for "current film")
Z-factor  (the Z-factor set for "current film");
Remaining Life (the percentage of remaining life of the quartz crystal)
Frequency (the resonant frequency of the quartz crystal)
Time  (the time since the thickness was zeroed)
