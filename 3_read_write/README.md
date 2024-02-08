DEVICE NUMBERS AND FILES

This kernel modules reads if there is a file connected to device number 90.

Improvements:
  In the case that there is already a file associated to the same device number, it is suposed that on the 32 bits of the int return value of register_chrdev,
  the 12 higher bits are reserved to the MAJOR device number and the 20 lower bits are reserved for the MINOR device number. 
  But in a program we are not suposed to rely on these suppositions especially as this may change with different hardware.
  So instead of using bitwise operation we should use the functions MAJOR() and MINOR(). I don't know yet how to use them but i will as soon as i learn about it.
