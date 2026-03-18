to make sleep button work:

sudo nano /etc/udev/rules.d/99-ir-receiver.rules

Add:
ACTION=="add", SUBSYSTEM=="usb", ATTRS{idVendor}== "045e", ATTRS{idProduct}=="006d", ATTR{power/wakeup}="enabled"

or if not working then

ACTION=="add", SUBSYSTEM=="usb", ATTR{power/wakeup}="enabled"

Apply the changes:
sudo udevadm control --reload-rules && sudo udevadm trigger