clear
KERNEL_SRC="/usr/src/kernel"
SPI_SRC="$KERNEL_SRC/drivers/spi"

#echo $KERNEL_SRC
#echo $SPI_SRC

sudo cp -f ./jetduino_gpio.c $SPI_SRC/jetduino_gpio.c

sudo make -C $KERNEL_SRC M=$SPI_SRC modules






